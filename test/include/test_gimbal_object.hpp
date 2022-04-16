#ifndef TEST_GIMBAL_OBJECT_HPP
#define TEST_GIMBAL_OBJECT_HPP

#include "test_gimbal.hpp"


namespace gimbal::test {

struct TestObject;

//error strings?
class CObject: public UnitTestSet {
Q_OBJECT
private:
    TestObject* pTestObj_ = NULL;
private slots:
    void newDefault(void);
    void ref(void);
    void unref(void);
    void extendedData(void);
    void newVaArgs(void);
    void newVariants(void);
    void propertyGet(void);
    void propertySet(void);
    void parenting(void);


};


}
#endif // TEST_GIMBAL_OBJECT_HPP
