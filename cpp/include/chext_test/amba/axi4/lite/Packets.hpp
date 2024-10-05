#ifndef CHEXT_TEST_AMBA_AXI4_LITE_PACKETS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_LITE_PACKETS_HPP_INCLUDED

#include <systemc>

namespace chext_test::amba::axi4::lite {

namespace detail {

// clang-format off
using sc_dt::sc_bv_base;

using std::uint8_t;
using std::uint64_t;
// clang-format on

struct Packets {
    struct Address {
        sc_bv_base addr;
        uint8_t prot;
    };

    using WriteAddress = Address;
    using ReadAddress = Address;

    struct ReadData {
        uint64_t data;
        uint8_t resp;
    };

    struct WriteData {
        uint64_t data;
        uint8_t strb;
    };

    struct WriteResponse {
        uint8_t resp;
    };
};

} // namespace detail

using detail::Packets;

} // namespace chext_test::amba::axi4::lite

#endif /* CHEXT_TEST_AMBA_AXI4_LITE_PACKETS_HPP_INCLUDED */
