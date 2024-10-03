#ifndef CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED
#define CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED

#include <chext_test/util/ReadyValid.hpp>
#include <chext_test/util/Reference.hpp>

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
 * - a default-constructable struct that consists of members with signal-like types
 * 
 * Therefore, the following types are signal-like:
 * 
 * - `sc_signal<int>`
 * 
 * - `struct Bundle { sc_signal<int> b1, b2; };`
 * 
 * - `struct Bundle { sc_signal<int> a; struct { sc_signal<int> b1, b2; } b; };`
 *
 * @tparam SignalType
 */
template<typename SignalType, typename DataType>
struct SignalDataMapper {
    using value_type = typename SignalType::value_type;

    static void poke(SignalType& signal, DataType const& data) {
        signal.write(data);
    }

    static void peek(SignalType const& signal, DataType& data) {
        data = signal.read();
    }
};

template<
    typename BitsSignal,
    bool PosEdgeClock = true,
    bool ActiveHighReset = true>
struct Source {
    Source(
        std::string name,
        sc_in_clk& clock,
        sc_in<bool>& reset
    )
        : clock { clock }
        , reset { reset }
        , ready { fmt::format("{}_ready", name).c_str() }
        , valid { fmt::format("{}_valid", name).c_str() } {
    }

    template<typename BitsData>
    void send(BitsData const& x) {
        util::ReadyValid<PosEdgeClock, ActiveHighReset>::send(
            clock,
            reset,
            ready,
            valid,
            [&] { SignalDataMapper<BitsSignal, BitsData>::poke(bits, x); }
        );
    }

private:
    sc_in_clk& clock;
    sc_in<bool>& reset;

public:
    BitsSignal bits;
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

template<
    typename BitsSignal,
    bool PosEdgeClock = true,
    bool ActiveHighReset = true>
struct Sink {
    Sink(
        std::string name,
        sc_in_clk& clock,
        sc_in<bool>& reset
    )
        : clock { clock }
        , reset { reset }
        , ready { fmt::format("{}_ready", name).c_str() }
        , valid { fmt::format("{}_valid", name).c_str() } {
    }

    template<typename BitsData>
    BitsData receive() {
        BitsData x;

        util::ReadyValid<PosEdgeClock, ActiveHighReset>::receive(
            clock,
            reset,
            ready,
            valid,
            [&] { SignalDataMapper<BitsSignal, BitsData>::peek(bits, x); }
        );

        return x;
    }

private:
    sc_in_clk& clock;
    sc_in<bool>& reset;

public:
    BitsSignal bits;
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

} // namespace detail

using detail::Sink;
using detail::Source;

} // namespace chext_test::elastic

#endif /* CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED */
