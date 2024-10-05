#ifndef CHEXT_TEST_AMBA_AXI4_FULL_SIGNALS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_FULL_SIGNALS_HPP_INCLUDED

#include <chext_test/amba/axi4/full/Packets.hpp>
#include <fmt/core.h>
#include <systemc>

namespace chext_test::amba::axi4::full {

namespace detail {

using namespace sc_core;
using namespace sc_dt;

template<unsigned WIDTH>
struct bv_bool_helper {
    using type = sc_bv<WIDTH>;

    static auto peek(sc_signal<type> const& lock) {
        return lock.read().to_uint();
    }
};

template<>
struct bv_bool_helper<1> {
    using type = bool;

    static auto peek(sc_signal<type> const& lock) {
        return lock.read();
    }
};

template<unsigned WIDTH>
using bv_bool_t = typename bv_bool_helper<WIDTH>::type;

template<
    unsigned ID_WIDTH,
    unsigned ADDR_WIDTH,
    unsigned DATA_WIDTH,
    unsigned ARUSER_WIDTH = 32,
    unsigned RUSER_WIDTH = 32,
    unsigned AWUSER_WIDTH = 32,
    unsigned WUSER_WIDTH = 32,
    unsigned BUSER_WIDTH = 32,
    bool AXI3_COMPAT = false>
struct Signals {
    static constexpr unsigned wId = ID_WIDTH;
    static constexpr unsigned wAddr = ADDR_WIDTH;
    static constexpr unsigned wData = DATA_WIDTH;
    static constexpr unsigned wUserAR = ARUSER_WIDTH;
    static constexpr unsigned wUserR = RUSER_WIDTH;
    static constexpr unsigned wUserAW = AWUSER_WIDTH;
    static constexpr unsigned wUserW = WUSER_WIDTH;
    static constexpr unsigned wUserB = BUSER_WIDTH;

    static constexpr bool axi3Compat = AXI3_COMPAT;
    static constexpr unsigned wLen = axi3Compat ? 4 : 8;
    static constexpr unsigned wLock = axi3Compat ? 2 : 1;

    struct ReadAddress {
        using value_type = Packets::ReadAddress;

        sc_signal<sc_bv<wId>> id;
        sc_signal<sc_bv<wAddr>> addr;
        sc_signal<sc_bv<wLen>> len;
        sc_signal<sc_bv<3>> size;
        sc_signal<sc_bv<2>> burst;
        sc_signal<bv_bool_t<wLock>> lock;
        sc_signal<sc_bv<4>> cache;
        sc_signal<sc_bv<3>> prot;
        sc_signal<sc_bv<4>> qos;
        sc_signal<sc_bv<4>> region;
        sc_signal<sc_bv<wUserAR>> user;

        ReadAddress(const char* name)
            : id(fmt::format("{}_id", name).c_str())
            , addr(fmt::format("{}_addr", name).c_str())
            , len(fmt::format("{}_len", name).c_str())
            , size(fmt::format("{}_size", name).c_str())
            , burst(fmt::format("{}_burst", name).c_str())
            , lock(fmt::format("{}_lock", name).c_str())
            , cache(fmt::format("{}_cache", name).c_str())
            , prot(fmt::format("{}_prot", name).c_str())
            , qos(fmt::format("{}_qos", name).c_str())
            , region(fmt::format("{}_region", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void write(Packets::ReadAddress const& packet) {
            id.write(packet.id);
            addr.write(packet.addr);
            len.write(packet.len);
            size.write(packet.size);
            burst.write(packet.burst);
            lock.write(packet.lock);
            cache.write(packet.cache);
            prot.write(packet.prot);
            qos.write(packet.qos);
            region.write(packet.region);
            user.write(packet.user);
        }

        Packets::ReadAddress read() {
            Packets::ReadAddress packet;
            packet.id = id.read();
            packet.addr = addr.read();
            packet.len = len.read().to_uint();
            packet.size = size.read().to_uint();
            packet.burst = burst.read().to_uint();
            packet.lock = bv_bool_helper<wLock>::peek(lock);
            packet.cache = cache.read().to_uint();
            packet.prot = prot.read().to_uint();
            packet.qos = qos.read().to_uint();
            packet.region = region.read().to_uint();
            packet.user = user.read();
            return packet;
        }
    };

    struct ReadData {
        using value_type = Packets::ReadData;

        sc_signal<sc_bv<wId>> id;
        sc_signal<sc_bv<wData>> data;
        sc_signal<sc_bv<2>> resp;
        sc_signal<bool> last;
        sc_signal<sc_bv<wUserR>> user;

        ReadData(const char* name)
            : id(fmt::format("{}_id", name).c_str())
            , data(fmt::format("{}_data", name).c_str())
            , resp(fmt::format("{}_resp", name).c_str())
            , last(fmt::format("{}_last", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void write(Packets::ReadData const& packet) {
            id.write(packet.id);
            data.write(packet.data);
            resp.write(packet.resp);
            last.write(packet.last);
            user.write(packet.user);
        }

        Packets::ReadData read() {
            Packets::ReadData packet;
            packet.id = id.read();
            packet.data = data.read();
            packet.resp = resp.read().to_uint();
            packet.last = last.read();
            packet.user = user.read();
            return packet;
        }
    };

    struct WriteAddress {
        using value_type = Packets::WriteAddress;

        sc_signal<sc_bv<wId>> id;
        sc_signal<sc_bv<wAddr>> addr;
        sc_signal<sc_bv<wLen>> len;
        sc_signal<sc_bv<3>> size;
        sc_signal<sc_bv<2>> burst;
        sc_signal<bv_bool_t<wLock>> lock;
        sc_signal<sc_bv<4>> cache;
        sc_signal<sc_bv<3>> prot;
        sc_signal<sc_bv<4>> qos;
        sc_signal<sc_bv<4>> region;
        sc_signal<sc_bv<wUserAW>> user;

        WriteAddress(const char* name)
            : id(fmt::format("{}_id", name).c_str())
            , addr(fmt::format("{}_addr", name).c_str())
            , len(fmt::format("{}_len", name).c_str())
            , size(fmt::format("{}_size", name).c_str())
            , burst(fmt::format("{}_burst", name).c_str())
            , lock(fmt::format("{}_lock", name).c_str())
            , cache(fmt::format("{}_cache", name).c_str())
            , prot(fmt::format("{}_prot", name).c_str())
            , qos(fmt::format("{}_qos", name).c_str())
            , region(fmt::format("{}_region", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void write(Packets::WriteAddress const& packet) {
            id.write(packet.id);
            addr.write(packet.addr);
            len.write(packet.len);
            size.write(packet.size);
            burst.write(packet.burst);
            lock.write(packet.lock);
            cache.write(packet.cache);
            prot.write(packet.prot);
            qos.write(packet.qos);
            region.write(packet.region);
            user.write(packet.user);
        }

        Packets::WriteAddress read() {
            Packets::WriteAddress packet;
            packet.id = id.read();
            packet.addr = addr.read();
            packet.len = len.read().to_uint();
            packet.size = size.read().to_uint();
            packet.burst = burst.read().to_uint();
            packet.lock = bv_bool_helper<wLock>::peek(lock);
            packet.cache = cache.read().to_uint();
            packet.prot = prot.read().to_uint();
            packet.qos = qos.read().to_uint();
            packet.region = region.read().to_uint();
            packet.user = user.read();
            return packet;
        }
    };

    struct WriteData {
        using value_type = Packets::WriteData;

        sc_signal<sc_bv<wData>> data;
        sc_signal<sc_bv<8>> strb;
        sc_signal<bool> last;
        sc_signal<sc_bv<wUserW>> user;

        WriteData(const char* name)
            : data(fmt::format("{}_data", name).c_str())
            , strb(fmt::format("{}_strb", name).c_str())
            , last(fmt::format("{}_last", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void write(Packets::WriteData const& packet) {
            data.write(packet.data);
            strb.write(packet.strb);
            last.write(packet.last);
            user.write(packet.user);
        }

        Packets::WriteData read() {
            Packets::WriteData packet;
            packet.data = data.read();
            packet.strb = strb.read();
            packet.last = last.read();
            packet.user = user.read();
            return packet;
        }
    };

    struct WriteResponse {
        using value_type = Packets::WriteResponse;

        sc_signal<sc_bv<wId>> id;
        sc_signal<sc_bv<2>> resp;
        sc_signal<sc_bv<wUserB>> user;

        WriteResponse(const char* name)
            : id(fmt::format("{}_id", name).c_str())
            , resp(fmt::format("{}_resp", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void write(Packets::WriteResponse const& packet) {
            id.write(packet.id);
            resp.write(packet.resp);
            user.write(packet.user);
        }

        Packets::WriteResponse read() {
            Packets::WriteResponse packet;
            packet.id = id.read();
            packet.resp = resp.read().to_uint();
            packet.user = user.read();
            return packet;
        }
    };
};

} // namespace detail

using detail::Signals;

} // namespace chext_test::amba::axi4::full

#endif /* CHEXT_TEST_AMBA_AXI4_FULL_SIGNALS_HPP_INCLUDED */
