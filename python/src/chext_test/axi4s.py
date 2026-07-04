from hdlinfo.protocols.amba import axi4s
import hdlinfo
from hdlscw import codegen, wrapper
from typing import *
from .common import *

__all__ = []


_SIGNAL_MAP = {
    "TREADY": "ready",
    "TVALID": "valid",
    "TDATA": "bits.data",
    "TSTRB": "bits.strb",
    "TKEEP": "bits.keep",
    "TLAST": "bits.last",
    "TID": "bits.id",
    "TDEST": "bits.dest",
    "TUSER": "bits.user",
}

def _getVerilatedSignalName(interface: hdlinfo.Interface, signal: str) -> str:
    if interface.kind == "axi4s_rtl_hier":
        return f"{interface.name}_{_SIGNAL_MAP[signal].replace('.', '_')}"

    return f"{interface.name}_{signal}"


def _cppBool(value: bool) -> str:
    return "true" if value else "false"


@wrapper.registerInterfaceHandlerCustom("InterfaceHandlerAxi4s")
class Axi4sInterfaceHandler(wrapper.StatefulInterfaceHandler):
    def __init__(self, wrapper: wrapper.Wrapper, cg: codegen.CodeGen) -> None:
        super().__init__(wrapper, cg)

        self._publicBlocks: List[codegen.Block] = []
        self._ctorInitBlocks: List[codegen.Block] = []
        self._ctorBlocks: List[codegen.Block] = []

        includeStr = self.getOptionStr("chext_test.includeStr", "<$>")

        def mkIncludeStr(s: str) -> str:
            return includeStr.replace("$", s)

        def hdrInclude(d: codegen.Dumper) -> None:
            d.iwriteln("/* BEGIN: chext_test includes for 'amba/axi4s' */")
            d.iwriteln(f"#include {mkIncludeStr('chext_test/amba/axi4s/Driver.hpp')}")
            d.iwriteln("/* END: chext_test includes for 'amba/axi4s' */")

            d.separate()

        def public(d: codegen.Dumper) -> None:
            d.iwriteln("/* BEGIN: chext_test public for 'amba/axi4s' */")
            dumpBlockList(d, self._publicBlocks)
            d.iwriteln("/* END: chext_test public for 'amba/axi4s' */")

            d.separate()

        def ctorInit(d: codegen.Dumper) -> None:
            dumpBlockList(d, self._ctorInitBlocks)

        def ctor(d: codegen.Dumper) -> None:
            d.iwriteln("/* BEGIN: chext_test ctor for 'amba/axi4s' */")
            dumpBlockList(d, self._ctorBlocks)
            d.iwriteln("/* END: chext_test ctor for 'amba/axi4s' */")

            d.separate()

        cg.addHdrIncludeBlock(hdrInclude)
        cg.addPublicBlock(public)
        cg.addCtorInit(ctorInit)
        cg.addCtorBlock(ctor)

    def processInterface(self, interface: hdlinfo.Interface) -> None:
        if "config" not in interface.args:
            raise Exception(f"interface '{interface.name}' with kind '{interface.kind}' has no 'config'!")

        cfg: axi4s.Config = interface.args["config"]

        role = None
        if interface.role == "slave":
            role = "Slave"
        elif interface.role == "master":
            role = "Master"
        else:
            raise RuntimeError(f"Invalid interface role: {interface.role}")

        params = ",".join([
            str(cfg.wData),
            str(cfg.wId),
            str(cfg.wDest),
            str(cfg.wUser),
            _cppBool(cfg.hasReady),
            _cppBool(cfg.hasStrobe),
            _cppBool(cfg.hasKeep),
            _cppBool(cfg.hasLast),
        ])

        name = interface.name

        def publicBlock(d: codegen.Dumper) -> None:
            d.iwriteln(f"chext_test::amba::axi4s::{role}<{params}> {name};")

        def ctorInitBlock(d: codegen.Dumper) -> None:
            clock = self.wrapper.getClock(interface.associatedClock)
            reset = self.wrapper.getReset(interface.associatedReset, True)
            d.iwrite(f"{name}(\"{name}\", {clock}, {reset})")
            d.writeln(",")

        def ctorBlock(d: codegen.Dumper) -> None:
            for signal in cfg.signals:
                verilatedSignalName = _getVerilatedSignalName(interface, signal)
                d.iwriteln(f"verilatedModule_.{verilatedSignalName}(this->{name}.{_SIGNAL_MAP[signal]});")
            d.separate()

        self._publicBlocks.append(publicBlock)
        self._ctorInitBlocks.append(ctorInitBlock)
        self._ctorBlocks.append(ctorBlock)

    @staticmethod
    def checkKind(kind: str) -> bool:
        return kind == "axi4s" or kind == "axi4s_rtl_hier"
