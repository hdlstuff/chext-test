# chext-test C++ Testbench API

This document describes the C++/SystemC testbench API provided by `chext-test`.
It is aimed at chext-generated or Verilated DUTs that expose ready/valid,
AXI4, AXI4-Lite, or AXI4-Stream interfaces.

The public umbrella header is:

```cpp
#include <chext_test/chext_test.hpp>
```

For smaller builds, include only the protocol headers you use, such as
`<chext_test/elastic/Driver.hpp>`,
`<chext_test/amba/axi4/full/Driver.hpp>`, or
`<chext_test/util/Spawn.hpp>`.

## Build Setup

When `chext_test` is installed, CMake exports the target `chext::chext_test`.
A typical Verilator/SystemC testbench executable looks like this:

```cmake
cmake_minimum_required(VERSION 3.28)
project(my_tb)

add_compile_definitions(SC_DISABLE_API_VERSION_CHECK)

find_package(SystemCLanguage REQUIRED)
find_package(verilator REQUIRED)
find_package(fmt REQUIRED)
find_package(chext_test REQUIRED)

add_executable(MyModule.tb src/MyModule.tb.cpp)
target_link_libraries(
    MyModule.tb
    PUBLIC
    SystemC::systemc
    fmt::fmt
    chext::chext_test
)

verilate(
    MyModule.tb
    SYSTEMC
    SOURCES hdl/MyModule.sv
    VERILATOR_ARGS --trace
)
```

See also `cpp/examples/project1/CMakeLists.txt`.

## Testbench Harness

`chext_test::TestBenchBase` is the simplest harness for single-test executables.
Subclass it, override `entry()`, and call `finish()` when the test is complete.

```cpp
#include <chext_test/chext_test.hpp>

using namespace sc_core;
using namespace chext_test;

class MyTestbench : public TestBenchBase {
public:
    MyTestbench()
        : TestBenchBase(sc_module_name("tb"))
        , clock("clock", 2.0, SC_NS)
        , reset("reset") {
        dut.clock(clock);
        dut.reset(reset);
    }

private:
    VMyModule dut { "dut" };
    sc_clock clock;
    sc_signal<bool> reset;

    void entry() override {
        resetDut();

        EXPECT_EQ(1, 1);
        ASSERT_(true);

        finish();
    }

    void resetDut() {
        wait(clock.negedge_event());
        reset.write(true);
        wait(clock.negedge_event());
        wait(clock.negedge_event());
        reset.write(false);
        wait(clock.negedge_event());
    }
};

int sc_main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    MyTestbench tb;
    tb.start();
    return 0;
}
```

`tb.start(duration)` repeatedly calls `sc_start(duration)` until `finish()` or a
failing `ASSERT_*` stops the simulation. The default step is `50 ns`.

Available assertions:

| Macro | Behavior |
| --- | --- |
| `EXPECT_(x)` / `CHEXT_TEST_EXPECT(x)` | Report failure and continue. |
| `ASSERT_(x)` / `CHEXT_TEST_ASSERT(x)` | Report failure and stop. |
| `EXPECT_EQ`, `EXPECT_NE`, `EXPECT_LT`, `EXPECT_LE`, `EXPECT_GT`, `EXPECT_GE` | Binary expectations. |
| `ASSERT_EQ`, `ASSERT_NE`, `ASSERT_LT`, `ASSERT_LE`, `ASSERT_GT`, `ASSERT_GE` | Binary assertions. |

Define `CHEXT_TEST_NO_SHORT_MACROS` before including the headers if you only
want the `CHEXT_TEST_*` macro names.

## Concurrency

Many bus operations must happen concurrently. `chext-test` provides two helper
macros over `sc_spawn`:

```cpp
sc_join j;

SC_SPAWN_TO(j) {
    dut.source.send(0x1234);
};

SC_SPAWN_TO(j) {
    auto value = dut.sink.receive();
    EXPECT_EQ(value, 0x1234u);
};

j.wait();
```

Use `SC_SPAWN { ... };` when you do not need to join the spawned process.

## Elastic Ready/Valid

Elastic drivers model a `bits`, `ready`, `valid` channel.

```cpp
using chext_test::elastic::Source;
using chext_test::elastic::Sink;

Source<chext_test::vutil::signal_t<32, SC_MANY_WRITERS>> source {
    "source", clock, reset
};
Sink<chext_test::vutil::signal_t<32, SC_MANY_WRITERS>> sink {
    "sink", clock, reset
};
```

Bind the generated signals to DUT ports:

```cpp
dut.source_bits(source.bits);
dut.source_ready(source.ready);
dut.source_valid(source.valid);

dut.sink_bits(sink.bits);
dut.sink_ready(sink.ready);
dut.sink_valid(sink.valid);
```

Then send and receive packets:

```cpp
source.send(0x55u);
auto value = sink.receive();

source.sendAsUInt64(0x100);
source.sendAsString("0d98"); // parsed as hexadecimal
auto received = sink.receiveAsUInt64();
```

`Source<T>` drives `valid` and waits for `ready`. `Sink<T>` drives `ready` and
waits for `valid`. By default the handshake samples on the positive clock edge
with active-high reset. The optional template parameters are:

```cpp
Source<BitsSignalT, PosEdgeClock, ActiveHighReset>
Sink<BitsSignalT, PosEdgeClock, ActiveHighReset>
```

Both `SourceBase` and `SinkBase` support `timeoutCycles(cycles)`. A nonzero
timeout throws `chext_test::util::Exception` if the peer does not handshake.

For structured payloads, the signal bundle type must provide:

```cpp
using value_type = MyPacket;
void writeFrom(MyPacket const&);
void readTo(MyPacket&) const;
```

`chext_test::elastic::DataLastSignals<W>` is a ready-made example for a
`{ data, last }` payload.

## Verilator Signal Widths

`chext_test::vutil::signal_t<W>` chooses the C++ payload type that Verilator
normally exposes for a port of width `W`:

| Width | Payload type |
| --- | --- |
| `1` | `bool` |
| `2..32` | `std::uint32_t` |
| `33..64` | `std::uint64_t` |
| wider, or affected by `--pins-bv` | `sc_dt::sc_bv<W>` |

Use `vutil::read<W>(signal, value)` and `vutil::write<W>(signal, value)` if you
need explicit conversion between Verilator payloads and SystemC bit vectors.

## AXI4 Full

The AXI4 full driver namespace is:

```cpp
namespace axi4f = chext_test::amba::axi4::full;
```

Instantiate by bus widths:

```cpp
axi4f::Slave<8, 32, 64> s_axi { "s_axi", clock, reset };
axi4f::Master<8, 32, 64> m_axi { "m_axi", clock, reset };
```

The names describe the DUT interface being connected:

| DUT port type | Testbench driver | Common operations |
| --- | --- | --- |
| DUT AXI slave port | `axi4::full::Slave` | `sendAW`, `sendW`, `receiveB`, `sendAR`, `receiveR` |
| DUT AXI master port | `axi4::full::Master` | `receiveAW`, `receiveW`, `sendB`, `receiveAR`, `sendR` |

Template parameters:

```cpp
Slave<
    ID_WIDTH,
    ADDR_WIDTH,
    DATA_WIDTH,
    READ = true,
    WRITE = true,
    HAS_LOCK = true,
    HAS_CACHE = true,
    HAS_PROT = true,
    HAS_QOS = true,
    HAS_REGION = true,
    AXI3_COMPAT = false,
    ARUSER_WIDTH = 0,
    RUSER_WIDTH = 0,
    AWUSER_WIDTH = 0,
    WUSER_WIDTH = 0,
    BUSER_WIDTH = 0>
```

Construct packets with `make*()` so bit-vector fields have the right widths:

```cpp
auto aw = s_axi.makeAW();
aw.id = 0x12;
aw.addr = 0x1000;
aw.len = 3;    // four beats
aw.size = 3;   // 8 bytes per beat
aw.burst = 1;  // INCR

s_axi.sendAW(aw);

for (int i = 0; i < 4; ++i) {
    auto w = s_axi.makeW();
    w.data = 0x1000 + i;
    w.strb = 0xff;
    w.last = i == 3;
    s_axi.sendW(w);
}

auto b = s_axi.receiveB();
EXPECT_EQ(b.resp, 0);
```

For buffer-style transactions, include
`<chext_test/amba/axi4/full/Transaction.hpp>`:

```cpp
std::array<std::uint8_t, 256> wr {};
std::array<std::uint8_t, 256> rd {};

chext_test::amba::axi4::full::write(s_axi, 0x4000, wr.size(), wr.data());
chext_test::amba::axi4::full::read(s_axi, 0x4000, rd.size(), rd.data());
EXPECT_(rd == wr);
```

The overloads accept an optional transfer-size log2 and logging flag:

```cpp
write(s_axi, addr, numBytes, data, sizeLog2, log);
read(s_axi, addr, numBytes, data, sizeLog2, log);
```

Use `sizeLog2 = -1` for the widest legal transfer size.

## AXI4-Lite

The AXI4-Lite API mirrors AXI4 full, but only takes address and data widths:

```cpp
namespace axi4l = chext_test::amba::axi4::lite;

axi4l::Slave<12, 32> regs { "regs", clock, reset };

auto aw = regs.makeAW();
aw.addr = 0x20;
regs.sendAW(aw);

auto w = regs.makeW();
w.data = 0xdeadbeef;
w.strb = 0xf;
regs.sendW(w);

auto b = regs.receiveB();
EXPECT_EQ(b.resp, 0);
```

`axi4::lite::Master<ADDR_WIDTH, DATA_WIDTH>` is used when the DUT has an
AXI4-Lite master port.

## AXI4-Stream

The AXI4-Stream namespace is:

```cpp
namespace axis = chext_test::amba::axi4s;
```

As with AXI4, `Master` and `Slave` describe the DUT-side interface:

```cpp
axis::Slave<64> stream_in { "stream_in", clock, reset };   // drive DUT input
axis::Master<64> stream_out { "stream_out", clock, reset }; // observe DUT output
```

Template parameters:

```cpp
Master<
    DATA_WIDTH,
    ID_WIDTH = 0,
    DEST_WIDTH = 0,
    USER_WIDTH = 0,
    HAS_READY = true,
    HAS_STROBE = true,
    HAS_KEEP = true,
    HAS_LAST = true>
```

Send or receive `axi4s::Packet` values:

```cpp
auto p = stream_in.makePacket();
p.data = 0x1122334455667788ULL;
p.keep = 0xff;
p.strb = 0xff;
p.last = true;

stream_in.send(p);

auto q = stream_out.receive();
EXPECT_EQ(q.last, true);
```

Optional fields are present only when the corresponding width or `HAS_*`
parameter enables them. If `HAS_READY` is false on an AXI4-Stream slave driver,
the driver holds its internal `ready` signal true.

## Complete Elastic Example

This example shows the usual pattern: wrap the Verilated DUT, bind protocol
drivers to ports, reset, spawn concurrent producers/consumers, and finish.

```cpp
#include <chext_test/chext_test.hpp>
#include <VElasticModule.h>

using namespace sc_core;
using namespace chext_test;

struct ElasticDut : sc_module {
    sc_in_clk clock;
    sc_in<bool> reset;

    elastic::Source<vutil::signal_t<32, SC_MANY_WRITERS>> in {
        "in", clock, reset
    };
    elastic::Sink<vutil::signal_t<32, SC_MANY_WRITERS>> out {
        "out", clock, reset
    };

    VElasticModule dut { "dut" };

    ElasticDut(sc_module_name const& name)
        : sc_module(name)
        , clock("clock")
        , reset("reset") {
        dut.clock(clock);
        dut.reset(reset);

        dut.in_bits(in.bits);
        dut.in_ready(in.ready);
        dut.in_valid(in.valid);

        dut.out_bits(out.bits);
        dut.out_ready(out.ready);
        dut.out_valid(out.valid);
    }
};

class Tb : public TestBenchBase {
public:
    Tb()
        : TestBenchBase(sc_module_name("tb"))
        , dut("wrapped_dut")
        , clock("clock", 2.0, SC_NS)
        , reset("reset") {
        dut.clock(clock);
        dut.reset(reset);
    }

private:
    ElasticDut dut;
    sc_clock clock;
    sc_signal<bool> reset;

    void entry() override {
        resetDut();

        sc_join j;

        SC_SPAWN_TO(j) {
            for (std::uint32_t i = 0; i < 16; ++i)
                dut.in.send(i);
        };

        SC_SPAWN_TO(j) {
            for (std::uint32_t i = 0; i < 16; ++i) {
                auto got = dut.out.receive();
                EXPECT_EQ(got, i);
            }
        };

        j.wait();
        finish();
    }

    void resetDut() {
        wait(clock.negedge_event());
        reset.write(true);
        wait(clock.negedge_event());
        wait(clock.negedge_event());
        reset.write(false);
        wait(clock.negedge_event());
    }
};

int sc_main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Tb tb;
    tb.start();

    return 0;
}
```

For more complete, compiling examples, see:

- `cpp/examples/project1/src/ElasticModule.tb.cpp`
- `cpp/examples/project1/src/AxiModule.tb.cpp`
- `../chext/sysc_tb/chext/amba/axi4/full/components/src/AddressGenerator.tb.cpp`
