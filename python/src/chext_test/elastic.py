from typing import *
from dataclasses import dataclass, field
from hdlscw import codegen, wrapper
from .common import *
import hdlinfo
import re

__all__ = ["ElasticProtocol"]

_protocolMap: Dict[str, "ElasticProtocol"] = {}
_protocolPattern = r'^(chext\.elastic\.Interface|readyValid|hdlinfo\.elastic\.Interface)\[([\w\.-]+)\]$'

@dataclass(frozen=True)
class ElasticProtocol:
    protocolName: str
    includeStr: Optional[str]
    bitsSignalType: Union[str, Callable[[hdlinfo.Interface], str]]
    portsToSignals: List[Tuple[str, str]] = field(default_factory=list)

    def __post_init__(self) -> None:
        if self.protocolName in _protocolMap:
            raise RuntimeError(f"An elastic protocol with name '{self.protocolName}' already exists!")

        _protocolMap[self.protocolName] = self


@wrapper.registerInterfaceHandler
class ElasticProtocolHandler(wrapper.StatefulInterfaceHandler):
    def __init__(self, wrapper: wrapper.Wrapper, cg: codegen.CodeGen) -> None:
        super().__init__(wrapper, cg)

        self._includeBlocks: List[codegen.Block] = []
        self._publicBlocks: List[codegen.Block] = []
        self._ctorInitBlocks: List[codegen.Block] = []
        self._ctorBlocks: List[codegen.Block] = []

        includeStr = self.getOptionStr("chext_test.includeStr", "<$>")

        def mkIncludeStr(s: str) -> str:
            return includeStr.replace("$", s)

        def hdrInclude(d: codegen.Dumper) -> None:
            d.iwriteln(f"/* BEGIN: chext_test includes for 'elastic' */")
            d.iwriteln(f"#include {mkIncludeStr('chext_test/elastic/Driver.hpp')}")
            dumpBlockList(d, self._includeBlocks)
            d.iwriteln(f"/* END: chext_test includes for 'elastic' */")

            d.separate()
        
        def public(d: codegen.Dumper) -> None:
            d.iwriteln(f"/* BEGIN: chext_test public for 'elastic' */")
            dumpBlockList(d, self._publicBlocks)
            d.iwriteln(f"/* END: chext_test public for 'elastic' */")

            d.separate()
        
        def ctorInit(d: codegen.Dumper) -> None:
            dumpBlockList(d, self._ctorInitBlocks)

        def ctor(d: codegen.Dumper) -> None:
            d.iwriteln(f"/* BEGIN: chext_test ctor for 'elastic' */")
            dumpBlockList(d, self._ctorBlocks)
            d.iwriteln(f"/* END: chext_test ctor for 'elastic' */")

            d.separate()

        cg.addHdrIncludeBlock(hdrInclude)
        cg.addPublicBlock(public)
        cg.addCtorInit(ctorInit)
        cg.addCtorBlock(ctor)

    @staticmethod
    def checkKind(kind: str) -> bool:
        match = re.match(_protocolPattern, kind)
        if match:
            protocolName = match.group(2)
            if protocolName not in _protocolMap:
                print(f"[warn] Elastic protocol is not registered: {protocolName}")
                return False
            return True
        else:
            return False


    def processInterface(self, interface: hdlinfo.Interface) -> None:
        # since checkKind is a static method (as a result of which we instantiate the handler)
        # we should process the interface.kind again. Is that ideal? No, but I do not really
        # think there is a better way...
        protocolName = re.match(_protocolPattern, interface.kind).group(2)
        ep = _protocolMap[protocolName]

        bitsSignalType = None
        if isinstance(ep.bitsSignalType, str):
            bitsSignalType = ep.bitsSignalType
        else:
            bitsSignalType = ep.bitsSignalType(interface)
        name = interface.name

        role = None
        if interface.role == "source":
            role = "Source"
        elif interface.role == "sink":
            role = "Sink"
        else:
            raise RuntimeError(f"Invalid interface role: {interface.role}")

        def publicBlock(d: codegen.Dumper) -> None:
            d.iwriteln(f"chext_test::elastic::{role}<{bitsSignalType}> {interface.name};")

        def ctorInitBlock(d: codegen.Dumper) -> None:
            clock = self.wrapper.getClock(interface.associatedClock)
            reset = self.wrapper.getReset(interface.associatedReset, True)
            d.iwrite(f"{name}(\"{name}\", {clock}, {reset})")
            d.writeln(",")

        def ctorBlock(d: codegen.Dumper) -> None:
            for (port, signal) in ep.portsToSignals:
                d.iwriteln(f"verilatorModule_.{name}_{port}(this->{name}.{signal});")
            d.separate()

        self._publicBlocks.append(publicBlock)
        self._ctorInitBlocks.append(ctorInitBlock)
        self._ctorBlocks.append(ctorBlock)


def registerElasticProtocol(ep: ElasticProtocol) -> None:
    acceptedKinds = [
        f"ReadyValid[{ep.protocolName}]",
        f"chext.elastic.Interface[{ep.protocolName}]",
        f"hdlinfo.elastic.Interface[{ep.protocolName}]"
    ]


    wrapper.registerInterfaceHandlerCustom(f"{ep.protocolName}_Handler")(ElasticProtocolHandler)
