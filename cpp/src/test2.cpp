#include <compare>
#include <iostream>
#include <sstream>
#include <tuple>

#include <fmt/core.h>
#include <jqreflect.hpp>

struct Test {
    int a;
    int b;
    std::string name;

    JQR_DECL(
        Test,
        JQR_MEMBER(a, jqr::opts::dump_fmt { "0x{:02x}" }),
        JQR_MEMBER(b, jqr::opts::dump { false }),
        JQR_MEMBER(name, jqr::opts::dump_fmt { "'{}'" })
    );

    void dump(fmt::memory_buffer& buf, const char* fmt) const {
        using jqr::get_or_else;

        using jqr::opts::dump;
        using jqr::opts::dump_fmt;
        using jqr::opts::dump_name;

        auto fn = [&](auto x) {
            if (get_or_else(x.options, dump { true }).b) {
                if (get_or_else(x.options, dump_name { true }).b) {
                    buf.append(std::string_view(x.name));
                    buf.append(std::string_view("="));
                }

                jqr::dump(x.t, buf, get_or_else(x.options, dump_fmt { "{}" }).fmt);
                buf.append(std::string_view(", "));
            }
        };

        std::apply([&](auto... x) { (fn(x), ...); }, members());
    }
};

struct Point {
    int x {}, y {};

    friend void printPoint(Point const& p) {}

    friend constexpr std::strong_ordering operator<=>(Point lhs, Point rhs) {
        if (lhs.x < rhs.x or (lhs.x == rhs.x and lhs.y < rhs.y))
            return std::strong_ordering::less;
        if (lhs.x > rhs.x or (lhs.x == rhs.x and lhs.y > rhs.y))
            return std::strong_ordering::greater;
        return std::strong_ordering::equivalent;
    }

    friend std::ostream& operator<<(std::ostream& os, Point s) {
        return os << '(' << s.x << ',' << s.y << ')';
    }
};

void print_three_way_comparison(const auto& p, const auto& q) {
    const auto cmp { p <=> q };
    std::cout << p
              << (cmp < 0 ? " <  " : cmp > 0 ? " >  "
                                             : " == ") // compares with 0
              << q << '\n';
}

void print_two_way_comparison(const auto& p, const auto& q) {
    std::cout << p
              << (p < q ? " <  " : p > q ? " >  "
                                         : " == ") // compares p and q
              << q << '\n';
}

extern const char* myString;

struct Test2 {
    void to_buffer(fmt::memory_buffer& buf, const char* fmt) const {
        buf.append(std::string_view("Test()"));
    }
};

int main() {
    /*
    const Point p1 { 0, 1 }, p2 { 0, 1 }, p3 { 0, 2 };

    print_three_way_comparison(p1, p2);
    print_two_way_comparison(p1, p2);

    print_three_way_comparison(p2, p3);
    print_two_way_comparison(p2, p3);

    print_three_way_comparison(p3, p2);
    print_two_way_comparison(p3, p2);

    */

    const Test t1 { 3, 4, "canberk" };
    std::cout << jqr::to_string(t1) << std::endl;

    /*

    std::apply([](const auto&... xs) {
        ((std::cout << xs.name << ", "), ...);
        std::cout << "\n";
    },
               t1.members());
    // std::cout << (t1 == t2) << std::endl;

    fmt::print(myString, 9, 8);

    std::cout << jqr::to_string(Test2 {}) << std::endl;
    */

    /*
    int x = 8;
    int y = 6;
    auto t = std::tie(x, y);
    std::apply([&](auto const& x) {}, t);
    */
}

const char* myString = "test\n";
