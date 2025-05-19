#if !defined(__SCMYMODULE_HPP_INCLUDED__)
#define __SCMYMODULE_HPP_INCLUDED__

#include <VmyModule.h>

#include <systemc>
#include <tlm>

/* BEGIN: chext_test includes added by 'EncodedDataListHook' */
#include <cstdint>
#include <fmt/core.h>
#include <jqr/comp_eq.hpp>
#include <jqr/core.hpp>
#include <jqr/dump.hpp>
/* END: chext_test includes added by 'EncodedDataListHook' */

#include <hdlscw/wrapper_base.hpp>

/* BEGIN: chext_test includes for 'amba/axi4' */
#include <chext_test/amba/axi4/full/Driver.hpp>
#include <chext_test/amba/axi4/lite/Driver.hpp>
/* END: chext_test includes for 'amba/axi4' */

/* BEGIN: chext_test includes for 'elastic' */
#include <chext_test/elastic/Driver.hpp>
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
        sc_core::sc_signal<sc_dt::sc_bv<8>, sc_core::SC_MANY_WRITERS> f0;

        // f1: UInt<18>
        sc_core::sc_signal<sc_dt::sc_bv<18>, sc_core::SC_MANY_WRITERS> f1;

        // f2_0: UInt<6>
        sc_core::sc_signal<sc_dt::sc_bv<6>, sc_core::SC_MANY_WRITERS> f2_0;

        // f2_1: UInt<6>
        sc_core::sc_signal<sc_dt::sc_bv<6>, sc_core::SC_MANY_WRITERS> f2_1;

        // f2_2: UInt<6>
        sc_core::sc_signal<sc_dt::sc_bv<6>, sc_core::SC_MANY_WRITERS> f2_2;

        // f2_3: UInt<6>
        sc_core::sc_signal<sc_dt::sc_bv<6>, sc_core::SC_MANY_WRITERS> f2_3;

        // f3_f0: UInt<15>
        sc_core::sc_signal<sc_dt::sc_bv<15>, sc_core::SC_MANY_WRITERS> f3_f0;

        // f3_f1: Bool
        sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> f3_f1;

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
                static_cast<std::uint8_t>(f0.read().to_uint64()),
                static_cast<std::uint32_t>(f1.read().to_uint64()),
                static_cast<std::uint8_t>(f2_0.read().to_uint64()),
                static_cast<std::uint8_t>(f2_1.read().to_uint64()),
                static_cast<std::uint8_t>(f2_2.read().to_uint64()),
                static_cast<std::uint8_t>(f2_3.read().to_uint64()),
                static_cast<std::uint16_t>(f3_f0.read().to_uint64()),
                f3_f1.read()
            };
        }

        void writeFrom(value_type const& x) {
            f0.write(x.f0);
            f1.write(x.f1);
            f2_0.write(x.f2_0);
            f2_1.write(x.f2_1);
            f2_2.write(x.f2_2);
            f2_3.write(x.f2_3);
            f3_f0.write(x.f3_f0);
            f3_f1.write(x.f3_f1);
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
        sc_core::sc_signal<sc_dt::sc_bv<6>, sc_core::SC_MANY_WRITERS> f0_0;

        // f0_1: UInt<6>
        sc_core::sc_signal<sc_dt::sc_bv<6>, sc_core::SC_MANY_WRITERS> f0_1;

        // f0_2: UInt<6>
        sc_core::sc_signal<sc_dt::sc_bv<6>, sc_core::SC_MANY_WRITERS> f0_2;

        // f0_3: UInt<6>
        sc_core::sc_signal<sc_dt::sc_bv<6>, sc_core::SC_MANY_WRITERS> f0_3;

        // f1_f0: UInt<15>
        sc_core::sc_signal<sc_dt::sc_bv<15>, sc_core::SC_MANY_WRITERS> f1_f0;

        // f1_f1: Bool
        sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> f1_f1;

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
                static_cast<std::uint8_t>(f0_0.read().to_uint64()),
                static_cast<std::uint8_t>(f0_1.read().to_uint64()),
                static_cast<std::uint8_t>(f0_2.read().to_uint64()),
                static_cast<std::uint8_t>(f0_3.read().to_uint64()),
                static_cast<std::uint16_t>(f1_f0.read().to_uint64()),
                f1_f1.read()
            };
        }

        void writeFrom(value_type const& x) {
            f0_0.write(x.f0_0);
            f0_1.write(x.f0_1);
            f0_2.write(x.f0_2);
            f0_3.write(x.f0_3);
            f1_f0.write(x.f1_f0);
            f1_f1.write(x.f1_f1);
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
    chext_test::amba::axi4::full::Master<4,32,256,4,4,0,0,0,false> m_axi;
    /* END: chext_test public for 'amba/axi4' */

    /* BEGIN: chext_test public for 'elastic' */
    chext_test::elastic::Source<sc_core::sc_signal<sc_dt::sc_bv<32>>> source1;
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

#endif /* !defined(__SCMYMODULE_HPP_INCLUDED__) */
