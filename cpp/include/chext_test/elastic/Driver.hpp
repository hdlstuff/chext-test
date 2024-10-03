#ifndef CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED
#define CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED

#include <chext_test/elastic/DataSignal.hpp>
#include <chext_test/util/ReadyValid.hpp>
#include <systemc>

namespace chext_test::elastic {

namespace detail {

using namespace sc_core;

struct Sender {
    Sender(
        std::string name,
        sc_in_clk& clock,
        sc_in<bool>& reset,
        DataSignalBase& data
    )
        : clock { clock }
        , reset { reset }
        , data { data }
        , ready { (name + "_ready").c_str() }
        , valid { (name + "_valid").c_str() } {
    }

    template<typename Packet>
    void send(Packet const& packet) {
        util::ReadyValid::send(
            clock,
            reset,
            ready,
            valid,
            [&] { data.poke(packet); }
        );
    }

private:
    sc_in_clk& clock;
    sc_in<bool>& reset;
    DataSignalBase& data;

public:
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

struct Receiver {
    Receiver(
        std::string name,
        sc_in_clk& clock,
        sc_in<bool>& reset,
        DataSignalBase& data
    )
        : clock { clock }
        , reset { reset }
        , data { data }
        , ready { (name + "_ready").c_str() }
        , valid { (name + "_valid").c_str() } {
    }

    template<typename Packet>
    Packet receive() {
        Packet p;

        util::ReadyValid::receive(
            clock,
            reset,
            ready,
            valid,
            [&] { data.peek(p); }
        );

        return p;
    }

private:
    sc_in_clk& clock;
    sc_in<bool>& reset;
    DataSignalBase& data;

public:
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

} // namespace detail

using detail::Receiver;
using detail::Sender;

} // namespace chext_test::elastic

#endif /* CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED */
