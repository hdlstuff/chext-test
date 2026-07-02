#ifndef CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED
#define CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED

#include <chext_test/elastic/Convert.hpp>
#include <chext_test/util/ReadyValid.hpp>
#include <chext_test/util/Reference.hpp>

#include <jqr/core.hpp>
#include <jqr/inspect.hpp>
#include <jqr/dump.hpp>
#include <jqr/comp_eq.hpp>

#include <systemc>

#include <fmt/core.h>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace chext_test::elastic {

JQR_DEFINE_INSPECT(HasRead, &T::read)
JQR_DEFINE_INSPECT(HasReadTo, &T::readTo)

template<typename SignalT, typename ValueT, typename Enable = void>
struct ReadHandler;

template<typename SignalT, typename ValueT>
struct ReadHandler<SignalT, ValueT, std::enable_if_t<HasRead_v<SignalT> && !HasReadTo_v<SignalT>>> {
    static void read(SignalT& signal, ValueT& value) {
        value = signal.read();
    }
};

template<typename SignalT, typename ValueT>
struct ReadHandler<SignalT, ValueT, std::enable_if_t<HasReadTo_v<SignalT>>> {
    static void read(SignalT& signal, ValueT& value) {
        signal.readTo(value);
    }
};

JQR_DEFINE_INSPECT(HasWrite, &T::write)
JQR_DEFINE_INSPECT(HasWriteFrom, &T::writeFrom)

template<typename SignalT, typename ValueT, typename Enable = void>
struct WriteHandler;

template<typename SignalT, typename ValueT>
struct WriteHandler<SignalT, ValueT, std::enable_if_t<HasWrite_v<SignalT> && !HasWriteFrom_v<SignalT>>> {
    static void write(SignalT& signal, ValueT const& value) {
        signal.write(value);
    }
};

template<typename SignalT, typename ValueT>
struct WriteHandler<SignalT, ValueT, std::enable_if_t<HasWriteFrom_v<SignalT>>> {
    static void write(SignalT& signal, ValueT const& value) {
        signal.writeFrom(value);
    }
};

// sc_signal<bool> has a private value_type. This is a workaround for that
template<typename T, typename = void /* for SFINAE */>
struct ValueTypeImpl {
    using value_type = typename T::value_type;
};

template<sc_core::sc_writer_policy POL>
struct ValueTypeImpl<sc_core::sc_signal<bool, POL>, void> {
    using value_type = bool;
};

struct ZeroWidth {
    JQR_DECL(ZeroWidth)

    JQR_TO_STRING
    JQR_OSTREAM
    JQR_COMP_EQ
};

struct ZeroWidthSignals {
    using value_type = ZeroWidth;

    ZeroWidthSignals(const char*) { /* empty ctor */ }
    void readTo(value_type&) {}
    void writeFrom(value_type const&) {}
};

namespace detail {

using namespace sc_core;

/**
 * @brief SignalDataMapper pokes/peeks a signal-like type.
 *
 * @note I define a signal-like type as the following:
 *
 * - `sc_signal<T>`
 *
 * - a struct that consists of members with signal-like types and has a `const char*` constructor
 *
 * - supports `.read()` and `.write()`
 *
 * - Has `::value_type`.
 *
 * Therefore, the following types are signal-like:
 *
 * - `sc_signal<int>`
 *
 * - `struct Bundle { sc_signal<int> b1, b2; };`
 *
 * - `struct Bundle { sc_signal<int> a; struct { sc_signal<int> b1, b2; } b; };`
 *
 * @tparam SignalT
 */

struct SinkBase {
    explicit SinkBase(std::string name, std::uint64_t timeoutCycles = 0)
        : name { std::move(name) }
        , timeoutCycles_ { timeoutCycles } {}

    SinkBase(SinkBase const&) = delete;
    SinkBase& operator=(SinkBase const&) = delete;
    SinkBase(SinkBase&&) = delete;
    SinkBase& operator=(SinkBase&&) = delete;

    std::uint64_t timeoutCycles() const noexcept {
        return timeoutCycles_;
    }

    void timeoutCycles(std::uint64_t cycles) noexcept {
        timeoutCycles_ = cycles;
    }

    virtual std::int32_t receiveAsInt32() = 0;
    virtual std::uint32_t receiveAsUInt32() = 0;
    virtual std::int64_t receiveAsInt64() = 0;
    virtual std::uint64_t receiveAsUInt64() = 0;
    virtual std::string receiveAsString() = 0;
    virtual void receiveToRef(util::Reference ref) = 0;

    template<typename T>
    T receiveAs() {
#define CHEXT_TEST_IMPL_RECEIVEAS_BRANCH(param1, param2) \
    if constexpr (std::is_same_v<T, param2>)             \
    return receiveAs##param1()

        /**/ CHEXT_TEST_IMPL_RECEIVEAS_BRANCH(Int32, std::int32_t);
        else CHEXT_TEST_IMPL_RECEIVEAS_BRANCH(UInt32, std::uint32_t);
        else CHEXT_TEST_IMPL_RECEIVEAS_BRANCH(Int64, std::int64_t);
        else CHEXT_TEST_IMPL_RECEIVEAS_BRANCH(UInt64, std::uint64_t);
        else CHEXT_TEST_IMPL_RECEIVEAS_BRANCH(String, std::string);
        else {
            T t;
            receiveToRef(t);
            return t;
        }

#undef CHEXT_TEST_IMPL_RECEIVEAS_BRANCH
    }

    virtual ~SinkBase() = default;

    const std::string name;

private:
    std::uint64_t timeoutCycles_;
};

template<
    typename BitsSignalT,
    bool PosEdgeClock = true,
    bool ActiveHighReset = true>
struct Sink : SinkBase {
    using BitsValueT = typename ValueTypeImpl<BitsSignalT>::value_type;

    Sink(
        std::string name,
        sc_in_clk const& clock,
        sc_in<bool> const& reset,
        std::uint64_t timeoutCycles = 0
    )
        : SinkBase { std::move(name), timeoutCycles }
        , clock { clock }
        , reset { reset }
        , bits { fmt::format("{}_bits", this->name).c_str() }
        , ready { fmt::format("{}_ready", this->name).c_str() }
        , valid { fmt::format("{}_valid", this->name).c_str() } {
        ready.write(false);
    }

#define CHEXT_TEST_IMPL_RECEIVEAS_FOR(param1, param2)                              \
    param2 receiveAs##param1() override {                                          \
        if constexpr (std::is_default_constructible_v<BitsValueT>)                 \
            return Converter<BitsValueT, param2>::convert(receive());              \
        else                                                                       \
            throw util::Exception("receiveAs" #param1 " is unsupported for this channel value type."); \
    }

    CHEXT_TEST_IMPL_RECEIVEAS_FOR(Int32, std::int32_t)
    CHEXT_TEST_IMPL_RECEIVEAS_FOR(UInt32, std::uint32_t)
    CHEXT_TEST_IMPL_RECEIVEAS_FOR(Int64, std::int64_t)
    CHEXT_TEST_IMPL_RECEIVEAS_FOR(UInt64, std::uint64_t)
    CHEXT_TEST_IMPL_RECEIVEAS_FOR(String, std::string)

#undef CHEXT_TEST_IMPL_RECEIVEAS_FOR

    void receiveTo(BitsValueT& value) {
        util::ReadyValid<PosEdgeClock, ActiveHighReset>::receive(
            clock,
            reset,
            ready,
            valid,
            [&] { ReadHandler<BitsSignalT, BitsValueT>::read(bits, value); },
            name,
            timeoutCycles()
        );
    }

    BitsValueT receive() {
        BitsValueT value;
        receiveTo(value);
        return value;
    }

    void receiveToRef(util::Reference ref) override {
        receiveTo(ref.get<BitsValueT>());
    }

private:
    sc_in_clk const& clock;
    sc_in<bool> const& reset;

public:
    BitsSignalT bits;
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

struct SourceBase {
    explicit SourceBase(std::string name, std::uint64_t timeoutCycles = 0)
        : name { std::move(name) }
        , timeoutCycles_ { timeoutCycles } {}

    SourceBase(SourceBase const&) = delete;
    SourceBase& operator=(SourceBase const&) = delete;
    SourceBase(SourceBase&&) = delete;
    SourceBase& operator=(SourceBase&&) = delete;

    std::uint64_t timeoutCycles() const noexcept {
        return timeoutCycles_;
    }

    void timeoutCycles(std::uint64_t cycles) noexcept {
        timeoutCycles_ = cycles;
    }

    virtual void sendAsInt32(std::int32_t const& x) = 0;
    virtual void sendAsUInt32(std::uint32_t const& x) = 0;
    virtual void sendAsInt64(std::int64_t const& x) = 0;
    virtual void sendAsUInt64(std::uint64_t const& x) = 0;
    virtual void sendAsString(std::string const& x) = 0;

    virtual void sendFromReference(util::ConstReference ref) = 0;

    template<typename T>
    void sendAs(T const& t) {
#define CHEXT_TEST_IMPL_SENDAS_BRANCH(param1, param2) \
    if constexpr (std::is_same_v<T, param2>)          \
    sendAs##param1(t)

        /**/ CHEXT_TEST_IMPL_SENDAS_BRANCH(Int32, std::int32_t);
        else CHEXT_TEST_IMPL_SENDAS_BRANCH(UInt32, std::uint32_t);
        else CHEXT_TEST_IMPL_SENDAS_BRANCH(Int64, std::int64_t);
        else CHEXT_TEST_IMPL_SENDAS_BRANCH(UInt64, std::uint64_t);
        else CHEXT_TEST_IMPL_SENDAS_BRANCH(String, std::string);
        else sendFromReference(t);

#undef CHEXT_TEST_IMPL_SENDAS_BRANCH
    }

    virtual ~SourceBase() = default;

    const std::string name;

private:
    std::uint64_t timeoutCycles_;
};

template<
    typename BitsSignalT,
    bool PosEdgeClock = true,
    bool ActiveHighReset = true>
struct Source : SourceBase {
    using BitsValueT = typename ValueTypeImpl<BitsSignalT>::value_type;

    Source(
        std::string name,
        sc_in_clk const& clock,
        sc_in<bool> const& reset,
        std::uint64_t timeoutCycles = 0
    )
        : SourceBase { std::move(name), timeoutCycles }
        , clock { clock }
        , reset { reset }
        , bits { fmt::format("{}_bits", this->name).c_str() }
        , ready { fmt::format("{}_ready", this->name).c_str() }
        , valid { fmt::format("{}_valid", this->name).c_str() } {
        valid.write(false);
    }

#define CHEXT_TEST_IMPL_SENDAS_FOR(param1, param2)       \
    void sendAs##param1(param2 const& x) {               \
        send(Converter<param2, BitsValueT>::convert(x)); \
    }

    CHEXT_TEST_IMPL_SENDAS_FOR(Int32, std::int32_t)
    CHEXT_TEST_IMPL_SENDAS_FOR(UInt32, std::uint32_t)
    CHEXT_TEST_IMPL_SENDAS_FOR(Int64, std::int64_t)
    CHEXT_TEST_IMPL_SENDAS_FOR(UInt64, std::uint64_t)
    CHEXT_TEST_IMPL_SENDAS_FOR(String, std::string)

#undef CHEXT_TEST_IMPL_SENDAS_FOR

    void send(BitsValueT const& value) {
        util::ReadyValid<PosEdgeClock, ActiveHighReset>::send(
            clock,
            reset,
            ready,
            valid,
            [&] { WriteHandler<BitsSignalT, BitsValueT>::write(bits, value); },
            name,
            timeoutCycles()
        );
    }

    void sendFromReference(util::ConstReference ref) override {
        send(ref.get<BitsValueT>());
    }

private:
    sc_in_clk const& clock;
    sc_in<bool> const& reset;

public:
    BitsSignalT bits;
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

} // namespace detail

// clang-format off
using detail::SinkBase;
using detail::Sink;

using detail::SourceBase;
using detail::Source;
// clang-format on

} // namespace chext_test::elastic

#endif /* CHEXT_TEST_ELASTIC_DRIVER_HPP_INCLUDED */
