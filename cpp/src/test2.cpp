#include <systemc>

#include <chrono>
#include <compare>
#include <iostream>
#include <sstream>
#include <tuple>

#include <fmt/chrono.h>
#include <fmt/core.h>

#include <jqr/core.hpp>
#include <jqr/dump.hpp>

namespace o = jqr::opts;

struct Student {
    std::string name, surname;
    float gpa;
    std::chrono::system_clock::time_point date;

    JQR_DECL(
        Student,
        JQR_MEMBER(name, o::dump_fmt { "'{}'" }, o::dump_name { false }),
        JQR_MEMBER(surname, o::dump_fmt { "'{}'" }, o::dump_name { false }),
        JQR_MEMBER(gpa, o::dump_fmt { "{:1.2f}" }),
        JQR_MEMBER(date, o::dump_fmt { "'{:%F %T}'" })
    );
    JQR_TO_STRING;
};

struct Test {
    int a;
    int b;

    struct Nested {
        int x, y;

        JQR_DECL(Nested, JQR_MEMBER(x), JQR_MEMBER(y));
    } n;

    JQR_DECL(
        Test,
        JQR_MEMBER(a, o::dump_fmt { "0d{:05d}" }),
        JQR_MEMBER(b, o::dump_fmt { "0b{:b}" }),
        JQR_MEMBER(n, o::dump_class { false })
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

    JQR_OPTIONS(o::dump_paren { false });
    JQR_TO_STRING;
};

int sc_main(int, char**) {
    Student s {
        .name = "Canberk",
        .surname = "Sönmez",
        .gpa = 4.0,
        .date = std::chrono::system_clock::now()
    };
    fmt::print("s = {}\n", s);

    int y;
    static Test t;
    constexpr auto x = jqr::members_of(t);

    Test t2 { 4, 5 };
    std::cout << t2.to_string() << std::endl;

    fmt::print("Is JQR? {}\n", jqr::is_jqr_v<Test>);
    fmt::print("Is JQR? {}\n", jqr::is_jqr_v<int>);

    fmt::print("Test object is: {}\n", Log<Test> { 5, t2 });

    return 0;
}
