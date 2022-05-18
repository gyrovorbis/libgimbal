#ifndef GIMBAL_GENERICS_HPP
#define GIMBAL_GENERICS_HPP

#include <iterator>
#include "../types/gimbal_exception.hpp"
#include "../core/gimbal_api_generators.hpp"

namespace gimbal {

template<typename CRTP, typename Key, typename Value, bool Writable>
class ReadWriteIndexable {
protected:

    class SubscriptProxy {
    public:
        using                   ThisType          = SubscriptProxy;
        static constexpr bool   ReadWrite         = Writable;
        using                   DerivedPtrType    = std::conditional_t<ReadWrite, CRTP*, const CRTP*>;

        DerivedPtrType      pObject_{};
        Key                 key_{};
    public:
        SubscriptProxy(DerivedPtrType pObject, Key key):
            pObject_(pObject), key_(std::move(key)) {}

        operator Value() const {
            return pObject_->getElement_(key_);
        }

        ThisType& operator=(Value value) {
            pObject_->setElement_(key_, std::move(value));
            return *this;
        }

        template<typename RValue>
            requires ReadWrite
         ThisType& operator=(RValue&& rhs) {
            pObject_->setElement_(key_, std::forward<RValue>(rhs));
            return *this;
        }
    };

public:

    Value operator[](Key key) const {
        return static_cast<const CRTP*>(this)->getElement_(key);
    }

    SubscriptProxy operator[](Key key) requires SubscriptProxy::ReadWrite {
        return SubscriptProxy { static_cast<CRTP*>(this), key };
    }
};



template<typename CRTP, typename Index, typename Value>
class ReadWriteContiguousIndexable:
    public ReadWriteIndexable<CRTP, Index, Value, true>
{
private:

    using IndexableProxy = typename ReadWriteIndexable<CRTP, Index, Value, true>::SubscriptProxy;

public:
    void checkBounds(Index index) const {
       if(index >= size()) Exception::checkThrow({Result::ErrorOutOfRange, "index > size"});
    }

    Index size(void) const {
        return static_cast<const CRTP*>(this)->getElementCount_();
    }

    Index length(void) const { return size(); }

    Index empty(void) const {
        return size() == 0;
    }

    Value at(Index index) const {
        checkBounds(index);
        return static_cast<const CRTP*>(this)->getElement_(index);
    }

    IndexableProxy at(Index index) {
        checkBounds(index);
        return IndexableProxy { static_cast<CRTP*>(this), index };
    }

    Value front(Index index) const {
        return *this[index];
    }

    IndexableProxy front(void) {
        return IndexableProxy { static_cast<CRTP*>(this), 0 };
    }

    Value back(void) const {
        return *this[size() - 1];
    }

    IndexableProxy back(void) {
        return IndexableProxy { static_cast<CRTP*>(this), size() - 1 };
    }
};

    namespace tags {
        struct RandomAccessIteratorBase {};
    }

    template<typename O, typename Index, typename Value, bool Reverse>
    struct RandomAccessIterator:
            public tags::RandomAccessIteratorBase /*,
            public std::iterator<std::random_access_iterator_tag, Value>*/ {
        constexpr static inline bool
              reverse               = Reverse;
        using object_type           = O;
        using iterator_type         = RandomAccessIterator<O, Index, Value, Reverse>;
        using const_iterator_type   = RandomAccessIterator<std::add_const_t<O>, Index, Value, Reverse>;
        using nonconst_iterator_type  = RandomAccessIterator<std::remove_const_t<O>, Index, Value, Reverse>;
        using value_type            = Value;
        using index_type            = std::make_signed_t<Index>;
        using reference             = std::conditional_t<std::is_const_v<O>,
                                        std::add_lvalue_reference_t<std::add_const_t<Value>>,
                                        std::add_lvalue_reference_t<Value>>;
        using const_reference       = std::add_lvalue_reference_t<std::add_const_t<Value>>;
        using pointer               = std::conditional_t<std::is_const_v<O>,
                                        std::add_pointer_t<std::add_const_t<Value>>,
                                        std::add_pointer_t<Value>>;
        using difference_type       = std::ptrdiff_t;
        using iterator_category     = std::random_access_iterator_tag;

        void checkBounds(Index index) const {
           if(index >= pObj_->getElementCount_()) Exception::checkThrow({Result::ErrorOutOfRange, "Index >= obj.elementCount"});
        }
#if 0
        template<typename T = reference>
        std::enable_if_t<!std::is_const_v<object_type>, T> operator* () {
            checkBounds(index_);
            return pObj_->getElement_(index_);
        }
#else
        reference operator* () {
            checkBounds(index_);
            return pObj_->getElement_(index_);
        }
#endif
        const_reference operator* () const {
            checkBounds(index_);
            return const_cast<const O*>(pObj_)->getElement_(index_);
        }

        pointer operator& () {
            checkBounds(index_);
            return &pObj_->getElement_(index_);
        }

        iterator_type& operator++ () {
            if constexpr(Reverse) --index_;
            else ++index_ ;
            return *this;
        }
        iterator_type operator++ (int) {
            const auto temp(*this);
            if constexpr(Reverse) --*this;
            else ++*this;
            return temp;
        }
        iterator_type& operator--() {
            if constexpr(Reverse) ++index_;
            else --index_;
            return *this;
        }
        iterator_type operator--(int) {
            const auto temp(*this);
            if constexpr(Reverse) ++*this;
            else --*this;
            return temp;
        }
        iterator_type& operator+=(index_type offset) { if constexpr(Reverse) index_ -= offset; else index_ += offset; return *this; }
        iterator_type& operator-=(index_type offset) { if constexpr(Reverse) index_ += offset; else index_ -= offset; return *this; }

        friend iterator_type operator+(const iterator_type& it, index_type offset) { auto temp(it); if constexpr(Reverse) temp.index_ -= offset; else temp.index_ += offset; return temp; }
        friend iterator_type operator+(index_type offset, const iterator_type& it) { if constexpr(!Reverse) return it + offset; else return it - offset; }
        friend iterator_type operator-(const iterator_type& it, index_type offset) { auto temp(it); if constexpr(Reverse) temp.index_ += offset; else temp.index_ -= offset; return temp; }
        iterator_type operator+(const iterator_type& rhs) const { if constexpr(!Reverse) return *this + rhs.index_; else return *this - rhs.index_; }
        //difference_type operator-(const iterator_type& rhs) const { auto temp(*this); if constexpr(Reverse) temp.index_ += rhs.index_; else temp.index_ -= rhs.index_; return temp.index_; }
        difference_type operator-(const iterator_type& rhs) const { auto temp(*this); temp.index_ -= rhs.index_; return temp.index_; }

        reference operator[](difference_type offset) const {
            return pObj_->getElement_(index_ + offset);
        }

        bool operator== (const iterator_type& rhs) const { return pObj_ == pObj_ && index_ == rhs.index_; }
        bool operator<=  (const iterator_type& rhs) const { return pObj_ == rhs.pObj_ && index_ <= rhs.index_; }
        bool operator<  (const iterator_type& rhs) const { return pObj_ == rhs.pObj_ && index_ < rhs.index_; }
        bool operator>  (const iterator_type& rhs) const { return pObj_ == rhs.pObj_ && index_ > rhs.index_; }
        bool operator>=  (const iterator_type& rhs) const { return pObj_ == rhs.pObj_ && index_ >= rhs.index_; }
        bool operator!= (const iterator_type& rhs) const { return !(*this == rhs); }

        RandomAccessIterator(void)=default;

        RandomAccessIterator(const const_iterator_type& rhs) requires(std::is_const_v<O>):
            pObj_(rhs.pObj_), index_(rhs.index_) {}

        RandomAccessIterator(O& objType, index_type index=-1):
            pObj_(&objType), index_(index) {}

        RandomAccessIterator(const nonconst_iterator_type& rhs):
            RandomAccessIterator(*rhs.pObj_, rhs.index_) {}

        O*    pObj_         =   nullptr;
        index_type index_   =   -1;
    };


template<typename CRTP, typename Index, typename Value>
class RandomAccessIterable {
public:

    using iterator                  = RandomAccessIterator<CRTP, Index, Value, false>;
    using reverse_iterator          = RandomAccessIterator<CRTP, Index, Value, true>;
    using const_iterator            = RandomAccessIterator<std::add_const_t<CRTP>, Index, Value, false>;
    using reverse_const_iterator    = RandomAccessIterator<std::add_const_t<CRTP>, Index, Value, true>;

    //static_assert(std::random_access_iterator<iterator>,                "iterator_not STL-compatible!");
    //static_assert(std::random_access_iterator<reverse_iterator>,        "reverse_iterator not STL-compatible!");
    static_assert(std::random_access_iterator<const_iterator>,          "const_iterator not STL-compatible!");
    static_assert(std::random_access_iterator<reverse_const_iterator>,  "reverse_const_iterator not STL-compatible!");

public:
    //return revierse iterators
   iterator                 begin(void) { return {*static_cast<CRTP*>(this), 0}; }
   auto                     begin(void) const { return cbegin(); }
   //reverse_iterator         rbegin(void) { return {*static_cast<CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()-1 }; }
   auto         rbegin(void) { return std::make_reverse_iterator(end()); }
   auto                     rbegin(void) const { return crbegin(); }
   iterator                 end(void) { return {*static_cast<CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()}; }
   auto                     end(void) const { return cend(); }
   //reverse_iterator         rend(void) { return {*static_cast<CRTP*>(this), -1}; }
   auto         rend(void) { return std::make_reverse_iterator(begin()); }
   auto                     rend(void) const { return crend(); }
   const_iterator           cbegin(void) const { return {*static_cast<const CRTP*>(this), 0}; }
   //reverse_const_iterator   crbegin(void) const { return {*static_cast<const CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()-1}; }
   auto   crbegin(void) const { return std::make_reverse_iterator(cend()); }
   const_iterator           cend(void) const { return {*static_cast<const CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()}; }
   //reverse_const_iterator   crend(void) const { return {*static_cast<const CRTP*>(this), -1}; }
   auto crend(void) const { return std::make_reverse_iterator(cbegin()); }
};

}

#endif // GIMBAL_GENERICS_HPP
