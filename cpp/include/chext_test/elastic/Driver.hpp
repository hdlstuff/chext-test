#ifndef CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED
#define CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED

#include <chext_test/util/ReadyValid.hpp>

#include <systemc>

#include <fmt/core.h>

namespace chext_test::elastic {

namespace detail {

using namespace sc_core;

/**
 * @brief SignalDataMapper pokes/peeks a signal-like type.
 *
 * @note I define a signal-like type as the following:
 *
 * - `sc_signal<T>`
 *
 * - a struct that consists of members with signal-like types and has a `const char*` constructor
 *
 * - supports `.read()` and `.write()`
 *
 * - Has `::value_type`.
 *
 * Therefore, the following types are signal-like:
 *
 * - `sc_signal<int>`
 *
 * - `struct Bundle { sc_signal<int> b1, b2; };`
 *
 * - `struct Bundle { sc_signal<int> a; struct { sc_signal<int> b1, b2; } b; };`
 *
 * @tparam SignalT
 */

template<
    typename BitsSignalT,
    bool PosEdgeClock = true,
    bool ActiveHighReset = true>
struct Sink {
    using BitsValueT = typename BitsSignalT::value_type;

    Sink(
        std::string name,
        sc_in_clk const& clock,
        sc_in<bool> const& reset
    )
        : clock { clock }
        , reset { reset }
        , bits { fmt::format("{}_bits", name).c_str() }
        , ready { fmt::format("{}_ready", name).c_str() }
        , valid { fmt::format("{}_valid", name).c_str() } {
    }

    BitsValueT receive() {
        BitsValueT x;

        util::ReadyValid<PosEdgeClock, ActiveHighReset>::receive(
            clock,
            reset,
            ready,
            valid,
            [&] { x = bits.read(); }
        );

        return x;
    }

private:
    sc_in_clk const& clock;
    sc_in<bool> const& reset;

public:
    BitsSignalT bits;
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

template<
    typename BitsSignalT,
    bool PosEdgeClock = true,
    bool ActiveHighReset = true>
struct Source {
    using BitsValueT = typename BitsSignalT::value_type;

    Source(
        std::string name,
        sc_in_clk const& clock,
        sc_in<bool> const& reset
    )
        : clock { clock }
        , reset { reset }
        , bits { fmt::format("{}_bits", name).c_str() }
        , ready { fmt::format("{}_ready", name).c_str() }
        , valid { fmt::format("{}_valid", name).c_str() } {
    }

    void send(BitsValueT const& x) {
        util::ReadyValid<PosEdgeClock, ActiveHighReset>::send(
            clock,
            reset,
            ready,
            valid,
            [&] { bits.write(x); }
        );
    }

private:
    sc_in_clk const& clock;
    sc_in<bool> const& reset;

public:
    BitsSignalT bits;
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

} // namespace detail

using detail::Sink;
using detail::Source;

} // namespace chext_test::elastic

#endif /* CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED */
