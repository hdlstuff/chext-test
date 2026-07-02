# chext-test repo notes

These are working notes for future agent sessions. They describe the current
mental model of the repository so changes can start from shared context.

## What this repository is

`chext-test` is a SystemC-based test framework for Chisel/Chext projects. It has
two cooperating parts:

- A C++ library under `cpp/` with reusable SystemC testbench utilities.
- A Python package under `python/` that plugs into `hdlscw`/`hdlinfo` and emits
  C++ wrapper code for known HDL interface metadata.

The intended user flow is:

1. A generated or handwritten wrapper exposes a Verilated DUT as a SystemC
   module.
2. HDL interface metadata identifies clocks, resets, AXI interfaces, and
   elastic/ready-valid interfaces.
3. Python handlers generate C++ wrapper members and port bindings for
   `chext_test` drivers.
4. C++ testbenches drive transactions through higher-level APIs such as
   `send`, `receive`, `sendAW`, `receiveR`, and `sendB` instead of manually
   toggling valid/ready signals.

## Main C++ areas

- `cpp/include/chext_test/chext_test.hpp` is the umbrella include.
- `cpp/include/chext_test/TestBench.hpp` defines `TestBenchBase` and the
  `EXPECT_*` / `ASSERT_*` style macros.
- `cpp/include/chext_test/util/ReadyValid.hpp` implements the handshake timing
  used by elastic send/receive operations.
- `cpp/include/chext_test/elastic/Driver.hpp` defines generic ready-valid
  `Source` and `Sink` drivers.
- `cpp/include/chext_test/elastic/Convert.hpp` provides value conversion used
  by dynamic `sendAs*` / `receiveAs*` APIs.
- `cpp/include/chext_test/elastic/DataLast.hpp` defines a built-in
  `{ data, last }` elastic payload type.
- `cpp/include/chext_test/amba/axi4/full/` defines AXI4 full packet, signal,
  and driver abstractions.
- `cpp/include/chext_test/amba/axi4/lite/` defines AXI4-Lite packet, signal,
  and driver abstractions.
- `cpp/include/jqr/` is a local reflection-ish helper layer for dumping,
  equality, hashing, and formatting packet/value types.
- `cpp/src/util/hal.cpp` adapts the HAL sleep interface to `sc_core::wait`.

The C++ library is configured in `cpp/CMakeLists.txt` as a static library. It
depends on SystemC, fmt, and `hdlstuff_hal`.

## Main Python areas

- `python/setup.py` packages `chext_test` and depends on `hdlinfo` and `hdlscw`.
- `python/src/chext_test/common.py` has small codegen dumping helpers.
- `python/src/chext_test/elastic.py` registers elastic protocol handlers and
  generates wrapper-local payload/signal structs from `EncodedDataList`.
- `python/src/chext_test/axi4.py` registers the AXI4 interface handler and maps
  HDL port names to C++ driver signal fields.
- `python/src/chext_test/axi4s.py` is currently a placeholder.

The Python side is primarily code generation glue, not a simulator runtime.

## Examples and tests

- `cpp/examples/project1/src/ElasticModule.tb.cpp` shows a handwritten
  ready-valid testbench around a Verilated module.
- `cpp/examples/project1/src/AxiModule.tb.cpp` shows a handwritten AXI4 and
  AXI4-Lite testbench.
- `python/test/test.hpp` and `python/test/test.cpp` appear to be generated
  wrapper output used as examples/regression material.
- `cpp/test/test1.cpp` exercises `TestBenchBase`, packet formatting, and JQR
  comparisons.
- `cpp/test/test2.cpp` exercises the JQR helpers directly.
- `cpp/test/test3.cpp` only includes HAL and higher-level AXI headers.

## Concepts to keep straight

- In elastic interfaces, `source` and `sink` roles are from the wrapper/testbench
  perspective. The C++ type chosen is `chext_test::elastic::Source` for a role
  named `source` and `chext_test::elastic::Sink` for a role named `sink`.
- AXI full and lite drivers are made of per-channel elastic sources/sinks.
- Signal structs expose a `value_type`, plus `writeFrom` and/or `readTo`, so the
  generic elastic driver can move typed packets in and out.
- Zero-width elastic payloads are represented by `ZeroWidth` and
  `ZeroWidthSignals`.
- Generated wrapper code binds Verilated ports by convention:
  `interface_signal` or `interface_bits_field` names are mapped to nested driver
  signal members.

## Current rough edges noticed

- The top-level README is very small.
- Some generated test output in `python/test/test.cpp` has duplicate
  `source4` initialization/member-looking content, which may be deliberate test
  fixture output or an issue in code generation.
- `python/src/chext_test/axi4s.py` is unimplemented.
- There are duplicate macro definitions for `CHEXT_TEST_EXPECT_EQ`,
  `CHEXT_TEST_ASSERT_EQ`, and their short aliases in `TestBench.hpp`.
- The code generation in `elastic.py` has some repeated type-width logic that
  may be worth centralizing if we touch that area heavily.

## Working approach

Before changing behavior, inspect the local pattern in the matching C++ header
or Python handler. This repo favors small header-only C++ abstractions and
codegen hooks that append blocks into existing `hdlscw` wrapper generation.

Keep edits scoped. Avoid broad refactors unless we are intentionally improving a
shared abstraction such as elastic payload generation, AXI signal mapping, or
JQR formatting support.
