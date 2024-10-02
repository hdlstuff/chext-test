#ifndef CHEXT_TEST_ELASTIC_RECEIVER_HPP_INCLUDED
#define CHEXT_TEST_ELASTIC_RECEIVER_HPP_INCLUDED

#include <chext_test/elastic/DataSignal.hpp>
#include <chext_test/util/Exception.hpp>
#include <systemc>

namespace chext_test::elastic {

namespace detail {

using namespace sc_core;

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

        while (clock.read() || reset.read())
            wait(clock.negedge_event());

        ready.write(true);

        do {
            wait(reset.posedge_event() | clock.posedge_event());
        } while (!valid.read() || reset.read());

        if (reset.read())
            throw util::Exception("Reset asserted during transmission!");

        data.peek(Reference { p });

        wait(clock.negedge_event());
        ready.write(false);

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

} // namespace chext_test::elastic

#endif /* CHEXT_TEST_ELASTIC_RECEIVER_HPP_INCLUDED */
