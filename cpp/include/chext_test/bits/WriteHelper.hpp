#ifndef CHEXT_TEST_BITS_WRITEHELPER_HPP_INCLUDED
#define CHEXT_TEST_BITS_WRITEHELPER_HPP_INCLUDED

#include <chext_test/bits/SignalTraits.hpp>
#include <chext_test/util/Exception.hpp>

#include <cstdint>
#include <string>
#include <type_traits>
#include <typeinfo>

#include <fmt/core.h>
#include <systemc>

namespace chext_test::bits {

/**
 * @brief This template class allows writing to a signal-like `Target` fro data type `Source`.
 * You should provide specialization to cover your own types with it.
 *
 * @tparam Source
 * @tparam Target
 * @tparam SFINAE
 */
template<typename Target, typename Source, typename = void /* for SFINAE */>
struct WriteHelper {
    static void write(Target& target, Source const& source) {
        throw util::Exception(fmt::format("WriteHelper<{}, {}> is not implemented!", typeid(Target).name(), typeid(Source).name()));
    }
};

#define CHEXT_TEST_IMPL_WRITE_HELPER_FOR_BV(type)               \
    template<unsigned W, sc_core::sc_writer_policy WP>          \
    struct WriteHelper<                                         \
        sc_core::sc_signal<sc_dt::sc_bv<W>, WP>,                \
        type,                                                   \
        void> {                                                 \
        template<typename Target>                               \
        static void write(Target& target, type const& source) { \
            target.write(source);                               \
        }                                                       \
    };

CHEXT_TEST_IMPL_WRITE_HELPER_FOR_BV(int)
CHEXT_TEST_IMPL_WRITE_HELPER_FOR_BV(unsigned int)
CHEXT_TEST_IMPL_WRITE_HELPER_FOR_BV(long)
CHEXT_TEST_IMPL_WRITE_HELPER_FOR_BV(unsigned long)
// CHEXT_TEST_IMPL_WRITE_HELPER_FOR_BV(std::int32_t)
// CHEXT_TEST_IMPL_WRITE_HELPER_FOR_BV(std::uint32_t)
CHEXT_TEST_IMPL_WRITE_HELPER_FOR_BV(std::int64_t)
CHEXT_TEST_IMPL_WRITE_HELPER_FOR_BV(std::uint64_t)

#if 0

template<typename Target>
struct WriteHelper<
    Target,
    std::string,
    std::enable_if_t<
        is_sc_signal_of_serializable<Target>::value>> {
    static void write(Target& target, std::string const& source) {
        using value_type = typename Target::value_type;
        target.write(value_type { source.c_str() });
    }
};

#endif

template<typename Target, typename Source>
struct WriteHelper< // clang-format off
    Target,
    Source,
    std::enable_if_t<
        is_sc_signal_of_integral<Target>::value &&
        std::is_integral_v<Source>
    >> { // clang-format on
    static void write(Target& target, Source const& source) {
        target.write(source);
    }
};

} // namespace chext_test::bits

#endif /* CHEXT_TEST_BITS_WRITEHELPER_HPP_INCLUDED */
