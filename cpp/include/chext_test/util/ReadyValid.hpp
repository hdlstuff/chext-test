#ifndef CHEXT_TEST_UTIL_READYVALID_HPP_INCLUDED
#define CHEXT_TEST_UTIL_READYVALID_HPP_INCLUDED

#include <chext_test/util/BoolWrapper.hpp>
#include <chext_test/util/Exception.hpp>
#include <fmt/core.h>

#include <cstdint>
#include <string_view>
#include <systemc>

namespace chext_test::util {

template<bool PosEdgeClock = true, bool ActiveHighReset = true>
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
        PokeDataFn pokeDataFn,
        std::string_view channelName,
        std::uint64_t timeoutCycles
    ) {
        auto clock_ = constBoolWrapper<!PosEdgeClock>(clock);
        auto reset_ = constBoolWrapper<!ActiveHighReset>(reset);
        std::uint64_t waitedCycles = 0;

        // wait until all the transitions happen
        sc_core::wait(sc_core::SC_ZERO_TIME);

        // if the clock is high, it is after a posedge
        while (clock_.read() || reset_.read())
            sc_core::wait(clock_.negedge_event());

        // we attempt sending the packet at negedge
        pokeDataFn();
        valid.write(true);

        do {
            sc_core::wait(reset_.posedge_event() | clock_.posedge_event());

            if (reset_.read()) {
                valid.write(false);
                throw Exception(fmt::format("elastic channel '{}': reset asserted during send.", channelName));
            }

            if (ready.read())
                break;

            if (timeoutCycles != 0 && ++waitedCycles >= timeoutCycles) {
                valid.write(false);
                throw Exception(fmt::format(
                    "elastic channel '{}': send timed out after {} cycles waiting for ready.",
                    channelName,
                    timeoutCycles
                ));
            }
        } while (!ready.read() || reset_.read());

        sc_core::wait(clock_.negedge_event());

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
        PeekDataFn peekDataFn,
        std::string_view channelName,
        std::uint64_t timeoutCycles
    ) {
        auto clock_ = constBoolWrapper<!PosEdgeClock>(clock);
        auto reset_ = constBoolWrapper<!ActiveHighReset>(reset);
        std::uint64_t waitedCycles = 0;

        sc_core::wait(sc_core::SC_ZERO_TIME);

        while (clock_.read() || reset_.read())
            sc_core::wait(clock_.negedge_event());

        ready.write(true);

        do {
            sc_core::wait(reset_.posedge_event() | clock_.posedge_event());

            if (reset_.read()) {
                ready.write(false);
                throw Exception(fmt::format("elastic channel '{}': reset asserted during receive.", channelName));
            }

            if (valid.read())
                break;

            if (timeoutCycles != 0 && ++waitedCycles >= timeoutCycles) {
                ready.write(false);
                throw Exception(fmt::format(
                    "elastic channel '{}': receive timed out after {} cycles waiting for valid.",
                    channelName,
                    timeoutCycles
                ));
            }
        } while (!valid.read() || reset_.read());

        peekDataFn();

        sc_core::wait(clock_.negedge_event());
        ready.write(false);
    }
};

}; // namespace chext_test::util

#endif /* CHEXT_TEST_UTIL_READYVALID_HPP_INCLUDED */
