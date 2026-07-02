#ifndef CHEXT_TEST_UTIL_VERILATOR_PORT_HPP_INCLUDED
#define CHEXT_TEST_UTIL_VERILATOR_PORT_HPP_INCLUDED

#include <cstdint>
#include <systemc>
#include <type_traits>

#ifndef CHEXT_TEST_VERILATOR_PINS_BV_VALUE
#define CHEXT_TEST_VERILATOR_PINS_BV_VALUE 65
#endif

namespace chext_test::util {

template<unsigned WIDTH>
struct verilator_port_type {
    using type = sc_dt::sc_bv<WIDTH>;
};

template<>
struct verilator_port_type<1> {
    using type = bool;
};

template<unsigned WIDTH>
    requires(WIDTH >= 2 && WIDTH <= 32 && WIDTH < CHEXT_TEST_VERILATOR_PINS_BV_VALUE)
struct verilator_port_type<WIDTH> {
    using type = std::uint32_t;
};

template<unsigned WIDTH>
    requires(WIDTH >= 33 && WIDTH <= 64 && WIDTH < CHEXT_TEST_VERILATOR_PINS_BV_VALUE)
struct verilator_port_type<WIDTH> {
    using type = std::uint64_t;
};

template<unsigned WIDTH>
using verilator_port_t = typename verilator_port_type<WIDTH>::type;

template<unsigned WIDTH>
struct verilator_port_helper {
    using type = verilator_port_t<WIDTH>;

    template<sc_core::sc_writer_policy WP>
    static auto peek(sc_core::sc_signal<type, WP> const& signal) {
        return signal.read();
    }

    template<sc_core::sc_writer_policy WP, typename ValueT>
    static void poke(sc_core::sc_signal<type, WP>& signal, ValueT const& value) {
        if constexpr (std::is_same_v<type, bool>)
            signal.write(static_cast<bool>(value));
        else if constexpr (std::is_integral_v<type>)
            signal.write(static_cast<type>(toUInt64(value)));
        else
            signal.write(value);
    }

    template<typename ValueT>
    static std::uint64_t toUInt64(ValueT const& value) {
        if constexpr (std::is_integral_v<ValueT>)
            return static_cast<std::uint64_t>(value);
        else
            return value.to_uint64();
    }
};

template<unsigned WIDTH>
using verilator_port_signal_t = typename verilator_port_helper<WIDTH>::type;

template<unsigned WIDTH, sc_core::sc_writer_policy WP>
std::uint64_t verilator_port_read_uint(sc_core::sc_signal<verilator_port_signal_t<WIDTH>, WP> const& signal) {
    return verilator_port_helper<WIDTH>::toUInt64(verilator_port_helper<WIDTH>::peek(signal));
}

template<unsigned WIDTH, sc_core::sc_writer_policy WP, typename ValueT>
void verilator_port_write(sc_core::sc_signal<verilator_port_signal_t<WIDTH>, WP>& signal, ValueT const& value) {
    verilator_port_helper<WIDTH>::poke(signal, value);
}

template<unsigned WIDTH, typename ValueT>
void verilator_port_assign_bv(sc_dt::sc_bv_base& target, ValueT const& value) {
    if constexpr (std::is_integral_v<ValueT>)
        target = sc_dt::sc_bv<WIDTH>(value);
    else
        target = value;
}

template<unsigned WIDTH, sc_core::sc_writer_policy WP>
void verilator_port_read_bv(sc_core::sc_signal<verilator_port_signal_t<WIDTH>, WP> const& signal, sc_dt::sc_bv_base& target) {
    verilator_port_assign_bv<WIDTH>(target, verilator_port_helper<WIDTH>::peek(signal));
}

} // namespace chext_test::util

#endif /* CHEXT_TEST_UTIL_VERILATOR_PORT_HPP_INCLUDED */
