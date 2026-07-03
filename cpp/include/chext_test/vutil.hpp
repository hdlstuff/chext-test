#ifndef CHEXT_TEST_VUTIL_HPP_INCLUDED
#define CHEXT_TEST_VUTIL_HPP_INCLUDED

#include <cstdint>
#include <systemc>
#include <type_traits>

#ifndef CHEXT_TEST_VERILATOR_PINS_BV_VALUE
#define CHEXT_TEST_VERILATOR_PINS_BV_VALUE 65
#endif

namespace chext_test::vutil {

namespace detail {

template<unsigned WIDTH, typename = void>
struct payload_impl {
    using type = sc_dt::sc_bv<WIDTH>;
};

template<>
struct payload_impl<1, void> {
    using type = bool;
};

template<unsigned WIDTH>
struct payload_impl<
    WIDTH,
    std::enable_if_t<WIDTH >= 2 && WIDTH <= 32 && WIDTH < CHEXT_TEST_VERILATOR_PINS_BV_VALUE>> {
    using type = std::uint32_t;
};

template<unsigned WIDTH>
struct payload_impl<
    WIDTH,
    std::enable_if_t<WIDTH >= 33 && WIDTH <= 64 && WIDTH < CHEXT_TEST_VERILATOR_PINS_BV_VALUE>> {
    using type = std::uint64_t;
};

} // namespace detail

// C++ payload type Verilator exposes for a port with WIDTH bits.
template<unsigned WIDTH>
using payload_t = typename detail::payload_impl<WIDTH>::type;

// SystemC signal carrying the Verilator-selected payload type.
template<unsigned WIDTH, sc_core::sc_writer_policy WP = sc_core::SC_ONE_WRITER>
using signal_t = sc_core::sc_signal<payload_t<WIDTH>, WP>;

constexpr unsigned nonzero_width(unsigned width) {
    return width > 0 ? width : 1;
}

constexpr unsigned width_or(unsigned width, unsigned fallback) {
    return width > 0 ? width : fallback;
}

struct absent_signal {
    explicit absent_signal(char const*) {}
};

// Optional signal helper for protocols where width 0 means the port is absent.
template<bool PRESENT, unsigned WIDTH, sc_core::sc_writer_policy WP = sc_core::SC_ONE_WRITER>
using optional_signal_t = std::conditional_t<PRESENT, signal_t<nonzero_width(WIDTH), WP>, absent_signal>;

namespace detail {

// Normalizes the operations that differ between Verilator payload choices.
// For a given WIDTH, `type` is bool/integer/sc_bv depending on --pins-bv;
// this helper provides the small conversions needed by the public read/write
// API without exposing those representation branches to protocol code.
template<unsigned WIDTH>
struct payload_helper {
    using type = payload_t<WIDTH>;

    template<sc_core::sc_writer_policy WP>
    static auto peek(sc_core::sc_signal<type, WP> const& signal) {
        return signal.read();
    }

    template<sc_core::sc_writer_policy WP, typename ValueT>
    static void poke(sc_core::sc_signal<type, WP>& signal, ValueT const& value) {
        if constexpr (std::is_same_v<type, bool>)
            signal.write(static_cast<bool>(value));
        else if constexpr (std::is_integral_v<type>)
            signal.write(static_cast<type>(to_uint64(value)));
        else
            signal.write(value);
    }

    template<typename ValueT>
    static std::uint64_t to_uint64(ValueT const& value) {
        if constexpr (std::is_integral_v<ValueT>)
            return static_cast<std::uint64_t>(value);
        else
            return value.to_uint64();
    }
};

} // namespace detail

// Read a Verilator-selected signal representation into a destination value.
template<unsigned WIDTH, sc_core::sc_writer_policy WP, typename ValueT>
void read(signal_t<WIDTH, WP> const& signal, ValueT& value) {
    auto const payload = detail::payload_helper<WIDTH>::peek(signal);

    if constexpr (std::is_integral_v<ValueT>)
        value = static_cast<ValueT>(detail::payload_helper<WIDTH>::to_uint64(payload));
    else
        value = payload;
}

// Write a value into a Verilator-selected signal representation.
template<unsigned WIDTH, sc_core::sc_writer_policy WP, typename ValueT>
void write(signal_t<WIDTH, WP>& signal, ValueT const& value) {
    detail::payload_helper<WIDTH>::poke(signal, value);
}

// Write only when a protocol field is present.
template<bool PRESENT, unsigned WIDTH, typename SignalT, typename ValueT>
void write_if(SignalT& signal, ValueT const& value) {
    if constexpr (PRESENT)
        write<nonzero_width(WIDTH)>(signal, value);
}

// Read only when a protocol field is present.
template<bool PRESENT, unsigned WIDTH, typename SignalT, typename ValueT>
void read_if(SignalT const& signal, ValueT& value) {
    if constexpr (PRESENT)
        read<nonzero_width(WIDTH)>(signal, value);
}

} // namespace chext_test::vutil

#endif /* CHEXT_TEST_VUTIL_HPP_INCLUDED */
