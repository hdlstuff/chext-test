from typing import *
from dataclasses import dataclass, field
from hdlscw import codegen, wrapper
from functools import cached_property
from .common import *
import hdlinfo
import re

T = TypeVar("T")
__all__ = ["ElasticProtocol", "translate", "mark"]

_protocolMap: Dict[str, "ElasticProtocol"] = {}
_protocolPattern = r'^(chext\.elastic\.Interface|readyValid|hdlinfo\.elastic\.Interface)\[([\w\.-]+)\]$'


@hdlinfo.register_dataclass_adv("chext.util.EncodedDataEntry")
@dataclass(frozen=True)
class EncodedDataEntry:
    path: str
    width: int
    tpe: str


@hdlinfo.register_dataclass_adv("chext.util.EncodedData")
@dataclass(frozen=True)
class EncodedData:
    name: str
    entries: List[EncodedDataEntry]

    @cached_property
    def is_primitive(self):
        return len(self.entries) == 1 and self.entries[0].path == ""


@hdlinfo.register_dataclass_adv("chext.util.EncodedDataList")
@dataclass(frozen=True)
class EncodedDataList:
    seq: List[EncodedData]


@dataclass
class Marked(Generic[T]):
    obj: T
    props: Dict[str, Any]


def mark(t: T, **kwargs) -> Marked[T]:
    return Marked(t, kwargs)


def translate(t: str) -> Marked[str]:
    return mark(t, translate=True)


@dataclass(frozen=True)
class ElasticProtocol:
    protocolName: str
    includeStr: List[Union[str, Marked[str]]] = field(default_factory=list)
    bitsSignalType: Union[str, Callable[[hdlinfo.Interface], str]] = "bool"
    portsToSignals: List[Tuple[str, str]] = field(default_factory=list)

    def __post_init__(self) -> None:
        if self.protocolName in _protocolMap:
            raise RuntimeError(
                f"An elastic protocol with name '{self.protocolName}' already exists!")

        _protocolMap[self.protocolName] = self


@wrapper.registerHook
class WrapperLocalElasticProtocolHook(wrapper.Hook):
    @staticmethod
    def run(wrapper: wrapper.Wrapper, cg: codegen.CodeGen, module: hdlinfo.Module) -> bool:
        includeStr = wrapper.config.requestOption("chext_test.includeStr", lambda x: x, "<$>")

        def mkIncludeStr(s: str) -> str:
            return includeStr.replace("$", s)

        def hdrIncludeBlock(d: codegen.Dumper) -> None:
            d.iwriteln(
                "/* BEGIN: chext_test includes added by 'EncodedDataListHook' */"
            )
            d.iwriteln("#include <cstdint>")
            d.iwriteln("#include <fmt/core.h>")
            d.iwriteln(f"#include {mkIncludeStr('jqr/comp_eq.hpp')}")
            d.iwriteln(f"#include {mkIncludeStr('jqr/core.hpp')}")
            d.iwriteln(f"#include {mkIncludeStr('jqr/dump.hpp')}")
            d.iwriteln(
                "/* END: chext_test includes added by 'EncodedDataListHook' */"
            )
            d.separate()

        def processEncodedData(encodedData: EncodedData):
            if encodedData.is_primitive:
                return

            def createValueType():
                def f(d: codegen.Dumper) -> None:
                    d.separate()
                    d.iwriteln(f"class {encodedData.name}_value {{")
                    d.indent_in()

                    members: List[EncodedDataEntry] = []

                    for member in encodedData.entries:
                        if member.width == 0:
                            continue

                        members.append(member)

                        dataType = None

                        if member.width == 1:
                            dataType = "bool"
                        elif member.width <= 8:
                            dataType = "std::uint8_t"
                        elif member.width <= 16:
                            dataType = "std::uint16_t"
                        elif member.width <= 32:
                            dataType = "std::uint32_t"
                        elif member.width <= 64:
                            dataType = "std::uint64_t"
                        else:
                            dataType = f"sc_dt::sc_bv<{member.width}>"

                        d.iwriteln(
                            f"// {member.path}: {member.tpe}"
                        )
                        d.iwriteln(
                            f"{dataType} {member.path};"
                        )
                        d.separate()

                    d.iwriteln("JQR_DECL(")
                    d.indent_in()

                    d.iwrite(f"{encodedData.name}_value")
                    d.writeln(",")

                    for member in members:
                        d.iwrite(f"JQR_MEMBER({member.path})")
                        d.writeln(",")

                    d.unwrite()
                    d.writeln()

                    d.indent_out()
                    d.iwriteln(")")

                    d.separate()

                    d.iwriteln("JQR_TO_STRING")
                    d.iwriteln("JQR_OSTREAM")
                    d.iwriteln("JQR_COMP_EQ")

                    d.indent_out()
                    d.iwriteln(f"}};")

                cg.addPublicBlock(f)

            def createSignalsType():
                def f(d: codegen.Dumper) -> None:
                    d.separate()
                    d.iwriteln(f"class {encodedData.name}_signals {{")
                    d.indent_in()

                    d.iwriteln(f"using value_type = {encodedData.name}_value;")
                    d.separate()

                    members: List[EncodedDataEntry] = []

                    for member in encodedData.entries:
                        if member.width == 0:
                            continue

                        members.append(member)

                        dataType = "bool" if member.width == 1 else f"sc_dt::sc_bv<{member.width}>"
                        d.iwriteln(
                            f"// {member.path}: {member.tpe}"
                        )
                        d.iwriteln(
                            f"sc_core::sc_signal<{dataType}, sc_core::SC_MANY_WRITERS> {member.path};"
                        )
                        d.separate()

                    if len(members) > 0:
                        # constructor
                        d.iwriteln(
                            f"{encodedData.name}_signals(const char* name) :"
                        )
                        d.indent_in()

                        for member in members:
                            d.iwrite(
                                f'{member.path}(fmt::format("{{}}_{member.path}", name).c_str())')
                            d.writeln(",")

                        d.unwrite()
                        d.writeln(f" {{ /* empty ctor */ }}")
                        d.indent_out()
                        d.separate()

                        # read to
                        d.iwriteln(
                            f"void readTo(value_type & x) {{")
                        d.indent_in()

                        d.iwriteln(f"x.~value_type();")
                        d.separate()

                        d.iwriteln(f"new (&x) value_type {{")
                        d.indent_in()

                        for member in members:
                            if member.width <= 64:
                                dataType = None

                                if member.width == 1:
                                    dataType = "bool"
                                elif member.width <= 8:
                                    dataType = "std::uint8_t"
                                elif member.width <= 16:
                                    dataType = "std::uint16_t"
                                elif member.width <= 32:
                                    dataType = "std::uint32_t"
                                elif member.width <= 64:
                                    dataType = "std::uint64_t"

                                d.iwrite(
                                    f'static_cast<{dataType}>({member.path}.read().to_uint64())'
                                )
                            else:
                                d.iwrite(
                                    f'{member.path}.read()'
                                )

                            d.writeln(",")

                        d.unwrite()
                        d.writeln()

                        d.indent_out()
                        d.iwriteln(f"}};")

                        d.indent_out()
                        d.iwriteln(f"}}")
                        d.separate()

                        # write from
                        d.iwriteln(
                            f"void writeFrom(value_type const& x) {{")
                        d.indent_in()

                        for member in members:
                            d.iwriteln(
                                f"{member.path}.write(x.{member.path});")

                        d.indent_out()
                        d.iwriteln(f"}}")
                        d.separate()

                    else:
                        d.iwriteln(
                            f"{encodedData.name}_signals(const char*) /* no members */ {{ /* empty ctor */ }}"
                        )
                        d.separate()

                    d.indent_out()
                    d.iwriteln(f"}};")

                cg.addPublicBlock(f)

            createValueType()
            createSignalsType()

        if (encodedDataList := wrapper.module.args.get("encodedDataList")) is not None:
            assert isinstance(encodedDataList, EncodedDataList)

            cg.addHdrIncludeBlock(hdrIncludeBlock)
            for encodedData in encodedDataList.seq:
                processEncodedData(encodedData)

        return True


@dataclass(frozen=True)
class WrapperLocalElasticProtocol:
    protocolName: str
    signalsType: str
    signals: List[str]
    isPrimitive: bool


@wrapper.registerInterfaceHandler
class ElasticProtocolHandler(wrapper.StatefulInterfaceHandler):
    def __init__(self, wrapper: wrapper.Wrapper, cg: codegen.CodeGen) -> None:
        super().__init__(wrapper, cg)

        self._includeBlocks: List[codegen.Block] = []
        self._publicBlocks: List[codegen.Block] = []
        self._ctorInitBlocks: List[codegen.Block] = []
        self._ctorBlocks: List[codegen.Block] = []
        self._wrapperLocalElasticProtocols: Dict[str, WrapperLocalElasticProtocol] = {}

        self._includeStr = self.getOptionStr("chext_test.includeStr", "<$>")

        def hdrInclude(d: codegen.Dumper) -> None:
            d.iwriteln(f"/* BEGIN: chext_test includes for 'elastic' */")
            d.iwriteln(
                f"#include {self._mkIncludeStr('chext_test/elastic/Driver.hpp')}")
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

        def registerWrapperLocalElasticProtocol(encodedData: EncodedData):
            signals = []

            for entry in encodedData.entries:
                if entry.width > 0:
                    signals.append(entry.path)

            self._wrapperLocalElasticProtocols[encodedData.name] = WrapperLocalElasticProtocol(
                encodedData.name,
                f"{encodedData.name}_signals",
                signals,
                encodedData.is_primitive
            )

        if (encodedDataList := wrapper.module.args.get("encodedDataList")) is not None:
            assert isinstance(encodedDataList, EncodedDataList)

            for encodedData in encodedDataList.seq:
                registerWrapperLocalElasticProtocol(encodedData)

    def _mkIncludeStr(self, s: str) -> str:
        return self._includeStr.replace("$", s)

    @staticmethod
    def checkKind(kind: str) -> bool:
        match = re.match(_protocolPattern, kind)

        if match:
            return True
        else:
            return False

    def _implementElasticProtocol(self, interface: hdlinfo.Interface, protocol: ElasticProtocol) -> None:
        bitsSignalType = None
        if isinstance(protocol.bitsSignalType, str):
            bitsSignalType = protocol.bitsSignalType
        else:
            bitsSignalType = protocol.bitsSignalType(interface)
        name = interface.name

        role = None
        if interface.role == "source":
            role = "Source"
        elif interface.role == "sink":
            role = "Sink"
        else:
            raise RuntimeError(f"Invalid interface role: {interface.role}")

        if protocol.includeStr is not None:
            if isinstance(protocol.includeStr, Marked) and isinstance(protocol.includeStr.obj, str):
                if protocol.includeStr.props.get("translate", False):
                    self._includeBlocks.append(
                        f"#include {self._mkIncludeStr(protocol.includeStr.obj)}")
                else:
                    self._includeBlocks.append(f"#include {protocol.includeStr.obj}")
            elif isinstance(protocol.includeStr, str):
                self._includeBlocks.append(f"#include {protocol.includeStr}")
            else:
                raise RuntimeError(f"Invalid includeStr: {protocol.includeStr}.")

        def publicBlock(d: codegen.Dumper) -> None:
            d.iwriteln(
                f"chext_test::elastic::{role}<{bitsSignalType}> {interface.name};"
            )

        def ctorInitBlock(d: codegen.Dumper) -> None:
            clock = self.wrapper.getClock(interface.associatedClock)
            reset = self.wrapper.getReset(interface.associatedReset, True)
            d.iwrite(f"{name}(\"{name}\", {clock}, {reset})")
            d.writeln(",")

        def ctorBlock(d: codegen.Dumper) -> None:
            for (port, signal) in protocol.portsToSignals:
                d.iwriteln(
                    f"verilatedModule_.{name}_{port}(this->{name}.{signal});"
                )

            d.separate()

        self._publicBlocks.append(publicBlock)
        self._ctorInitBlocks.append(ctorInitBlock)
        self._ctorBlocks.append(ctorBlock)

    def _implementWrapperLocalElasticProtocol(self, interface: hdlinfo.Interface, protocol: WrapperLocalElasticProtocol) -> None:
        if protocol.isPrimitive:
            raise RuntimeError("WrapperLocalElasticProtocol shall not be a primitive!")

        bitsSignalType = protocol.signalsType
        name = interface.name

        role = None
        if interface.role == "source":
            role = "Source"
        elif interface.role == "sink":
            role = "Sink"
        else:
            raise RuntimeError(f"Invalid interface role: {interface.role}")

        def publicBlock(d: codegen.Dumper) -> None:
            d.iwriteln(
                f"chext_test::elastic::{role}<{bitsSignalType}> {interface.name};"
            )

        def ctorInitBlock(d: codegen.Dumper) -> None:
            clock = self.wrapper.getClock(interface.associatedClock)
            reset = self.wrapper.getReset(interface.associatedReset, True)
            d.iwrite(f"{name}(\"{name}\", {clock}, {reset})")
            d.writeln(",")

        def ctorBlock(d: codegen.Dumper) -> None:
            for signal in protocol.signals:
                d.iwriteln(
                    f"verilatedModule_.{name}_bits_{signal}(this->{name}.bits.{signal});"
                )
            d.iwriteln(f"verilatedModule_.{name}_ready(this->{name}.ready);")
            d.iwriteln(f"verilatedModule_.{name}_valid(this->{name}.valid);")

            d.separate()

        self._publicBlocks.append(publicBlock)
        self._ctorInitBlocks.append(ctorInitBlock)
        self._ctorBlocks.append(ctorBlock)

    def processInterface(self, interface: hdlinfo.Interface) -> None:
        protocolName = re.match(_protocolPattern, interface.kind).group(2)
        assert protocolName is not None

        protocol0 = _protocolMap.get(protocolName, None)
        protocol1 = self._wrapperLocalElasticProtocols.get(protocolName, None)

        if protocol0 is not None:
            return self._implementElasticProtocol(interface, protocol0)

        if protocol1 is not None:
            return self._implementWrapperLocalElasticProtocol(interface, protocol1)

        raise RuntimeError(f"ElasticProtocolHandler: no handler is registered for '{protocolName}'.")


def registerBasicElasticProtocols():
    def registerData():
        def signalType(interface: hdlinfo.Interface) -> str:
            if "width" not in interface.args:
                raise RuntimeError(
                    "chext.elastic.Data: interface missing argument 'width'.")

            width = interface.args["width"]

            if not isinstance(width, int):
                raise RuntimeError(
                    "chext.elastic.Data: interface argument 'width' must be an integer.")

            return f"sc_core::sc_signal<sc_dt::sc_bv<{width}>>"

        ElasticProtocol(
            "chext.elastic.Data",
            None,
            signalType,
            [
                ("bits", "bits"),
                ("ready", "ready"),
                ("valid", "valid")
            ]
        )

    def registerDataLast():
        def signalType(interface: hdlinfo.Interface) -> str:
            if "width" not in interface.args:
                raise RuntimeError(
                    "chext.elastic.Data: interface missing argument 'width'.")

            width = interface.args["width"]

            if not isinstance(width, int):
                raise RuntimeError(
                    "chext.elastic.Data: interface argument 'width' must be an integer.")

            return f"chext_test::elastic::DataLastSignals<{width}>"

        ElasticProtocol(
            "chext.elastic.DataLast",
            translate("chext_test/elastic/DataLast.hpp"),
            signalType,
            [
                ("bits_data", "bits.data"),
                ("bits_last", "bits.last"),
                ("ready", "ready"),
                ("valid", "valid")
            ]
        )

    registerData()
    registerDataLast()


registerBasicElasticProtocols()
