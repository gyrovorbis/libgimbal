#include <elysian_qtest.hpp>
#include <test_gimbal_variant.hpp>
#include <test_gimbal_context.hpp>
#include <test_gimbal_vector.hpp>
#include <test_gimbal_ref.hpp>
#include <test_gimbal_closure.hpp>
#include <test_gimbal_string_view.hpp>
#include <test_gimbal_string_ref.hpp>
#include <test_gimbal_quark.hpp>
#include <test_gimbal_string.hpp>
//#include <test_gimbal_meta.hpp>
#include <test_gimbal_algorithms.hpp>
#include <test_gimbal_api.hpp>
#include <test_gimbal_hash_map.hpp>
#include <test_gimbal_buffer.hpp>
#include <test_gimbal_byte_array.hpp>
#include <test_gimbal_type.hpp>
#include <test_gimbal_object.hpp>

using namespace elysian;

int main(int argc, char* argv[]) {
    UnitTestSuite testSuite;
    //auto* pCtx = new gimbal::test::StandardContext;
    //GblContext_globalSet(new gimbal::test::MonitorableContext(pCtx));



qDebug() << "\n";
    testSuite.enqueueTestSet(new gimbal::test::Api);
    testSuite.enqueueTestSet(new gimbal::test::Context());
    testSuite.enqueueTestSet(new gimbal::test::Vector());
    testSuite.enqueueTestSet(new gimbal::test::Ref());
    testSuite.enqueueTestSet(new gimbal::test::Closure());
    testSuite.enqueueTestSet(new gimbal::test::StringView());
    testSuite.enqueueTestSet(new gimbal::test::StringRef());
    testSuite.enqueueTestSet(new gimbal::test::String());
 //   testSuite.enqueueTestSet(new gimbal::test::Quark());
    testSuite.enqueueTestSet(new gimbal::test::Variant());
    testSuite.enqueueTestSet(new gimbal::test::Algorithms());
    testSuite.enqueueTestSet(new gimbal::test::HashMap());
    testSuite.enqueueTestSet(new gimbal::test::Buffer());
    //testSuite.enqueueTestSet(new gimbal::test::ByteArray());
    testSuite.enqueueTestSet(new gimbal::test::Type());
    testSuite.enqueueTestSet(new gimbal::test::CObject());
qDebug() << "DONEZ";
    return !testSuite.exec(argc, argv);
}
