#if !defined(__SCMYMODULE_HPP_INCLUDED__)
#define __SCMYMODULE_HPP_INCLUDED__

#include <VmyModule.h>

#include <systemc>
#include <tlm>

/* BEGIN: chext_test includes added by 'EncodedDataListHook' */
#include <cstdint>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <jqr/core.hpp>
#include <jqr/comp_eq.hpp>
#include <jqr/dump.hpp>
#include <chext_test/vutil.hpp>
/* END: chext_test includes added by 'EncodedDataListHook' */

#include <hdlscw/wrapper_base.hpp>

/* BEGIN: chext_test includes for 'amba/axi4' */
#include <chext_test/amba/axi4/full/Driver.hpp>
#include <chext_test/amba/axi4/lite/Driver.hpp>
/* END: chext_test includes for 'amba/axi4' */

/* BEGIN: chext_test includes for 'amba/axi4s' */
#include <chext_test/amba/axi4s/Driver.hpp>
/* END: chext_test includes for 'amba/axi4s' */

/* BEGIN: chext_test includes for 'elastic' */
#include <chext_test/elastic/Driver.hpp>
#include <chext_test/vutil.hpp>
#include <chext_test/elastic/DataLast.hpp>
#include <Packet.hpp>
/* END: chext_test includes for 'elastic' */

/** @brief ScmyModule */
class ScmyModule final :
    public sc_core::sc_module,
    public hdlscw::wrapper_base {
public:

    explicit ScmyModule(sc_core::sc_module_name const& moduleName = "");

    SC_HAS_PROCESS(ScmyModule);

    struct MyTestBundle1_value {
        // f0: UInt<8>
        std::uint8_t f0;

        // f1: UInt<18>
        std::uint32_t f1;

        // f2_0: UInt<6>
        std::uint8_t f2_0;

        // f2_1: UInt<6>
        std::uint8_t f2_1;

        // f2_2: UInt<6>
        std::uint8_t f2_2;

        // f2_3: UInt<6>
        std::uint8_t f2_3;

        // f3_f0: UInt<15>
        std::uint16_t f3_f0;

        // f3_f1: Bool
        bool f3_f1;

        JQR_DECL(
            MyTestBundle1_value,
            JQR_MEMBER(f0),
            JQR_MEMBER(f1),
            JQR_MEMBER(f2_0),
            JQR_MEMBER(f2_1),
            JQR_MEMBER(f2_2),
            JQR_MEMBER(f2_3),
            JQR_MEMBER(f3_f0),
            JQR_MEMBER(f3_f1)
        )

        JQR_TO_STRING
        JQR_OSTREAM
        JQR_COMP_EQ
    };

    struct MyTestBundle1_signals {
        using value_type = MyTestBundle1_value;

        // f0: UInt<8>
        chext_test::vutil::signal_t<8, sc_core::SC_MANY_WRITERS> f0;

        // f1: UInt<18>
        chext_test::vutil::signal_t<18, sc_core::SC_MANY_WRITERS> f1;

        // f2_0: UInt<6>
        chext_test::vutil::signal_t<6, sc_core::SC_MANY_WRITERS> f2_0;

        // f2_1: UInt<6>
        chext_test::vutil::signal_t<6, sc_core::SC_MANY_WRITERS> f2_1;

        // f2_2: UInt<6>
        chext_test::vutil::signal_t<6, sc_core::SC_MANY_WRITERS> f2_2;

        // f2_3: UInt<6>
        chext_test::vutil::signal_t<6, sc_core::SC_MANY_WRITERS> f2_3;

        // f3_f0: UInt<15>
        chext_test::vutil::signal_t<15, sc_core::SC_MANY_WRITERS> f3_f0;

        // f3_f1: Bool
        chext_test::vutil::signal_t<1, sc_core::SC_MANY_WRITERS> f3_f1;

        private:
            template<unsigned W, typename ValueT, typename SignalT>
            static ValueT readField(SignalT const& signal) {
                ValueT value {};
                chext_test::vutil::read<W>(signal, value);
                return value;
            }

        public:
            MyTestBundle1_signals(const char* name) :
                f0(fmt::format("{}_f0", name).c_str()),
                f1(fmt::format("{}_f1", name).c_str()),
                f2_0(fmt::format("{}_f2_0", name).c_str()),
                f2_1(fmt::format("{}_f2_1", name).c_str()),
                f2_2(fmt::format("{}_f2_2", name).c_str()),
                f2_3(fmt::format("{}_f2_3", name).c_str()),
                f3_f0(fmt::format("{}_f3_f0", name).c_str()),
                f3_f1(fmt::format("{}_f3_f1", name).c_str()) { /* empty ctor */ }

            void readTo(value_type & x) {
                x.~value_type();

                new (&x) value_type {
                    readField<8, std::uint8_t>(f0),
                    readField<18, std::uint32_t>(f1),
                    readField<6, std::uint8_t>(f2_0),
                    readField<6, std::uint8_t>(f2_1),
                    readField<6, std::uint8_t>(f2_2),
                    readField<6, std::uint8_t>(f2_3),
                    readField<15, std::uint16_t>(f3_f0),
                    readField<1, bool>(f3_f1)
                };
            }

            void writeFrom(value_type const& x) {
                chext_test::vutil::write<8>(f0, x.f0);
                chext_test::vutil::write<18>(f1, x.f1);
                chext_test::vutil::write<6>(f2_0, x.f2_0);
                chext_test::vutil::write<6>(f2_1, x.f2_1);
                chext_test::vutil::write<6>(f2_2, x.f2_2);
                chext_test::vutil::write<6>(f2_3, x.f2_3);
                chext_test::vutil::write<15>(f3_f0, x.f3_f0);
                chext_test::vutil::write<1>(f3_f1, x.f3_f1);
            }

    };

    struct MyTestBundle2_value {
        // f0_0: UInt<6>
        std::uint8_t f0_0;

        // f0_1: UInt<6>
        std::uint8_t f0_1;

        // f0_2: UInt<6>
        std::uint8_t f0_2;

        // f0_3: UInt<6>
        std::uint8_t f0_3;

        // f1_f0: UInt<15>
        std::uint16_t f1_f0;

        // f1_f1: Bool
        bool f1_f1;

        JQR_DECL(
            MyTestBundle2_value,
            JQR_MEMBER(f0_0),
            JQR_MEMBER(f0_1),
            JQR_MEMBER(f0_2),
            JQR_MEMBER(f0_3),
            JQR_MEMBER(f1_f0),
            JQR_MEMBER(f1_f1)
        )

        JQR_TO_STRING
        JQR_OSTREAM
        JQR_COMP_EQ
    };

    struct MyTestBundle2_signals {
        using value_type = MyTestBundle2_value;

        // f0_0: UInt<6>
        chext_test::vutil::signal_t<6, sc_core::SC_MANY_WRITERS> f0_0;

        // f0_1: UInt<6>
        chext_test::vutil::signal_t<6, sc_core::SC_MANY_WRITERS> f0_1;

        // f0_2: UInt<6>
        chext_test::vutil::signal_t<6, sc_core::SC_MANY_WRITERS> f0_2;

        // f0_3: UInt<6>
        chext_test::vutil::signal_t<6, sc_core::SC_MANY_WRITERS> f0_3;

        // f1_f0: UInt<15>
        chext_test::vutil::signal_t<15, sc_core::SC_MANY_WRITERS> f1_f0;

        // f1_f1: Bool
        chext_test::vutil::signal_t<1, sc_core::SC_MANY_WRITERS> f1_f1;

        private:
            template<unsigned W, typename ValueT, typename SignalT>
            static ValueT readField(SignalT const& signal) {
                ValueT value {};
                chext_test::vutil::read<W>(signal, value);
                return value;
            }

        public:
            MyTestBundle2_signals(const char* name) :
                f0_0(fmt::format("{}_f0_0", name).c_str()),
                f0_1(fmt::format("{}_f0_1", name).c_str()),
                f0_2(fmt::format("{}_f0_2", name).c_str()),
                f0_3(fmt::format("{}_f0_3", name).c_str()),
                f1_f0(fmt::format("{}_f1_f0", name).c_str()),
                f1_f1(fmt::format("{}_f1_f1", name).c_str()) { /* empty ctor */ }

            void readTo(value_type & x) {
                x.~value_type();

                new (&x) value_type {
                    readField<6, std::uint8_t>(f0_0),
                    readField<6, std::uint8_t>(f0_1),
                    readField<6, std::uint8_t>(f0_2),
                    readField<6, std::uint8_t>(f0_3),
                    readField<15, std::uint16_t>(f1_f0),
                    readField<1, bool>(f1_f1)
                };
            }

            void writeFrom(value_type const& x) {
                chext_test::vutil::write<6>(f0_0, x.f0_0);
                chext_test::vutil::write<6>(f0_1, x.f0_1);
                chext_test::vutil::write<6>(f0_2, x.f0_2);
                chext_test::vutil::write<6>(f0_3, x.f0_3);
                chext_test::vutil::write<15>(f1_f0, x.f1_f0);
                chext_test::vutil::write<1>(f1_f1, x.f1_f1);
            }

    };

    /* BEGIN: clock ports (decl) */
    sc_core::sc_in_clk clock;
    /* END: clock ports (decl) */

    /* BEGIN: reset ports (decl) */
    sc_core::sc_in<bool> reset;
    /* END: reset ports (decl) */

    /* BEGIN: interrupt ports (decl) */
    sc_core::sc_out<bool> irq;
    /* END: interrupt ports (decl) */

    /* BEGIN: data ports (decl) */
    /* END: data ports (decl) */

    sc_core::sc_module* getThisModule() noexcept override {
        return this;
    }

    sc_core::sc_module const* getThisModule() const noexcept override {
        return this;
    }

    sc_core::sc_module* getVerilatedModule() noexcept override {
        return &verilatedModule_;
    }

    sc_core::sc_module const* getVerilatedModule() const noexcept override {
        return &verilatedModule_;
    }

    #if defined(VERILATED_TRACE_ENABLED)
    void traceVerilated(VerilatedVcdC* tfp, int levels, int options = 0) override {
        return verilatedModule_.trace(tfp, levels, options);
    }
    #endif

    /* BEGIN: chext_test public for 'amba/axi4' */
    chext_test::amba::axi4::lite::Slave<20,32> s_axil_management;
    chext_test::amba::axi4::full::Master<4,32,256,true,true,true,true,false,false,true,false,4,4,0,0,0> m_axi;
    /* END: chext_test public for 'amba/axi4' */

    /* BEGIN: chext_test public for 'amba/axi4s' */
    chext_test::amba::axi4s::Master<64,4,2,8,true,true,true,true> m_axis;
    chext_test::amba::axi4s::Slave<32,0,0,1,false,false,true,false> s_axis_hier;
    /* END: chext_test public for 'amba/axi4s' */

    /* BEGIN: chext_test public for 'elastic' */
    chext_test::elastic::Source<chext_test::vutil::signal_t<32>> source1;
    chext_test::elastic::Source<chext_test::elastic::DataLastSignals<64>> source2;
    chext_test::elastic::Source<PacketSignals<128>> source3;
    chext_test::elastic::Source<MyTestBundle1_signals> source4;
    chext_test::elastic::Source<MyTestBundle2_signals> source4;
    /* END: chext_test public for 'elastic' */

    virtual ~ScmyModule();

private:

    VmyModule verilatedModule_;

    /* BEGIN: inverted reset signals */
    sc_core::sc_signal<bool> reset_INVERTED_;
    /* END: inverted reset signals */

    void generateInvertedResetPorts();

protected:

}; /* class ScmyModule */

inline
ScmyModule::ScmyModule(sc_core::sc_module_name const& moduleName) :
    sc_module{ moduleName },
    verilatedModule_("verilatedModule"),
    clock("clock"),
    reset("reset"),
    irq("irq"),
    s_axil_management("s_axil_management", clock, reset),
    m_axi("m_axi", clock, reset),
    m_axis("m_axis", clock, reset),
    s_axis_hier("s_axis_hier", clock, reset),
    source1("source1", clock, reset),
    source2("source2", clock, reset),
    source3("source3", clock, reset),
    source4("source4", clock, reset),
    source4("source4", clock, reset) {

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
    verilatedModule_.s_axil_management_AWVALID(this->s_axil_management.aw.valid);
    verilatedModule_.s_axil_management_AWREADY(this->s_axil_management.aw.ready);
    verilatedModule_.s_axil_management_AWADDR(this->s_axil_management.aw.bits.addr);
    verilatedModule_.s_axil_management_WVALID(this->s_axil_management.w.valid);
    verilatedModule_.s_axil_management_WREADY(this->s_axil_management.w.ready);
    verilatedModule_.s_axil_management_WDATA(this->s_axil_management.w.bits.data);
    verilatedModule_.s_axil_management_WSTRB(this->s_axil_management.w.bits.strb);
    verilatedModule_.s_axil_management_BVALID(this->s_axil_management.b.valid);
    verilatedModule_.s_axil_management_BREADY(this->s_axil_management.b.ready);
    verilatedModule_.s_axil_management_BRESP(this->s_axil_management.b.bits.resp);
    verilatedModule_.s_axil_management_ARVALID(this->s_axil_management.ar.valid);
    verilatedModule_.s_axil_management_ARREADY(this->s_axil_management.ar.ready);
    verilatedModule_.s_axil_management_ARADDR(this->s_axil_management.ar.bits.addr);
    verilatedModule_.s_axil_management_RVALID(this->s_axil_management.r.valid);
    verilatedModule_.s_axil_management_RREADY(this->s_axil_management.r.ready);
    verilatedModule_.s_axil_management_RDATA(this->s_axil_management.r.bits.data);
    verilatedModule_.s_axil_management_RRESP(this->s_axil_management.r.bits.resp);

    verilatedModule_.m_axi_AWVALID(this->m_axi.aw.valid);
    verilatedModule_.m_axi_AWREADY(this->m_axi.aw.ready);
    verilatedModule_.m_axi_AWADDR(this->m_axi.aw.bits.addr);
    verilatedModule_.m_axi_AWREGION(this->m_axi.aw.bits.region);
    verilatedModule_.m_axi_AWCACHE(this->m_axi.aw.bits.cache);
    verilatedModule_.m_axi_AWBURST(this->m_axi.aw.bits.burst);
    verilatedModule_.m_axi_AWSIZE(this->m_axi.aw.bits.size);
    verilatedModule_.m_axi_AWLEN(this->m_axi.aw.bits.len);
    verilatedModule_.m_axi_AWID(this->m_axi.aw.bits.id);
    verilatedModule_.m_axi_AWLOCK(this->m_axi.aw.bits.lock);
    verilatedModule_.m_axi_WVALID(this->m_axi.w.valid);
    verilatedModule_.m_axi_WREADY(this->m_axi.w.ready);
    verilatedModule_.m_axi_WDATA(this->m_axi.w.bits.data);
    verilatedModule_.m_axi_WSTRB(this->m_axi.w.bits.strb);
    verilatedModule_.m_axi_WLAST(this->m_axi.w.bits.last);
    verilatedModule_.m_axi_BVALID(this->m_axi.b.valid);
    verilatedModule_.m_axi_BREADY(this->m_axi.b.ready);
    verilatedModule_.m_axi_BRESP(this->m_axi.b.bits.resp);
    verilatedModule_.m_axi_BID(this->m_axi.b.bits.id);
    verilatedModule_.m_axi_ARVALID(this->m_axi.ar.valid);
    verilatedModule_.m_axi_ARREADY(this->m_axi.ar.ready);
    verilatedModule_.m_axi_ARADDR(this->m_axi.ar.bits.addr);
    verilatedModule_.m_axi_ARUSER(this->m_axi.ar.bits.user);
    verilatedModule_.m_axi_ARREGION(this->m_axi.ar.bits.region);
    verilatedModule_.m_axi_ARCACHE(this->m_axi.ar.bits.cache);
    verilatedModule_.m_axi_ARBURST(this->m_axi.ar.bits.burst);
    verilatedModule_.m_axi_ARSIZE(this->m_axi.ar.bits.size);
    verilatedModule_.m_axi_ARLEN(this->m_axi.ar.bits.len);
    verilatedModule_.m_axi_ARID(this->m_axi.ar.bits.id);
    verilatedModule_.m_axi_ARLOCK(this->m_axi.ar.bits.lock);
    verilatedModule_.m_axi_RVALID(this->m_axi.r.valid);
    verilatedModule_.m_axi_RREADY(this->m_axi.r.ready);
    verilatedModule_.m_axi_RDATA(this->m_axi.r.bits.data);
    verilatedModule_.m_axi_RRESP(this->m_axi.r.bits.resp);
    verilatedModule_.m_axi_RUSER(this->m_axi.r.bits.user);
    verilatedModule_.m_axi_RID(this->m_axi.r.bits.id);
    verilatedModule_.m_axi_RLAST(this->m_axi.r.bits.last);

    /* END: chext_test ctor for 'amba/axi4' */

    /* BEGIN: chext_test ctor for 'amba/axi4s' */
    verilatedModule_.m_axis_TVALID(this->m_axis.valid);
    verilatedModule_.m_axis_TREADY(this->m_axis.ready);
    verilatedModule_.m_axis_TDATA(this->m_axis.bits.data);
    verilatedModule_.m_axis_TKEEP(this->m_axis.bits.keep);
    verilatedModule_.m_axis_TSTRB(this->m_axis.bits.strb);
    verilatedModule_.m_axis_TLAST(this->m_axis.bits.last);
    verilatedModule_.m_axis_TID(this->m_axis.bits.id);
    verilatedModule_.m_axis_TDEST(this->m_axis.bits.dest);
    verilatedModule_.m_axis_TUSER(this->m_axis.bits.user);

    verilatedModule_.s_axis_hier_valid(this->s_axis_hier.valid);
    verilatedModule_.s_axis_hier_bits_data(this->s_axis_hier.bits.data);
    verilatedModule_.s_axis_hier_bits_keep(this->s_axis_hier.bits.keep);
    verilatedModule_.s_axis_hier_bits_user(this->s_axis_hier.bits.user);

    /* END: chext_test ctor for 'amba/axi4s' */

    /* BEGIN: chext_test ctor for 'elastic' */
    verilatedModule_.source1_bits(this->source1.bits);
    verilatedModule_.source1_ready(this->source1.ready);
    verilatedModule_.source1_valid(this->source1.valid);

    verilatedModule_.source2_bits_data(this->source2.bits.data);
    verilatedModule_.source2_bits_last(this->source2.bits.last);
    verilatedModule_.source2_ready(this->source2.ready);
    verilatedModule_.source2_valid(this->source2.valid);

    verilatedModule_.source3_bits_data(this->source3.bits.data);
    verilatedModule_.source3_bits_id(this->source3.bits.id);
    verilatedModule_.source3_ready(this->source3.ready);
    verilatedModule_.source3_valid(this->source3.valid);

    verilatedModule_.source4_bits_f0(this->source4.bits.f0);
    verilatedModule_.source4_bits_f1(this->source4.bits.f1);
    verilatedModule_.source4_bits_f2_0(this->source4.bits.f2_0);
    verilatedModule_.source4_bits_f2_1(this->source4.bits.f2_1);
    verilatedModule_.source4_bits_f2_2(this->source4.bits.f2_2);
    verilatedModule_.source4_bits_f2_3(this->source4.bits.f2_3);
    verilatedModule_.source4_bits_f3_f0(this->source4.bits.f3_f0);
    verilatedModule_.source4_bits_f3_f1(this->source4.bits.f3_f1);
    verilatedModule_.source4_ready(this->source4.ready);
    verilatedModule_.source4_valid(this->source4.valid);

    verilatedModule_.source4_bits_f0_0(this->source4.bits.f0_0);
    verilatedModule_.source4_bits_f0_1(this->source4.bits.f0_1);
    verilatedModule_.source4_bits_f0_2(this->source4.bits.f0_2);
    verilatedModule_.source4_bits_f0_3(this->source4.bits.f0_3);
    verilatedModule_.source4_bits_f1_f0(this->source4.bits.f1_f0);
    verilatedModule_.source4_bits_f1_f1(this->source4.bits.f1_f1);
    verilatedModule_.source4_ready(this->source4.ready);
    verilatedModule_.source4_valid(this->source4.valid);

    /* END: chext_test ctor for 'elastic' */

}

inline
ScmyModule::~ScmyModule() {

}

inline
void ScmyModule::generateInvertedResetPorts() {

    reset_INVERTED_.write(!reset.read());

}

#endif /* !defined(__SCMYMODULE_HPP_INCLUDED__) */
