#include <elysian_qtest.hpp>
#include <test_gimbal_variant.hpp>
#include <test_gimbal_context.hpp>
#include <test_gimbal_vector.hpp>
#include <test_gimbal_string.hpp>
//#include <test_gimbal_meta.hpp>
#include <test_gimbal_api.hpp>

using namespace elysian;

int main(int argc, char* argv[]) {
    UnitTestSuite testSuite;


qDebug() << "\n";
    testSuite.enqueueTestSet(new gimbal::test::Api);
    testSuite.enqueueTestSet(new gimbal::test::Context());
    testSuite.enqueueTestSet(new gimbal::test::Vector());
    testSuite.enqueueTestSet(new gimbal::test::String());
  //  testSuite.enqueueTestSet(new gimbal::test::MetaType());
    testSuite.enqueueTestSet(new gimbal::test::Variant());
qDebug() << "DONEZ";
    return !testSuite.exec(argc, argv);
}
