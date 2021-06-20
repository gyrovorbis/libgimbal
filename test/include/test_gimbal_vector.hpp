#ifndef TEST_GIMBAL_VECTOR_HPP
#define TEST_GIMBAL_VECTOR_HPP

#include "test_gimbal.hpp"
#include <gimbal/gimbal_container.hpp>
#include <sstream>


/* TODO: test::gimbal::string already handles stack vs heap validation,
 *       but that logic should be moved to here.
*/
namespace gimbal::test {

using namespace gimbal;

class Vector: public UnitTestSet {
Q_OBJECT
public:
    Vector(gimbal::test::MonitorableContext* pCtx=nullptr):
        UnitTestSet(pCtx? pCtx : new gimbal::test::TestContext()) {}
private:

    struct Emplacer_ {
    private:
        gimbal::Context* pCtx_;
        float x_;
        int y_;
    public:
        Emplacer_(gimbal::Context* pCtx=nullptr, float x=FP_NAN, int y=INT_MAX):
            pCtx_(pCtx), x_(x), y_(y) {}

        ~Emplacer_(void) {
            GBL_API_BEGIN(static_cast<GblContext>(*pCtx_));
            GBL_API_WARN("DELETING EMPLACER[%f, %d]", x_, y_);
            GBL_API_END_BLOCK();
        }

        bool operator==(const Emplacer_ rhs) const {
            return x_ == rhs.x_ && y_ == rhs.y_ && pCtx_ == rhs.pCtx_;
        }
    };

    template<typename T=void*>
    void verifyContainer_(const gimbal::type_compatible_container_readable<T> auto& cont, std::initializer_list<T> expected={}) {
        QCOMPARE(std::size(cont), std::size(expected));
        auto expIt = std::cbegin(expected);
        auto conIt = std::cbegin(cont);
        gimbal::Size i = 0;
        while(expIt != std::cend(expected) && conIt != std::cend(cont)) {
            QCOMPARE(*conIt, *expIt);
            QCOMPARE(cont[i], *expIt);
            QCOMPARE(cont.at(i), *expIt);
            QCOMPARE(cont.data()[i], *expIt);
            if(i == 0) QCOMPARE(cont.front(), *expIt);
            if(i == expected.size()) QCOMPARE(cont.back(), *expIt);
            ++expIt; ++conIt; ++i;
        }

        QCOMPARE(expIt, std::cend(expected));
        QCOMPARE(conIt, std::cend(cont));
        QCOMPARE(i, std::size(cont));
        QCOMPARE(cont.empty(), std::size(cont) == 0);

    }

    template<typename T=void*>
    void verifyVector_(const gimbal::Vector<T>& view, const std::initializer_list<T>& expected={}, gimbal::Context* pCtx=nullptr, gimbal::Size allocSize=sizeof(gimbal::Vector<T>)) {
        verifyContainer_(view, expected);
        QCOMPARE(view, expected);
        QVERIFY(view.capacity() >= std::size(view));
        QCOMPARE(view.getContext(), pCtx);
        QCOMPARE(view.get_allocator(), pCtx);
        QCOMPARE(view.getStackBytes(), allocSize - sizeof(GblVector) + sizeof(GblVector::stackBuffer));
    }

private slots:
    void cleanup(void);
    void constructValue(void);
    void constructCopy(void);
    void constructMove(void);

    void comparison(void);

    void assignValue(void);
    void assignCopy(void);
    void assignMove(void);

    void pushBack(void);
    void pushFront(void);
    void emplaceBack(void);
    void assign(void);
    void insert(void);
    void emplace(void);
    void popBack(void);
    void erase(void);

    void plusEquals(void);

    void reserve(void);
    void resize(void);
    void clear(void);
    void shrinkToFit(void);

    void stdErase(void);
    void stdSwap(void);
};


inline void Vector::constructValue(void) {
    verifyVector_(gimbal::Vector());
    verifyVector_(gimbal::Vector(pCtx()), {}, pCtx());
    verifyVector_(gimbal::Vector { 1, 2, 4, 5}, {1, 2, 4, 5 });
    verifyVector_(gimbal::Vector<char>((std::array{'a', 'b', 'c', 'd' }), pCtx()), {'a', 'b', 'c', 'd'}, pCtx());
    verifyVector_(gimbal::Vector<float>((std::vector{3.14f, 2.48f, -17.0f}), pCtx()), {3.14f, 2.48f, -17.0f}, pCtx());
    const char* stringList[] = {
        "Hello", "There", "Bitchweasel!"
    };
    verifyVector_(gimbal::Vector<const char*>(stringList, sizeof(stringList)/sizeof(stringList[0]), pCtx()), {stringList[0], stringList[1], stringList[2]}, pCtx());
    const auto srcStdVec = std::vector{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    verifyVector_(gimbal::Vector(std::cbegin(srcStdVec)+3, std::cbegin(srcStdVec)+8, pCtx()), {4, 5, 6, 7, 8}, pCtx());
}

inline void Vector::constructCopy(void) {
    std::initializer_list<int> init1 { 1, 2, 3, 4 };
    gimbal::Vector v1 = init1;
    gimbal::Vector<int> v2(v1, pCtx());
    const GblVector* pGbl1 = static_cast<const GblVector*>(&v2);
    gimbal::Vector<int> v3(pGbl1, pCtx());
    gimbal::Vector<int> v33(pGbl1, pCtx());
    gimbal::VectorView<int> view1(pGbl1);
    gimbal::VectorView view11(pGbl1);
    gimbal::VectorView<int> view2(v2);
    gimbal::VectorView view22(v2);
    gimbal::Vector v4(view1);
    gimbal::Vector<int> v5(view2, pCtx());

    verifyVector_(v1, init1);
    verifyVector_(v2, init1, pCtx());
    verifyVector_(v3, init1, pCtx());
    verifyVector_(gimbal::Vector(view1), init1);
    verifyVector_(gimbal::Vector(view2), init1);
    verifyVector_(v4, init1);
    verifyVector_(v5, init1, pCtx());

    QCOMPARE(v1, v2);
    QCOMPARE(v1, v3);
    QCOMPARE(v1, v5);
    QCOMPARE(v1, view1);
    QCOMPARE(v1, view2);
    QCOMPARE(v1, *pGbl1);
    QCOMPARE(view1, v1);
    QCOMPARE(view1, init1);
    QCOMPARE(view1 ,view2);
    QCOMPARE(view1, *pGbl1);
    QCOMPARE(view1, view2);
    QCOMPARE(*pGbl1, v4);
    QCOMPARE(gimbal::VectorView<int>(*pGbl1), *pGbl1);
    QCOMPARE(*pGbl1, view2);
    QCOMPARE(gimbal::VectorView<int>(pGbl1), init1);
}

inline void Vector::constructMove(void) {
    std::initializer_list<int> init = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    gimbal::Vector<int> v1(init, pCtx());
    auto* pData = v1.data();
    gimbal::Vector<int> v2(std::move(v1), pCtx());

    QCOMPARE(v1.size(), 0);
    QVERIFY(v1.data() != pData);

    QCOMPARE(v2, init);
    QCOMPARE(v2.data(), pData);
    QVERIFY(v1 != v2);

    GblVector* pVec = static_cast<GblVector*>(&v2);
    gimbal::Vector<int> v3(std::move(*pVec), pCtx());
    QCOMPARE(v2.size(), 0);
    QVERIFY(v2.data() != pData);

    QCOMPARE(v3, init);
    QCOMPARE(v3.data(), pData);
    QVERIFY(v3 != v2);
}

inline void Vector::comparison(void) {
    auto test = [&]<typename T>(const gimbal::Vector<T>& vec, auto&& other, bool equal=true) {
        if(equal) {
            QCOMPARE(vec, other);
            QVERIFY(!(vec != other));
        }
        else {
            QVERIFY(vec != other);
            QVERIFY(!(vec == other));
        }
    };

    test(gimbal::Vector({1, 2, 3, 4}), std::initializer_list<int>{1, 2, 3, 4});
    std::array charArray = { 'a', 'b' };
    test(gimbal::Vector{'a', 'b'}, charArray);
    test(gimbal::Vector({1.2f, 1.3f, 1.4f}, pCtx()), std::vector { 1.2f, 1.3f, 1.4f });
    QCOMPARE(gimbal::VectorView(gimbal::Vector<int>(std::pmr::vector<int>({1, 2, 3, 4, 5}, pCtx()))), (std::array{1, 2, 3, 4, 5}));

    QVERIFY(gimbal::Vector({1, 2, 3, 0}) < (std::initializer_list<int>{1, 2, 3, 4}));
    QVERIFY((gimbal::Vector{'a', 'c'}) > charArray);
    QVERIFY(gimbal::VectorView(gimbal::Vector<int>(std::pmr::vector<int>({1, 2, 3, 4, 5}, pCtx()))) <= (std::array{1, 2, 3, 4, 5}));
    gimbal::Vector vectrix = {1, 2};
    QVERIFY(gimbal::VectorView(vectrix) >= static_cast<GblVector&>(vectrix));
}

inline void Vector::assignValue(void) {
    gimbal::Vector vec1;
    gimbal::Vector<int> vec2;
    gimbal::Vector<char> vec3;
    gimbal::Vector<float> vec4;
    gimbal::Vector<const char*> vec5;

    vec2 = { 1, 2, 4, 5};
    verifyVector_(vec2, {1, 2, 4, 5 });

    vec3 = std::array{'a', 'b', 'c', 'd' };
    verifyVector_(vec3, {'a', 'b', 'c', 'd'});

    vec4 = std::vector{3.14f, 2.48f, -17.0f};
    verifyVector_(vec4, {3.14f, 2.48f, -17.0f});
    const char* stringList[] = {
        "Hello", "There", "Bitchweasel!"
    };

    vec5.assign(stringList, sizeof(stringList)/sizeof(stringList[0]));
    verifyVector_(vec5, {stringList[0], stringList[1], stringList[2]});

    const auto srcStdVec = std::vector{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    vec2.assign(std::cbegin(srcStdVec)+3, std::cbegin(srcStdVec)+8);
    verifyVector_(vec2, {4, 5, 6, 7, 8});
}

inline void Vector::assignCopy(void) {
    std::initializer_list<int> init1 = { 1, 2, 3, 4 };
    gimbal::Vector<int> v1; v1 = init1;
    gimbal::Vector<int> v2(pCtx()); v2 = v1;
    const GblVector* pGbl1 = static_cast<const GblVector*>(&v2);
    gimbal::Vector<int> v3(pCtx()); v3 = pGbl1;
    gimbal::Vector v33(pCtx()); v33 = pGbl1;
    gimbal::VectorView<int> view1(pGbl1);
    gimbal::VectorView<int> view2(v2);
    gimbal::Vector<int> v4; v4 = view1;
    gimbal::Vector<int> v5(pCtx()); v5 = view2;

    verifyVector_(v1, init1);
    verifyVector_(v2, init1, pCtx());
    verifyVector_(v3, init1, pCtx());
    verifyVector_(gimbal::Vector(view1), init1);
    verifyVector_(gimbal::Vector(view2), init1);
    verifyVector_(v4, init1);
    verifyVector_(v5, init1, pCtx());

    QCOMPARE(v1, v2);
    QCOMPARE(v1, v3);
    QCOMPARE(v1, v5);
    QCOMPARE(v1, view1);
    QCOMPARE(v1, view2);
    QCOMPARE(v1, *pGbl1);
    QCOMPARE(view1, v1);
    QCOMPARE(view1, init1);
    QCOMPARE(view1 ,view2);
    QCOMPARE(view1, *pGbl1);
    QCOMPARE(view1, view2);
    QCOMPARE(*pGbl1, v4);
    QCOMPARE(gimbal::VectorView<int>(*pGbl1), *pGbl1);
    QCOMPARE(*pGbl1, view2);
    QCOMPARE(gimbal::VectorView<int>(pGbl1), init1);
}

inline void Vector::assignMove(void) {
    std::initializer_list<int> init = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    gimbal::Vector<int> v1(pCtx()); v1 = init;
    auto* pData = v1.data();
    gimbal::Vector<int> v2(pCtx()); v2 = std::move(v1);

    QCOMPARE(v1.size(), 0);
    QVERIFY(v1.data() != pData);

    QCOMPARE(v2, init);
    QCOMPARE(v2.data(), pData);
    QVERIFY(v1 != v2);

    GblVector* pVec = static_cast<GblVector*>(&v2);
    gimbal::Vector<int> v3(pCtx()); v3 = std::move(*pVec);
    QCOMPARE(v2.size(), 0);
    QVERIFY(v2.data() != pData);

    QCOMPARE(v3, init);
    QCOMPARE(v3.data(), pData);
    QVERIFY(v3 != v2);
}

inline void Vector::pushBack(void) {
    auto test = [&]<typename T>(std::initializer_list<T> init) {
        gimbal::Vector<T> vec(pCtx());
        for(auto val: init) {
            vec.push_back(val);
        }
        QCOMPARE(vec, init);
    };
    test({1, 2, 3, 4, 5, 6, 7});
    test({'a', 'b', 'c', 'd', 'e', 'f'});
    test({-12.34f, 34.1112f});
    test({"Fuck", "You", "Asshole"});
}

inline void Vector::pushFront(void) {
    auto test = [&]<typename T>(std::initializer_list<T> init) {
        gimbal::Vector<T> vec(pCtx());
        for(auto val: init) {
            vec.push_front(val);
        }
        QVERIFY(std::equal(std::crbegin(vec), std::crend(vec), std::cbegin(init)));
    };
   test({1, 2, 3, 4, 5, 6, 7});
    test({'a', 'b', 'c', 'd', 'e', 'f'});
    test({-12.34f, 34.1112f});
    test({"Fuck", "You", "Asshole"});
}

inline void Vector::emplaceBack(void) {
    gimbal::Vector<Emplacer_> vec;
    vec.emplace_back(pCtx(), 3.14f, 22);
    vec.emplace_back(pCtx(), -34.124f, 0);
    QVERIFY(Emplacer_(pCtx(), 3.14f, 22) == vec.at(0));
    QVERIFY(vec.at(1) == Emplacer_(pCtx(), -34.124f, 0));
}

inline void Vector::assign(void) {
    gimbal::Vector vec({ 1, 2, 3 }, pCtx());
    std::vector stdVec{ 5, 6, 7, 8, 9, 10 };
    std::array array { 64, 67, 68 };
    std::initializer_list<int> init2{-8, -9, -10 };
    auto test = [&]<typename T>(std::initializer_list<T> expected, auto&&... args) {
        gimbal::Vector<T> vec(this->pCtx());
        vec.assign(std::forward<decltype(args)>(args)...);
        QCOMPARE(vec, expected);
    };
    test({17, 17, 17}, 3, 17);
    test({1, 2, 3}, vec.data(), 3);
    test({5, 6, 7, 8, 9, 10}, stdVec);
    test({-8, -9, -10}, init2);
    test({1, 2, 3}, vec.cbegin(), vec.cend());
    test({3, 2, 1}, vec.crbegin(), vec.crend());
    // FUCKIN FIX ME
    test({10, 9, 8, 7, 6, 5}, stdVec.crbegin(), stdVec.crend());
    test({-9, -10}, std::cbegin(init2)+1, std::cend(init2));
    test({68, 67, 64}, std::crbegin(array), std::crend(array));
    test({64, 67, 68}, array);
}

inline void Vector::insert(void) {
    gimbal::Vector vec({ 1, 2, 3 }, pCtx());
    std::vector stdVec{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::initializer_list<int> init{0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10 };
    std::array array { 64, 67, 68 };
    std::initializer_list<int> init2{-8, -9, -10 };

    auto test = [&]<typename T>(gimbal::Vector<T> vec, std::initializer_list<T> expected, typename gimbal::Vector<T>::const_iterator pos, auto&&... args) {
        auto it = vec.insert(pos, std::forward<decltype(args)>(args)...);
        QCOMPARE(vec, expected);
        QCOMPARE(it-vec.begin(), pos-vec.cbegin());
    };

    test(vec, {0, 1, 2, 3}, vec.cbegin(), 0);
    test(vec, {1, 2, 17, 17, 17, 3}, vec.cbegin()+2, 3, 17);
    test(vec, {1, 2, 1, 2, 3, 3}, vec.begin()+2, vec.data(), 3);
    test(vec, {1, 4, 5, 6, 2, 3}, vec.begin()+1, stdVec.begin()+4, stdVec.begin()+7);
    test(vec, {-3, -2, -1, 0, 1, 2, 3}, vec.begin(), std::crbegin(init)+7, std::crend(init));
    test(vec, { 64, 67, 68, 1, 2, 3}, vec.begin(), array);
    test(vec, { 1, 2, -8, -9, -10, 3}, vec.begin()+2, init2);
    test(vec, {1, 2, 3, 1, 2, 3}, vec.end(), vec);
}

inline void Vector::emplace(void) {
    gimbal::Vector<Emplacer_> vec(3, Emplacer_(pCtx()), pCtx());

    auto test = [&]<typename T>(gimbal::Vector<T>& vec, auto&& position, auto&&... args) {
        const auto oldSize = vec.size();
        auto it = vec.emplace(position, std::forward<decltype(args)>(args)...);
        QCOMPARE(oldSize+1, vec.size());
        const auto index = std::distance(std::begin(vec), it);
        QCOMPARE(index, std::distance(std::begin(vec), position));
        Emplacer_ e(std::forward<decltype(args)>(args)...);
        QCOMPARE(*it, e);
        QCOMPARE(e, vec[static_cast<gimbal::Size>(index)]);
    };

    test(vec, vec.begin(), pCtx(), 1.11f, 12);
}

inline void Vector::popBack(void) {
    gimbal::Vector vec = { 1, 2, 3, 4, 5 };
    verifyVector_(vec, {1, 2, 3, 4, 5});
    vec.popBack();
    verifyVector_(vec, {1, 2, 3, 4});
    vec.pop_back();
    verifyVector_(vec, {1, 2, 3});
}

inline void Vector::erase(void) {
    gimbal::Vector vec({ 1, 2, 3 }, pCtx());
    auto test = [&]<typename T>(gimbal::Vector<T> vec, std::initializer_list<T> expected, auto&&... args) {
        vec.erase(std::forward<decltype(args)>(args)...);
        QCOMPARE(vec, expected);
    };

    test(vec, {2, 3}, std::begin(vec));
    test(vec, {1}, std::begin(vec)+1, std::end(vec));
    test(vec, {}, std::begin(vec), std::end(vec));
}

inline void Vector::plusEquals(void) {
    gimbal::Vector vec({ 1, 2, 3 }, pCtx());
    std::vector stdVec{ 5, 6, 7, 8, 9, 10 };
    std::array array { 64, 67, 68 };
    std::initializer_list<int> init2{-8, -9, -10 };
    auto test = [&]<typename T>(gimbal::Vector<T> vec, std::initializer_list<T> expected, auto&& arg) {
        vec += std::forward<decltype(arg)>(arg);
        QCOMPARE(vec, expected);
    };

    test(vec, {1, 2, 3, 4}, 4);
    test(vec, {1, 2, 3, 1, 2, 3}, vec);
    test(vec, {1, 2, 3, 5, 6, 7, 8, 9, 10}, stdVec);
    test(vec, {1, 2, 3, 64, 67, 68}, array);
    test(vec, {1, 2, 3, -8, -9, -10}, init2);
    test(vec, {1, 2, 3, 1, 2, 3}, gimbal::VectorView(vec));
    test(vec, {1, 2, 3, 1, 2, 3}, static_cast<GblVector&>(vec));
}

inline void Vector::reserve(void) {
    std::initializer_list<int> init = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    gimbal::Vector<int> vec1(init, pCtx());
    gimbal::Size oldCapacity = vec1.capacity();
    vec1.reserve(10);
    QCOMPARE(vec1.size(), 10);
    QCOMPARE(vec1.capacity(), oldCapacity);
    QCOMPARE(vec1, init);
    vec1.reserve(256);
    QCOMPARE(vec1.size(), 10);
    QVERIFY(vec1.capacity() > oldCapacity);
    QVERIFY(vec1.capacity() >= 256);
    QVERIFY(std::equal(std::cbegin(init), std::cend(init), std::cbegin(vec1)));
    oldCapacity = vec1.capacity();
    vec1.reserve(10);
    QCOMPARE(vec1.size(), 10);
    QCOMPARE(vec1.capacity(), oldCapacity);
    QCOMPARE(vec1, init);
}

inline void Vector::resize(void) {
    std::initializer_list<int> init = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    gimbal::Vector<int> vec1(init, pCtx());
    vec1.resize(10);
    QCOMPARE(vec1.size(), 10);
    QCOMPARE(vec1, init);
    vec1.resize(31);
    QCOMPARE(vec1.size(), 31);
    QVERIFY(std::equal(std::cbegin(init), std::cend(init), std::cbegin(vec1)));
    vec1.resize(10);
    QCOMPARE(vec1.size(), 10);
    QCOMPARE(vec1, init);
}

inline void Vector::clear(void) {
    std::initializer_list<int> init = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    gimbal::Vector<int> vec(init, pCtx());
    vec.resize(256);
    vec.clear();
    QVERIFY(vec.empty());
}

inline void Vector::shrinkToFit(void) {
    std::initializer_list<int> init = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    gimbal::Vector<int> vec(init, pCtx());
    vec.reserve(256);
    QCOMPARE(vec.capacity(), 256);
    vec.shrink_to_fit();
    QCOMPARE(vec.size(), 10);
    QCOMPARE(vec.capacity(), vec.size());
    QCOMPARE(vec, init);
}

inline void Vector::stdErase(void) {
    gimbal::Vector vex = { 12, 3, 34 };
    gimbal::erase(vex, 3);
    QCOMPARE(vex.size(), 2);

    erase_if(vex, [](int x) { return x == 34; });
    QCOMPARE(vex.size(), 1);
    QCOMPARE(vex[0], 12);
}

inline void Vector::cleanup(void) {
    QCOMPARE(getContext()->getAllocTracker().getActiveAllocationCount(), 0);
}

inline void Vector::stdSwap(void) {
    auto test = [&]<typename T>(gimbal::Vector<T> v1, gimbal::Vector<T> v2) {
        gimbal::Vector<T> v1Old(v1);
        gimbal::Vector<T> v2Old(v2);
        swap(v1, v2);
        QCOMPARE(v1, v2Old);
        QCOMPARE(v2, v1Old);
    };

    test(gimbal::Vector{1, 2, 4}, gimbal::Vector{5, 6, 8});
    test(gimbal::Vector((std::array{3, 5})), gimbal::Vector<int>(4, 4));
}

}

#endif // TEST_GIMBAL_VECTOR_HPP
