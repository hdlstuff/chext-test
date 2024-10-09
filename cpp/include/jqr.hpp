/**
 * @file jqr.hpp
 * @author Canberk Sönmez (canberk.sonmez.409@gmail.com)
 * @brief jnbrq's simple reflection library
 * @date 2024-10-09
 *
 * Copyright (c) Canberk Sönmez 2024
 *
 */

#ifndef JQREFLECT_HPP_INCLUDED
#define JQREFLECT_HPP_INCLUDED

#include <fmt/format.h>

#include <cstdint>

#include <string>
#include <type_traits>
#include <utility>

namespace jqr {

namespace opts {

#define JQR_DEFINE_OPT(name, type) \
    struct name {                  \
        type v;                    \
    }

// hashing options
JQR_DEFINE_OPT(hash, bool);

// dumping options
JQR_DEFINE_OPT(dump, bool);
JQR_DEFINE_OPT(dump_name, bool);
JQR_DEFINE_OPT(dump_fmt, const char*);
JQR_DEFINE_OPT(dump_class, bool);
JQR_DEFINE_OPT(dump_paren, bool);

#undef JQR_DEFINE_OPT

}; // namespace opts

template<typename T, typename Enable = void>
struct mk_hash {
    static std::size_t hash(T const&) { return 0; }
};

template<typename T>
struct mk_hash<T, std::void_t<decltype(std::hash<T> {})>> {
    static std::size_t hash(T const& t) {
        return std::hash<T> {}(t);
    }
};

template<typename T>
inline std::size_t hash(T const& t) {
    return mk_hash<T>::hash(t);
}

template<typename T, typename = void>
struct has_member_dump : std::false_type {};

template<typename T>
struct has_member_dump<
    T,
    std::void_t<
        decltype( //
            std::declval<T>().dump(std::declval<fmt::memory_buffer&>(), std::make_tuple())
        )>> : std::true_type {};

template<typename T, typename... Options>
void dump(T const& t, fmt::memory_buffer& buf, std::tuple<Options...> const& options);


} // namespace jqr


#define JQR_IMPL_COMPARISON_FOR(op) \
    friend constexpr bool operator op(this_struct const& a, this_struct const& b) { return a.tie() op b.tie(); }

#define JQR_HAS_COMPARISON      \
    JQR_IMPL_COMPARISON_FOR(==) \
    JQR_IMPL_COMPARISON_FOR(!=) \
    JQR_IMPL_COMPARISON_FOR(<)  \
    JQR_IMPL_COMPARISON_FOR(<=) \
    JQR_IMPL_COMPARISON_FOR(>)  \
    JQR_IMPL_COMPARISON_FOR(>=)

#endif /* JQREFLECT_HPP_INCLUDED */
