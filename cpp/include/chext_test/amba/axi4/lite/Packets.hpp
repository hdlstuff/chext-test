#ifndef CHEXT_TEST_AMBA_AXI4_LITE_PACKETS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_LITE_PACKETS_HPP_INCLUDED

#include <chext_test/amba/axi4/Config.hpp>
#include <jqr/comp_eq.hpp>
#include <jqr/dump.hpp>
#include <systemc>

namespace chext_test::amba::axi4::lite {

namespace detail {

using sc_dt::sc_bv_base;

using std::uint8_t;

namespace o = jqr::opts;
using chext_test::util::ScDumpOptions;

struct AddressBase {
protected:
    explicit AddressBase(Config const& config);

public:
    AddressBase(AddressBase const&) = default;
    AddressBase& operator=(AddressBase const&) = delete;

    sc_bv_base addr;
    uint8_t prot;
};

struct ReadAddress : AddressBase {
    explicit ReadAddress(Config const& config);

    ReadAddress(ReadAddress const&) = default;
    ReadAddress& operator=(ReadAddress const&) = delete;

    JQR_DECL(
        ReadAddress,
        JQR_MEMBER(addr, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(prot, o::dump_fmt { "{:#06b}" })
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
        JQR_MEMBER(addr, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(prot, o::dump_fmt { "{:#06b}" })
    )

    JQR_TO_STRING
    JQR_OSTREAM
    JQR_COMP_EQ
};

struct ReadData {
    explicit ReadData(Config const& config);

    ReadData(ReadData const&) = default;
    ReadData& operator=(ReadData const&) = delete;

    sc_bv_base data;
    uint8_t resp;

    JQR_DECL(
        ReadData,
        JQR_MEMBER(data, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(resp, o::dump_fmt { "{:#04b}" })
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

    JQR_DECL(
        WriteData,
        JQR_MEMBER(data, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(strb, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 })
    )

    JQR_TO_STRING
    JQR_OSTREAM
    JQR_COMP_EQ
};

struct WriteResponse {
    explicit WriteResponse(Config const& config);

    WriteResponse(WriteResponse const&) = default;
    WriteResponse& operator=(WriteResponse const&) = delete;

    uint8_t resp;

    JQR_DECL(
        WriteResponse,
        JQR_MEMBER(resp, o::dump_fmt { "{:#04b}" })
    )

    JQR_TO_STRING
    JQR_OSTREAM
    JQR_COMP_EQ
};

inline AddressBase::AddressBase(Config const& config)
    : addr((int)config.wAddr)
    , prot(0) {}

inline ReadAddress::ReadAddress(Config const& config)
    : AddressBase(config) {}

inline WriteAddress::WriteAddress(Config const& config)
    : AddressBase(config) {}

inline ReadData::ReadData(Config const& config)
    : data((int)config.wData)
    , resp(0) {}

inline WriteData::WriteData(Config const& config)
    : data((int)config.wData)
    , strb((int)config.wStrobe) {}

inline WriteResponse::WriteResponse(Config const&)
    : resp(0) {}

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

} // namespace chext_test::amba::axi4::lite

#endif /* CHEXT_TEST_AMBA_AXI4_LITE_PACKETS_HPP_INCLUDED */
