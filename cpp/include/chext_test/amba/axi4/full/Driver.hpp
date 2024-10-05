#ifndef CHEXT_TEST_AMBA_AXI4_FULL_DRIVER_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_FULL_DRIVER_HPP_INCLUDED

#include <chext_test/amba/axi4/full/Signals.hpp>
#include <chext_test/elastic/Driver.hpp>

#include <fmt/core.h>

#include <systemc>

namespace chext_test::amba::axi4::full {

namespace detail {

using elastic::Sink;
using elastic::Source;

using namespace sc_core;

struct MasterBase {
    using PacketsT = Packets;

    virtual PacketsT::ReadAddress receiveAR() = 0;
    virtual void sendR(PacketsT::ReadData const& x) = 0;
    virtual PacketsT::WriteAddress receiveAW() = 0;
    virtual PacketsT::WriteData receiveW() = 0;
    virtual void sendB(PacketsT::WriteResponse const& x) = 0;

    virtual ~MasterBase() = default;
};

template<
    unsigned ID_WIDTH,
    unsigned ADDR_WIDTH,
    unsigned DATA_WIDTH,
    unsigned ARUSER_WIDTH = 32,
    unsigned RUSER_WIDTH = 32,
    unsigned AWUSER_WIDTH = 32,
    unsigned WUSER_WIDTH = 32,
    unsigned BUSER_WIDTH = 32,
    bool AXI3_COMPAT = false>
struct Master : MasterBase {
    using SignalsT = Signals<
        ID_WIDTH,
        ADDR_WIDTH,
        DATA_WIDTH,
        ARUSER_WIDTH,
        RUSER_WIDTH,
        AWUSER_WIDTH,
        WUSER_WIDTH,
        BUSER_WIDTH,
        AXI3_COMPAT>;

    Master(const char* name, sc_in_clk const& clock, sc_in<bool> const& reset)
        : ar { fmt::format("{}_ar", name).c_str(), clock, reset }
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
        return ar.receive();
    }

    void sendR(PacketsT::ReadData const& x) override {
        r.send(x);
    }

    PacketsT::WriteAddress receiveAW() override {
        return aw.receive();
    }

    PacketsT::WriteData receiveW() override {
        return w.receive();
    }

    void sendB(PacketsT::WriteResponse const& x) override {
        b.send(x);
    }

    virtual ~Master() = default;
};

struct SlaveBase {
    using PacketsT = Packets;

    virtual void sendAR(PacketsT::ReadAddress const& x) = 0;
    virtual PacketsT::ReadData receiveR() = 0;
    virtual void sendAW(PacketsT::WriteAddress const& x) = 0;
    virtual void sendW(PacketsT::WriteData const& x) = 0;
    virtual PacketsT::WriteResponse receiveB() = 0;

    virtual ~SlaveBase() = default;
};

template<
    unsigned ID_WIDTH,
    unsigned ADDR_WIDTH,
    unsigned DATA_WIDTH,
    unsigned ARUSER_WIDTH = 32,
    unsigned RUSER_WIDTH = 32,
    unsigned AWUSER_WIDTH = 32,
    unsigned WUSER_WIDTH = 32,
    unsigned BUSER_WIDTH = 32,
    bool AXI3_COMPAT = false>
struct Slave : SlaveBase {
    using SignalsT = Signals<
        ID_WIDTH,
        ADDR_WIDTH,
        DATA_WIDTH,
        ARUSER_WIDTH,
        RUSER_WIDTH,
        AWUSER_WIDTH,
        WUSER_WIDTH,
        BUSER_WIDTH,
        AXI3_COMPAT>;

    Slave(const char* name, sc_in_clk const& clock, sc_in<bool> const& reset)
        : ar { fmt::format("{}_ar", name).c_str(), clock, reset }
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
        return r.receive();
    }

    void sendAW(PacketsT::WriteAddress const& x) override {
        aw.send(x);
    }

    void sendW(PacketsT::WriteData const& x) override {
        w.send(x);
    }

    PacketsT::WriteResponse receiveB() override {
        return b.receive();
    }

    virtual ~Slave() = default;
};

} // namespace detail

using detail::Master;
using detail::Slave;

} // namespace chext_test::amba::axi4

#endif /* CHEXT_TEST_AMBA_AXI4_FULL_DRIVER_HPP_INCLUDED */
