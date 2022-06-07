#ifndef TEST_GIMBAL_STRING_HPP
#define TEST_GIMBAL_STRING_HPP

#include "test_gimbal.hpp"

#define GBL_TEST_STRING_STACK_BUFFER_DEFAULT_SIZE    8

namespace gimbal {
    class String;
}

namespace gimbal::test {

/* Different types:
1) StringView
2) GblStringBuffer
3) String

Different axes:
1) Stack size
2) Stack vs Heap
3) Context vs No Context

*/

class String: public UnitTestSet {
Q_OBJECT
public:
    String(gimbal::test::MonitorableContext* pCtx=nullptr):
        UnitTestSet(pCtx? pCtx : new gimbal::test::TestContext()) {}
private:

    void verifyString_(const gimbal::String& str, const char* pString);
    void verifyStack_(const gimbal::String& str, gimbal::Size stackSize=GBL_TEST_STRING_STACK_BUFFER_DEFAULT_SIZE);
    void verifyHeap_(const gimbal::String& str);
private slots:
    void cleanup(void);

    void createDefault(void);
    void createNullptr(void);

    void createStackCString(void);
    void createStackLiteral(void);
    void createStackStdString(void);
    void createStackStdPmrString(void);
    void createStackStringView(void);
    void createStackCCopy(void);
    void createStackCMove(void);
    void createStackCppCopy(void);
    void createStackCppMove(void);
    void createHeapCString(void);

    void assignHeapCCopy(void);
    void assignHeapCMove(void);
    void assignHeapCppCopy(void);
    void assignHeapCppMove(void);
    void assignHeapValueTypes(void);

    void operatorSubscriptRead(void);
    void operatorSubscriptWrite(void);

    void stdIteratorConst(void);
    void stdIterator(void);
    void rangeBasedFor(void);
    void stdHash(void);
    void stdSwap(void);
    void stdOStream(void);
    void stdIStream(void);

    void clear(void);
    void reserve(void);
    void resize(void);

    void sprintf(void);
    void varArgs(void);
    void concat(void);
    void operatorAdd(void);
};

}

#endif // TEST_GIMBAL_STRING_HPP
