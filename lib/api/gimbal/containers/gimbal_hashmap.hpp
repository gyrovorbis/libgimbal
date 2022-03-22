#ifndef GIMBAL_HASHMAP_HPP
#define GIMBAL_HASHMAP_HPP

#include "gimbal_hashset.hpp"

namespace gimbal {

template<typename K,
         typename T,
         typename H = std::hash<K>,
         typename P = std::equal_to<K>>
class HashMap:
    public HashSet<std::pair<const K, T>, H, P>,
    public ReadWriteIndexable<HashMap<K, T, H, P>, K, T, true>
{
public:
    friend class ReadWriteIndexable<HashMap<K, T, H, P>, K, T, true>;

    using HashSetBase       = HashSet<std::pair<const K, T>, H, P>;
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
 /* using iterator         =
    using const_iterator   =
    using local_iterator   =
    using const_local_iterator =
    using node_type         =
    using insert_return_type = */
protected:
    const T&    getElement_(const K& key) const noexcept;
    void        setElement_(const K& key, const T& value) noexcept;

public:

    HashMap(size_type   capacity = 0,
            H&          hash     = H(),
            P&          pred     = P(),
            Context*    pCtx     = nullptr);

    HashMap(std::initializer_list<value_type> init,
            size_type   capacity = 0,
            H&          hash = H(),
            P&          pred = P(),
            Context*    pCtx = nullptr);

    template<typename M>
    void insert_or_assign(const key_type k, M&& obj) noexcept;

    bool erase(const key_type& k) noexcept;

    const mapped_type& at(const key_type& key) const noexcept;
    mapped_type& at(const key_type& key) noexcept;

    bool contains(const key_type& key) const noexcept;
    //iterator find(const key_type& key) const;

};



// ======== IMPLEMENTATION ========

template<typename K, typename T, typename H, typename P>
inline HashMap<K, T, H, P>::HashMap(size_type   capacity,
        H&          hash,
        P&          pred,
        Context*    pCtx):
    HashSetBase(capacity, hash, pred, pCtx) {}

template<typename K, typename T, typename H, typename P>
inline HashMap<K, T, H, P>::HashMap(std::initializer_list<value_type> init,
        size_type   capacity,
        H&          hash,
        P&          pred,
        Context*    pCtx):
    HashMap(capacity < init.size()? init.size() : capacity, hash, pred, pCtx)
{
    for(auto& item : init) {
        HashSetBase::insert_or_assign(&item);
    }
}

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
inline void HashMap<K, T, H, P>::insert_or_assign(const key_type k, M&& obj) noexcept {
    std::pair<K, T> pair { k, std::forward<M>(obj) };
    HashSetBase::insert_or_assign(&pair);
}

template<typename K, typename T, typename H, typename P>
inline bool HashMap<K, T, H, P>::erase(const key_type& k) noexcept {
    std::pair<K, T> pair { k, T{} };
    return HashSetBase::erase(&pair);
}

template<typename K, typename T, typename H, typename P>
inline auto HashMap<K, T, H, P>::at(const key_type& k) const noexcept -> const mapped_type& {
    std::pair<K, T> pair { k, T{} };
    return HashSetBase::at(&pair);
}

template<typename K, typename T, typename H, typename P>
inline auto HashMap<K, T, H, P>::at(const key_type& k) noexcept -> mapped_type& {
    std::pair<K, T> pair { k, T{} };
    return HashSetBase::at(&pair);
}

template<typename K, typename T, typename H, typename P>
inline bool HashMap<K, T, H, P>::contains(const key_type& k) const noexcept {
    std::pair<K, T> pair { k, T{} };
    return HashSetBase::contains(&pair);
}


}

#endif // GIMBAL_HASHMAP_HPP
