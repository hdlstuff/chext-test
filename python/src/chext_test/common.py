from hdlscw import wrapper
from hdlscw import codegen

import hdlinfo
import abc
from typing import *

T = TypeVar('T')


def dumpBlockList(d: codegen.Dumper, l: List[codegen.Block]) -> None:
    for x in l:
        if isinstance(x, str):
            d.iwriteln(x)
        else:
            x(d)


class StatefulInterfaceHandler(abc.ABC):
    def __init__(self, wrapper: wrapper.Wrapper, cg: codegen.CodeGen) -> None:
        self._wrapper = wrapper
        self._cg = cg

    @property
    def wrapper(self) -> wrapper.Wrapper:
        return self._wrapper

    @property
    def cg(self) -> codegen.CodeGen:
        return self._cg

    def getOption(self, name: str, t: Callable[[str], T], default: T) -> T:
        return self.wrapper.config.requestOption(name, t, default)

    def getOptionStr(self, name: str, default: str) -> str:
        return self.getOption(name, str, default)

    def getOptionInt(self, name: str, default: int) -> int:
        return self.getOption(name, int, default)

    @staticmethod
    @abc.abstractmethod
    def checkKind(kind: str) -> bool:
        return False

    @abc.abstractmethod
    def processInterface(self, interface: hdlinfo.Interface) -> None:
        pass


def transformStatefulInterfaceHandler(t: Type[StatefulInterfaceHandler]) -> Type[wrapper.InterfaceHandler]:
    attrName = f"_transformStatefulInterfaceHandler_{t.__qualname__.replace(".", "_")}"

    class StatelessInterfaceHandler(wrapper.InterfaceHandler):
        @staticmethod
        def processInterface(wrapper: wrapper.Wrapper, cg: codegen.CodeGen, interface: hdlinfo.Interface) -> bool:
            if not t.checkKind(interface.kind):
                return False

            o: StatefulInterfaceHandler = wrapper.getAttr(attrName)

            if o is None:
                o = wrapper.setAttr(attrName, t(wrapper, cg))

            o.processInterface(interface)
            return True

    return StatelessInterfaceHandler
