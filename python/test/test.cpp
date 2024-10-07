#include <ScmyModule.hpp>

ScmyModule::ScmyModule(sc_core::sc_module_name const& moduleName) :
    sc_module{ moduleName },
    verilatedModule_("verilatedModule"),
    clock("clock"),
    reset("reset"),
    irq("irq"),
    s_axil_management("s_axil_management", verilatedModule_.clock, verilatedModule_.reset),
    m_axi("m_axi", verilatedModule_.clock, verilatedModule_.reset),
    source1("source1", verilatedModule_.clock, verilatedModule_.reset),
    source2("source2", verilatedModule_.clock, verilatedModule_.reset),
    source3("source3", verilatedModule_.clock, verilatedModule_.reset) {

    /* BEGIN: clock ports (conn) */
    verilatedModule_.clock(clock);
    /* END: clock ports (conn) */

    /* BEGIN: reset ports (conn) */
    verilatedModule_.reset(reset);
    /* END: reset ports (conn) */

    /* generate inverted resets */
    SC_METHOD(generateInvertedResetPorts);
    sensitive
        << reset
    ;

    /* BEGIN: interrupt ports (conn) */
    verilatedModule_.irq(irq);
    /* END: interrupt ports (conn) */

    /* BEGIN: data ports (conn) */
    /* END: data ports (conn) */

    /* BEGIN: register ports */
    set("clock", clock);
    set("reset", reset);
    set("irq", irq);
    /* END: register ports */

    /* BEGIN: chext_test ctor for 'amba/axi4' */
    verilatorModule_.s_axil_management_AWVALID(this->s_axil_management.aw.valid);
    verilatorModule_.s_axil_management_AWREADY(this->s_axil_management.aw.ready);
    verilatorModule_.s_axil_management_AWADDR(this->s_axil_management.aw.bits.addr);
    verilatorModule_.s_axil_management_WVALID(this->s_axil_management.w.valid);
    verilatorModule_.s_axil_management_WREADY(this->s_axil_management.w.ready);
    verilatorModule_.s_axil_management_WDATA(this->s_axil_management.w.bits.data);
    verilatorModule_.s_axil_management_WSTRB(this->s_axil_management.w.bits.strb);
    verilatorModule_.s_axil_management_BVALID(this->s_axil_management.b.valid);
    verilatorModule_.s_axil_management_BREADY(this->s_axil_management.b.ready);
    verilatorModule_.s_axil_management_BRESP(this->s_axil_management.b.bits.resp);
    verilatorModule_.s_axil_management_ARVALID(this->s_axil_management.ar.valid);
    verilatorModule_.s_axil_management_ARREADY(this->s_axil_management.ar.ready);
    verilatorModule_.s_axil_management_ARADDR(this->s_axil_management.ar.bits.addr);
    verilatorModule_.s_axil_management_RVALID(this->s_axil_management.r.valid);
    verilatorModule_.s_axil_management_RREADY(this->s_axil_management.r.ready);
    verilatorModule_.s_axil_management_RDATA(this->s_axil_management.r.bits.data);
    verilatorModule_.s_axil_management_RRESP(this->s_axil_management.r.bits.resp);

    verilatorModule_.m_axi_AWVALID(this->m_axi.aw.valid);
    verilatorModule_.m_axi_AWREADY(this->m_axi.aw.ready);
    verilatorModule_.m_axi_AWADDR(this->m_axi.aw.bits.addr);
    verilatorModule_.m_axi_AWREGION(this->m_axi.aw.bits.region);
    verilatorModule_.m_axi_AWCACHE(this->m_axi.aw.bits.cache);
    verilatorModule_.m_axi_AWBURST(this->m_axi.aw.bits.burst);
    verilatorModule_.m_axi_AWSIZE(this->m_axi.aw.bits.size);
    verilatorModule_.m_axi_AWLEN(this->m_axi.aw.bits.len);
    verilatorModule_.m_axi_AWID(this->m_axi.aw.bits.id);
    verilatorModule_.m_axi_AWLOCK(this->m_axi.aw.bits.lock);
    verilatorModule_.m_axi_WVALID(this->m_axi.w.valid);
    verilatorModule_.m_axi_WREADY(this->m_axi.w.ready);
    verilatorModule_.m_axi_WDATA(this->m_axi.w.bits.data);
    verilatorModule_.m_axi_WSTRB(this->m_axi.w.bits.strb);
    verilatorModule_.m_axi_WLAST(this->m_axi.w.bits.last);
    verilatorModule_.m_axi_BVALID(this->m_axi.b.valid);
    verilatorModule_.m_axi_BREADY(this->m_axi.b.ready);
    verilatorModule_.m_axi_BRESP(this->m_axi.b.bits.resp);
    verilatorModule_.m_axi_BID(this->m_axi.b.bits.id);
    verilatorModule_.m_axi_ARVALID(this->m_axi.ar.valid);
    verilatorModule_.m_axi_ARREADY(this->m_axi.ar.ready);
    verilatorModule_.m_axi_ARADDR(this->m_axi.ar.bits.addr);
    verilatorModule_.m_axi_ARUSER(this->m_axi.ar.bits.user);
    verilatorModule_.m_axi_ARREGION(this->m_axi.ar.bits.region);
    verilatorModule_.m_axi_ARCACHE(this->m_axi.ar.bits.cache);
    verilatorModule_.m_axi_ARBURST(this->m_axi.ar.bits.burst);
    verilatorModule_.m_axi_ARSIZE(this->m_axi.ar.bits.size);
    verilatorModule_.m_axi_ARLEN(this->m_axi.ar.bits.len);
    verilatorModule_.m_axi_ARID(this->m_axi.ar.bits.id);
    verilatorModule_.m_axi_ARLOCK(this->m_axi.ar.bits.lock);
    verilatorModule_.m_axi_RVALID(this->m_axi.r.valid);
    verilatorModule_.m_axi_RREADY(this->m_axi.r.ready);
    verilatorModule_.m_axi_RDATA(this->m_axi.r.bits.data);
    verilatorModule_.m_axi_RRESP(this->m_axi.r.bits.resp);
    verilatorModule_.m_axi_RUSER(this->m_axi.r.bits.user);
    verilatorModule_.m_axi_RID(this->m_axi.r.bits.id);
    verilatorModule_.m_axi_RLAST(this->m_axi.r.bits.last);

    /* END: chext_test ctor for 'amba/axi4' */

    /* BEGIN: chext_test ctor for 'elastic' */
    verilatorModule_.source1_bits(this->source1.bits);
    verilatorModule_.source1_ready(this->source1.ready);
    verilatorModule_.source1_valid(this->source1.valid);

    verilatorModule_.source2_bits_data(this->source2.bits.data);
    verilatorModule_.source2_bits_last(this->source2.bits.last);
    verilatorModule_.source2_ready(this->source2.ready);
    verilatorModule_.source2_valid(this->source2.valid);

    verilatorModule_.source3_bits_data(this->source3.bits.data);
    verilatorModule_.source3_bits_id(this->source3.bits.id);
    verilatorModule_.source3_ready(this->source3.ready);
    verilatorModule_.source3_valid(this->source3.valid);

    /* END: chext_test ctor for 'elastic' */

}

ScmyModule::~ScmyModule() {

}

void ScmyModule::generateInvertedResetPorts() {

    reset_INVERTED_.write(!reset.read());

}

