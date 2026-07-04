#ifndef CHEXT_TEST_AMBA_AXI4S_SIGNALS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4S_SIGNALS_HPP_INCLUDED

#include <chext_test/amba/axi4s/Packets.hpp>
#include <chext_test/vutil.hpp>

#include <fmt/core.h>

#include <systemc>

namespace chext_test::amba::axi4s {

template<
    unsigned DATA_WIDTH,
    unsigned ID_WIDTH = 0,
    unsigned DEST_WIDTH = 0,
    unsigned USER_WIDTH = 0,
    bool HAS_STROBE = true,
    bool HAS_KEEP = true,
    bool HAS_LAST = true>
struct Signals {
    using value_type = Packet;

    static constexpr Config config {
        ConfigSpec {
            .wData = DATA_WIDTH,
            .wId = ID_WIDTH,
            .wDest = DEST_WIDTH,
            .wUser = USER_WIDTH,
            .hasStrobe = HAS_STROBE,
            .hasKeep = HAS_KEEP,
            .hasLast = HAS_LAST,
        }
    };

private:
    static constexpr bool hasId = ID_WIDTH > 0;
    static constexpr bool hasDest = DEST_WIDTH > 0;
    static constexpr bool hasUser = USER_WIDTH > 0;

public:
    vutil::signal_t<DATA_WIDTH, sc_core::SC_MANY_WRITERS> data;
    [[no_unique_address]] vutil::optional_signal_t<HAS_STROBE, config.wStrobe, sc_core::SC_MANY_WRITERS> strb;
    [[no_unique_address]] vutil::optional_signal_t<HAS_KEEP, config.wKeep, sc_core::SC_MANY_WRITERS> keep;
    [[no_unique_address]] vutil::optional_signal_t<HAS_LAST, 1, sc_core::SC_MANY_WRITERS> last;
    [[no_unique_address]] vutil::optional_signal_t<hasId, ID_WIDTH, sc_core::SC_MANY_WRITERS> id;
    [[no_unique_address]] vutil::optional_signal_t<hasDest, DEST_WIDTH, sc_core::SC_MANY_WRITERS> dest;
    [[no_unique_address]] vutil::optional_signal_t<hasUser, USER_WIDTH, sc_core::SC_MANY_WRITERS> user;

    explicit Signals(const char* name)
        : data(fmt::format("{}_data", name).c_str())
        , strb(fmt::format("{}_strb", name).c_str())
        , keep(fmt::format("{}_keep", name).c_str())
        , last(fmt::format("{}_last", name).c_str())
        , id(fmt::format("{}_id", name).c_str())
        , dest(fmt::format("{}_dest", name).c_str())
        , user(fmt::format("{}_user", name).c_str()) {}

    void writeFrom(value_type const& packet) {
        vutil::write<DATA_WIDTH>(data, packet.data);
        vutil::write_if<HAS_STROBE, config.wStrobe>(strb, packet.strb);
        vutil::write_if<HAS_KEEP, config.wKeep>(keep, packet.keep);
        vutil::write_if<HAS_LAST, 1>(last, packet.last);
        vutil::write_if<hasId, ID_WIDTH>(id, packet.id);
        vutil::write_if<hasDest, DEST_WIDTH>(dest, packet.dest);
        vutil::write_if<hasUser, USER_WIDTH>(user, packet.user);
    }

    void readTo(value_type& packet) const {
        vutil::read<DATA_WIDTH>(data, packet.data);
        vutil::read_if<HAS_STROBE, config.wStrobe>(strb, packet.strb);
        vutil::read_if<HAS_KEEP, config.wKeep>(keep, packet.keep);
        vutil::read_if<HAS_LAST, 1>(last, packet.last);
        vutil::read_if<hasId, ID_WIDTH>(id, packet.id);
        vutil::read_if<hasDest, DEST_WIDTH>(dest, packet.dest);
        vutil::read_if<hasUser, USER_WIDTH>(user, packet.user);
    }
};

} // namespace chext_test::amba::axi4s

#endif /* CHEXT_TEST_AMBA_AXI4S_SIGNALS_HPP_INCLUDED */
