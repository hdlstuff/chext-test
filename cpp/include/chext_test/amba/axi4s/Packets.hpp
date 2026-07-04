#ifndef CHEXT_TEST_AMBA_AXI4S_PACKETS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4S_PACKETS_HPP_INCLUDED

#include <chext_test/amba/axi4s/Config.hpp>
#include <chext_test/util/ScDump.hpp>
#include <chext_test/util/Util.hpp>

#include <jqr/comp_eq.hpp>
#include <jqr/dump.hpp>

#include <systemc>

namespace chext_test::amba::axi4s {

namespace detail {

namespace o = jqr::opts;
using chext_test::util::ScDumpOptions;

struct Packet {
    explicit Packet(Config const& config)
        : data((int)config.wData)
        , strb((int)config.wStrobe)
        , keep((int)config.wKeep)
        , last(false)
        , id((int)util::width_or(config.wId, 32))
        , dest((int)util::width_or(config.wDest, 32))
        , user((int)util::width_or(config.wUser, 32)) {}

    Packet(Packet const&) = default;
    Packet& operator=(Packet const&) = delete;

    sc_dt::sc_bv_base data;
    sc_dt::sc_bv_base strb;
    sc_dt::sc_bv_base keep;
    bool last;
    sc_dt::sc_bv_base id;
    sc_dt::sc_bv_base dest;
    sc_dt::sc_bv_base user;

    JQR_DECL(
        Packet,
        JQR_MEMBER(data, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(strb, ScDumpOptions { .numrep = sc_dt::SC_BIN, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(keep, ScDumpOptions { .numrep = sc_dt::SC_BIN, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(last, o::dump_fmt { "{:#03b}" }),
        JQR_MEMBER(id, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(dest, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 }),
        JQR_MEMBER(user, ScDumpOptions { .numrep = sc_dt::SC_HEX, .hasPrefix = true, .groupWidth = 4 })
    )

    JQR_TO_STRING
    JQR_OSTREAM
    JQR_COMP_EQ
};

} // namespace detail

using detail::Packet;

} // namespace chext_test::amba::axi4s

#endif /* CHEXT_TEST_AMBA_AXI4S_PACKETS_HPP_INCLUDED */
