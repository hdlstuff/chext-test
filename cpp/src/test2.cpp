#include <compare>
#include <iostream>
#include <sstream>
#include <tuple>

#include <fmt/core.h>
#include <jqr/core.hpp>
#include <jqr/dump.hpp>

struct Test2 {
    int c;
    int d;

    struct Test3 {
        int x, y;

        JQR_DECL(Test3, JQR_MEMBER(x), JQR_MEMBER(y));
    } q;

    JQR_DECL(
        Test2,
        JQR_MEMBER(c, jqr::opts::dump_fmt { "0d{:05d}" }),
        JQR_MEMBER(d, jqr::opts::dump_fmt { "0b{:b}" }),
        JQR_MEMBER(q, jqr::opts::dump_class { false })
    );

    JQR_TO_STRING;
};

template<typename T>
struct Log {
    int t;
    T obj;

    JQR_DECL(
        Log,
        JQR_MEMBER(t),
        JQR_MEMBER(obj)
    );

    JQR_TO_STRING;
};

int main() {
    int y;
    static Test2 t;
    constexpr auto x = jqr::members_of(t);

    Test2 t2 { 4, 5 };
    std::cout << t2.to_string() << std::endl;

    fmt::print("Is JQR? {}\n", jqr::is_jqr_v<Test2>);
    fmt::print("Is JQR? {}\n", jqr::is_jqr_v<int>);

    fmt::print("Test object is: {}\n", Log<Test2> { 5, t2 });
}

#if 0

struct Test {
    int a;
    int b;
    const char* name;

    struct Test2 {
        int c;
        int d;

        JQR_DECL(
            Test2,
            JQR_MEMBER(c),
            JQR_MEMBER(d, jqr::opts::dump_fmt { "0d{:05d}" })
        );
    } x;

    JQR_DECL(
        Test,
        JQR_MEMBER(a, jqr::opts::dump_fmt { "0x{:02x}" }),
        JQR_MEMBER(b, jqr::opts::dump { false }),
        JQR_MEMBER(name, jqr::opts::dump_fmt { "'{}'" }, jqr::opts::dump_name { false }),
        JQR_MEMBER(x, jqr::opts::dump { true })
    );
};

int main() {
    static constexpr Test t1 { 3, 4, "canberk" };

    constexpr auto members = t1.members();
    std::string s = jqr::to_string(t1);

    std::cout << jqr::to_string(t1, std::make_tuple(jqr::opts::dump_paren{false})) << std::endl;

    std::cout << Test::Test2::type_name() << std::endl;
}

#endif
