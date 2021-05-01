#ifndef TEST_GIMBAL_STRING_HPP
#define TEST_GIMBAL_STRING_HPP

#undef GBL_STRING_BUFFER_BASE_STACK_SIZE
#define GBL_STRING_BUFFER_BASE_STACK_SIZE 8

#include <elysian_qtest.hpp>
#include <gimbal/gimbal_string.hpp>
namespace gimbal::test {

class String: public elysian::UnitTestSet {
Q_OBJECT
private slots:
    void createDefault(void);
    void createLiteral(void);
};

using namespace gimbal;

inline void String::createDefault(void) {
    gimbal::String string;
    QCOMPARE(string.getContext(), nullptr);
    QVERIFY(string.isEmpty());
    QVERIFY(string.isStack());
    QVERIFY(!string.isHeap());
    QCOMPARE(string.getCapacity(), GBL_STRING_BUFFER_BASE_STACK_SIZE);
    QCOMPARE(string.getStackSize(), GBL_STRING_BUFFER_BASE_STACK_SIZE);
    QCOMPARE(string.getLength(), 0);
    QCOMPARE(string.getCString(), "");
    QVERIFY(string == "");
}

inline void String::createLiteral(void) {
    gimbal::String string("Fuck");
    QCOMPARE(string.getContext(), nullptr);
    QVERIFY(!string.isEmpty());
    QVERIFY(string.isStack());
    QVERIFY(!string.isHeap());
    QCOMPARE(string.getCapacity(), GBL_STRING_BUFFER_BASE_STACK_SIZE);
    QCOMPARE(string.getStackSize(), GBL_STRING_BUFFER_BASE_STACK_SIZE);
        QCOMPARE(string.getCString(), "Fuck");
    QCOMPARE(string.getLength(), strlen("Fuck"));
        QVERIFY(string == "Fuck");

}

#undef GBL_STRING_BUFFER_BASE_STACK_SIZE




}

#endif // TEST_GIMBAL_STRING_HPP
