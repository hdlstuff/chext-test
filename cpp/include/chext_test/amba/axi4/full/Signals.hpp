#ifndef CHEXT_TEST_AMBA_AXI4_FULL_SIGNALS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_FULL_SIGNALS_HPP_INCLUDED

#include <chext_test/amba/axi4/full/Packets.hpp>
#include <chext_test/util/Exception.hpp>
#include <chext_test/util/VerilatorPort.hpp>

#include <fmt/core.h>

#include <systemc>
#include <type_traits>

namespace chext_test::amba::axi4::full {

namespace detail {

using namespace sc_core;
using namespace sc_dt;

constexpr unsigned notZeroOr(unsigned x, unsigned y) {
    return x > 0 ? x : y;
}

inline void validateWidth(char const* field, int actual, unsigned expected) {
    if (actual != (int)expected) {
        throw util::Exception(fmt::format(
            "AXI4 packet field '{}' has width {}, expected {}",
            field,
            actual,
            expected
        ));
    }
}

struct AbsentSignal {
    explicit AbsentSignal(char const*) {}
};

template<bool PRESENT, unsigned WIDTH>
using optional_signal_t = std::conditional_t<
    PRESENT,
    sc_signal<util::verilator_port_signal_t<notZeroOr(WIDTH, 1)>, SC_MANY_WRITERS>,
    AbsentSignal>;

template<unsigned WIDTH>
using signal_t = sc_signal<util::verilator_port_signal_t<WIDTH>, SC_MANY_WRITERS>;

// Verilator may expose a port as bool/uint/sc_bv depending on --pins-bv.
// Packet destinations are either dynamic bit-vectors or scalar metadata fields.
template<bool PRESENT, unsigned WIDTH, typename SignalT, typename ValueT>
void writePacketFieldToPortIfPresent(SignalT& signal, ValueT const& value) {
    if constexpr (PRESENT)
        util::verilator_port_write<notZeroOr(WIDTH, 1)>(signal, value);
}

template<bool PRESENT, unsigned WIDTH, typename SignalT, typename ValueT>
void readPortToPacketBvIfPresent(SignalT const& signal, ValueT& value) {
    if constexpr (PRESENT)
        util::verilator_port_read_bv<notZeroOr(WIDTH, 1)>(signal, value);
}

template<bool PRESENT, unsigned WIDTH, typename SignalT, typename ValueT>
void readPortToPacketScalarIfPresent(SignalT const& signal, ValueT& value) {
    if constexpr (PRESENT)
        value = static_cast<ValueT>(util::verilator_port_read_uint<notZeroOr(WIDTH, 1)>(signal));
}

template<unsigned WIDTH, typename SignalT, typename ValueT>
void writePacketFieldToPort(SignalT& signal, ValueT const& value) {
    util::verilator_port_write<WIDTH>(signal, value);
}

template<unsigned WIDTH, typename SignalT, typename ValueT>
void readPortToPacketBv(SignalT const& signal, ValueT& value) {
    util::verilator_port_read_bv<WIDTH>(signal, value);
}

template<unsigned WIDTH, typename SignalT, typename ValueT>
void readPortToPacketScalar(SignalT const& signal, ValueT& value) {
    value = static_cast<ValueT>(util::verilator_port_read_uint<WIDTH>(signal));
}

template<
    unsigned ID_WIDTH,
    unsigned ADDR_WIDTH,
    unsigned DATA_WIDTH,
    bool READ = true,
    bool WRITE = true,
    bool HAS_LOCK = true,
    bool HAS_CACHE = true,
    bool HAS_PROT = true,
    bool HAS_QOS = true,
    bool HAS_REGION = true,
    bool AXI3_COMPAT = false,
    unsigned ARUSER_WIDTH = 0,
    unsigned RUSER_WIDTH = 0,
    unsigned AWUSER_WIDTH = 0,
    unsigned WUSER_WIDTH = 0,
    unsigned BUSER_WIDTH = 0>
struct Signals {
    static constexpr Config config {
        ConfigSpec {
            .wId = ID_WIDTH,
            .wAddr = ADDR_WIDTH,
            .wData = DATA_WIDTH,
            .read = READ,
            .write = WRITE,
            .lite = false,
            .hasLock = HAS_LOCK,
            .hasCache = HAS_CACHE,
            .hasProt = HAS_PROT,
            .hasQos = HAS_QOS,
            .hasRegion = HAS_REGION,
            .axi3Compat = AXI3_COMPAT,
            .wUserAR = ARUSER_WIDTH,
            .wUserR = RUSER_WIDTH,
            .wUserAW = AWUSER_WIDTH,
            .wUserW = WUSER_WIDTH,
            .wUserB = BUSER_WIDTH,
        }
    };

private:
    static constexpr bool hasId = config.wId > 0;
    static constexpr bool hasUserAR = config.wUserAR > 0;
    static constexpr bool hasUserR = config.wUserR > 0;
    static constexpr bool hasUserAW = config.wUserAW > 0;
    static constexpr bool hasUserW = config.wUserW > 0;
    static constexpr bool hasUserB = config.wUserB > 0;

public:
    struct ReadAddress {
        using value_type = Packets::ReadAddress;

        [[no_unique_address]] optional_signal_t<hasId, config.wId> id;
        signal_t<config.wAddr> addr;
        signal_t<config.wLen> len;
        signal_t<3> size;
        signal_t<2> burst;
        [[no_unique_address]] optional_signal_t<config.hasLock, config.wLock> lock;
        [[no_unique_address]] optional_signal_t<config.hasCache, config.wCache> cache;
        [[no_unique_address]] optional_signal_t<config.hasProt, config.wProt> prot;
        [[no_unique_address]] optional_signal_t<config.hasQos, config.wQos> qos;
        [[no_unique_address]] optional_signal_t<config.hasRegion, config.wRegion> region;
        [[no_unique_address]] optional_signal_t<hasUserAR, config.wUserAR> user;

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

        void writeFrom(value_type const& packet) {
            validateWidth("ar.id", packet.id.length(), notZeroOr(config.wId, 32));
            validateWidth("ar.addr", packet.addr.length(), config.wAddr);
            validateWidth("ar.user", packet.user.length(), notZeroOr(config.wUserAR, 32));
            writePacketFieldToPortIfPresent<hasId, config.wId>(id, packet.id);
            writePacketFieldToPort<config.wAddr>(addr, packet.addr);
            writePacketFieldToPort<config.wLen>(len, packet.len);
            writePacketFieldToPort<3>(size, packet.size);
            writePacketFieldToPort<2>(burst, packet.burst);
            writePacketFieldToPortIfPresent<config.hasLock, config.wLock>(lock, packet.lock);
            writePacketFieldToPortIfPresent<config.hasCache, config.wCache>(cache, packet.cache);
            writePacketFieldToPortIfPresent<config.hasProt, config.wProt>(prot, packet.prot);
            writePacketFieldToPortIfPresent<config.hasQos, config.wQos>(qos, packet.qos);
            writePacketFieldToPortIfPresent<config.hasRegion, config.wRegion>(region, packet.region);
            writePacketFieldToPortIfPresent<hasUserAR, config.wUserAR>(user, packet.user);
        }

        void readTo(value_type& packet) const {
            readPortToPacketBvIfPresent<hasId, config.wId>(id, packet.id);
            readPortToPacketBv<config.wAddr>(addr, packet.addr);
            readPortToPacketScalar<config.wLen>(len, packet.len);
            readPortToPacketScalar<3>(size, packet.size);
            readPortToPacketScalar<2>(burst, packet.burst);
            readPortToPacketScalarIfPresent<config.hasLock, config.wLock>(lock, packet.lock);
            readPortToPacketScalarIfPresent<config.hasCache, config.wCache>(cache, packet.cache);
            readPortToPacketScalarIfPresent<config.hasProt, config.wProt>(prot, packet.prot);
            readPortToPacketScalarIfPresent<config.hasQos, config.wQos>(qos, packet.qos);
            readPortToPacketScalarIfPresent<config.hasRegion, config.wRegion>(region, packet.region);
            readPortToPacketBvIfPresent<hasUserAR, config.wUserAR>(user, packet.user);
        }
    };

    struct ReadData {
        using value_type = Packets::ReadData;

        [[no_unique_address]] optional_signal_t<hasId, config.wId> id;
        signal_t<config.wData> data;
        signal_t<2> resp;
        signal_t<1> last;
        [[no_unique_address]] optional_signal_t<hasUserR, config.wUserR> user;

        ReadData(const char* name)
            : id(fmt::format("{}_id", name).c_str())
            , data(fmt::format("{}_data", name).c_str())
            , resp(fmt::format("{}_resp", name).c_str())
            , last(fmt::format("{}_last", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("r.id", packet.id.length(), notZeroOr(config.wId, 32));
            validateWidth("r.data", packet.data.length(), config.wData);
            validateWidth("r.user", packet.user.length(), notZeroOr(config.wUserR, 32));
            writePacketFieldToPortIfPresent<hasId, config.wId>(id, packet.id);
            writePacketFieldToPort<config.wData>(data, packet.data);
            writePacketFieldToPort<2>(resp, packet.resp);
            writePacketFieldToPort<1>(last, packet.last);
            writePacketFieldToPortIfPresent<hasUserR, config.wUserR>(user, packet.user);
        }

        void readTo(value_type& packet) const {
            readPortToPacketBvIfPresent<hasId, config.wId>(id, packet.id);
            readPortToPacketBv<config.wData>(data, packet.data);
            readPortToPacketScalar<2>(resp, packet.resp);
            readPortToPacketScalar<1>(last, packet.last);
            readPortToPacketBvIfPresent<hasUserR, config.wUserR>(user, packet.user);
        }
    };

    struct WriteAddress {
        using value_type = Packets::WriteAddress;

        [[no_unique_address]] optional_signal_t<hasId, config.wId> id;
        signal_t<config.wAddr> addr;
        signal_t<config.wLen> len;
        signal_t<3> size;
        signal_t<2> burst;
        [[no_unique_address]] optional_signal_t<config.hasLock, config.wLock> lock;
        [[no_unique_address]] optional_signal_t<config.hasCache, config.wCache> cache;
        [[no_unique_address]] optional_signal_t<config.hasProt, config.wProt> prot;
        [[no_unique_address]] optional_signal_t<config.hasQos, config.wQos> qos;
        [[no_unique_address]] optional_signal_t<config.hasRegion, config.wRegion> region;
        [[no_unique_address]] optional_signal_t<hasUserAW, config.wUserAW> user;

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

        void writeFrom(value_type const& packet) {
            validateWidth("aw.id", packet.id.length(), notZeroOr(config.wId, 32));
            validateWidth("aw.addr", packet.addr.length(), config.wAddr);
            validateWidth("aw.user", packet.user.length(), notZeroOr(config.wUserAW, 32));
            writePacketFieldToPortIfPresent<hasId, config.wId>(id, packet.id);
            writePacketFieldToPort<config.wAddr>(addr, packet.addr);
            writePacketFieldToPort<config.wLen>(len, packet.len);
            writePacketFieldToPort<3>(size, packet.size);
            writePacketFieldToPort<2>(burst, packet.burst);
            writePacketFieldToPortIfPresent<config.hasLock, config.wLock>(lock, packet.lock);
            writePacketFieldToPortIfPresent<config.hasCache, config.wCache>(cache, packet.cache);
            writePacketFieldToPortIfPresent<config.hasProt, config.wProt>(prot, packet.prot);
            writePacketFieldToPortIfPresent<config.hasQos, config.wQos>(qos, packet.qos);
            writePacketFieldToPortIfPresent<config.hasRegion, config.wRegion>(region, packet.region);
            writePacketFieldToPortIfPresent<hasUserAW, config.wUserAW>(user, packet.user);
        }

        void readTo(value_type& packet) const {
            readPortToPacketBvIfPresent<hasId, config.wId>(id, packet.id);
            readPortToPacketBv<config.wAddr>(addr, packet.addr);
            readPortToPacketScalar<config.wLen>(len, packet.len);
            readPortToPacketScalar<3>(size, packet.size);
            readPortToPacketScalar<2>(burst, packet.burst);
            readPortToPacketScalarIfPresent<config.hasLock, config.wLock>(lock, packet.lock);
            readPortToPacketScalarIfPresent<config.hasCache, config.wCache>(cache, packet.cache);
            readPortToPacketScalarIfPresent<config.hasProt, config.wProt>(prot, packet.prot);
            readPortToPacketScalarIfPresent<config.hasQos, config.wQos>(qos, packet.qos);
            readPortToPacketScalarIfPresent<config.hasRegion, config.wRegion>(region, packet.region);
            readPortToPacketBvIfPresent<hasUserAW, config.wUserAW>(user, packet.user);
        }
    };

    struct WriteData {
        using value_type = Packets::WriteData;

        signal_t<config.wData> data;
        signal_t<config.wStrobe> strb;
        signal_t<1> last;
        [[no_unique_address]] optional_signal_t<hasUserW, config.wUserW> user;

        WriteData(const char* name)
            : data(fmt::format("{}_data", name).c_str())
            , strb(fmt::format("{}_strb", name).c_str())
            , last(fmt::format("{}_last", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("w.data", packet.data.length(), config.wData);
            validateWidth("w.strb", packet.strb.length(), config.wStrobe);
            validateWidth("w.user", packet.user.length(), notZeroOr(config.wUserW, 32));
            writePacketFieldToPort<config.wData>(data, packet.data);
            writePacketFieldToPort<config.wStrobe>(strb, packet.strb);
            writePacketFieldToPort<1>(last, packet.last);
            writePacketFieldToPortIfPresent<hasUserW, config.wUserW>(user, packet.user);
        }

        void readTo(value_type& packet) const {
            readPortToPacketBv<config.wData>(data, packet.data);
            readPortToPacketBv<config.wStrobe>(strb, packet.strb);
            readPortToPacketScalar<1>(last, packet.last);
            readPortToPacketBvIfPresent<hasUserW, config.wUserW>(user, packet.user);
        }
    };

    struct WriteResponse {
        using value_type = Packets::WriteResponse;

        [[no_unique_address]] optional_signal_t<hasId, config.wId> id;
        signal_t<2> resp;
        [[no_unique_address]] optional_signal_t<hasUserB, config.wUserB> user;

        WriteResponse(const char* name)
            : id(fmt::format("{}_id", name).c_str())
            , resp(fmt::format("{}_resp", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("b.id", packet.id.length(), notZeroOr(config.wId, 32));
            validateWidth("b.user", packet.user.length(), notZeroOr(config.wUserB, 32));
            writePacketFieldToPortIfPresent<hasId, config.wId>(id, packet.id);
            writePacketFieldToPort<2>(resp, packet.resp);
            writePacketFieldToPortIfPresent<hasUserB, config.wUserB>(user, packet.user);
        }

        void readTo(value_type& packet) const {
            readPortToPacketBvIfPresent<hasId, config.wId>(id, packet.id);
            readPortToPacketScalar<2>(resp, packet.resp);
            readPortToPacketBvIfPresent<hasUserB, config.wUserB>(user, packet.user);
        }
    };
};

} // namespace detail

using detail::Signals;

} // namespace chext_test::amba::axi4::full

#endif /* CHEXT_TEST_AMBA_AXI4_FULL_SIGNALS_HPP_INCLUDED */
