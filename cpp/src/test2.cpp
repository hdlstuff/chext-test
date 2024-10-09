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
#include <jqr/hash.hpp>

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

        JQR_DECL(Nested, JQR_MEMBER(x), JQR_MEMBER(y))
    } n;

    JQR_DECL(
        Test,
        JQR_MEMBER(a, o::dump_fmt { "0d{:05d}" }),
        JQR_MEMBER(b, o::dump_fmt { "0b{:b}" }),
        JQR_MEMBER(n, o::dump_class { false })
    )

    JQR_TO_STRING
};

template<typename T>
struct Log {
    int t;
    T obj;

    JQR_DECL(
        Log,
        JQR_MEMBER(t),
        JQR_MEMBER(obj)
    )

    JQR_OPTIONS(o::dump_paren { false })
    JQR_TO_STRING
};

struct Struct1 {
    int a, b, c;

    bool operator==(Struct1 const&) const = default;

    JQR_DECL(
        Struct1,
        JQR_MEMBER(a),
        JQR_MEMBER(b),
        JQR_MEMBER(c)
    )
};

// must be in global scope
JQR_DEFINE_STD_HASH(Struct1)

struct Struct2 {
    int a, b, c;

    JQR_DECL(
        Struct2,
        JQR_MEMBER(a),
        JQR_MEMBER(b),
        JQR_MEMBER(c, o::hash { false })
    )
};

#include <unordered_map>

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

    fmt::print("object hash: {}\n", jqr::hash(std::string("canberk")));

    Struct1 s1 { 10, 20, 30 };
    Struct2 s2 { 10, 20, 30 };

    fmt::print("object hash: s1={}, s2={}\n", jqr::hash(s1), jqr::hash(s2));

    std::unordered_map<Struct1, std::string> map;

    return 0;
}
