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
    void newInPlaceVaArgs(void);
    void newVaArgsWithClass(void);
    void newInPlaceVaArgsWithClass(void);
    void newInPlaceVaArgsWithClassInPlace(void);
    void newVariants(void);
    void newVariantsWithClass(void);
    void newInPlaceVariants(void);
    void newInPlaceVariantsWithClass(void);
    void propertyGet(void);
    void propertySet(void);
    void parenting(void);
    void classSwizzle(void);
    void eventNotify(void);
    void eventNotifyAncestors(void);
    void eventSendAncestors(void);
    void eventFilters(void);


    // iteration, saving, loading, variant interop

};


}
#endif // TEST_GIMBAL_OBJECT_HPP
