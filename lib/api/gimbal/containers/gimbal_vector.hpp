#ifndef GIMBAL_VECTOR_HPP
#define GIMBAL_VECTOR_HPP

#include "gimbal_container.hpp"
#include "gimbal_vector.h"
#include "../objects/gimbal_context.hpp"
#include <algorithm>


namespace gimbal {

class Context;

namespace tags {
    struct VectorBase {};
}

template<typename V>
concept vector_base =
        std::is_base_of_v<gimbal::tags::VectorBase, V> &&
        requires(V v) {
            typename V::value_type;
        };

template<typename V, typename T>
concept type_compatible_vector_base =
    vector_base<V> &&
    type_compatible_contiguous_container<V, T>;

template<typename V1, typename V2>
concept vector_compatible_gbl =
        requires(V1 v1, V2 v2) {
            typename V1::value_type;
            typename V2::value_type;
        } &&
        std::same_as<typename V1::ValueType, typename V2::ValueType>;


template<typename T=void*>
class Vector;

template<typename T=void*>
class VectorView;

template<typename CRTP, typename T>
class VectorViewBase:
    public tags::VectorBase,
    public ReadWriteContiguousIndexable<CRTP, Size, T>,
    public RandomAccessIterable<CRTP, Size, T>
{
public:
    using VectorViewBaseType        = VectorViewBase<CRTP, T>;
    using DerivedType               = CRTP;
    using IterableType              = RandomAccessIterable<CRTP, Size, T>;
    using IndexableType             = ReadWriteContiguousIndexable<CRTP, Size, T>;

    // C++ STL-compliant traits
    using value_type                = T;
    using allocator_type            = Context*;
    using size_type                 = Size;
    using difference_type           = std::ptrdiff_t;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using pointer                   = T*;
    using const_pointer             = const T*;

    using iterator                  = typename IterableType::iterator;
    using const_iterator            = typename IterableType::const_iterator;
    using reverse_iterator          = typename IterableType::reverse_iterator;
    using reverse_const_iterator    = typename IterableType::reverse_const_iterator;

protected:
    decltype(auto) vec_(void) const {
        return static_cast<const CRTP*>(this)->getVector_();
    }

    decltype(auto) vec_(void) {
        return static_cast<CRTP*>(this)->getVector_();
    }

    void* at_(Size index) const {
        void* pData = nullptr;
        Exception::checkThrow([&]() {
            pData = GblVector_at(vec_(), index);
        });
        return pData;
    }

public:
    operator const GblVector*() const { return vec_(); }

    // === FIXME UP ===
    const_reference getElement_(size_type index) const {
        return *static_cast<const T*>(at_(index));
    }

    reference getElement_(size_type index) {
        return *static_cast<T*>(at_(index));
    }

    void setElement_(size_type index, value_type value) {
        *static_cast<pointer>(at_(index)) = value;
    }

    size_t getElementCount_(void) const {
        return getSize();
    }

    // === / FIXME UP ===

    // Accessors
    bool isEmpty(void) const {
        return GblVector_empty(vec_());
    }

    bool isStack(void) const {
        return GblVector_stack(vec_());
    }

    bool isHeap(void) const { return !isStack(); }
#ifdef GBL_PMR_VECTOR
    std::pmr::vector<value_type> toStdPmrVector(void) const {
        gimbal::pmr::vector<value_type> stdVec(&front(), &back(), getContext());
    }
#endif

    std::vector<value_type> toStdVector(void) const {
        std::vector<value_type> stdVec(&front(), &back());
    }

    Size getStackBytes(void) const {
        return GblVector_stackBytes(vec_());
    }

    Context* getContext(void) const {
        return Context::fromGblObj(GblVector_context(vec_()));
    }

    allocator_type get_allocator(void) const {
        return getContext();
    }

    Size getSize(void) const {
        return GblVector_size(vec_());
    }

    Size getSizeBytes(void) const {
        return getSize() * getElementSize();
    }

    Size size_bytes(void) const {
        return getSizeBytes();
    }

    Size getCapacity(void) const {
        return GblVector_capacity(vec_());
    }

    Size capacity(void) const {
        return getCapacity();
    }

    Size getElementSize(void) const {
        return GblVector_elementSize(vec_());
    }

    size_type element_size(void) const { return getElementSize(); }

    size_type max_size(void) const { //kinda bullshit, no idea if you can allocate that much
        return std::numeric_limits<size_type>::max();
    }

    const void* getData(void) const {
       return GblVector_data(vec_());
    }

    constexpr pointer data(void) {
        return static_cast<pointer>(const_cast<void*>(getData()));
    }

    constexpr const_pointer data(void) const {
        return static_cast<const_pointer>(const_cast<VectorViewBaseType*>(this)->data());
    }

    T& front(void) const {
        T* pValue = nullptr;
        Exception::checkThrow([&]() {
            pValue = reinterpret_cast<T*>(GblVector_front(vec_()));
        });
        return *pValue;
    }

    T& back(void) const {
        T* pValue = nullptr;
        Exception::checkThrow([&]() {
            pValue = reinterpret_cast<T*>(GblVector_back(vec_()));
        });
        return *pValue;
    }

    friend bool operator==(const CRTP& vec, const type_compatible_container_readable<T> auto& con) {
        if(std::size(vec) != std::size(con)) return false;
        return std::equal(vec.cbegin(), vec.cend(), std::cbegin(con));
    }

    friend constexpr auto operator<=>(const CRTP& vec, const type_compatible_container_readable<T> auto& con) noexcept {
#ifdef GBL_LEX_CMP_3WAY
        return std::lexicographical_compare_three_way(std::cbegin(vec), std::cend(vec), std::cbegin(con), std::cend(con));
#else
        return std::lexicographical_compare(std::cbegin(vec), std::cend(vec), std::cbegin(con), std::cend(con));
#endif
    }

    friend bool operator==(const CRTP& vec, const VectorView<T>& rhs) {
        if(std::size(vec) != std::size(rhs)) return false;
        return std::equal(vec.cbegin(), vec.cend(), rhs.cbegin());
    }

    friend constexpr auto operator<=>(const CRTP& vec, const VectorView<T>& rhs) noexcept {
#ifdef GBL_LEX_CMP_3WAY
        return std::lexicographical_compare_three_way(std::cbegin(vec), std::cend(vec), std::cbegin(rhs), std::cend(rhs));
#else
        return std::lexicographical_compare(std::cbegin(vec), std::cend(vec), std::cbegin(rhs), std::cend(rhs));
#endif
    }

};

template<typename T>
class VectorView final: public VectorViewBase<VectorView<T>, T> {
private:
    const GblVector* pGblVector_ = nullptr;
public:
    VectorView(void) = default;

    VectorView(const GblVector& gblVector):
        pGblVector_(&gblVector) {}

    VectorView(const GblVector* pGblVector):
        pGblVector_(pGblVector) {}

    VectorView(const Vector<T>& vector):
        VectorView(static_cast<const GblVector*>(&vector)) {}

    const GblVector* getVector_(void) const { return pGblVector_; }

    bool isValid(void) const { return pGblVector_; }
};

template<typename T>
VectorView(Vector<T> base) -> VectorView<T>;


/* SHIT THAT ISN'T DONE
 - 1) emplace
 - 2) swap
 - 3) insert
 - 4) shrink_to_fit
 - 5) erase_if
 - 6) operator <=> (DONE, BITCH)
 - 7) C++ destructor callbacks!!1111
        Should eventually be some Vtable looking shit ideally:
        typedef struct GblVectorTypeInfo {
            GblMetaType* pMetaType;
            GblSize elementSize;
            void (*pDestructor)(void* pData, GblSize count);
            void (*pAssign)(void* pLhs, const void* pRhs);
            void (*pMove)(void* pLhs, void* pRhs);
            void (*pCompare)(const void* pLhs, const void* pRhs, GBL_OPERATOR_COMPARISON comp, GblBool* pResult);
        } GblVectorTypeInfo;
 */

template<typename T>
class Vector:
        public GblVector,
        public VectorViewBase<Vector<T>, T>
{
protected:

    void destruct_(Size startPos, Size count=1) {
        if constexpr(std::is_destructible_v<T>) {
            for(Size i = startPos; i < count; ++i) {
                this->data()[i].~T();
            }
        }
    }

public:
    using VectorType            = Vector<T>;
    using VectorViewBaseType    = VectorViewBase<Vector<T>, T>;
    using ViewType              = VectorViewBaseType;
    using MoveValues            = std::tuple<void*, Size, Size>;
    using iterator              = typename VectorViewBaseType::iterator;
    using const_iterator        = typename VectorViewBaseType::const_iterator;
    using difference_type       = typename VectorViewBaseType::difference_type;

    const GblVector* getVector_(void) const {
        return static_cast<const GblVector*>(this);
    }

    GblVector* getVector_(void) {
        return static_cast<GblVector*>(this);
    }

    // =========== VALUE CONSTRUCTORS ==========

    // explicit null constructor
    explicit Vector(std::nullptr_t, Size allocSize=sizeof(VectorType)):
        Vector(nullptr, 0, nullptr, allocSize) {}

    // default constructor
    explicit Vector(Context* pCtx=nullptr, Size allocSize=sizeof(VectorType)):
        Vector(nullptr, 0, pCtx, allocSize) {}

    // raw pointer constructor
    explicit Vector(const T* pInitialData, Size elementCount, Context* pCtx=nullptr, Size allocSize=sizeof(VectorType), Size elementSize=sizeof(T)) {
        Exception::checkThrow(GblVector_construct(this, elementSize, elementCount, pInitialData, allocSize, pCtx));
    }

    // fill constructor
    explicit Vector(Size count, const T& value=T(), Context* pCtx=nullptr, Size allocSize=sizeof(VectorType)) {
        T* pTempArray = static_cast<T*>(GBL_ALLOCA(count * sizeof(T)));
        std::fill(pTempArray, pTempArray+count, value);
        Exception::checkThrow(GblVector_construct(this, sizeof(T), count, pTempArray, allocSize, pCtx));
    }

    // initializer list constructor
    Vector(std::initializer_list<T> c, Context* pCtx=nullptr, Size allocSize=sizeof(VectorType)):
        Vector(pCtx, allocSize)
    {
        insert(this->begin(), c.begin(), c.end());
    }


    // generic container constructor
    explicit Vector(type_compatible_container_readable<T> auto&& c, Context* pCtx=nullptr, Size allocSize=sizeof(VectorType)):
        Vector(pCtx, allocSize)
    {
        *this = std::forward<decltype(c)>(c);
    }

    // generic iterator constructor
    explicit Vector(const type_compatible_iterator_readable<T> auto& first, const type_compatible_iterator_readable<T> auto& last, Context* pCtx=nullptr, Size allocSize=sizeof(VectorType)):
        Vector(pCtx, allocSize)
    {
        insert(this->end(), first, last);
    }

    // =========== COPY CONSTRUCTORS ==========

    // copy constructors - regular
    Vector(const VectorType& other, Context* pCtx=nullptr, Size allocSize=sizeof(VectorType)):
        Vector(VectorView(other), pCtx, allocSize) {}

    // copy constructor - view
    Vector(VectorView<T> other, Context* pCtx=nullptr, Size allocSize=sizeof(VectorType)):
        Vector(other.data(), other.size(), pCtx, allocSize) {}


    // =========== MOVE CONSTRUCTORS ==========

    // move constructor - regular
    Vector(VectorType&& rhs, Context* pCtx=nullptr, Size allocSize=sizeof(VectorType)):
        Vector(static_cast<GblVector&&>(rhs), pCtx, allocSize) {}

    // move constructor - GblVector
    Vector(GblVector&& rhs, Context* pCtx=nullptr, Size allocSize=sizeof(VectorType)):
        Vector(pCtx, allocSize)
    {
        *this = std::move(rhs);
    }

    static MoveValues take(GblVector* pVec) {
        auto moveValues = MoveValues(nullptr, 0, 0);
        auto& [pData, size, capacity] = moveValues;
        Exception::checkThrow(GblVector_take(pVec, &pData, &size, &capacity));
        return moveValues;
    }

    auto take(void) {
        return take(this);
    }

    void give(MoveValues moveValues) {
        auto& [pData, size, capacity] = moveValues;
        Exception::checkThrow(GblVector_give(this, pData, size, capacity));
    }

    ~Vector(void) {
        destruct_(0, size());
        Exception::checkThrow(GblVector_destruct(this));
    }

    using VectorViewBaseType::size;
    using VectorViewBaseType::capacity;

    void reserve(Size count) {
        Exception::checkThrow(GblVector_reserve(this, count));
    }

    void resize(Size newSize) {
        if(newSize < size()) {
            destruct_(newSize, size() - newSize);
        }
        Exception::checkThrow(GblVector_resize(this, newSize));
    }

    void shrink_to_fit(void) {
        Exception::checkThrow(GblVector_shrinkToFit(this));
    }

    void pushBack(T value) {
        Exception::checkThrow(GblVector_pushBack(this, &value));
    }

    void push_back(T value) {
        pushBack(std::move(value));
    }

    template<typename... Args>
    T& emplace_back(Args&&... args) {
        resize(size()+1);
        void* pData = this->at_(size()-1);
        T* pObj = new (pData) T(std::forward<Args>(args)...);
        return *pObj;
    }

    void pushFront(T value) {
        Exception::checkThrow(GblVector_pushFront(this, &value));
    }

    void push_front(T value) {
        pushFront(std::move(value));
    }

    iterator insert(const_iterator position, const T* pValue, Size count) {
        const auto index = std::distance(this->cbegin(), position);
        Exception::checkThrow([&]() {
            GblVector_insert(this, index, count, pValue);
        });
        return iterator(*this, index);
    }

    iterator insert(const_iterator position, const T& val) {
        return insert(std::move(position), &val, 1);
    }

    iterator insert(const_iterator position, gimbal::Size size, const T& val) {
        T* pTempArray = static_cast<T*>(GBL_ALLOCA(size * sizeof(T)));
        std::fill(pTempArray, pTempArray+size, val);
        return insert(std::move(position), pTempArray, size);
    }

    iterator insert(const_iterator position, const type_compatible_iterator_readable<T> auto& first, const type_compatible_iterator_readable<T> auto& last) {
        const auto size = std::distance(first, last);
        T* pTempArray = static_cast<T*>(GBL_ALLOCA(size * sizeof(T)));
        std::copy(first, last, pTempArray);
        return insert(std::move(position), pTempArray, size);
    }

    template<type_compatible_iterator_readable<T> It>
        requires contiguous_iterator<It>
    iterator insert(const_iterator position, const It& first, const It& last) {
        return insert(std::move(position), &first[0], std::distance(first, last));
    }

    iterator insert(const_iterator position, const type_compatible_container_readable<T> auto &c) {
        return insert(std::move(position), std::cbegin(c), std::cend(c));
    }

    iterator insert(const_iterator position, const type_compatible_contiguous_container_readable<T> auto &c) {
        return insert(std::move(position), c.data(), std::size(c));
    }

    template<typename... Args>
    iterator emplace(const_iterator position, Args&&... args) {
        const auto index = std::distance(this->cbegin(), position);
        void* pData = nullptr;
        Exception::checkThrow([&]() {
            pData = GblVector_insert(this, index, 1, nullptr);
        });
         T* pObj = new (pData) T(std::forward<Args>(args)...);
         return iterator(*this, index);
    }

    void popBack(void) {// throw if bullshit
        destruct_(this->getSize()-1, 1);
        erase(this->end()-1);
    }

    void pop_back(void) {
        popBack();
    }

    iterator erase(const_iterator pos) {
        return erase(pos, pos+1);
    }

    iterator erase(const_iterator first, const_iterator last) {
        const auto index = std::distance<const_iterator>(this->begin(), first);
        const auto count = std::distance(first, last);
        destruct_(index, count);
        Exception::checkThrow(GblVector_erase(this, index, count));
        return iterator(*this, index+count);
    }

    void clear(void) {
        destruct_(0, this->getSize());
        Exception::checkThrow(GblVector_clear(this));
    }

    void assign(const T* pData, Size elementCount) {
        Exception::checkThrow(GblVector_assign(this, pData, elementCount));
    }

    void assign(gimbal::Size size, const T& value) {
        T* pTempArray = static_cast<T*>(GBL_ALLOCA(size * sizeof(T)));
        std::fill(pTempArray, pTempArray+size, value);
        assign(pTempArray, size);
    }

    void assign(const type_compatible_contiguous_container_readable<T> auto &c) {
        assign(c.data(), std::size(c));
    }

    void assign(const type_compatible_container_readable<T> auto& c) {
        T* pTempArray = static_cast<T*>(GBL_ALLOCA(std::size(c) * sizeof(T)));
        std::copy(begin(c), end(c), pTempArray);
        Exception::checkThrow(GblVector_assign(this, pTempArray, std::size(c)));
    }

    void assign(const type_compatible_iterator_readable<T> auto& first, const type_compatible_iterator_readable<T> auto& last) {
        const auto size = std::distance(first, last);
        T* pTempArray = static_cast<T*>(GBL_ALLOCA(size * sizeof(T)));
        std::copy(first, last, pTempArray);
        assign(pTempArray, size);
    }

    template<type_compatible_iterator_readable<T> It>
        requires contiguous_iterator<It>
    void assign(const It& first, const It& last) {

            const T* pData = &first[0];
            const auto dist = std::distance(first, last);

        assign(pData, dist);
    }

    const VectorType& operator=(VectorView<T> rhs) {
        Exception::checkThrow(GblVector_assign(this, rhs.data(), rhs.size()));
        return *this;
    }

    const VectorType& operator=(const VectorType& rhs) {
        return *this = VectorView<T>(rhs);
    }

    const VectorType& operator=(VectorType&& rhs) {
        return *this = static_cast<GblVector&&>(rhs);
    }

    const VectorType& operator=(GblVector&& rhs) {
        VectorView<T> rhsView(rhs); //C++ wrapper "viewing" C object

        if(this->getContext() == rhsView.getContext() && rhsView.isHeap()) {
            give(take(&rhs));
        } else {
            *this = rhsView; //do regular-ass copy assignment for generic view
        }
        return *this;
    }

    const VectorType& operator=(const type_compatible_container<T> auto&& c) {
        // check for supported move semantics!
#ifdef GBL_PMR_VECTOR
        if constexpr(std::same_as<decltype(c), gimbal::pmr::vector<T>&&>) {
            if(this->get_allocator() == c.get_allocator()) {
                assign(c);
            }
        } else {
#endif
            assign(c);
#ifdef GBL_PMR_VECTOR
        }
#endif
        return *this;
    }

    const VectorType& operator+=(const type_compatible_container_readable<T> auto& view) {
        insert(this->end(), view);
        return *this;
    }

    const VectorType& operator+=(const type_compatible_iterator_readable<T> auto& it) {
        insert(this->end(), it);
        return *this;
    }

    const VectorType& operator+=(const T& value) {
        insert(this->end(), 1, value);
        return *this;
    }

    const VectorType& operator+=(VectorView<T> view) {
        insert(this->end(), view.data(), view.size());
        return *this;
    }
};


// overflow when copying shit into the bitch without enough room on stack when no context is present
template<Size ExtraStackSize, typename T=void*>
class FlexibleVector final: public Vector<T> {
public:
    using FlexibleVectorType = FlexibleVector<ExtraStackSize, T>;
    constexpr static const inline Size ExtraStackBufferSize = ExtraStackSize;
private:
    char extraStackBuffer_[ExtraStackSize] = { 0 };
public:

};
/*
template<typename T>
Vector(std::initializer_list<T> list, Context* pCtx=nullptr, Size allocSize=sizeof(Vector<T>)) -> Vector<T>;
*/

template<generic_container_readable Container,
         typename T = container_value_t<Container>>
Vector(const Container& c, Context* pCtx=nullptr, Size allocSize=sizeof(Vector<T>)) -> Vector<T>;


template<std::input_iterator Iterator,
         typename T = iterator_value_t<Iterator>>
Vector(Iterator begin, Iterator end, Context* pCtx=nullptr, Size allocSize=sizeof(Vector<T>)) -> Vector<T>;

template<typename T>
Vector(VectorView<T> view, Context* pCtx=nullptr, Size allocSize=sizeof(Vector<T>)) -> Vector<T>;

template<typename T, size_t N>
Vector(std::array<T, N> array, Context* pCtx=nullptr, Size allocSize=sizeof(Vector<T>)) -> Vector<T>;

Vector(GblVector gblVec, Context* pCtx=nullptr, Size allocSize=sizeof(Vector<void*>)) -> Vector<void*>;
Vector(const GblVector* pVec, Context* pCtx=nullptr, Size allocSize=sizeof(Vector<void*>)) -> Vector<void*>;


template<typename T>
gimbal::Size erase(gimbal::Vector<T>& vec, const T& value) {
    auto it = std::remove(vec.begin(), vec.end(), value);
    auto r = std::distance(it, vec.end());
    vec.erase(it, vec.end());
    return r;
}

template<typename T, typename Pred>
gimbal::Size erase_if(gimbal::Vector<T>& vec, Pred pred) {
    auto it = std::remove_if(vec.begin(), vec.end(), pred);
    auto r = std::distance(it, vec.end());
    vec.erase(it, vec.end());
    return r;
}

template<typename T>
void swap(gimbal::Vector<T>& lhs, gimbal::Vector<T>& rhs) {
    void* plData    = GBL_ALLOCA(lhs.size_bytes());
    const auto lSize = lhs.size();
    memcpy(plData, lhs.data(), lhs.size_bytes());
    lhs = rhs;
    rhs.assign(static_cast<const T*>(plData), lSize);
}

}

#endif // GIMBAL_VECTOR_HPP
