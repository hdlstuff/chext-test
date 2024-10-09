#ifndef CHEXT_TEST_AMBA_AXI4_FULL_PACKETS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_FULL_PACKETS_HPP_INCLUDED

#include <systemc>

#include <jqr/comp_eq.hpp>
#include <jqr/dump.hpp>

namespace chext_test::amba::axi4::full {

namespace detail {

using sc_dt::sc_bv_base;
using std::uint8_t;

struct Packets {
    struct Address {
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

        JQR_DECL(
            Address,
            JQR_MEMBER(id),
            JQR_MEMBER(addr),
            JQR_MEMBER(len),
            JQR_MEMBER(size),
            JQR_MEMBER(burst),
            JQR_MEMBER(lock),
            JQR_MEMBER(cache),
            JQR_MEMBER(prot),
            JQR_MEMBER(qos),
            JQR_MEMBER(region),
            JQR_MEMBER(user)
        )

        JQR_TO_STRING
        JQR_COMP_EQ
    };

    using WriteAddress = Address;
    using ReadAddress = Address;

    struct ReadData {
        sc_bv_base id;
        sc_bv_base data;
        uint8_t resp;
        bool last;
        sc_bv_base user;

        JQR_DECL(
            ReadData,
            JQR_MEMBER(id),
            JQR_MEMBER(data),
            JQR_MEMBER(resp),
            JQR_MEMBER(last),
            JQR_MEMBER(user)
        )

        JQR_TO_STRING
        JQR_COMP_EQ
    };

    struct WriteData {
        sc_bv_base data;
        sc_bv_base strb;
        bool last;
        sc_bv_base user;

        JQR_DECL(
            WriteData,
            JQR_MEMBER(data),
            JQR_MEMBER(strb),
            JQR_MEMBER(last),
            JQR_MEMBER(user)
        )

        JQR_TO_STRING
        JQR_COMP_EQ
    };

    struct WriteResponse {
        sc_bv_base id;
        uint8_t resp;
        sc_bv_base user;

        JQR_DECL(
            WriteResponse,
            JQR_MEMBER(id),
            JQR_MEMBER(resp),
            JQR_MEMBER(user)
        )

        JQR_TO_STRING
        JQR_COMP_EQ
    };
};

} // namespace detail

using detail::Packets;

} // namespace chext_test::amba::axi4::full

#endif /* CHEXT_TEST_AMBA_AXI4_FULL_PACKETS_HPP_INCLUDED */
