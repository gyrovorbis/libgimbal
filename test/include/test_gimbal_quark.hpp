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
    verifyResult(gblQuarkReinit(hCtx(), 64, 12));
}

inline void Quark::fromStatic(void) {
    GblQuark quark = gblQuarkFromStringStatic("staticString");
    QCOMPARE(QString(quark_cast(quark)), "staticString");
    quark = gblQuarkFromStringStatic("Staxy");
    QCOMPARE(QString(quark_cast(quark)), "Staxy");
    QCOMPARE(gblQuarkCount(), 2);
    QCOMPARE(gblQuarkBytesUsed(), 0);
}



/*
inline void CObject::newDefault(void) {
    TestObject* pObj = TEST_OBJECT(GblObject_new(TEST_OBJECT_TYPE, NULL));
}*/

}
#endif

#endif // TEST_GIMBAL_QUARK_HPP
