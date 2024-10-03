#ifndef CHEXT_TEST_AMBA_AXI4_SIGNALS_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_SIGNALS_HPP_INCLUDED

#include <chext_test/amba/axi4/Packets.hpp>
#include <fmt/core.h>
#include <systemc>

namespace chext_test::amba::axi4 {

namespace detail {

using namespace sc_core;
using namespace sc_dt;

template<
    unsigned ID_WIDTH,
    unsigned ADDR_WIDTH,
    unsigned DATA_WIDTH,
    bool AXI3_COMPAT = false,
    unsigned ARUSER_WIDTH = 32,
    unsigned RUSER_WIDTH = 32,
    unsigned AWUSER_WIDTH = 32,
    unsigned WUSER_WIDTH = 32,
    unsigned BUSER_WIDTH = 32>
struct Signals {
    static constexpr unsigned wId = ID_WIDTH;
    static constexpr unsigned wAddr = ADDR_WIDTH;
    static constexpr unsigned wData = DATA_WIDTH;
    static constexpr bool axi3Compat = AXI3_COMPAT;
    static constexpr unsigned wLen = axi3Compat ? 4 : 8;
    static constexpr unsigned wLock = axi3Compat ? 2 : 1;
    static constexpr unsigned wUserAR = ARUSER_WIDTH;
    static constexpr unsigned wUserR = RUSER_WIDTH;
    static constexpr unsigned wUserAW = AWUSER_WIDTH;
    static constexpr unsigned wUserW = WUSER_WIDTH;
    static constexpr unsigned wUserB = BUSER_WIDTH;

    sc_in_clk& ACLK;
    sc_in<bool>& ARESETn;

    sc_signal<bool> ARREADY;
    sc_signal<bool> ARVALID;
    sc_signal<sc_bv<wId>> ARID;
    sc_signal<sc_bv<wAddr>> ARADDR;
    sc_signal<sc_bv<wLen>> ARLEN;
    sc_signal<sc_bv<3>> ARSIZE;
    sc_signal<sc_bv<2>> ARBURST;
    sc_signal<sc_bv<wLock>> ARLOCK;
    sc_signal<sc_bv<4>> ARCACHE;
    sc_signal<sc_bv<3>> ARPROT;
    sc_signal<sc_bv<4>> ARQOS;
    sc_signal<sc_bv<4>> ARREGION;
    sc_signal<sc_bv<wUserAR>> ARUSER;

    sc_signal<bool> RREADY;
    sc_signal<bool> RVALID;
    sc_signal<sc_bv<wId>> RID;
    sc_signal<sc_bv<wData>> RDATA;
    sc_signal<sc_bv<2>> RRESP;
    sc_signal<bool> RLAST;
    sc_signal<sc_bv<wUserR>> RUSER;

    sc_signal<bool> AWREADY;
    sc_signal<bool> AWVALID;
    sc_signal<sc_bv<wId>> AWID;
    sc_signal<sc_bv<wAddr>> AWADDR;
    sc_signal<sc_bv<wLen>> AWLEN;
    sc_signal<sc_bv<3>> AWSIZE;
    sc_signal<sc_bv<2>> AWBURST;
    sc_signal<sc_bv<wLock>> AWLOCK;
    sc_signal<sc_bv<4>> AWCACHE;
    sc_signal<sc_bv<3>> AWPROT;
    sc_signal<sc_bv<4>> AWQOS;
    sc_signal<sc_bv<4>> AWREGION;
    sc_signal<sc_bv<wUserAW>> AWUSER;

    sc_signal<bool> WREADY;
    sc_signal<bool> WVALID;
    sc_signal<sc_bv<wData>> WDATA;
    sc_signal<sc_bv<8>> WSTRB;
    sc_signal<bool> WLAST;
    sc_signal<sc_bv<wUserW>> WUSER;

    sc_signal<bool> BREADY;
    sc_signal<bool> BVALID;
    sc_signal<sc_bv<wId>> BID;
    sc_signal<sc_bv<2>> BRESP;
    sc_signal<sc_bv<wUserB>> BUSER;

    Signals(const char* name)
        : ACLK(fmt::format("{}_ACLK", name).c_str())
        , ARESETn(fmt::format("{}_ARESETn", name).c_str())
        , ARREADY(fmt::format("{}_ARREADY", name).c_str())
        , ARVALID(fmt::format("{}_ARVALID", name).c_str())
        , ARID(fmt::format("{}_ARID", name).c_str())
        , ARADDR(fmt::format("{}_ARADDR", name).c_str())
        , ARLEN(fmt::format("{}_ARLEN", name).c_str())
        , ARSIZE(fmt::format("{}_ARSIZE", name).c_str())
        , ARBURST(fmt::format("{}_ARBURST", name).c_str())
        , ARLOCK(fmt::format("{}_ARLOCK", name).c_str())
        , ARCACHE(fmt::format("{}_ARCACHE", name).c_str())
        , ARPROT(fmt::format("{}_ARPROT", name).c_str())
        , ARQOS(fmt::format("{}_ARQOS", name).c_str())
        , ARREGION(fmt::format("{}_ARREGION", name).c_str())
        , ARUSER(fmt::format("{}_ARUSER", name).c_str())
        , RREADY(fmt::format("{}_RREADY", name).c_str())
        , RVALID(fmt::format("{}_RVALID", name).c_str())
        , RID(fmt::format("{}_RID", name).c_str())
        , RDATA(fmt::format("{}_RDATA", name).c_str())
        , RRESP(fmt::format("{}_RRESP", name).c_str())
        , RLAST(fmt::format("{}_RLAST", name).c_str())
        , RUSER(fmt::format("{}_RUSER", name).c_str())
        , AWREADY(fmt::format("{}_AWREADY", name).c_str())
        , AWVALID(fmt::format("{}_AWVALID", name).c_str())
        , AWID(fmt::format("{}_AWID", name).c_str())
        , AWADDR(fmt::format("{}_AWADDR", name).c_str())
        , AWLEN(fmt::format("{}_AWLEN", name).c_str())
        , AWSIZE(fmt::format("{}_AWSIZE", name).c_str())
        , AWBURST(fmt::format("{}_AWBURST", name).c_str())
        , AWLOCK(fmt::format("{}_AWLOCK", name).c_str())
        , AWCACHE(fmt::format("{}_AWCACHE", name).c_str())
        , AWPROT(fmt::format("{}_AWPROT", name).c_str())
        , AWQOS(fmt::format("{}_AWQOS", name).c_str())
        , AWREGION(fmt::format("{}_AWREGION", name).c_str())
        , AWUSER(fmt::format("{}_AWUSER", name).c_str())
        , WREADY(fmt::format("{}_WREADY", name).c_str())
        , WVALID(fmt::format("{}_WVALID", name).c_str())
        , WDATA(fmt::format("{}_WDATA", name).c_str())
        , WSTRB(fmt::format("{}_WSTRB", name).c_str())
        , WLAST(fmt::format("{}_WLAST", name).c_str())
        , WUSER(fmt::format("{}_WUSER", name).c_str())
        , BREADY(fmt::format("{}_BREADY", name).c_str())
        , BVALID(fmt::format("{}_BVALID", name).c_str())
        , BID(fmt::format("{}_BID", name).c_str())
        , BRESP(fmt::format("{}_BRESP", name).c_str())
        , BUSER(fmt::format("{}_BUSER", name).c_str()) {
    }

    void sendAR(AddressPacket const& ar) override {
        ARVALID.write(true);
        ARID.write(ar.id);
        ARADDR.write(ar.addr);
        ARLEN.write(ar.len);
        ARSIZE.write(ar.size);
        ARBURST.write(ar.burst);
        ARLOCK.write(ar.lock);
        ARQOS.write(ar.qos);
        ARREGION.write(ar.region);
        ARUSER.write(ar.user);
    }

    AddressPacket recvAR() override {
        AddressPacket ar;
        ar.id = ARID.read();
        ar.addr = ARADDR.read();
        ar.len = ARLEN.read();
        ar.size = ARSIZE.read();
        ar.burst = ARBURST.read();
        ar.lock = ARLOCK.read();
        ar.qos = ARQOS.read();
        ar.region = ARREGION.read();
        ar.user = ARUSER.read();
        return ar;
    }

    void sendAW(AddressPacket const& aw) override {
        AWVALID.write(true);
        AWID.write(aw.id);
        AWADDR.write(aw.addr);
        AWLEN.write(aw.len);
        AWSIZE.write(aw.size);
        AWBURST.write(aw.burst);
        AWLOCK.write(aw.lock);
        AWQOS.write(aw.qos);
        AWREGION.write(aw.region);
        AWUSER.write(aw.user);
    }

    AddressPacket recvAW() override {
        AddressPacket aw;
        aw.id = AWID.read();
        aw.addr = AWADDR.read();
        aw.len = AWLEN.read();
        aw.size = AWSIZE.read();
        aw.burst = AWBURST.read();
        aw.lock = AWLOCK.read();
        aw.qos = AWQOS.read();
        aw.region = AWREGION.read();
        aw.user = AWUSER.read();
        return aw;
    }

    void sendW(WriteDataPacket const& w) override {
        WVALID.write(true);
        WDATA.write(w.data);
        WSTRB.write(w.strb);
        WLAST.write(w.last);
        WUSER.write(w.user);
    }

    WriteDataPacket recvW() override {
        WriteDataPacket w;
        w.data = WDATA.read();
        w.strb = WSTRB.read();
        w.last = WLAST.read();
        w.user = WUSER.read();
        return w;
    }

    void sendR(ReadDataPacket const& r) override {
        RVALID.write(true);
        RID.write(r.id);
        RDATA.write(r.data);
        RRESP.write(r.resp);
        RLAST.write(r.last);
        RUSER.write(r.user);
    }

    ReadDataPacket recvR() override {
        ReadDataPacket r;
        r.id = RID.read();
        r.data = RDATA.read();
        r.resp = RRESP.read();
        r.last = RLAST.read();
        r.user = RUSER.read();
        return r;
    }

    void sendB(WriteResponsePacket const& b) override {
        BVALID.write(true);
        BID.write(b.id);
        BRESP.write(b.resp);
    }

    WriteResponsePacket recvB() override {
        WriteResponsePacket b;
        b.id = BID.read();
        b.resp = BRESP.read();
        return b;
    }
};

} // namespace detail

using detail::Signals;

} // namespace chext_test::amba::axi4

#endif /* CHEXT_TEST_AMBA_AXI4_SIGNALS_HPP_INCLUDED */
