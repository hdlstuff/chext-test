#include <chext_test/elastic.hpp>

#include <iostream>

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include <VElasticModule.h>

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
        , verilatedModule_("dut") {

        verilatedModule_.clock(clock);
        verilatedModule_.reset(reset);

        verilatedModule_.source1_bits(source1_data);
        verilatedModule_.source1_ready(source1.ready);
        verilatedModule_.source1_valid(source1.valid);

        verilatedModule_.source2_bits(source2_data);
        verilatedModule_.source2_ready(source2.ready);
        verilatedModule_.source2_valid(source2.valid);

        verilatedModule_.sink_bits(sink_data);
        verilatedModule_.sink_ready(sink.ready);
        verilatedModule_.sink_valid(sink.valid);
    }

public:
    Sender source1;
    Sender source2;
    Receiver sink;

private:
    DataSignal<uint32_t> source1_data;
    DataSignal<uint32_t> source2_data;
    DataSignal<uint32_t> sink_data;

    VElasticModule verilatedModule_;
};

struct Testbench : sc_module {
    SC_HAS_PROCESS(Testbench);

    Testbench(sc_module_name const& name = "Testbench")
        : sc_module { name }
        , elasticModule_ { "elasticModule" }
        , clock_("clock")
        , reset_("reset") {

        elasticModule_.reset(reset_);
        elasticModule_.clock(clock_);

        SC_THREAD(thread0);
    }

private:
    ElasticModule elasticModule_;

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
                elasticModule_.source1.send<uint32_t>(i * 10 + 3);
        };

        SC_SPAWN {
            wait(20, SC_NS);
            for (int i = 0; i < 50; ++i)
                elasticModule_.source2.send<uint32_t>(i * 20 + 3);
        };

        SC_SPAWN {
            wait(20, SC_NS);
            for (int i = 0; i < 50; ++i)
                std::cout << "received: " << elasticModule_.sink.receive<uint32_t>() << std::endl;
        };
    }
};

int sc_main(int argc, char** argv) {
    Testbench testbench;
    sc_start(50, SC_NS);

    return 0;
}
