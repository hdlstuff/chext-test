#ifndef CHEXT_TEST_AMBA_AXI4_FULL_DRIVER_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_FULL_DRIVER_HPP_INCLUDED

#include <chext_test/amba/axi4/full/Signals.hpp>
#include <chext_test/elastic/Driver.hpp>

#include <fmt/core.h>

#include <cassert>
#include <systemc>

namespace chext_test::amba::axi4::full {

namespace detail {

using elastic::Sink;
using elastic::Source;

using namespace sc_core;

struct MasterBase {
    using PacketsT = Packets;

    explicit MasterBase(Config config)
        : config { config } {
        assert(!config.lite);
    }

    virtual PacketsT::ReadAddress receiveAR() = 0;
    virtual void sendR(PacketsT::ReadData const& x) = 0;
    virtual PacketsT::WriteAddress receiveAW() = 0;
    virtual PacketsT::WriteData receiveW() = 0;
    virtual void sendB(PacketsT::WriteResponse const& x) = 0;

    PacketsT::ReadAddress makeAR() const { return PacketsT::ReadAddress { config }; }
    PacketsT::ReadData makeR() const { return PacketsT::ReadData { config }; }
    PacketsT::WriteAddress makeAW() const { return PacketsT::WriteAddress { config }; }
    PacketsT::WriteData makeW() const { return PacketsT::WriteData { config }; }
    PacketsT::WriteResponse makeB() const { return PacketsT::WriteResponse { config }; }

    virtual ~MasterBase() = default;

    const Config config;
};

template<
    unsigned ID_WIDTH,
    unsigned ADDR_WIDTH,
    unsigned DATA_WIDTH,
    bool READ = true,
    bool WRITE = true,
    bool HAS_LOCK = true,
    bool HAS_CACHE = true,
    bool HAS_PROT = true,
    bool HAS_QOS = true,
    bool HAS_REGION = true,
    bool AXI3_COMPAT = false,
    unsigned ARUSER_WIDTH = 0,
    unsigned RUSER_WIDTH = 0,
    unsigned AWUSER_WIDTH = 0,
    unsigned WUSER_WIDTH = 0,
    unsigned BUSER_WIDTH = 0>
struct Master : MasterBase {
    using SignalsT = Signals<
        ID_WIDTH,
        ADDR_WIDTH,
        DATA_WIDTH,
        READ,
        WRITE,
        HAS_LOCK,
        HAS_CACHE,
        HAS_PROT,
        HAS_QOS,
        HAS_REGION,
        AXI3_COMPAT,
        ARUSER_WIDTH,
        RUSER_WIDTH,
        AWUSER_WIDTH,
        WUSER_WIDTH,
        BUSER_WIDTH>;

    Master(const char* name, sc_in_clk const& clock, sc_in<bool> const& reset)
        : MasterBase { SignalsT::config }
        , ar { fmt::format("{}_ar", name).c_str(), clock, reset }
        , r { fmt::format("{}_r", name).c_str(), clock, reset }
        , aw { fmt::format("{}_aw", name).c_str(), clock, reset }
        , w { fmt::format("{}_w", name).c_str(), clock, reset }
        , b { fmt::format("{}_b", name).c_str(), clock, reset } {
    }

    Sink<typename SignalsT::ReadAddress> ar;
    Source<typename SignalsT::ReadData> r;

    Sink<typename SignalsT::WriteAddress> aw;
    Sink<typename SignalsT::WriteData> w;
    Source<typename SignalsT::WriteResponse> b;

    PacketsT::ReadAddress receiveAR() override {
        auto packet = this->makeAR();
        ar.receiveTo(packet);
        return packet;
    }

    void sendR(PacketsT::ReadData const& x) override {
        r.send(x);
    }

    PacketsT::WriteAddress receiveAW() override {
        auto packet = this->makeAW();
        aw.receiveTo(packet);
        return packet;
    }

    PacketsT::WriteData receiveW() override {
        auto packet = this->makeW();
        w.receiveTo(packet);
        return packet;
    }

    void sendB(PacketsT::WriteResponse const& x) override {
        b.send(x);
    }

    virtual ~Master() = default;
};

struct SlaveBase {
    using PacketsT = Packets;

    explicit SlaveBase(Config config)
        : config { config } {
        assert(!config.lite);
    }

    virtual void sendAR(PacketsT::ReadAddress const& x) = 0;
    virtual PacketsT::ReadData receiveR() = 0;
    virtual void sendAW(PacketsT::WriteAddress const& x) = 0;
    virtual void sendW(PacketsT::WriteData const& x) = 0;
    virtual PacketsT::WriteResponse receiveB() = 0;

    PacketsT::ReadAddress makeAR() const { return PacketsT::ReadAddress { config }; }
    PacketsT::ReadData makeR() const { return PacketsT::ReadData { config }; }
    PacketsT::WriteAddress makeAW() const { return PacketsT::WriteAddress { config }; }
    PacketsT::WriteData makeW() const { return PacketsT::WriteData { config }; }
    PacketsT::WriteResponse makeB() const { return PacketsT::WriteResponse { config }; }

    virtual ~SlaveBase() = default;

    const Config config;
};

template<
    unsigned ID_WIDTH,
    unsigned ADDR_WIDTH,
    unsigned DATA_WIDTH,
    bool READ = true,
    bool WRITE = true,
    bool HAS_LOCK = true,
    bool HAS_CACHE = true,
    bool HAS_PROT = true,
    bool HAS_QOS = true,
    bool HAS_REGION = true,
    bool AXI3_COMPAT = false,
    unsigned ARUSER_WIDTH = 0,
    unsigned RUSER_WIDTH = 0,
    unsigned AWUSER_WIDTH = 0,
    unsigned WUSER_WIDTH = 0,
    unsigned BUSER_WIDTH = 0>
struct Slave : SlaveBase {
    using SignalsT = Signals<
        ID_WIDTH,
        ADDR_WIDTH,
        DATA_WIDTH,
        READ,
        WRITE,
        HAS_LOCK,
        HAS_CACHE,
        HAS_PROT,
        HAS_QOS,
        HAS_REGION,
        AXI3_COMPAT,
        ARUSER_WIDTH,
        RUSER_WIDTH,
        AWUSER_WIDTH,
        WUSER_WIDTH,
        BUSER_WIDTH>;

    Slave(const char* name, sc_in_clk const& clock, sc_in<bool> const& reset)
        : SlaveBase { SignalsT::config }
        , ar { fmt::format("{}_ar", name).c_str(), clock, reset }
        , r { fmt::format("{}_r", name).c_str(), clock, reset }
        , aw { fmt::format("{}_aw", name).c_str(), clock, reset }
        , w { fmt::format("{}_w", name).c_str(), clock, reset }
        , b { fmt::format("{}_b", name).c_str(), clock, reset } {
    }

    Source<typename SignalsT::ReadAddress> ar;
    Sink<typename SignalsT::ReadData> r;

    Source<typename SignalsT::WriteAddress> aw;
    Source<typename SignalsT::WriteData> w;
    Sink<typename SignalsT::WriteResponse> b;

    void sendAR(PacketsT::ReadAddress const& x) override {
        ar.send(x);
    }

    PacketsT::ReadData receiveR() override {
        auto packet = this->makeR();
        r.receiveTo(packet);
        return packet;
    }

    void sendAW(PacketsT::WriteAddress const& x) override {
        aw.send(x);
    }

    void sendW(PacketsT::WriteData const& x) override {
        w.send(x);
    }

    PacketsT::WriteResponse receiveB() override {
        auto packet = this->makeB();
        b.receiveTo(packet);
        return packet;
    }

    virtual ~Slave() = default;
};

} // namespace detail

using axi4::Config;
using axi4::ConfigSpec;

using detail::MasterBase;
using detail::Master;

using detail::SlaveBase;
using detail::Slave;

} // namespace chext_test::amba::axi4::full

#endif /* CHEXT_TEST_AMBA_AXI4_FULL_DRIVER_HPP_INCLUDED */
