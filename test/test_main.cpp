#include <elysian_qtest.hpp>
#include <variant_tests.hpp>
#include <test_gimbal_context.hpp>
#include <test_gimbal_string.hpp>

using namespace elysian;

int main(int argc, char* argv[]) {
    UnitTestSuite testSuite;
qDebug() << "\n";
    testSuite.enqueueTestSet(new gimbal::test::Context);
    testSuite.enqueueTestSet(new gimbal::test::String);
qDebug() << "DONEZ";
    return !testSuite.exec(argc, argv);
}
