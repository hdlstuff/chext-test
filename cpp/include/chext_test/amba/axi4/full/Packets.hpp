#ifndef CHEXT_TEST_AMBA_AXI4_FULL_PACKETS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_FULL_PACKETS_HPP_INCLUDED

#include <chext_test/amba/axi4/Config.hpp>
#include <chext_test/util/ScDump.hpp>
#include <jqr/comp_eq.hpp>
#include <jqr/dump.hpp>
#include <systemc>

namespace chext_test::amba::axi4::full {

namespace detail {

using sc_dt::sc_bv;
using sc_dt::sc_bv_base;
using std::uint8_t;

namespace o = jqr::opts;
using chext_test::util::ScDumpOptions;

struct AddressBase {
protected:
    explicit AddressBase(Config const& config, unsigned wUser);

public:
    AddressBase(AddressBase const&) = default;
    AddressBase& operator=(AddressBase const&) = delete;

    sc_bv_base id;
    sc_bv_base addr;
    uint8_t len;
    uint8_t size;
    uint8_t burst;
    uint8_t lock;
    uint8_t cache;
    uint8_t prot;
    uint8_t qos;
    uint8_t region;
    sc_bv_base user;
};

struct ReadAddress : AddressBase {
    explicit ReadAddress(Config const& config);

    ReadAddress(ReadAddress const&) = default;
    ReadAddress& operator=(ReadAddress const&) = delete;

    JQR_DECL(
        ReadAddress,
        JQR_MEMBER(id, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(addr, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(len, o::dump_fmt { "{0:#04x}=0d{0:03d}" }),
        JQR_MEMBER(size, o::dump_fmt { "{0:#05b}=0d{0:1d}" }),
        JQR_MEMBER(burst, o::dump_fmt { "{:#04b}" }),
        JQR_MEMBER(lock, o::dump_fmt { "{:#04b}" }),
        JQR_MEMBER(cache, o::dump_fmt { "{:#06b}" }),
        JQR_MEMBER(prot, o::dump_fmt { "{:#06b}" }),
        JQR_MEMBER(qos, o::dump_fmt { "{:#06b}" }),
        JQR_MEMBER(region, o::dump_fmt { "{:#03x}" }),
        JQR_MEMBER(user, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 })
    )

    JQR_TO_STRING
    JQR_OSTREAM
    JQR_COMP_EQ
};

struct WriteAddress : AddressBase {
    explicit WriteAddress(Config const& config);

    WriteAddress(WriteAddress const&) = default;
    WriteAddress& operator=(WriteAddress const&) = delete;

    JQR_DECL(
        WriteAddress,
        JQR_MEMBER(id, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(addr, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(len, o::dump_fmt { "{0:#04x}=0d{0:03d}" }),
        JQR_MEMBER(size, o::dump_fmt { "{0:#05b}=0d{0:1d}" }),
        JQR_MEMBER(burst, o::dump_fmt { "{:#04b}" }),
        JQR_MEMBER(lock, o::dump_fmt { "{:#04b}" }),
        JQR_MEMBER(cache, o::dump_fmt { "{:#06b}" }),
        JQR_MEMBER(prot, o::dump_fmt { "{:#06b}" }),
        JQR_MEMBER(qos, o::dump_fmt { "{:#06b}" }),
        JQR_MEMBER(region, o::dump_fmt { "{:#03x}" }),
        JQR_MEMBER(user, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 })
    )

    JQR_TO_STRING
    JQR_OSTREAM
    JQR_COMP_EQ
};

struct ReadData {
    explicit ReadData(Config const& config);

    ReadData(ReadData const&) = default;
    ReadData& operator=(ReadData const&) = delete;

    sc_bv_base id;
    sc_bv_base data;
    uint8_t resp;
    bool last;
    sc_bv_base user;

    JQR_DECL(
        ReadData,
        JQR_MEMBER(id, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(data, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(resp, o::dump_fmt { "{:#04b}" }),
        JQR_MEMBER(last, o::dump_fmt { "{:#03b}" }),
        JQR_MEMBER(user, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 })
    )

    JQR_TO_STRING
    JQR_OSTREAM
    JQR_COMP_EQ
};

struct WriteData {
    explicit WriteData(Config const& config);

    WriteData(WriteData const&) = default;
    WriteData& operator=(WriteData const&) = delete;

    sc_bv_base data;
    sc_bv_base strb;
    bool last;
    sc_bv_base user;

    JQR_DECL(
        WriteData,
        JQR_MEMBER(data, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(strb, ScDumpOptions { .numrep = sc_dt::SC_BIN, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(last, o::dump_fmt { "{:#03b}" }),
        JQR_MEMBER(user, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 })
    )

    JQR_TO_STRING
    JQR_OSTREAM
    JQR_COMP_EQ
};

struct WriteResponse {
    explicit WriteResponse(Config const& config);

    WriteResponse(WriteResponse const&) = default;
    WriteResponse& operator=(WriteResponse const&) = delete;

    sc_bv_base id;
    uint8_t resp;
    sc_bv_base user;

    JQR_DECL(
        WriteResponse,
        JQR_MEMBER(id, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(resp, o::dump_fmt { "{:#04b}" }),
        JQR_MEMBER(user, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 })
    )

    JQR_TO_STRING
    JQR_OSTREAM
    JQR_COMP_EQ
};

constexpr unsigned storageWidth(unsigned width, unsigned fallback = 32) {
    return width > 0 ? width : fallback;
}

inline AddressBase::AddressBase(Config const& config, unsigned wUser)
    : id((int)storageWidth(config.wId))
    , addr((int)config.wAddr)
    , len(0)
    , size(0)
    , burst(0)
    , lock(0)
    , cache(0)
    , prot(0)
    , qos(0)
    , region(0)
    , user((int)storageWidth(wUser)) {}

inline ReadAddress::ReadAddress(Config const& config)
    : AddressBase(config, config.wUserAR) {}

inline WriteAddress::WriteAddress(Config const& config)
    : AddressBase(config, config.wUserAW) {}

inline ReadData::ReadData(Config const& config)
    : id((int)storageWidth(config.wId))
    , data((int)config.wData)
    , resp(0)
    , last(false)
    , user((int)storageWidth(config.wUserR)) {}

inline WriteData::WriteData(Config const& config)
    : data((int)config.wData)
    , strb((int)config.wStrobe)
    , last(false)
    , user((int)storageWidth(config.wUserW)) {}

inline WriteResponse::WriteResponse(Config const& config)
    : id((int)storageWidth(config.wId))
    , resp(0)
    , user((int)storageWidth(config.wUserB)) {}

struct Packets {
    using ReadAddress = detail::ReadAddress;
    using WriteAddress = detail::WriteAddress;
    using ReadData = detail::ReadData;
    using WriteData = detail::WriteData;
    using WriteResponse = detail::WriteResponse;

};

} // namespace detail

using axi4::Config;
using axi4::ConfigSpec;
using detail::Packets;

} // namespace chext_test::amba::axi4::full

#endif /* CHEXT_TEST_AMBA_AXI4_FULL_PACKETS_HPP_INCLUDED */
