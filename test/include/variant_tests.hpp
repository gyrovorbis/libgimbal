#ifndef VARIANT_TESTS_HPP
#define VARIANT_TESTS_HPP

#include <QtTest>
#include <gimbal/gimbal_variant.hpp>


class VariantTestSet: public QObject {
public slots:
    void test1();
    void test2();
    void test3();
};

inline void VariantTestSet::test1() {
    QVERIFY(true == true);
}

inline void VariantTestSet::test2() {
    QCOMPARE(QString("hello").toUpper(),
             QString("HELLO"));
}

inline void VariantTestSet::test3() {
    QEXPECT_FAIL("", "Example of making the next shit fail acceptably", Continue);
    QCOMPARE(1, -1);
}



#endif // VARIANT_TESTS_HPP
