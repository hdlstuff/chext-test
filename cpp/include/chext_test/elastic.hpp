#ifndef CHEXT_TEST_ELASTIC_HPP_INCLUDED
#define CHEXT_TEST_ELASTIC_HPP_INCLUDED

#include <stdexcept>
#include <typeinfo>

#include <systemc>

using namespace sc_dt;
using namespace sc_core;

struct SimulationException : std::exception {
    SimulationException(std::string msg)
        : msg_ { std::move(msg) } {
    }

    const char* what() const noexcept {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

struct Reference;
struct ConstReference;

struct Reference {
    Reference(std::type_info const& typeInfo, void* ptr)
        : typeInfo_ { typeInfo }
        , ptr_ { ptr } {
    }

    template<typename T>
    Reference(T& t)
        : Reference(typeid(T), (void*)&t) {
    }

    Reference(Reference const&) = default;
    Reference(Reference&&) = default;

    Reference& operator=(Reference const&) = default;
    Reference& operator=(Reference&&) = default;

    template<typename T>
    bool is() const { return typeid(T) == typeInfo_; }

    template<typename T>
    T& get() {
        if (!is<T>())
            throw std::bad_cast();

        return *((T*)ptr_);
    }

    template<typename T>
    T const& get() const {
        if (!is<T>())
            throw std::bad_cast("The stored reference is not of the requested type.");

        return *((T const*)ptr_);
    }

    ConstReference asConst() const;

    ~Reference() = default;

private:
    std::type_info const& typeInfo_;
    void* ptr_;
};

struct ConstReference {
    ConstReference(std::type_info const& typeInfo, void const* ptr)
        : typeInfo_ { typeInfo }
        , ptr_ { ptr } {
    }

    template<typename T>
    ConstReference(T const& t)
        : ConstReference(typeid(T), (void const*)&t) {
    }

    ConstReference(ConstReference const&) = default;
    ConstReference(ConstReference&&) = default;

    ConstReference& operator=(ConstReference const&) = default;
    ConstReference& operator=(ConstReference&&) = default;

    template<typename T>
    bool is() const { return typeid(T) == typeInfo_; }

    template<typename T>
    T const& get() const {
        if (!is<T>())
            throw std::bad_cast();

        return *((T const*)ptr_);
    }

    ~ConstReference() = default;

private:
    std::type_info const& typeInfo_;
    void const* ptr_;
};

inline ConstReference Reference::asConst() const {
    return ConstReference(typeInfo_, ptr_);
}

struct DataSignalBase {
    virtual void poke(ConstReference cref) = 0;
    virtual void peek(Reference ref) = 0;

    template<typename D>
    void poke(D const& data) {
        poke(ConstReference { data });
    }

    template<typename D>
    D peek() {
        D data;
        peek(Reference { data });
        return data /* NRVO */;
    }

    virtual ~DataSignalBase() = default;
};

template<typename D>
struct DataSignal : DataSignalBase, sc_signal<D, SC_MANY_WRITERS> {
    using base_signal = sc_signal<D, SC_MANY_WRITERS>;
    using base_signal::base_signal;

    void poke(ConstReference cref) {
        base_signal::write(cref.get<D>());
    }

    void peek(Reference ref) {
        ref.get<D>() = base_signal::read();
    }
};

struct Sender {
    Sender(
        std::string name,
        sc_in_clk& clock,
        sc_in<bool>& reset,
        DataSignalBase& data
    )
        : clock { clock }
        , reset { reset }
        , data { data }
        , ready { (name + "_ready").c_str() }
        , valid { (name + "_valid").c_str() } {
    }

    template<typename Packet>
    void send(Packet const& packet) {
        while (clock.read() || reset.read())
            wait(clock.negedge_event());

        // we attempt sending the packet at negedge
        data.poke(packet);
        valid.write(true);

        do {
            wait(reset.posedge_event() | clock.posedge_event());
        } while (!ready.read() || reset.read());

        if (reset.read())
            throw SimulationException("Reset asserted durinu transmission!");

        wait(clock.negedge_event());

        // we stop asserting the valid at negedge
        valid.write(false);
    }

private:
    sc_in_clk& clock;
    sc_in<bool>& reset;
    DataSignalBase& data;

public:
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

struct Receiver {
    Receiver(
        std::string name,
        sc_in_clk& clock,
        sc_in<bool>& reset,
        DataSignalBase& data
    )
        : clock { clock }
        , reset { reset }
        , data { data }
        , ready { (name + "_ready").c_str() }
        , valid { (name + "_valid").c_str() } {
    }

    template<typename Packet>
    Packet receive() {
        Packet p;

        while (clock.read() || reset.read())
            wait(clock.negedge_event());

        ready.write(true);

        do {
            wait(reset.posedge_event() | clock.posedge_event());
        } while (!valid.read() || reset.read());

        if (reset.read())
            throw SimulationException("Reset asserted during transmission!");

        data.peek(Reference { p });

        wait(clock.negedge_event());
        ready.write(false);

        return p;
    }

private:
    sc_in_clk& clock;
    sc_in<bool>& reset;
    DataSignalBase& data;

public:
    sc_signal<bool, SC_MANY_WRITERS> ready;
    sc_signal<bool, SC_MANY_WRITERS> valid;
};

#endif /* CHEXT_TEST_ELASTIC_HPP_INCLUDED */
