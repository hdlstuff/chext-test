#ifndef CHEXT_TEST_ELASTIC_DATASIGNAL_HPP_INCLUDED
#define CHEXT_TEST_ELASTIC_DATASIGNAL_HPP_INCLUDED

#include <chext_test/util/Reference.hpp>
#include <systemc>

namespace chext_test::elastic {

namespace detail {

using namespace sc_core;
using util::ConstReference;
using util::Reference;

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

} // namespace detail

using detail::DataSignalBase;
using detail::DataSignal;

} // namespace chext_test::elastic

#endif /* CHEXT_TEST_ELASTIC_DATASIGNAL_HPP_INCLUDED */
