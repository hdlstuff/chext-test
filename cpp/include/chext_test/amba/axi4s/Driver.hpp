#ifndef CHEXT_TEST_AMBA_AXI4S_DRIVER_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4S_DRIVER_HPP_INCLUDED

#include <chext_test/amba/axi4s/Signals.hpp>
#include <chext_test/elastic/Driver.hpp>

#include <systemc>

namespace chext_test::amba::axi4s {

template<
    unsigned DATA_WIDTH,
    unsigned ID_WIDTH = 0,
    unsigned DEST_WIDTH = 0,
    unsigned USER_WIDTH = 0,
    bool HAS_READY = true,
    bool HAS_STROBE = true,
    bool HAS_KEEP = true,
    bool HAS_LAST = true>
struct Master : elastic::Sink<Signals<DATA_WIDTH, ID_WIDTH, DEST_WIDTH, USER_WIDTH, HAS_STROBE, HAS_KEEP, HAS_LAST>> {
    using SignalsT = Signals<DATA_WIDTH, ID_WIDTH, DEST_WIDTH, USER_WIDTH, HAS_STROBE, HAS_KEEP, HAS_LAST>;
    using PacketT = typename SignalsT::value_type;
    using Base = elastic::Sink<SignalsT>;

    static constexpr Config config {
        ConfigSpec {
            .wData = DATA_WIDTH,
            .wId = ID_WIDTH,
            .wDest = DEST_WIDTH,
            .wUser = USER_WIDTH,
            .hasReady = HAS_READY,
            .hasStrobe = HAS_STROBE,
            .hasKeep = HAS_KEEP,
            .hasLast = HAS_LAST,
        }
    };

    Master(const char* name, sc_core::sc_in_clk const& clock, sc_core::sc_in<bool> const& reset)
        : Base { name, clock, reset } {}

    PacketT makePacket() const {
        return PacketT { config };
    }

    PacketT receive() {
        auto packet = makePacket();
        this->receiveTo(packet);
        return packet;
    }
};

template<
    unsigned DATA_WIDTH,
    unsigned ID_WIDTH = 0,
    unsigned DEST_WIDTH = 0,
    unsigned USER_WIDTH = 0,
    bool HAS_READY = true,
    bool HAS_STROBE = true,
    bool HAS_KEEP = true,
    bool HAS_LAST = true>
struct Slave : elastic::Source<Signals<DATA_WIDTH, ID_WIDTH, DEST_WIDTH, USER_WIDTH, HAS_STROBE, HAS_KEEP, HAS_LAST>> {
    using SignalsT = Signals<DATA_WIDTH, ID_WIDTH, DEST_WIDTH, USER_WIDTH, HAS_STROBE, HAS_KEEP, HAS_LAST>;
    using PacketT = typename SignalsT::value_type;
    using Base = elastic::Source<SignalsT>;

    static constexpr Config config {
        ConfigSpec {
            .wData = DATA_WIDTH,
            .wId = ID_WIDTH,
            .wDest = DEST_WIDTH,
            .wUser = USER_WIDTH,
            .hasReady = HAS_READY,
            .hasStrobe = HAS_STROBE,
            .hasKeep = HAS_KEEP,
            .hasLast = HAS_LAST,
        }
    };

    Slave(const char* name, sc_core::sc_in_clk const& clock, sc_core::sc_in<bool> const& reset)
        : Base { name, clock, reset } {
        if constexpr (!HAS_READY)
            this->ready.write(true);
    }

    PacketT makePacket() const {
        return PacketT { config };
    }
};

} // namespace chext_test::amba::axi4s

#endif /* CHEXT_TEST_AMBA_AXI4S_DRIVER_HPP_INCLUDED */
