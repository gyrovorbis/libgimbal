#ifndef TEST_GIMBAL_QUARK_HPP
#define TEST_GIMBAL_QUARK_HPP


#if 1
#include "test_gimbal.hpp"
#include <gimbal/types/gimbal_quark.hpp>

namespace gimbal::test {

//error strings?
class Quark: public UnitTestSet {
Q_OBJECT
private:
    static const char* quark_cast(GblQuark quark) {
        return reinterpret_cast<const char*>(quark);
    }
private slots:
    void reinit(void);
    void fromStatic(void);

};

inline void Quark::reinit(void) {
    verifyResult(GblQuark_init(hCtx(), 64, 12));
}

inline void Quark::fromStatic(void) {
    GblQuark quark = GblQuark_fromStringStatic("staticString");
    QCOMPARE(QString(quark_cast(quark)), "staticString");
    quark = GblQuark_fromStringStatic("Staxy");
    QCOMPARE(QString(quark_cast(quark)), "Staxy");
    QCOMPARE(GblQuark_count(), 2);
    QCOMPARE(GblQuark_bytesUsed(), 0);
}



/*
inline void CObject::newDefault(void) {
    TestObject* pObj = TEST_OBJECT(GblObject_new(TEST_OBJECT_TYPE, NULL));
}*/

}
#endif

#endif // TEST_GIMBAL_QUARK_HPP
