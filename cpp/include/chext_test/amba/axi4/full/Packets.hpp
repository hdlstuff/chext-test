#ifndef CHEXT_TEST_AMBA_AXI4_FULL_PACKETS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_FULL_PACKETS_HPP_INCLUDED

#include <systemc>

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
    };

    using WriteAddress = Address;
    using ReadAddress = Address;

    struct ReadData {
        sc_bv_base id;
        sc_bv_base data;
        uint8_t resp;
        bool last;
        sc_bv_base user;
    };

    struct WriteData {
        sc_bv_base data;
        sc_bv_base strb;
        bool last;
        sc_bv_base user;
    };

    struct WriteResponse {
        sc_bv_base id;
        uint8_t resp;
        sc_bv_base user;
    };
};

} // namespace detail

using detail::Packets;

} // namespace chext_test::amba::axi4::full

#endif /* CHEXT_TEST_AMBA_AXI4_FULL_PACKETS_HPP_INCLUDED */
