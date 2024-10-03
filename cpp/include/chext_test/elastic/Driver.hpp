#ifndef CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED
#define CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED

#include <chext_test/elastic/DataSignal.hpp>
#include <chext_test/util/ReadyValid.hpp>

#include <systemc>

#include <fmt/core.h>

namespace chext_test::elastic {

namespace detail {

using namespace sc_core;

/**
 * @brief SignalAdapter pokes/peeks a signal-like type.
 *
 * @tparam SignalType
 */
template<typename SignalType>
struct SignalAdapter {
    using value_type = typename SignalType::value_type;

    static void poke(SignalType& signal, ConstReference cref) {
        signal.write(cref.get<value_type>());
    }

    static void peek(SignalType const& signal, Reference ref) {
        ref.get<value_type>() = signal.read();
    }
};

template<
    typename DataSignalType,
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

    template<typename Packet>
    void send(Packet const& packet) {
        util::ReadyValid<PosEdgeClock, ActiveHighReset>::send(
            clock,
            reset,
            ready,
            valid,
            [&] { SignalAdapter<DataSignalType>::poke(data, packet); }
        );
    }

private:
    sc_in_clk& clock;
    sc_in<bool>& reset;

public:
    DataSignalType data;
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

template<
    typename DataSignalType,
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

    template<typename Packet>
    Packet receive() {
        Packet packet;

        util::ReadyValid<PosEdgeClock, ActiveHighReset>::receive(
            clock,
            reset,
            ready,
            valid,
            [&] { SignalAdapter<DataSignalType>::peek(data, packet); }
        );

        return packet;
    }

private:
    sc_in_clk& clock;
    sc_in<bool>& reset;

public:
    DataSignalType data;
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

} // namespace detail

using detail::Sink;
using detail::Source;

} // namespace chext_test::elastic

#endif /* CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED */
