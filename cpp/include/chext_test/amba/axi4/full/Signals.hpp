#ifndef CHEXT_TEST_AMBA_AXI4_FULL_SIGNALS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_FULL_SIGNALS_HPP_INCLUDED

#include <chext_test/amba/axi4/full/Packets.hpp>
#include <chext_test/util/Exception.hpp>
#include <chext_test/vutil.hpp>

#include <fmt/core.h>

#include <systemc>
#include <type_traits>

namespace chext_test::amba::axi4::full {

namespace detail {

using namespace sc_core;
using namespace sc_dt;

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

        [[no_unique_address]] vutil::optional_signal_t<hasId, config.wId, SC_MANY_WRITERS> id;
        vutil::signal_t<config.wAddr, SC_MANY_WRITERS> addr;
        vutil::signal_t<config.wLen, SC_MANY_WRITERS> len;
        vutil::signal_t<3, SC_MANY_WRITERS> size;
        vutil::signal_t<2, SC_MANY_WRITERS> burst;
        [[no_unique_address]] vutil::optional_signal_t<config.hasLock, config.wLock, SC_MANY_WRITERS> lock;
        [[no_unique_address]] vutil::optional_signal_t<config.hasCache, config.wCache, SC_MANY_WRITERS> cache;
        [[no_unique_address]] vutil::optional_signal_t<config.hasProt, config.wProt, SC_MANY_WRITERS> prot;
        [[no_unique_address]] vutil::optional_signal_t<config.hasQos, config.wQos, SC_MANY_WRITERS> qos;
        [[no_unique_address]] vutil::optional_signal_t<config.hasRegion, config.wRegion, SC_MANY_WRITERS> region;
        [[no_unique_address]] vutil::optional_signal_t<hasUserAR, config.wUserAR, SC_MANY_WRITERS> user;

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
            validateWidth("ar.id", packet.id.length(), vutil::width_or(config.wId, 32));
            validateWidth("ar.addr", packet.addr.length(), config.wAddr);
            validateWidth("ar.user", packet.user.length(), vutil::width_or(config.wUserAR, 32));
            vutil::write_if<hasId, config.wId>(id, packet.id);
            vutil::write<config.wAddr>(addr, packet.addr);
            vutil::write<config.wLen>(len, packet.len);
            vutil::write<3>(size, packet.size);
            vutil::write<2>(burst, packet.burst);
            vutil::write_if<config.hasLock, config.wLock>(lock, packet.lock);
            vutil::write_if<config.hasCache, config.wCache>(cache, packet.cache);
            vutil::write_if<config.hasProt, config.wProt>(prot, packet.prot);
            vutil::write_if<config.hasQos, config.wQos>(qos, packet.qos);
            vutil::write_if<config.hasRegion, config.wRegion>(region, packet.region);
            vutil::write_if<hasUserAR, config.wUserAR>(user, packet.user);
        }

        void readTo(value_type& packet) const {
            vutil::read_if<hasId, config.wId>(id, packet.id);
            vutil::read<config.wAddr>(addr, packet.addr);
            vutil::read<config.wLen>(len, packet.len);
            vutil::read<3>(size, packet.size);
            vutil::read<2>(burst, packet.burst);
            vutil::read_if<config.hasLock, config.wLock>(lock, packet.lock);
            vutil::read_if<config.hasCache, config.wCache>(cache, packet.cache);
            vutil::read_if<config.hasProt, config.wProt>(prot, packet.prot);
            vutil::read_if<config.hasQos, config.wQos>(qos, packet.qos);
            vutil::read_if<config.hasRegion, config.wRegion>(region, packet.region);
            vutil::read_if<hasUserAR, config.wUserAR>(user, packet.user);
        }
    };

    struct ReadData {
        using value_type = Packets::ReadData;

        [[no_unique_address]] vutil::optional_signal_t<hasId, config.wId, SC_MANY_WRITERS> id;
        vutil::signal_t<config.wData, SC_MANY_WRITERS> data;
        vutil::signal_t<2, SC_MANY_WRITERS> resp;
        vutil::signal_t<1, SC_MANY_WRITERS> last;
        [[no_unique_address]] vutil::optional_signal_t<hasUserR, config.wUserR, SC_MANY_WRITERS> user;

        ReadData(const char* name)
            : id(fmt::format("{}_id", name).c_str())
            , data(fmt::format("{}_data", name).c_str())
            , resp(fmt::format("{}_resp", name).c_str())
            , last(fmt::format("{}_last", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("r.id", packet.id.length(), vutil::width_or(config.wId, 32));
            validateWidth("r.data", packet.data.length(), config.wData);
            validateWidth("r.user", packet.user.length(), vutil::width_or(config.wUserR, 32));
            vutil::write_if<hasId, config.wId>(id, packet.id);
            vutil::write<config.wData>(data, packet.data);
            vutil::write<2>(resp, packet.resp);
            vutil::write<1>(last, packet.last);
            vutil::write_if<hasUserR, config.wUserR>(user, packet.user);
        }

        void readTo(value_type& packet) const {
            vutil::read_if<hasId, config.wId>(id, packet.id);
            vutil::read<config.wData>(data, packet.data);
            vutil::read<2>(resp, packet.resp);
            vutil::read<1>(last, packet.last);
            vutil::read_if<hasUserR, config.wUserR>(user, packet.user);
        }
    };

    struct WriteAddress {
        using value_type = Packets::WriteAddress;

        [[no_unique_address]] vutil::optional_signal_t<hasId, config.wId, SC_MANY_WRITERS> id;
        vutil::signal_t<config.wAddr, SC_MANY_WRITERS> addr;
        vutil::signal_t<config.wLen, SC_MANY_WRITERS> len;
        vutil::signal_t<3, SC_MANY_WRITERS> size;
        vutil::signal_t<2, SC_MANY_WRITERS> burst;
        [[no_unique_address]] vutil::optional_signal_t<config.hasLock, config.wLock, SC_MANY_WRITERS> lock;
        [[no_unique_address]] vutil::optional_signal_t<config.hasCache, config.wCache, SC_MANY_WRITERS> cache;
        [[no_unique_address]] vutil::optional_signal_t<config.hasProt, config.wProt, SC_MANY_WRITERS> prot;
        [[no_unique_address]] vutil::optional_signal_t<config.hasQos, config.wQos, SC_MANY_WRITERS> qos;
        [[no_unique_address]] vutil::optional_signal_t<config.hasRegion, config.wRegion, SC_MANY_WRITERS> region;
        [[no_unique_address]] vutil::optional_signal_t<hasUserAW, config.wUserAW, SC_MANY_WRITERS> user;

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
            validateWidth("aw.id", packet.id.length(), vutil::width_or(config.wId, 32));
            validateWidth("aw.addr", packet.addr.length(), config.wAddr);
            validateWidth("aw.user", packet.user.length(), vutil::width_or(config.wUserAW, 32));
            vutil::write_if<hasId, config.wId>(id, packet.id);
            vutil::write<config.wAddr>(addr, packet.addr);
            vutil::write<config.wLen>(len, packet.len);
            vutil::write<3>(size, packet.size);
            vutil::write<2>(burst, packet.burst);
            vutil::write_if<config.hasLock, config.wLock>(lock, packet.lock);
            vutil::write_if<config.hasCache, config.wCache>(cache, packet.cache);
            vutil::write_if<config.hasProt, config.wProt>(prot, packet.prot);
            vutil::write_if<config.hasQos, config.wQos>(qos, packet.qos);
            vutil::write_if<config.hasRegion, config.wRegion>(region, packet.region);
            vutil::write_if<hasUserAW, config.wUserAW>(user, packet.user);
        }

        void readTo(value_type& packet) const {
            vutil::read_if<hasId, config.wId>(id, packet.id);
            vutil::read<config.wAddr>(addr, packet.addr);
            vutil::read<config.wLen>(len, packet.len);
            vutil::read<3>(size, packet.size);
            vutil::read<2>(burst, packet.burst);
            vutil::read_if<config.hasLock, config.wLock>(lock, packet.lock);
            vutil::read_if<config.hasCache, config.wCache>(cache, packet.cache);
            vutil::read_if<config.hasProt, config.wProt>(prot, packet.prot);
            vutil::read_if<config.hasQos, config.wQos>(qos, packet.qos);
            vutil::read_if<config.hasRegion, config.wRegion>(region, packet.region);
            vutil::read_if<hasUserAW, config.wUserAW>(user, packet.user);
        }
    };

    struct WriteData {
        using value_type = Packets::WriteData;

        vutil::signal_t<config.wData, SC_MANY_WRITERS> data;
        vutil::signal_t<config.wStrobe, SC_MANY_WRITERS> strb;
        vutil::signal_t<1, SC_MANY_WRITERS> last;
        [[no_unique_address]] vutil::optional_signal_t<hasUserW, config.wUserW, SC_MANY_WRITERS> user;

        WriteData(const char* name)
            : data(fmt::format("{}_data", name).c_str())
            , strb(fmt::format("{}_strb", name).c_str())
            , last(fmt::format("{}_last", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("w.data", packet.data.length(), config.wData);
            validateWidth("w.strb", packet.strb.length(), config.wStrobe);
            validateWidth("w.user", packet.user.length(), vutil::width_or(config.wUserW, 32));
            vutil::write<config.wData>(data, packet.data);
            vutil::write<config.wStrobe>(strb, packet.strb);
            vutil::write<1>(last, packet.last);
            vutil::write_if<hasUserW, config.wUserW>(user, packet.user);
        }

        void readTo(value_type& packet) const {
            vutil::read<config.wData>(data, packet.data);
            vutil::read<config.wStrobe>(strb, packet.strb);
            vutil::read<1>(last, packet.last);
            vutil::read_if<hasUserW, config.wUserW>(user, packet.user);
        }
    };

    struct WriteResponse {
        using value_type = Packets::WriteResponse;

        [[no_unique_address]] vutil::optional_signal_t<hasId, config.wId, SC_MANY_WRITERS> id;
        vutil::signal_t<2, SC_MANY_WRITERS> resp;
        [[no_unique_address]] vutil::optional_signal_t<hasUserB, config.wUserB, SC_MANY_WRITERS> user;

        WriteResponse(const char* name)
            : id(fmt::format("{}_id", name).c_str())
            , resp(fmt::format("{}_resp", name).c_str())
            , user(fmt::format("{}_user", name).c_str()) {}

        void writeFrom(value_type const& packet) {
            validateWidth("b.id", packet.id.length(), vutil::width_or(config.wId, 32));
            validateWidth("b.user", packet.user.length(), vutil::width_or(config.wUserB, 32));
            vutil::write_if<hasId, config.wId>(id, packet.id);
            vutil::write<2>(resp, packet.resp);
            vutil::write_if<hasUserB, config.wUserB>(user, packet.user);
        }

        void readTo(value_type& packet) const {
            vutil::read_if<hasId, config.wId>(id, packet.id);
            vutil::read<2>(resp, packet.resp);
            vutil::read_if<hasUserB, config.wUserB>(user, packet.user);
        }
    };
};

} // namespace detail

using detail::Signals;

} // namespace chext_test::amba::axi4::full

#endif /* CHEXT_TEST_AMBA_AXI4_FULL_SIGNALS_HPP_INCLUDED */
