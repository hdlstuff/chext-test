#include <list>

#include <fmt/core.h>

#include <chext_test/util/ScDump.hpp>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

#define EXPECT_(x) TestBenchBase::__expect((x), __FILE__, __LINE__, #x)
#define EXPECT_EQ(a, b) TestBenchBase::__expect_eq((a), (b), __FILE__, __LINE__, #a, #b)
#define EXPECT_NE(a, b) TestBenchBase::__expect_ne((a), (b), __FILE__, __LINE__, #a, #b)
#define EXPECT_LT(a, b) TestBenchBase::__expect_lt((a), (b), __FILE__, __LINE__, #a, #b)
#define EXPECT_LE(a, b) TestBenchBase::__expect_le((a), (b), __FILE__, __LINE__, #a, #b)
#define EXPECT_GT(a, b) TestBenchBase::__expect_gt((a), (b), __FILE__, __LINE__, #a, #b)
#define EXPECT_GE(a, b) TestBenchBase::__expect_ge((a), (b), __FILE__, __LINE__, #a, #b)
#define EXPECT_EQ(a, b) TestBenchBase::__expect_eq((a), (b), __FILE__, __LINE__, #a, #b)

#define ASSERT_(x) TestBenchBase::__assert((x), __FILE__, __LINE__, #x)
#define ASSERT_EQ(a, b) TestBenchBase::__assert_eq((a), (b), __FILE__, __LINE__, #a, #b)
#define ASSERT_NE(a, b) TestBenchBase::__assert_ne((a), (b), __FILE__, __LINE__, #a, #b)
#define ASSERT_LT(a, b) TestBenchBase::__assert_lt((a), (b), __FILE__, __LINE__, #a, #b)
#define ASSERT_LE(a, b) TestBenchBase::__assert_le((a), (b), __FILE__, __LINE__, #a, #b)
#define ASSERT_GT(a, b) TestBenchBase::__assert_gt((a), (b), __FILE__, __LINE__, #a, #b)
#define ASSERT_GE(a, b) TestBenchBase::__assert_ge((a), (b), __FILE__, __LINE__, #a, #b)
#define ASSERT_EQ(a, b) TestBenchBase::__assert_eq((a), (b), __FILE__, __LINE__, #a, #b)

class TestBenchBase : public sc_module {
public:
    TestBenchBase(sc_module_name const& name = "tb")
        : sc_module { name } {
        SC_THREAD(entry);
    }

    void start(sc_time const& duration = sc_time(50, SC_NS)) {
        while (!stopped_) {
            sc_start(duration);
        }
    }

    virtual ~TestBenchBase() = default;

protected:
    void finish() {
        stop();
    }

    void __expect(bool expr, char const* file = nullptr, int line = -1, char const* msg = nullptr) {
        if (expr)
            return;

        fmt::print(
            "[ {} ] EXPECT Fail: {} is FALSE (t = {}) ({}:{})\n",
            this->name(),
            msg,
            sc_time_stamp().to_string(),
            file, line
        );
    }

    void __expect_not(bool expr, char const* file = nullptr, int line = -1, char const* msg = nullptr) {
        if (!expr)
            return;

        fmt::print(
            "[ {} ] EXPECT_NOT Fail: {} is TRUE (t = {}) ({}:{})\n",
            this->name(),
            msg,
            sc_time_stamp().to_string(),
            file, line
        );
    }

#define EXPECT_BINARY_OP(param1, param2, param3, param4)                                                \
    template<typename T1, typename T2>                                                                  \
    void __expect_##param1(                                                                             \
        T1 const& a,                                                                                    \
        T2 const& b,                                                                                    \
        char const* file = nullptr,                                                                     \
        int line = -1,                                                                                  \
        char const* a_str = nullptr,                                                                    \
        char const* b_str = nullptr                                                                     \
    ) {                                                                                                 \
        if (a param3 b)                                                                                 \
            return;                                                                                     \
                                                                                                        \
        fmt::print(                                                                                     \
            "[ {} ] EXPECT_" #param2 " Fail: {} " #param4 " {} ({} " #param4 " {}) (t = {}) ({}:{})\n", \
            this->name(),                                                                               \
            a_str, b_str,                                                                               \
            a, b,                                                                                       \
            sc_time_stamp().to_string(),                                                                \
            file, line                                                                                  \
        );                                                                                              \
    }

    EXPECT_BINARY_OP(eq, EQ, ==, !=)
    EXPECT_BINARY_OP(ne, NE, !=, ==)
    EXPECT_BINARY_OP(lt, LT, <, >=)
    EXPECT_BINARY_OP(le, LE, <=, >)
    EXPECT_BINARY_OP(gt, GT, >, <=)
    EXPECT_BINARY_OP(ge, GE, >=, <)

#undef EXPECT_BINARY_OP

    void __assert_(bool expr, char const* file = nullptr, int line = -1, char const* msg = nullptr) {
        if (expr)
            return;

        fmt::print(
            "[ {} ] ASSERT Fail: {} is FALSE (t = {}) ({}:{})\n",
            this->name(),
            msg,
            sc_time_stamp().to_string(),
            file, line
        );

        stop();
    }

    void __assert_not(bool expr, char const* file = nullptr, int line = -1, char const* msg = nullptr) {
        if (!expr)
            return;

        fmt::print(
            "[ {} ] ASSERT_NOT Fail: {} is TRUE (t = {}) ({}:{})\n",
            this->name(),
            msg,
            sc_time_stamp().to_string(),
            file, line
        );

        stop();
    }

#define ASSERT_BINARY_OP(param1, param2, param3, param4)                                                \
    template<typename T>                                                                                \
    void __assert_##param1(                                                                             \
        T const& a,                                                                                     \
        T const& b,                                                                                     \
        char const* file = nullptr,                                                                     \
        int line = -1,                                                                                  \
        char const* a_str = nullptr,                                                                    \
        char const* b_str = nullptr                                                                     \
    ) {                                                                                                 \
        if (a param3 b)                                                                                 \
            return;                                                                                     \
                                                                                                        \
        fmt::print(                                                                                     \
            "[ {} ] ASSERT_" #param2 " Fail: {} " #param4 " {} ({} " #param4 " {}) (t = {}) ({}:{})\n", \
            this->name(),                                                                               \
            a_str, b_str,                                                                               \
            a, b,                                                                                       \
            sc_time_stamp().to_string(),                                                                \
            file, line                                                                                  \
        );                                                                                              \
        stop();                                                                                         \
    }

    ASSERT_BINARY_OP(eq, EQ, ==, !=)
    ASSERT_BINARY_OP(ne, NE, !=, ==)
    ASSERT_BINARY_OP(lt, LT, <, >=)
    ASSERT_BINARY_OP(le, LE, <=, >)
    ASSERT_BINARY_OP(gt, GT, >, <=)
    ASSERT_BINARY_OP(ge, GE, >=, <)

#undef ASSERT_BINARY_OP

    virtual void entry() = 0;

private:
    bool stopped_ = false;

    void stop() {
        sc_stop();
        stopped_ = true;
    }
};

#include <chext_test/amba/axi4/full/Packets.hpp>

#include <fmt/ostream.h>

template<unsigned W>
struct fmt::formatter<sc_bv<W>> : ostream_formatter {};

template<>
struct fmt::formatter<sc_bv_base> : ostream_formatter {};

template<unsigned W>
struct fmt::formatter<sc_uint<W>> : ostream_formatter {};

template<>
struct fmt::formatter<sc_uint_base> : ostream_formatter {};

struct TestBench1 : TestBenchBase {
    TestBench1()
        : TestBenchBase { "TestBench1" } {
    }

protected:
    void entry() override {
        fmt::print("hello world!\n");
        wait(50, SC_NS);
        fmt::print("bye!\n");

        int x = 98;
        int y = 9;
        EXPECT_EQ(x, y);

        EXPECT_LT(x, y);

        chext_test::amba::axi4::full::Packets::Address ar1, ar2;
        ar1.addr = sc_bv<20>(0x10000);
        ar2.addr = sc_bv<20>(0x20000);

        sc_bv_base num1{ sc_bv<16>(0x1400) };
        sc_bv_base num2 { 32 };

        num2 = 0x1400;

        fmt::print("lengths of ar1 and ar2: {}, {}\n", ar1.addr.length(), ar2.addr.length());
        fmt::print("num1 = {}, num2 = {}, lengths: {}, {}\n", num1, num2, num1.length(), num2.length());

        using chext_test::util::scDump;
        using chext_test::util::ScDumpOptions;

        num2 = 0xFFFF'FFFF;
        fmt::print("num2 = {}\n", scDump(num2, { .numrep = SC_HEX, .hasPrefix = true, .groupWidth = 4 }));

        num1 = 0xffff'1234;
        num2 = sc_bv<8>(0x99);
        fmt::print("num1 = {}, num2 = {}, lengths: {}, {}\n", num1, num2, num1.length(), num2.length());

        // lesson learned: sc_bv_base retains its original length that it was constructed with

        EXPECT_EQ(ar1, ar2);

        sc_bv<8> a(89);
        sc_bv<9> b(100);
        EXPECT_EQ(a, b);

        sc_bv_base xx(a);
        sc_bv_base yy(b);

        EXPECT_EQ(xx, yy);
        EXPECT_NE(xx, yy);
        // EXPECT_LE(xx, yy);

        sc_uint_base xxx(a);
        sc_uint_base yyy(b);

        EXPECT_EQ(xxx, yyy);
        EXPECT_NE(xxx, yyy);
        EXPECT_LE(xxx, yyy);
        EXPECT_GT(xxx, yyy);

        wait(10000, SC_NS);

        fmt::print("{}\n", sc_time_stamp().to_string());

        finish();
    }
};

int sc_main(int argc, char** argv) {
    TestBench1 tb;
    tb.start();
    return 0;
}
