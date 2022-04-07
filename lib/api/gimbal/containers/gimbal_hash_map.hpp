#ifndef GIMBAL_HASHMAP_HPP
#define GIMBAL_HASHMAP_HPP

#include "gimbal_hash_set.hpp"

namespace gimbal {


namespace impl {
    template<typename K,
             typename V,
             typename H=std::hash<K>>
    struct HashMapHasherAdapter;

    template<typename K,
             typename V,
             typename P=std::equal_to<K>>
    struct HashMapEqualToAdapter;
}

template<typename K,
         typename T,
         typename H = std::hash<K>,
         typename P = std::equal_to<K>>
class HashMap:
    public HashSet<std::pair<const K, T>,
                   impl::HashMapHasherAdapter<K, T, H>,
                   impl::HashMapEqualToAdapter<K, T, P>>,
    public ReadWriteIndexable<HashMap<K, T, H, P>, K, T, true>
{
public:
    friend class ReadWriteIndexable<HashMap<K, T, H, P>, K, T, true>;

    using HashSetType       = HashSet<std::pair<const K, T>,
                                      impl::HashMapHasherAdapter<K, T, H>,
                                      impl::HashMapEqualToAdapter<K, T, P>>;
    using HashSetBaseType   = typename HashSetType::BaseType;
    using key_type          = K;
    using mapped_type       = T;
    using value_type        = std::pair<const K, T>;
    using size_type         = Size;
    using difference_type   = std::ptrdiff_t;
    using hasher            = H;
    using key_equal         = P;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const pointer;
    using iterator          = typename HashSetType::iterator;
    using const_iterator    = typename HashSetType::const_iterator;
 /* using iterator         =
    using const_iterator   =
    using local_iterator   =
    using const_local_iterator =
    using node_type         =
    using insert_return_type = */
protected:
    auto    getElement_(const K& key) const noexcept -> const T&;
    void    setElement_(const K& key, const T& value) noexcept;

public:

            HashMap(size_type   capacity = 0,
                    const H&    hash     = H(),
                    const P&    pred     = P(),
                    Context*    pCtx     = nullptr);
            HashMap(size_type   capacity,
                    const H&    hash,
                    Context*    pCtx);
            HashMap(size_type capacity,
                    Context* pCtx);
   explicit HashMap(Context* pCtx);

            HashMap(std::initializer_list<value_type> init,
                    size_type   capacity = 0,
                    const H&    hash = H(),
                    const P&    pred = P(),
                    Context*    pCtx = nullptr);
            HashMap(std::initializer_list<value_type> init,
                    size_type   capacity,
                    const H&    hash,
                    Context*    pCtx);
            HashMap(std::initializer_list<value_type> init,
                    size_type   capacity,
                    Context*    pCtx);


    template<typename M>
    void    insert_or_assign(const key_type& k, M&& obj) noexcept;

    bool    erase(const key_type& k) noexcept;

    auto    at(const key_type& key) const -> const mapped_type&;
    auto    at(const key_type& key) -> mapped_type&;

    auto    find(const key_type& key) const noexcept -> const_iterator;
    auto    find(const key_type& key) noexcept -> iterator;

    bool    contains(const key_type& key) const noexcept;
    //iterator find(const key_type& key) const;

};

#if 0
template<typename K,
         typename T,
         typename H = std::hash<K>,
         typename P = std::equal_to<K>>
HashMap(std::initializer_list<std::pair<const K, T>> list,
        Size capacity=0,
        const H& hash=H(),
        const P& pred=P(),
        Context* pCtx=nullptr) -> HashMap<K, T, H, P>;
#endif



// ======== IMPLEMENTATION ========


namespace impl {

    template<typename K,
             typename V,
             typename H>
    struct HashMapHasherAdapter {
        const H& hasher;

        HashMapHasherAdapter(const H& hasher_=H()):
            hasher(hasher_) {}

        Hash operator()(const std::pair<K, V>& value) const {
            return hasher(value.first);
        }
    };

    template<typename K,
             typename V,
             typename P>
    struct HashMapEqualToAdapter {
        const P& pred;

        HashMapEqualToAdapter(const P& pred_=P()):
            pred(pred_) {}

        bool operator()(const std::pair<K, V>& lhs, const std::pair<K, V>& rhs) const {
            return pred(lhs.first, rhs.first);
        }
    };

}

template<typename K, typename T, typename H, typename P>
inline HashMap<K, T, H, P>::HashMap(size_type   capacity,
        const H&    hash,
        const P&    pred,
        Context*    pCtx):
    HashSetType(capacity, hash, pred, pCtx) {}

template<typename K, typename T, typename H, typename P>
inline HashMap<K, T, H, P>::HashMap(std::initializer_list<value_type> init,
        size_type   capacity,
        const H&    hash,
        const P&    pred,
        Context*    pCtx):
    HashMap(capacity < init.size()? init.size() : capacity, hash, pred, pCtx)
{
    //INEFFICIENT AS SHIT COPYING?
    for(auto item : init) {
        HashSetType::insert(item);
    }
}

template<typename K, typename T, typename H, typename P>
inline HashMap<K, T, H, P>::HashMap(size_type   capacity,
        const H&    hash,
        Context*    pCtx):
    HashMap(capacity, hash, P(), pCtx) {}

template<typename K, typename T, typename H, typename P>
inline HashMap<K, T, H, P>::HashMap(size_type capacity, Context* pCtx):
  HashMap(capacity, H(), P(), pCtx) {}

template<typename K, typename T, typename H, typename P>
inline HashMap<K, T, H, P>::HashMap(Context* pCtx):
    HashMap(0, pCtx) {}


template<typename K, typename T, typename H, typename P>
inline HashMap<K, T, H, P>::HashMap(std::initializer_list<value_type> init,
        size_type   capacity,
        const H&    hash,
        Context*    pCtx):
  HashMap(std::move(init), capacity, hash, P(), pCtx) {}

template<typename K, typename T, typename H, typename P>
inline HashMap<K, T, H, P>::HashMap(std::initializer_list<value_type> init,
        size_type   capacity,
        Context*    pCtx):
  HashMap(std::move(init), capacity, H(), pCtx) {}

template<typename K, typename T, typename H, typename P>
inline const T& HashMap<K, T, H, P>::getElement_(const K& key) const noexcept {
    return at(key);
}

template<typename K, typename T, typename H, typename P>
inline void HashMap<K, T, H, P>::setElement_(const K& key, const T& value) noexcept {
    insert_or_assign(key, value);
}

template<typename K, typename T, typename H, typename P>
template<typename M>
inline void HashMap<K, T, H, P>::insert_or_assign(const key_type& k, M&& obj) noexcept {
    std::pair<K, T> item { k, std::forward<M>(obj) };
    HashSetType::insert_or_assign(item);
}

template<typename K, typename T, typename H, typename P>
inline bool HashMap<K, T, H, P>::erase(const key_type& k) noexcept {
    std::pair<K, T> pair { k, T{} };
    return HashSetType::erase(pair);
}

template<typename K, typename T, typename H, typename P>
inline auto HashMap<K, T, H, P>::at(const key_type& k) const -> const mapped_type& {
    std::pair<K, T> pair { k, T{} };
    const auto& result = HashSetBaseType::at(pair);
    return result.second;
}

template<typename K, typename T, typename H, typename P>
inline auto HashMap<K, T, H, P>::at(const key_type& k) -> mapped_type& {
    std::pair<K, T> pair { k, T{} };
    auto& result = HashSetType::at(pair);
    return result.second;
}

template<typename K, typename T, typename H, typename P>
inline auto HashMap<K, T, H, P>::find(const key_type& k) const noexcept -> const_iterator {
    std::pair<K, T> pair (k, T{});
    return HashSetType::find(pair);
}

template<typename K, typename T, typename H, typename P>
inline auto HashMap<K, T, H, P>::find(const key_type& k) noexcept -> iterator {
    std::pair<K, T> pair (k, T{});
    return HashSetType::find(pair);
}

template<typename K, typename T, typename H, typename P>
inline bool HashMap<K, T, H, P>::contains(const key_type& k) const noexcept {
    std::pair<K, T> pair { k, T{} };
    return HashSetType::contains(pair);
}


}

#endif // GIMBAL_HASHMAP_HPP
