#ifndef JQR_HASH_HPP_INCLUDED
#define JQR_HASH_HPP_INCLUDED

namespace jqr {

template <typename T, typename Enable = void>
struct mk_hash {
    static std::size_t hash(T const&) { return 0; }
};

}

#endif /* JQR_HASH_HPP_INCLUDED */
