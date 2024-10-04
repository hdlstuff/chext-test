#ifndef CHEXT_TEST_BITS_BITS_HPP_INCLUDED
#define CHEXT_TEST_BITS_BITS_HPP_INCLUDED

#include <chext_test/util/Exception.hpp>

#include <cstdint>

#include <string>
#include <type_traits>
#include <typeinfo>

#include <fmt/core.h>
#include <systemc>

namespace chext_test::bits {

/// @note Add other types of useful functionality that you want (like packets with the last signal?)

namespace detail {

#define CHEXT_TEST_IMPL_HAS_READ_TO_FOR(type)       \
    template<typename T, typename = void>           \
    struct has_read_to_##type : std::false_type {}; \
                                                    \
    template<typename T>                            \
    struct has_read_to_##type<T, std::void_t<decltype(std::declval<T>().read().to_##type())>> : std::true_type {};

CHEXT_TEST_IMPL_HAS_READ_TO_FOR(int)
CHEXT_TEST_IMPL_HAS_READ_TO_FOR(uint)
CHEXT_TEST_IMPL_HAS_READ_TO_FOR(long)
CHEXT_TEST_IMPL_HAS_READ_TO_FOR(ulong)
CHEXT_TEST_IMPL_HAS_READ_TO_FOR(int64)
CHEXT_TEST_IMPL_HAS_READ_TO_FOR(uint64)
CHEXT_TEST_IMPL_HAS_READ_TO_FOR(string)

#undef CHEXT_TEST_IMPL_HAS_READ_TO_FOR

template<typename T>
struct is_sc_signal : std::false_type {};

template<typename T, sc_core::sc_writer_policy WP>
struct is_sc_signal<sc_core::sc_signal<T, WP>> : std::true_type {};

template<typename Base, typename T, typename = void>
struct is_sc_signal_of : std::false_type {};

template<typename Base, typename T, sc_core::sc_writer_policy WP>
struct is_sc_signal_of<Base, sc_core::sc_signal<T, WP>, std::enable_if_t<std::is_base_of_v<Base, T>>> : std::true_type {};

template<typename T, typename = void>
struct is_sc_signal_of_integral : std::false_type {};

template<typename T, sc_core::sc_writer_policy WP>
struct is_sc_signal_of_integral<sc_core::sc_signal<T, WP>, std::enable_if_t<std::is_integral_v<T>>> : std::true_type {};

static_assert(is_sc_signal_of<sc_dt::sc_bv_base, sc_core::sc_signal<sc_dt::sc_bv<31>>>::value);
static_assert(!is_sc_signal_of<sc_dt::sc_lv_base, sc_core::sc_signal<sc_dt::sc_bv<31>>>::value);
static_assert(!is_sc_signal_of_integral<sc_core::sc_signal<sc_dt::sc_bv<31>>>::value);
static_assert(is_sc_signal_of_integral<sc_core::sc_signal<std::uint32_t>>::value);

template<typename T>
struct is_sc_signal_of_serializable {
    static constexpr bool value = is_sc_signal_of<sc_dt::sc_bv_base, T>::value || is_sc_signal_of<sc_dt::sc_lv_base, T>::value || is_sc_signal_of<sc_dt::sc_int_base, T>::value || is_sc_signal_of<sc_dt::sc_uint_base, T>::value;
};

} // namespace detail

template<typename T, typename = void>
struct BitsPeek {

#define CHEXT_TEST_IMPL_PEEK_FOR(param1, param2)                                                            \
    static void peek##param1(T const& t, param2& x) {                                                       \
        throw util::Exception(fmt::format("peek" #param1 " is not implemented for {}.", typeid(T).name())); \
    }

    CHEXT_TEST_IMPL_PEEK_FOR(Int, int)
    CHEXT_TEST_IMPL_PEEK_FOR(UInt, unsigned int)
    CHEXT_TEST_IMPL_PEEK_FOR(Long, long)
    CHEXT_TEST_IMPL_PEEK_FOR(ULong, unsigned long)
    CHEXT_TEST_IMPL_PEEK_FOR(Int32, std::int32_t)
    CHEXT_TEST_IMPL_PEEK_FOR(UInt32, std::uint32_t)
    CHEXT_TEST_IMPL_PEEK_FOR(Int64, std::int64_t)
    CHEXT_TEST_IMPL_PEEK_FOR(UInt64, std::uint64_t)
    CHEXT_TEST_IMPL_PEEK_FOR(String, std::string)

#undef CHEXT_TEST_IMPL_PEEK_FOR
};

template<typename T>
struct BitsPeek<T, std::enable_if_t<detail::is_sc_signal<T>::value>> {

#define CHEXT_TEST_IMPL_PEEK_FOR(param1, param2, param3)                                                        \
    static void peek##param1(T const& t, param3& x) {                                                           \
        if constexpr (detail::has_read_to_##param2<T>::value)                                                   \
            x = t.read().to_##param2();                                                                         \
        else if constexpr (!std::is_same_v<param3, std::string>)                                                \
            x = t.read();                                                                                       \
        else                                                                                                    \
            throw util::Exception(fmt::format("peek" #param1 " is not implemented for {}.", typeid(T).name())); \
    }

    CHEXT_TEST_IMPL_PEEK_FOR(Int, int, int)
    CHEXT_TEST_IMPL_PEEK_FOR(UInt, uint, unsigned int)
    CHEXT_TEST_IMPL_PEEK_FOR(Long, long, long)
    CHEXT_TEST_IMPL_PEEK_FOR(ULong, ulong, unsigned long)
    CHEXT_TEST_IMPL_PEEK_FOR(Int32, int, std::int32_t)
    CHEXT_TEST_IMPL_PEEK_FOR(UInt32, uint, std::uint32_t)
    CHEXT_TEST_IMPL_PEEK_FOR(Int64, int64, std::int64_t)
    CHEXT_TEST_IMPL_PEEK_FOR(UInt64, uint64, std::uint64_t)
    CHEXT_TEST_IMPL_PEEK_FOR(String, string, std::string)

#undef CHEXT_TEST_IMPL_PEEK_FOR
};

template<typename T, typename = void>
struct BitsPoke {

#define CHEXT_TEST_IMPL_POKE_FOR(param1, param2)                                                            \
    static void poke##param1(T& t, param2 const& x) {                                                       \
        throw util::Exception(fmt::format("peek" #param1 " is not implemented for {}.", typeid(T).name())); \
    }

    CHEXT_TEST_IMPL_POKE_FOR(Int, int)
    CHEXT_TEST_IMPL_POKE_FOR(UInt, unsigned int)
    CHEXT_TEST_IMPL_POKE_FOR(Long, long)
    CHEXT_TEST_IMPL_POKE_FOR(ULong, unsigned long)
    CHEXT_TEST_IMPL_POKE_FOR(Int32, std::int32_t)
    CHEXT_TEST_IMPL_POKE_FOR(UInt32, std::uint32_t)
    CHEXT_TEST_IMPL_POKE_FOR(Int64, std::int64_t)
    CHEXT_TEST_IMPL_POKE_FOR(UInt64, std::uint64_t)
    CHEXT_TEST_IMPL_POKE_FOR(String, std::string)

#undef CHEXT_TEST_IMPL_POKE_FOR
};

template<typename T>
struct BitsPoke<T, std::enable_if_t<detail::is_sc_signal<T>::value>> {
#define CHEXT_TEST_IMPL_POKE_FOR(param1, param2)      \
    static void poke##param1(T& t, param2 const& x) { \
        t.write(x);                                   \
    }

    CHEXT_TEST_IMPL_POKE_FOR(Int, int)
    CHEXT_TEST_IMPL_POKE_FOR(UInt, unsigned int)
    CHEXT_TEST_IMPL_POKE_FOR(Long, long)
    CHEXT_TEST_IMPL_POKE_FOR(ULong, unsigned long)
    CHEXT_TEST_IMPL_POKE_FOR(Int32, std::int32_t)
    CHEXT_TEST_IMPL_POKE_FOR(UInt32, std::uint32_t)
    CHEXT_TEST_IMPL_POKE_FOR(Int64, std::int64_t)
    CHEXT_TEST_IMPL_POKE_FOR(UInt64, std::uint64_t)

    static void pokeString(T& t, std::string const& x) {
        throw util::Exception(fmt::format("peekString is not implemented for {}.", typeid(T).name()));
    }

#undef CHEXT_TEST_IMPL_POKE_FOR
};

}; // namespace chext_test::bits

#endif /* CHEXT_TEST_BITS_BITS_HPP_INCLUDED */
