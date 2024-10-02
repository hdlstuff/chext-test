#ifndef CHEXT_TEST_ELASTIC_SENDER_HPP_INCLUDED
#define CHEXT_TEST_ELASTIC_SENDER_HPP_INCLUDED

#include <chext_test/elastic/DataSignal.hpp>
#include <chext_test/util/Exception.hpp>
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
        while (clock.read() || reset.read())
            wait(clock.negedge_event());

        // we attempt sending the packet at negedge
        data.poke(packet);
        valid.write(true);

        do {
            wait(reset.posedge_event() | clock.posedge_event());
        } while (!ready.read() || reset.read());

        if (reset.read())
            throw util::Exception("Reset asserted during transmission!");

        wait(clock.negedge_event());

        // we stop asserting the valid at negedge
        valid.write(false);
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

using detail::Sender;

} // namespace chext_test::elastic

#endif /* CHEXT_TEST_ELASTIC_SENDER_HPP_INCLUDED */
