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

template<typename T, typename = void>
struct has_read : std::false_type {};

template<typename T>
struct has_read<T, std::void_t<decltype(std::declval<T>().read())>> : std::true_type {};

} // namespace detail

template<typename T, typename = void>
struct BitsPeek {

#define CHEXT_TEST_IMPL_PEEK_FOR(param1, param2, param3)                                                \
    static param3 peek##param1(T const& t) {                                                            \
        if constexpr (detail::has_read_to_##param2<T>::value)                                           \
            return t.read().to_##param2();                                                              \
        else if constexpr (detail::has_read<T>::value && !std::is_same_v<param3, std::string>)          \
            return t.read();                                                                            \
        else                                                                                            \
            throw util::Exception(fmt::format("peekInt is not implemented for {}.", typeid(T).name())); \
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

private:
    T const& t_;
};

template<typename T, typename = void>
struct BitsPoke {
    BitsPoke(T const& t) {}

    void pokeInt(int x) {
        throw util::Exception(fmt::format("pokeInt is not implemented for {}.", typeid(T).name()));
    }

    void pokeUInt(unsigned int x) {
        throw util::Exception(fmt::format("pokeUInt is not implemented for {}.", typeid(T).name()));
    }

    void pokeLong(long x) {
        throw util::Exception(fmt::format("pokeLong is not implemented for {}.", typeid(T).name()));
    }

    void pokeULong(unsigned long x) {
        throw util::Exception(fmt::format("pokeULong is not implemented for {}.", typeid(T).name()));
    }

    void pokeInt32(std::int32_t x) {
        throw util::Exception(fmt::format("pokeInt32 is not implemented for {}.", typeid(T).name()));
    }

    void pokeUInt32(std::uint32_t x) {
        throw util::Exception(fmt::format("pokeUInt32 is not implemented for {}.", typeid(T).name()));
    }

    void pokeInt64(std::int64_t x) {
        throw util::Exception(fmt::format("pokeInt64 is not implemented for {}.", typeid(T).name()));
    }

    void pokeUInt64(std::uint64_t x) {
        throw util::Exception(fmt::format("pokeUInt64 is not implemented for {}.", typeid(T).name()));
    }

    void pokeString(const std::string& str) {
        throw util::Exception(fmt::format("pokeString is not implemented for {}.", typeid(T).name()));
    }
};

}; // namespace chext_test::bits

#endif /* CHEXT_TEST_BITS_BITS_HPP_INCLUDED */
