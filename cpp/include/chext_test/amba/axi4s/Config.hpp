#ifndef CHEXT_TEST_AMBA_AXI4S_CONFIG_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4S_CONFIG_HPP_INCLUDED

#include <jqr/dump.hpp>

namespace chext_test::amba::axi4s {

struct Config;

struct ConfigSpec {
    unsigned wData { 0 };
    unsigned wId { 0 };
    unsigned wDest { 0 };
    unsigned wUser { 0 };

    bool hasReady { true };
    bool hasStrobe { true };
    bool hasKeep { true };
    bool hasLast { true };

    constexpr Config toConfig() const;

    JQR_DECL(
        ConfigSpec,
        JQR_MEMBER(wData),
        JQR_MEMBER(wId),
        JQR_MEMBER(wDest),
        JQR_MEMBER(wUser),
        JQR_MEMBER(hasReady),
        JQR_MEMBER(hasStrobe),
        JQR_MEMBER(hasKeep),
        JQR_MEMBER(hasLast)
    )
};

struct Config {
    constexpr explicit Config(ConfigSpec spec)
        : wData(spec.wData)
        , wId(spec.wId)
        , wDest(spec.wDest)
        , wUser(spec.wUser)
        , hasReady(spec.hasReady)
        , hasStrobe(spec.hasStrobe)
        , hasKeep(spec.hasKeep)
        , hasLast(spec.hasLast)
        , wStrobe(spec.wData / 8)
        , wKeep(spec.wData / 8) {}

    constexpr Config(
        unsigned wData,
        unsigned wId = 0,
        unsigned wDest = 0,
        unsigned wUser = 0,
        bool hasReady = true,
        bool hasStrobe = true,
        bool hasKeep = true,
        bool hasLast = true
    )
        : Config(ConfigSpec {
              .wData = wData,
              .wId = wId,
              .wDest = wDest,
              .wUser = wUser,
              .hasReady = hasReady,
              .hasStrobe = hasStrobe,
              .hasKeep = hasKeep,
              .hasLast = hasLast,
          }) {}

    const unsigned wData;
    const unsigned wId;
    const unsigned wDest;
    const unsigned wUser;

    const bool hasReady;
    const bool hasStrobe;
    const bool hasKeep;
    const bool hasLast;

    const unsigned wStrobe;
    const unsigned wKeep;

    constexpr ConfigSpec toSpec() const {
        return ConfigSpec {
            .wData = wData,
            .wId = wId,
            .wDest = wDest,
            .wUser = wUser,
            .hasReady = hasReady,
            .hasStrobe = hasStrobe,
            .hasKeep = hasKeep,
            .hasLast = hasLast,
        };
    }

    JQR_DECL(
        Config,
        JQR_MEMBER(wData),
        JQR_MEMBER(wId),
        JQR_MEMBER(wDest),
        JQR_MEMBER(wUser),
        JQR_MEMBER(hasReady),
        JQR_MEMBER(hasStrobe),
        JQR_MEMBER(hasKeep),
        JQR_MEMBER(hasLast),
        JQR_MEMBER(wStrobe),
        JQR_MEMBER(wKeep)
    )
};

constexpr Config ConfigSpec::toConfig() const {
    return Config { *this };
}

} // namespace chext_test::amba::axi4s

#endif /* CHEXT_TEST_AMBA_AXI4S_CONFIG_HPP_INCLUDED */
