#ifndef CHEXT_TEST_BITS_READHELPER_HPP_INCLUDED
#define CHEXT_TEST_BITS_READHELPER_HPP_INCLUDED

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
 * @brief This template class allows reading from a signal-like `Source` to data type `Target`.
 * You should provide specialization to cover your own types with it.
 *
 * @tparam Source
 * @tparam Target
 * @tparam SFINAE
 */
template<typename Source, typename Target, typename = void /* for SFINAE */>
struct ReadHelper {
    static void read(Source const& source, Target& target) {
        throw util::Exception(fmt::format("ReadHelper<{}, {}> is not implemented!", typeid(Source).name(), typeid(Target).name()));
    }
};

#define CHEXT_TEST_IMPL_READ_HELPER_FOR_SERIALIZABLE(type, func) \
    template<typename Source>                                    \
    struct ReadHelper<                                           \
        Source,                                                  \
        type,                                                    \
        std::enable_if_t<                                        \
            is_sc_signal_of_serializable<Source>::value>> {      \
        static void read(Source const& source, type& target) {   \
            target = source.read().func();                       \
        }                                                        \
    };

// CHEXT_TEST_IMPL_READ_HELPER_FOR_SERIALIZABLE(int, to_int)
// CHEXT_TEST_IMPL_READ_HELPER_FOR_SERIALIZABLE(unsigned int, to_uint)
// CHEXT_TEST_IMPL_READ_HELPER_FOR_SERIALIZABLE(long, to_long)
// CHEXT_TEST_IMPL_READ_HELPER_FOR_SERIALIZABLE(unsigned long, to_ulong)
// CHEXT_TEST_IMPL_READ_HELPER_FOR_SERIALIZABLE(std::int32_t, to_int)
// CHEXT_TEST_IMPL_READ_HELPER_FOR_SERIALIZABLE(std::uint32_t, to_uint)
CHEXT_TEST_IMPL_READ_HELPER_FOR_SERIALIZABLE(std::int64_t, to_int64)
CHEXT_TEST_IMPL_READ_HELPER_FOR_SERIALIZABLE(std::uint64_t, to_uint64)
CHEXT_TEST_IMPL_READ_HELPER_FOR_SERIALIZABLE(std::string, to_string)

template<typename Source, typename Target>
struct ReadHelper< // clang-format off
    Source,
    Target,
    std::enable_if_t<
        is_sc_signal_of_integral<Source>::value &&
        std::is_integral_v<Target>
    >> { // clang-format on
    static void read(Source const& source, Target& target) {
        target = source.read();
    }
};

} // namespace chext_test::bits

#endif /* CHEXT_TEST_BITS_READHELPER_HPP_INCLUDED */
