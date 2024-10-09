/**
 * @file jqreflect.hpp
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
// Helper to check if a type exists in a tuple
template <typename T, typename Tuple>
struct has_type;

template <typename T, typename... Ts>
struct has_type<T, std::tuple<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};

template <typename T, typename Tuple, bool = has_type<T, Tuple>::value>
struct get_or_else_impl;

template <typename T, typename... Ts>
struct get_or_else_impl<T, std::tuple<Ts...>, true> {
    static constexpr T get(const std::tuple<Ts...>& tpl, const T& default_value) {
        return std::get<T>(tpl);  // Returns the tuple element of type T
    }
};

template <typename T, typename... Ts>
struct get_or_else_impl<T, std::tuple<Ts...>, false> {
    static constexpr T get(const std::tuple<Ts...>& tpl, const T& default_value) {
        return default_value;  // Returns the default value
    }
};

template <typename T, typename... Ts>
T get_or_else(const std::tuple<Ts...>& tpl, const T& default_value) {
    return get_or_else_impl<T, std::tuple<Ts...>>::get(tpl, default_value);
}

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

template<typename T, typename Enable = void>
struct mk_dump {
    static void dump(T const&, fmt::memory_buffer& buf, const char* fmt) {
        buf.append(std::string_view("???"));
    }
};

template<typename T>
struct mk_dump<
    T,
    std::void_t<decltype(&T::dump)>> {
    static void dump(T const& t, fmt::memory_buffer& buf, const char* fmt) {
        t.dump(buf, fmt);
    }
};

template <typename T>
struct mk_dump_fmt_impl {
    static constexpr void dump(T const& t, fmt::memory_buffer& buf, const char* fmt) {
        fmt::format_to(std::back_inserter(buf), fmt, t);
    }
};

#define JQR_DEFINE_FMT_DUMP(type) template <> struct mk_dump<type, void> : mk_dump_fmt_impl<type> {};

JQR_DEFINE_FMT_DUMP(int)
JQR_DEFINE_FMT_DUMP(unsigned int)
JQR_DEFINE_FMT_DUMP(long)
JQR_DEFINE_FMT_DUMP(unsigned long)
JQR_DEFINE_FMT_DUMP(long long)
JQR_DEFINE_FMT_DUMP(unsigned long long)
JQR_DEFINE_FMT_DUMP(std::string)

template<typename T>
inline void dump(T const& t, fmt::memory_buffer& buf, const char* fmt = nullptr) {
    mk_dump<T>::dump(t, buf, fmt);
}

template<typename T>
inline std::string to_string(T const& t, const char* fmt = nullptr) {
    auto buf = fmt::memory_buffer {};
    dump(t, buf, fmt);
    return std::string { buf.data(), buf.size() };
}

namespace opts {

struct hash {
    bool b;
};

struct dump {
    bool b;
};

struct dump_name {
    bool b;
};

struct dump_fmt {
    const char* fmt;
};

}; // namespace opts

template<typename T, typename... Options>
struct member_info_t {
    using type = T;

    const char* name;
    T& t;

    std::tuple<Options...> options;
};

template<typename T, typename... Options>
auto member_info(const char* name, T& t, Options... options) {
    return member_info_t<T, Options...> { name, t, std::make_tuple(options...) };
}

} // namespace jqr

#define JQR_MEMBER(x, ...) jqr::member_info(#x, x __VA_OPT__(, ) __VA_ARGS__)

#define JQR_IMPL_COMPARISON_FOR(op) \
    friend constexpr bool operator op(this_struct const& a, this_struct const& b) { return a.tie() op b.tie(); }

#define JQR_HAS_COMPARISON      \
    JQR_IMPL_COMPARISON_FOR(==) \
    JQR_IMPL_COMPARISON_FOR(!=) \
    JQR_IMPL_COMPARISON_FOR(<)  \
    JQR_IMPL_COMPARISON_FOR(<=) \
    JQR_IMPL_COMPARISON_FOR(>)  \
    JQR_IMPL_COMPARISON_FOR(>=)

#define JQR_DECL(name, ...)                                      \
    static constexpr const char* struct_name() { return #name; } \
    using this_struct = name;                                    \
                                                                 \
    constexpr auto members() noexcept {                          \
        return std::make_tuple(__VA_ARGS__);                     \
    }                                                            \
                                                                 \
    constexpr auto members() const noexcept {                    \
        return std::make_tuple(__VA_ARGS__);                     \
    }

#endif /* JQREFLECT_HPP_INCLUDED */
