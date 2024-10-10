#ifndef CHEXT_TEST_AMBA_AXI4_LITE_SIGNALS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_LITE_SIGNALS_HPP_INCLUDED

#include <chext_test/amba/axi4/lite/Packets.hpp>
#include <fmt/core.h>
#include <systemc>

namespace chext_test::amba::axi4::lite {

namespace detail {

using namespace sc_core;
using namespace sc_dt;

template<unsigned ADDR_WIDTH, unsigned DATA_WIDTH>
struct Signals {
    static constexpr unsigned wAddr = ADDR_WIDTH;
    static constexpr unsigned wData = DATA_WIDTH;

    static_assert(DATA_WIDTH == 32 || DATA_WIDTH == 64);

    struct ReadAddress {
        using value_type = Packets::ReadAddress;

        sc_signal<sc_bv<wAddr>, SC_MANY_WRITERS> addr;
        sc_signal<sc_bv<3>, SC_MANY_WRITERS> prot;

        ReadAddress(const char* name)
            : addr(fmt::format("{}_addr", name).c_str())
            , prot(fmt::format("{}_prot", name).c_str()) {}

        void write(Packets::ReadAddress const& packet) {
            addr.write(packet.addr);
            prot.write(packet.prot);
        }

        Packets::ReadAddress read() {
            return {
                .addr = addr.read(),
                .prot = prot.read().to_uint()
            };
        }
    };

    struct ReadData {
        using value_type = Packets::ReadData;

        sc_signal<sc_bv<wData>, SC_MANY_WRITERS> data;
        sc_signal<sc_bv<2>, SC_MANY_WRITERS> resp;

        ReadData(const char* name)
            : data(fmt::format("{}_data", name).c_str())
            , resp(fmt::format("{}_resp", name).c_str()) {}

        void write(Packets::ReadData const& packet) {
            data.write(packet.data);
            resp.write(packet.resp);
        }

        Packets::ReadData read() {
            return {
                .data = data.read(),
                .resp = resp.read().to_uint()
            };
        }
    };

    struct WriteAddress {
        using value_type = Packets::WriteAddress;

        sc_signal<sc_bv<wAddr>, SC_MANY_WRITERS> addr;
        sc_signal<sc_bv<3>, SC_MANY_WRITERS> prot;

        WriteAddress(const char* name)
            : addr(fmt::format("{}_addr", name).c_str())
            , prot(fmt::format("{}_prot", name).c_str()) {}

        void write(Packets::WriteAddress const& packet) {
            addr.write(packet.addr);
            prot.write(packet.prot);
        }

        Packets::WriteAddress read() {
            return {
                .addr = addr.read(),
                .prot = prot.read().to_uint()
            };
        }
    };

    struct WriteData {
        using value_type = Packets::WriteData;

        sc_signal<sc_bv<wData>, SC_MANY_WRITERS> data;
        sc_signal<sc_bv<8>, SC_MANY_WRITERS> strb;

        WriteData(const char* name)
            : data(fmt::format("{}_data", name).c_str())
            , strb(fmt::format("{}_strb", name).c_str()) {}

        void write(Packets::WriteData const& packet) {
            data.write(packet.data);
            strb.write(packet.strb);
        }

        Packets::WriteData read() {
            return {
                .data = data.read(),
                .strb = strb.read()
            };
        }
    };

    struct WriteResponse {
        using value_type = Packets::WriteResponse;

        sc_signal<sc_bv<2>, SC_MANY_WRITERS> resp;

        WriteResponse(const char* name)
            : resp(fmt::format("{}_resp", name).c_str()) {}

        void write(Packets::WriteResponse const& packet) {
            resp.write(packet.resp);
        }

        Packets::WriteResponse read() {
            return {
                .resp = resp.read().to_uint()
            };
        }
    };
};

} // namespace detail

using detail::Signals;

} // namespace chext_test::amba::axi4::lite

#endif /* CHEXT_TEST_AMBA_AXI4_LITE_SIGNALS_HPP_INCLUDED */
