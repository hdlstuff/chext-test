#ifndef CHEXT_TEST_AMBA_AXI4_CONFIG_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_CONFIG_HPP_INCLUDED

#include <jqr/dump.hpp>

namespace chext_test::amba::axi4 {

struct Config;

struct ConfigSpec {
    unsigned wId { 0 };
    unsigned wAddr { 0 };
    unsigned wData { 0 };

    bool read { true };
    bool write { true };
    bool lite { false };

    bool hasLock { true };
    bool hasCache { true };
    bool hasProt { true };
    bool hasQos { true };
    bool hasRegion { true };

    bool axi3Compat { false };

    unsigned wUserAR { 0 };
    unsigned wUserR { 0 };
    unsigned wUserAW { 0 };
    unsigned wUserW { 0 };
    unsigned wUserB { 0 };

    constexpr Config toConfig() const;

    JQR_DECL(
        ConfigSpec,
        JQR_MEMBER(wId),
        JQR_MEMBER(wAddr),
        JQR_MEMBER(wData),
        JQR_MEMBER(read),
        JQR_MEMBER(write),
        JQR_MEMBER(lite),
        JQR_MEMBER(hasLock),
        JQR_MEMBER(hasCache),
        JQR_MEMBER(hasProt),
        JQR_MEMBER(hasQos),
        JQR_MEMBER(hasRegion),
        JQR_MEMBER(axi3Compat),
        JQR_MEMBER(wUserAR),
        JQR_MEMBER(wUserR),
        JQR_MEMBER(wUserAW),
        JQR_MEMBER(wUserW),
        JQR_MEMBER(wUserB)
    )
};

struct Config {
    constexpr explicit Config(ConfigSpec spec)
        : wId(spec.wId)
        , wAddr(spec.wAddr)
        , wData(spec.wData)
        , read(spec.read)
        , write(spec.write)
        , lite(spec.lite)
        , hasLock(spec.hasLock)
        , hasCache(spec.hasCache)
        , hasProt(spec.hasProt)
        , hasQos(spec.hasQos)
        , hasRegion(spec.hasRegion)
        , axi3Compat(spec.axi3Compat)
        , wUserAR(spec.wUserAR)
        , wUserR(spec.wUserR)
        , wUserAW(spec.wUserAW)
        , wUserW(spec.wUserW)
        , wUserB(spec.wUserB)
        , wStrobe(spec.wData / 8)
        , wLen(spec.axi3Compat ? 4 : 8)
        , wLock(spec.hasLock ? (spec.axi3Compat ? 2 : 1) : 0)
        , wCache(spec.hasCache ? 4 : 0)
        , wProt(spec.hasProt ? 3 : 0)
        , wQos(spec.hasQos ? 4 : 0)
        , wRegion(spec.hasRegion ? 4 : 0) {}

    constexpr Config(
        unsigned wId,
        unsigned wAddr,
        unsigned wData,
        bool read = true,
        bool write = true,
        bool lite = false,
        bool hasLock = true,
        bool hasCache = true,
        bool hasProt = true,
        bool hasQos = true,
        bool hasRegion = true,
        bool axi3Compat = false,
        unsigned wUserAR = 0,
        unsigned wUserR = 0,
        unsigned wUserAW = 0,
        unsigned wUserW = 0,
        unsigned wUserB = 0
    )
        : Config(ConfigSpec {
              .wId = wId,
              .wAddr = wAddr,
              .wData = wData,
              .read = read,
              .write = write,
              .lite = lite,
              .hasLock = hasLock,
              .hasCache = hasCache,
              .hasProt = hasProt,
              .hasQos = hasQos,
              .hasRegion = hasRegion,
              .axi3Compat = axi3Compat,
              .wUserAR = wUserAR,
              .wUserR = wUserR,
              .wUserAW = wUserAW,
              .wUserW = wUserW,
              .wUserB = wUserB,
          }) {}

    const unsigned wId;
    const unsigned wAddr;
    const unsigned wData;

    const bool read;
    const bool write;
    const bool lite;

    const bool hasLock;
    const bool hasCache;
    const bool hasProt;
    const bool hasQos;
    const bool hasRegion;

    const bool axi3Compat;

    const unsigned wUserAR;
    const unsigned wUserR;
    const unsigned wUserAW;
    const unsigned wUserW;
    const unsigned wUserB;

    const unsigned wStrobe;
    const unsigned wLen;
    const unsigned wLock;
    const unsigned wCache;
    const unsigned wProt;
    const unsigned wQos;
    const unsigned wRegion;

    constexpr ConfigSpec toSpec() const {
        return ConfigSpec {
            .wId = wId,
            .wAddr = wAddr,
            .wData = wData,
            .read = read,
            .write = write,
            .lite = lite,
            .hasLock = hasLock,
            .hasCache = hasCache,
            .hasProt = hasProt,
            .hasQos = hasQos,
            .hasRegion = hasRegion,
            .axi3Compat = axi3Compat,
            .wUserAR = wUserAR,
            .wUserR = wUserR,
            .wUserAW = wUserAW,
            .wUserW = wUserW,
            .wUserB = wUserB,
        };
    }

    JQR_DECL(
        Config,
        JQR_MEMBER(wId),
        JQR_MEMBER(wAddr),
        JQR_MEMBER(wData),
        JQR_MEMBER(read),
        JQR_MEMBER(write),
        JQR_MEMBER(lite),
        JQR_MEMBER(hasLock),
        JQR_MEMBER(hasCache),
        JQR_MEMBER(hasProt),
        JQR_MEMBER(hasQos),
        JQR_MEMBER(hasRegion),
        JQR_MEMBER(axi3Compat),
        JQR_MEMBER(wUserAR),
        JQR_MEMBER(wUserR),
        JQR_MEMBER(wUserAW),
        JQR_MEMBER(wUserW),
        JQR_MEMBER(wUserB),
        JQR_MEMBER(wStrobe),
        JQR_MEMBER(wLen),
        JQR_MEMBER(wLock),
        JQR_MEMBER(wCache),
        JQR_MEMBER(wProt),
        JQR_MEMBER(wQos),
        JQR_MEMBER(wRegion)
    )
};

constexpr Config ConfigSpec::toConfig() const {
    return Config { *this };
}

} // namespace chext_test::amba::axi4

#endif /* CHEXT_TEST_AMBA_AXI4_CONFIG_HPP_INCLUDED */
