#ifndef CHEXT_TEST_UTIL_READYVALID_HPP_INCLUDED
#define CHEXT_TEST_UTIL_READYVALID_HPP_INCLUDED

#include <chext_test/util/Exception.hpp>
#include <systemc>

namespace chext_test::util {

struct ReadyValid {
    template<
        typename ClockT,
        typename ResetT,
        typename ReadyT,
        typename ValidT,
        typename PokeDataFn //
        >
    static inline void send(
        ClockT const& clock,
        ResetT const& reset,
        ReadyT const& ready,
        ValidT& valid,
        PokeDataFn pokeDataFn
    ) {
        while (clock.read() || reset.read())
            sc_core::wait(clock.negedge_event());

        // we attempt sending the packet at negedge
        pokeDataFn();
        valid.write(true);

        do {
            sc_core::wait(reset.posedge_event() | clock.posedge_event());
        } while (!ready.read() || reset.read());

        if (reset.read())
            throw Exception("Reset asserted during transmission!");

        sc_core::wait(clock.negedge_event());

        // we stop asserting the valid at negedge
        valid.write(false);
    }

    template<
        typename ClockT,
        typename ResetT,
        typename ReadyT,
        typename ValidT,
        typename PeekDataFn //
        >
    static inline void receive(
        ClockT const& clock,
        ResetT const& reset,
        ReadyT& ready,
        ValidT const& valid,
        PeekDataFn peekDataFn
    ) {
        while (clock.read() || reset.read())
            sc_core::wait(clock.negedge_event());

        ready.write(true);

        do {
            sc_core::wait(reset.posedge_event() | clock.posedge_event());
        } while (!valid.read() || reset.read());

        if (reset.read())
            throw Exception("Reset asserted during transmission!");

        peekDataFn();

        sc_core::wait(clock.negedge_event());
        ready.write(false);
    }
};

}; // namespace chext_test::util

#endif /* CHEXT_TEST_UTIL_READYVALID_HPP_INCLUDED */
