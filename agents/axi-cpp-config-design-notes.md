# AXI C++ config alignment notes

These notes summarize the AXI C++ support design discussion for future
implementation work.

## Goal

Align the C++ test/support side with the current Chext/Chisel AXI config model,
while keeping the SystemC and Verilator constraints explicit.

Chext AXI4 config field order should be preserved in generated C++ template
parameters where possible:

```text
wId, wAddr, wData,
read, write, lite,
hasLock, hasCache, hasProt, hasQos, hasRegion,
axi3Compat,
wUserAR, wUserR, wUserAW, wUserW, wUserB
```

This order matters because these types are mainly consumed from generated code.

Derived AXI4 values should match Chext:

```text
wStrobe = wData / 8
wLen = axi3Compat ? 4 : 8
wLock = hasLock ? (axi3Compat ? 2 : 1) : 0
wCache = hasCache ? 4 : 0
wProt = hasProt ? 3 : 0
wQos = hasQos ? 4 : 0
wRegion = hasRegion ? 4 : 0
```

AXI4-Lite can remain a separate C++ namespace/type for now, but should expose
Chext-compatible config metadata such as `lite = true` and `wStrobe`.

AXI4S is currently absent in C++ and Python wrapper support. When added, it
should use the same principles: Chext field order, derived widths, optional
ports, and no zero-width SystemC signals.

## Config ownership

`Signals` should expose the canonical compile-time config as:

```cpp
static constexpr Config config;
```

Use the full word `config`, not `cfg`.

Concrete `Master`/`Slave` classes should pass `SignalsT::config` into their
base class constructor. The base class should own a public immutable runtime
copy:

```cpp
struct MasterBase {
    explicit MasterBase(Config config)
        : config { config } {}

    const Config config;
};
```

This avoids a virtual `config()` function. The vtable then only contains
behavioral driver operations, and testbenches can access metadata directly:

```cpp
axi.config
axi.config.makeAW()
```

The duplication between `SignalsT::config` and `MasterBase::config` is
intentional: `SignalsT::config` is the compile-time source of truth, and the
base-owned `const Config config` is the runtime/public view. `Config` is plain
width/boolean metadata, so this copy is cheap.

Because the base contains a `const` member, driver objects become non-assignable.
That is acceptable because SystemC driver objects contain signals and should not
be assignable anyway.

## Zero-width and optional ports

Do not instantiate `sc_bv<0>`.

Optional AXI fields such as lock/cache/prot/qos/region/user should have logical
widths that may be zero in `Config`, but the SystemC signal layer should avoid
creating zero-width signals. The preferred direction is to represent optional
generated ports with optional/non-instantiated signal members and have generated
wrapper code bind only signals listed by HDLINFO/Chext metadata.

This keeps the logical config faithful to Chext while avoiding undefined or
tool-specific SystemC zero-width behavior.

## Verilator port type mapping

The existing `bv_bool_t` helper exists because Verilator maps some port widths
to nice native C++/SystemC types depending on options. Replace that local
special case with a general width-to-type trait.

Use a compile-time value matching Verilator `--pins-bv`:

```cpp
CHEXT_TEST_VERILATOR_PINS_BV_VALUE
```

Default should likely be `65`, giving ordinary scalar mapping up to 64 bits:

```text
1      -> bool
2..32  -> uint32_t
33..64 -> uint64_t
65+    -> sc_dt::sc_bv<W>
```

If Verilator is invoked with `--pins-bv 2`, CMake should also define:

```cpp
CHEXT_TEST_VERILATOR_PINS_BV_VALUE=2
```

Then width 1 remains `bool`, and width 2+ maps to `sc_bv<W>`.

## Packet construction and sc_bv_base

Current AXI packets store dynamic bit vectors as `sc_dt::sc_bv_base`. This is
dangerous when packets are default-constructed, because `sc_bv_base` defaults
to width 32. Normal assignment into an existing `sc_bv_base` does not resize it,
which can cause stale-width aliasing and data corruption.

We considered a wrapper type that resizes/reconstructs on assignment, but
rejected it as too magical. The preferred design is to make packet widths part
of construction.

AXI packets should be created from config:

```cpp
auto ar = axi.makeAR();
auto aw = axi.makeAW();
auto w = axi.makeW();
auto r = axi.makeR();
auto b = axi.makeB();
```

and equivalently:

```cpp
auto aw = axi.config.makeAW();
```

`Config` should provide the actual factories. `MasterBase` and `SlaveBase` can
provide non-virtual convenience methods that simply call `config.makeAW()` and
friends. These do not need to be virtual because `config` is a public immutable
base member.

Use separate `ReadAddress` and `WriteAddress` packet structs instead of a shared
address type that takes a boolean write/read argument. Separate concrete types
are clearer and avoid ambiguous calls such as `Address(config, true)`.
Note that the base class of Read/Write Address must not be instantiated, make it private in a detail namespace.

## Packet special member functions

AXI packet default construction should not be available from user code. A packet
without config-derived field widths violates the packet invariant.

Packet copy assignment should be deleted because it would assign into existing
`sc_bv_base` fields, which do not resize. That is the operation that can
silently corrupt data.

Packet copy construction is acceptable. The Accellera SystemC reference
implementation of `sc_bv_base` copy construction copies `m_len` and `m_size`
from the source object before copying data, so it preserves width. Assignment
does not reinitialize those fields.

Move support should be avoided. The visible Accellera `sc_bv_base` and
`sc_bv<W>` declarations provide copy constructors and assignment operators, but
do not show explicit move constructors. Prefer old-C++-style copy semantics.

Recommended packet shape:

```cpp
struct WriteData {
    explicit WriteData(Config const& config);

    WriteData(WriteData const&) = default;
    WriteData& operator=(WriteData const&) = delete;

    // Do not declare move constructor or move assignment. Declaring the copy
    // operations suppresses implicit move generation, and rvalues can still
    // bind to the const-copy constructor.
};
```

Do not explicitly delete the move constructor unless rvalue construction should
fail. If `WriteData(WriteData&&) = delete` is declared, calls like
`vector.push_back(config.makeW())` may select the deleted move constructor
instead of copying.

### Container consequences

Non-default-constructible, non-assignable packet types are still usable in
standard containers as long as they are copy-constructible.

Works:

```cpp
std::vector<Packet> v;
v.push_back(config.makeAW());
v.emplace_back(config.makeAW());
std::vector<Packet> ten(10, config.makeAW());
```

Does not work:

```cpp
std::vector<Packet> ten(10); // needs Packet()
v[0] = config.makeAW();      // needs assignment
std::fill(v.begin(), v.end(), config.makeAW());
```

Workarounds include construction-oriented APIs, `reserve` plus `push_back`, and
`std::optional<Packet>` for fixed slots that need delayed construction.

## AXI driver receive path

Do not complicate the generic elastic driver with a `receiveWith` helper.

AXI concrete receive methods should construct packets from config and then use
`receiveTo`:

```cpp
PacketsT::WriteAddress receiveAW() override {
    auto packet = SignalsT::config.makeAW();
    aw.receiveTo(packet);
    return packet;
}
```

Direct use of the raw elastic channel's `receive()` on AXI packet types can be
unsupported if packets are not default-constructible. The public AXI driver API
still remains ergonomic and safe.

## Runtime validation

Factories guide testbenches toward correct packet construction, but runtime
width checks at AXI signal boundaries are still useful. `writeFrom`/`readTo`
should detect malformed or legacy packets and fail loudly instead of silently
truncating or corrupting data.

For required fields, validate exact expected width. For logically absent
optional fields, prefer accepting only zero/default values and do not bind a
real generated port.
