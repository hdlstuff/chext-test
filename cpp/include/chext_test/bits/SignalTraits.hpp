#ifndef CHEXT_TEST_BITS_SIGNALTRAITS_HPP_INCLUDED
#define CHEXT_TEST_BITS_SIGNALTRAITS_HPP_INCLUDED

#include <systemc>
#include <type_traits>

namespace chext_test::bits {

template<typename T>
struct is_sc_signal : std::false_type {};

template<typename T, sc_core::sc_writer_policy WP>
struct is_sc_signal<sc_core::sc_signal<T, WP>> : std::true_type {};

template<typename Base, typename T, typename = void>
struct is_sc_signal_of : std::false_type {};

template<typename Base, typename T, sc_core::sc_writer_policy WP>
struct is_sc_signal_of<Base, sc_core::sc_signal<T, WP>, std::enable_if_t<std::is_base_of_v<Base, T> && !std::is_same_v<Base, T>>> : std::true_type {};

template<typename T, typename = void>
struct is_sc_signal_of_integral : std::false_type {};

template<typename T, sc_core::sc_writer_policy WP>
struct is_sc_signal_of_integral<sc_core::sc_signal<T, WP>, std::enable_if_t<std::is_integral_v<T>>> : std::true_type {};

template<typename T>
struct is_sc_signal_of_serializable {
    static constexpr bool value = is_sc_signal_of<sc_dt::sc_bv_base, T>::value || is_sc_signal_of<sc_dt::sc_lv_base, T>::value || is_sc_signal_of<sc_dt::sc_int_base, T>::value || is_sc_signal_of<sc_dt::sc_uint_base, T>::value;
};

// some basic compile-time tests
static_assert(is_sc_signal_of<sc_dt::sc_bv_base, sc_core::sc_signal<sc_dt::sc_bv<31>>>::value);
static_assert(!is_sc_signal_of<sc_dt::sc_lv_base, sc_core::sc_signal<sc_dt::sc_bv<31>>>::value);
static_assert(!is_sc_signal_of_integral<sc_core::sc_signal<sc_dt::sc_bv<31>>>::value);
static_assert(is_sc_signal_of_integral<sc_core::sc_signal<std::uint32_t>>::value);

} // namespace chext_test::bits

#endif /* CHEXT_TEST_BITS_SIGNALTRAITS_HPP_INCLUDED */
