#include <elysian_qtest.hpp>
#include <test_gimbal_variant.hpp>
#include <test_gimbal_context.hpp>
#include <test_gimbal_vector.hpp>
#include <test_gimbal_string.hpp>
//#include <test_gimbal_meta.hpp>
#include <test_gimbal_algorithms.hpp>
#include <test_gimbal_api.hpp>
#include <test_gimbal_hashmap.hpp>
#include <test_gimbal_buffer.hpp>
#include <test_gimbal_type.hpp>

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
    testSuite.enqueueTestSet(new gimbal::test::Algorithms());
    testSuite.enqueueTestSet(new gimbal::test::HashMap());
    testSuite.enqueueTestSet(new gimbal::test::Buffer());
    testSuite.enqueueTestSet(new gimbal::test::Type());
qDebug() << "DONEZ";
    return !testSuite.exec(argc, argv);
}
