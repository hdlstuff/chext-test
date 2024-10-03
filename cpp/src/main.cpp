#include <chext_test/amba/axi4/Master.hpp>
#include <chext_test/elastic/Driver.hpp>
using namespace chext_test::elastic;

#include <iostream>

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include <VElasticModule.h>
#include <verilated_vcd_sc.h>

#include <sysc/kernel/sc_spawn.h>
constexpr struct {
    template<typename T>
    void operator+(T&& t) const {
        sc_spawn(std::forward<T>(t));
    }
} sc_spawn_helper;

#define SC_SPAWN sc_spawn_helper + [&]

struct ElasticModule : sc_module {
    SC_HAS_PROCESS(ElasticModule);

    sc_in_clk clock;
    sc_in<bool> reset;

    ElasticModule(sc_module_name const& name = "ElasticModule")
        : sc_module { name }
        , clock("clock")
        , reset("reset")
        , source1 { "source1", clock, reset, source1_data }
        , source2 { "source2", clock, reset, source2_data }
        , sink { "sink", clock, reset, sink_data }
        , verilatedModule("dut") {

        verilatedModule.clock(clock);
        verilatedModule.reset(reset);

        verilatedModule.source1_bits(source1_data);
        verilatedModule.source1_ready(source1.ready);
        verilatedModule.source1_valid(source1.valid);

        verilatedModule.source2_bits(source2_data);
        verilatedModule.source2_ready(source2.ready);
        verilatedModule.source2_valid(source2.valid);

        verilatedModule.sink_bits(sink_data);
        verilatedModule.sink_ready(sink.ready);
        verilatedModule.sink_valid(sink.valid);
    }

public:
    Sender source1;
    Sender source2;
    Receiver sink;

private:
    DataSignal<uint32_t> source1_data;
    DataSignal<uint32_t> source2_data;
    DataSignal<uint32_t> sink_data;

public:
    VElasticModule verilatedModule;
};

struct Testbench : sc_module {
    SC_HAS_PROCESS(Testbench);

    Testbench(sc_module_name const& name = "Testbench")
        : sc_module { name }
        , dut { "elasticModule" }
        , clock_("clock")
        , reset_("reset") {

        dut.reset(reset_);
        dut.clock(clock_);

        SC_THREAD(thread0);
    }

    ElasticModule dut;

private:
    sc_clock clock_;
    sc_signal<bool> reset_;

    void thread0() {
        wait(clock_.negedge_event());
        reset_.write(true);

        wait(clock_.negedge_event());
        wait(clock_.negedge_event());

        reset_.write(false);

        wait(clock_.negedge_event());

        SC_SPAWN {
            wait(20, SC_NS);

            for (int i = 0; i < 50; ++i)
                dut.source1.send<uint32_t>(i * 10 + 3);
        };

        SC_SPAWN {
            wait(20, SC_NS);
            for (int i = 0; i < 50; ++i)
                dut.source2.send<uint32_t>(i * 20 + 3);
        };

        SC_SPAWN {
            wait(20, SC_NS);
            for (int i = 0; i < 50; ++i)
                std::cout << "received: " << dut.sink.receive<uint32_t>() << std::endl;
        };
    }
};

int sc_main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Testbench testbench;

    sc_start(SC_ZERO_TIME);

    std::unique_ptr<VerilatedVcdSc> trace_file = std::make_unique<VerilatedVcdSc>();
    testbench.dut.verilatedModule.trace(trace_file.get(), 99);
    trace_file->open("ElasticModule.vcd");

    sc_start(50, SC_NS);

    trace_file->close();

    return 0;
}
