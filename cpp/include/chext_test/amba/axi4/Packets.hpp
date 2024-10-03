#ifndef CHEXT_TEST_AMBA_AXI4_CHANNELS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_CHANNELS_HPP_INCLUDED

#include <systemc>

namespace chext_test::amba::axi4 {

namespace detail {

using std::uint8_t;
using namespace sc_dt;

struct AddressPacket {
    sc_bv_base id;
    sc_bv_base addr;
    uint8_t len;
    uint8_t size;
    uint8_t burst;
    uint8_t lock;
    uint8_t qos;
    uint8_t region;
    sc_bv_base user;
};

struct ReadDataPacket {
    sc_bv_base id;
    sc_bv_base data;
    uint8_t resp;
    bool last;
    sc_bv_base user;
};

struct WriteDataPacket {
    sc_bv_base data;
    sc_bv_base strb;
    bool last;
    sc_bv_base user;
};

struct WriteResponsePacket {
    sc_bv_base id;
    uint8_t resp;
    sc_bv_base user;
};

} // namespace detail

using detail::AddressPacket;
using detail::ReadDataPacket;
using detail::WriteDataPacket;
using detail::WriteResponsePacket;

} // namespace chext_test::amba::axi4

#endif /* CHEXT_TEST_AMBA_AXI4_CHANNELS_HPP_INCLUDED */
