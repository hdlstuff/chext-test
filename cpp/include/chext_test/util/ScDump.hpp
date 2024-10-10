#ifndef CHEXT_TEST_UTIL_DUMPSYSC_HPP_INCLUDED
#define CHEXT_TEST_UTIL_DUMPSYSC_HPP_INCLUDED

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <systemc>

#include <cassert>
#include <type_traits>

namespace chext_test::util {

struct ScDumpOptions {
    sc_dt::sc_numrep numrep { sc_dt::SC_DEC };
    bool hasPrefix { false };
    int width { -1 };
    const char* fmt { "{}" };
    int groupWidth { -1 };
    char groupSep { '_' };
};

template<typename T, typename Enable = void>
struct ScDump;

template<typename T>
struct ScDump<T, std::enable_if_t<std::is_base_of_v<sc_dt::sc_bv_base, T>>> {
    T const& t;
    ScDumpOptions const& options;

    void operator()(fmt::memory_buffer& buf) const {
        if (options.hasPrefix) {
            buf.push_back('0');
            if (options.numrep == sc_dt::SC_DEC)
                buf.push_back('d');
            else if (options.numrep == sc_dt::SC_HEX)
                buf.push_back('x');
            else if (options.numrep == sc_dt::SC_OCT)
                buf.push_back('o');
            else if (options.numrep == sc_dt::SC_BIN)
                buf.push_back('b');
            else
                buf.push_back('?');
        }

        auto s = group(t.to_string(options.numrep, false), options.groupWidth, options.groupSep);
        fmt::format_to(std::back_inserter(buf), fmt::runtime(options.fmt), s);
    }

private:
    static std::string group(const std::string& s, int groupWidth, char groupSep) {
        if (groupWidth < -1)
            return s;

        fmt::memory_buffer buf1, buf2;
        unsigned digitsPrinted = 0;

        for (auto it = s.rbegin(); it != s.rend(); ++it) {
            buf1.push_back(*it);
            digitsPrinted++;

            if ((digitsPrinted % groupWidth) == 0 && digitsPrinted != s.size())
                buf1.push_back(groupSep);
        }

        const char* c = buf1.data();
        for (std::size_t idx = 0; idx < buf1.size(); ++idx) {
            buf2.push_back(c[buf1.size() - idx - 1]);
        }

        return std::string(buf2.data(), buf2.size());
    }
};

template<typename T>
inline auto scDump(T const& t, ScDumpOptions const& options) {
    return ScDump<T> { t, options };
}

template<typename T>
std::ostream& operator<<(std::ostream& os, ScDump<T> const& dump) {
    fmt::memory_buffer buf;
    dump(buf);
    return (os << std::string_view(buf.data(), buf.size()));
};

/*
template <typename T>
void scDump(T const& t, )
*/

}; // namespace chext_test::util

namespace fmt {

template<typename T>
struct formatter<chext_test::util::ScDump<T>> : ostream_formatter {};

}; // namespace fmt

#endif /* CHEXT_TEST_UTIL_DUMPSYSC_HPP_INCLUDED */
