#ifndef CHEXT_TEST_AMBA_AXI4_LITE_SIGNALS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_LITE_SIGNALS_HPP_INCLUDED

#include <chext_test/amba/axi4/lite/Packets.hpp>
#include <chext_test/util/Exception.hpp>
#include <chext_test/vutil.hpp>
#include <fmt/core.h>
#include <systemc>

namespace chext_test::amba::axi4::lite {

namespace detail {

using namespace sc_core;
using namespace sc_dt;

inline void validateWidth(char const* field, int actual, unsigned expected) {
    if (actual != (int)expected) {
        throw util::Exception(fmt::format(
            "AXI4-Lite packet field '{}' has width {}, expected {}",
            field,
            actual,
            expected
        ));
    }
}

template<unsigned ADDR_WIDTH, unsigned DATA_WIDTH>
struct Signals {
    static constexpr Config config {
        ConfigSpec {
            .wAddr = ADDR_WIDTH,
            .wData = DATA_WIDTH,
            .lite = true,
            .hasLock = false,
            .hasCache = false,
            .hasQos = false,
            .hasRegion = false,
        }
    };

    static constexpr unsigned wAddr = ADDR_WIDTH;
    static constexpr unsigned wData = DATA_WIDTH;

    static constexpr unsigned wStrobe = config.wStrobe;
    static constexpr unsigned wStrb = wStrobe;

    static_assert(DATA_WIDTH == 32 || DATA_WIDTH == 64);

    struct ReadAddress {
        using value_type = Packets::ReadAddress;

        vutil::signal_t<wAddr, SC_MANY_WRITERS> addr;
        vutil::signal_t<3, SC_MANY_WRITERS> prot;

        ReadAddress(const char* name)
            : addr(fmt::format("{}_addr", name).c_str())
            , prot(fmt::format("{}_prot", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("ar.addr", packet.addr.length(), wAddr);
            vutil::write<wAddr>(addr, packet.addr);
            vutil::write<3>(prot, packet.prot);
        }

        void readTo(value_type& packet) const {
            vutil::read<wAddr>(addr, packet.addr);
            vutil::read<3>(prot, packet.prot);
        }
    };

    struct ReadData {
        using value_type = Packets::ReadData;

        vutil::signal_t<wData, SC_MANY_WRITERS> data;
        vutil::signal_t<2, SC_MANY_WRITERS> resp;

        ReadData(const char* name)
            : data(fmt::format("{}_data", name).c_str())
            , resp(fmt::format("{}_resp", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("r.data", packet.data.length(), wData);
            vutil::write<wData>(data, packet.data);
            vutil::write<2>(resp, packet.resp);
        }

        void readTo(value_type& packet) const {
            vutil::read<wData>(data, packet.data);
            vutil::read<2>(resp, packet.resp);
        }
    };

    struct WriteAddress {
        using value_type = Packets::WriteAddress;

        vutil::signal_t<wAddr, SC_MANY_WRITERS> addr;
        vutil::signal_t<3, SC_MANY_WRITERS> prot;

        WriteAddress(const char* name)
            : addr(fmt::format("{}_addr", name).c_str())
            , prot(fmt::format("{}_prot", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("aw.addr", packet.addr.length(), wAddr);
            vutil::write<wAddr>(addr, packet.addr);
            vutil::write<3>(prot, packet.prot);
        }

        void readTo(value_type& packet) const {
            vutil::read<wAddr>(addr, packet.addr);
            vutil::read<3>(prot, packet.prot);
        }
    };

    struct WriteData {
        using value_type = Packets::WriteData;

        vutil::signal_t<wData, SC_MANY_WRITERS> data;
        vutil::signal_t<wStrb, SC_MANY_WRITERS> strb;

        WriteData(const char* name)
            : data(fmt::format("{}_data", name).c_str())
            , strb(fmt::format("{}_strb", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("w.data", packet.data.length(), wData);
            validateWidth("w.strb", packet.strb.length(), wStrobe);
            vutil::write<wData>(data, packet.data);
            vutil::write<wStrb>(strb, packet.strb);
        }

        void readTo(value_type& packet) const {
            vutil::read<wData>(data, packet.data);
            vutil::read<wStrb>(strb, packet.strb);
        }
    };

    struct WriteResponse {
        using value_type = Packets::WriteResponse;

        vutil::signal_t<2, SC_MANY_WRITERS> resp;

        WriteResponse(const char* name)
            : resp(fmt::format("{}_resp", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            vutil::write<2>(resp, packet.resp);
        }

        void readTo(value_type& packet) const {
            vutil::read<2>(resp, packet.resp);
        }
    };
};

} // namespace detail

using detail::Signals;

} // namespace chext_test::amba::axi4::lite

#endif /* CHEXT_TEST_AMBA_AXI4_LITE_SIGNALS_HPP_INCLUDED */
