#include <chext_test/elastic.hpp>
#include <iostream>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

struct MyModule : sc_module {
    SC_HAS_PROCESS(MyModule);

    MyModule()
        : sc_module(sc_module_name("MyModule"))
        , clock_("clock")
        , signal_("signal") {
        SC_THREAD(thread1);
        SC_THREAD(thread2);
    }

private:
    void thread1() {
        sc_bv_base test("0x07FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
        sc_unsigned test2(512);
        test2 = "0x07FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
        std::cout << std::hex << test2 << std::endl;
        std::cout << std::hex << test2.length() << std::endl;

        signal_.write(false);

        clock_.write(false);
        wait(1, SC_NS);

        clock_.write(true);
        ::wait();
        signal_.write(true);

        wait(1, SC_NS);

        clock_.write(false);
        wait(1, SC_NS);
    }

    void thread2() {
        wait(clock_.posedge_event());
        std::cout << "thread 2: " << sc_time_stamp() << std::endl;
        std::cout << "value of signal: " << signal_.read() << std::endl;
    }

    sc_signal<bool> clock_;
    sc_signal<bool> signal_;
};

int sc_main(int argc, char** argv) {
    MyModule myModule;
    sc_start(10, SC_NS);

    return 0;
}
