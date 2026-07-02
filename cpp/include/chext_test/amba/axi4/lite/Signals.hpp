#ifndef CHEXT_TEST_AMBA_AXI4_LITE_SIGNALS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_LITE_SIGNALS_HPP_INCLUDED

#include <chext_test/amba/axi4/lite/Packets.hpp>
#include <chext_test/util/Exception.hpp>
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

        sc_signal<sc_bv<wAddr>, SC_MANY_WRITERS> addr;
        sc_signal<sc_bv<3>, SC_MANY_WRITERS> prot;

        ReadAddress(const char* name)
            : addr(fmt::format("{}_addr", name).c_str())
            , prot(fmt::format("{}_prot", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("ar.addr", packet.addr.length(), wAddr);
            addr.write(packet.addr);
            prot.write(packet.prot);
        }

        void readTo(value_type& packet) const {
            packet.addr = addr.read();
            packet.prot = prot.read().to_uint();
        }
    };

    struct ReadData {
        using value_type = Packets::ReadData;

        sc_signal<sc_bv<wData>, SC_MANY_WRITERS> data;
        sc_signal<sc_bv<2>, SC_MANY_WRITERS> resp;

        ReadData(const char* name)
            : data(fmt::format("{}_data", name).c_str())
            , resp(fmt::format("{}_resp", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("r.data", packet.data.length(), wData);
            data.write(packet.data);
            resp.write(packet.resp);
        }

        void readTo(value_type& packet) const {
            packet.data = data.read();
            packet.resp = resp.read().to_uint();
        }
    };

    struct WriteAddress {
        using value_type = Packets::WriteAddress;

        sc_signal<sc_bv<wAddr>, SC_MANY_WRITERS> addr;
        sc_signal<sc_bv<3>, SC_MANY_WRITERS> prot;

        WriteAddress(const char* name)
            : addr(fmt::format("{}_addr", name).c_str())
            , prot(fmt::format("{}_prot", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("aw.addr", packet.addr.length(), wAddr);
            addr.write(packet.addr);
            prot.write(packet.prot);
        }

        void readTo(value_type& packet) const {
            packet.addr = addr.read();
            packet.prot = prot.read().to_uint();
        }
    };

    struct WriteData {
        using value_type = Packets::WriteData;

        sc_signal<sc_bv<wData>, SC_MANY_WRITERS> data;
        sc_signal<sc_bv<wStrb>, SC_MANY_WRITERS> strb;

        WriteData(const char* name)
            : data(fmt::format("{}_data", name).c_str())
            , strb(fmt::format("{}_strb", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("w.data", packet.data.length(), wData);
            validateWidth("w.strb", packet.strb.length(), wStrobe);
            data.write(packet.data);
            strb.write(packet.strb);
        }

        void readTo(value_type& packet) const {
            packet.data = data.read();
            packet.strb = strb.read();
        }
    };

    struct WriteResponse {
        using value_type = Packets::WriteResponse;

        sc_signal<sc_bv<2>, SC_MANY_WRITERS> resp;

        WriteResponse(const char* name)
            : resp(fmt::format("{}_resp", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            resp.write(packet.resp);
        }

        void readTo(value_type& packet) const {
            packet.resp = resp.read().to_uint();
        }
    };
};

} // namespace detail

using detail::Signals;

} // namespace chext_test::amba::axi4::lite

#endif /* CHEXT_TEST_AMBA_AXI4_LITE_SIGNALS_HPP_INCLUDED */
