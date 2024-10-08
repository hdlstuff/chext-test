#include <compare>
#include <iostream>
#include <sstream>
#include <tuple>

template<typename T>
using type_identity_t = T;

#define DECL_STRUCT(name, ...)                                   \
    static constexpr const char* struct_name() { return #name; } \
    using this_struct = name;                                    \
                                                                 \
    constexpr auto tie() noexcept {                              \
        return std::tie(__VA_ARGS__);                            \
    }                                                            \
                                                                 \
    constexpr auto tie() const noexcept {                        \
        return std::tie(__VA_ARGS__);                            \
    }

#define IMPL_COMPARISON_FOR(op) \
    friend constexpr bool operator op(this_struct const& a, this_struct const& b) { return a.tie() op b.tie(); }

#define HAS_COMPARISON      \
    IMPL_COMPARISON_FOR(==) \
    IMPL_COMPARISON_FOR(!=) \
    IMPL_COMPARISON_FOR(<)  \
    IMPL_COMPARISON_FOR(<=) \
    IMPL_COMPARISON_FOR(>)  \
    IMPL_COMPARISON_FOR(>=)

#define HAS_TO_STRING                         \
    std::string to_string() const {           \
        std::ostringstream os;                \
                                              \
        std::apply([&](auto const&... xs) {   \
            os << this->struct_name() << "("; \
            ((os << xs << ", "), ...);        \
            os << ")";                        \
        },                                    \
                   this->tie());              \
        return os.str();                      \
    }

struct Test {
    int a;
    int b;

    DECL_STRUCT(Test, a, b);
    HAS_TO_STRING;
    HAS_COMPARISON;
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

int main() {
    const Point p1 { 0, 1 }, p2 { 0, 1 }, p3 { 0, 2 };

    print_three_way_comparison(p1, p2);
    print_two_way_comparison(p1, p2);

    print_three_way_comparison(p2, p3);
    print_two_way_comparison(p2, p3);

    print_three_way_comparison(p3, p2);
    print_two_way_comparison(p3, p2);

    const Test t1 { 3, 4 }, t2 { 3, 4 };
    std::cout << t1.to_string() << std::endl;
    std::cout << (t1 == t2) << std::endl;

    /*
    int x = 8;
    int y = 6;
    auto t = std::tie(x, y);
    std::apply([&](auto const& x) {}, t);
    */
}
