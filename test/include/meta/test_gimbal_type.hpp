#ifndef TEST_GIMBAL_TYPE_HPP
#define TEST_GIMBAL_TYPE_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_type.h"

#define GBL_TEST_CASE_QT_SLOT(x) \
    inline void x(void) { verifyResult(::x(gblCtx())); }


namespace gimbal::test {

class Type: public UnitTestSet {
    Q_OBJECT
private:

private slots:
    void init_(void);
    void invalidType(void);
    void builtinTypes(void);
    void blankType(void);
    void reregisterType(void);
    void unregisterType(void);
    void cClassedType(void);
    void derivedType(void);
    void placementDerivedType(void);
    void interfaceType(void);
    void implementInterfaceType(void);

    GBL_TYPE_TEST_SET(GBL_TEST_CASE_QT_SLOT)


};



}

#endif // TEST_GIMBAL_TYPE_H
