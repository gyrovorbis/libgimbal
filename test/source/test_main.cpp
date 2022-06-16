#include <elysian_qtest.hpp>
#include <types/test_gimbal_variant.hpp>
#include <objects/test_gimbal_context.hpp>
#include <containers/test_gimbal_linked_list.hpp>
#include <containers/test_gimbal_doubly_linked_list.hpp>
#include <containers/test_gimbal_array_map.hpp>
#include <containers/test_gimbal_vector.hpp>
#include <containers/test_gimbal_tree_set.hpp>
#include <types/test_gimbal_ref.hpp>
#include <containers/test_gimbal_byte_array.h>
#include <types/test_gimbal_closure.hpp>
#include <strings/test_gimbal_string_view.hpp>
#include <strings/test_gimbal_string_ref.hpp>
#include <strings/test_gimbal_quark.hpp>
#include <strings/test_gimbal_string.hpp>
//#include <test_gimbal_meta.hpp>
#include <algorithms/test_gimbal_algorithms.hpp>
#include <test_gimbal_api.hpp>
#include <containers/test_gimbal_hash_map.hpp>
#include <containers/test_gimbal_byte_array.hpp>
#include <meta/test_gimbal_type.hpp>
#include <objects/test_gimbal_object.hpp>
#include <meta/test_gimbal_enum.hpp>
#include <meta/test_gimbal_flags.hpp>
#include <meta/test_gimbal_opaque.hpp>
#include <meta/test_gimbal_boxed.hpp>

using namespace elysian;

int main(int argc, char* argv[]) {
    UnitTestSuite testSuite;
    //auto* pCtx = new gimbal::test::StandardContext;
    //GblContext_globalSet(new gimbal::test::MonitorableContext(pCtx))

qDebug() << "\n";
    testSuite.enqueueTestSet(new gimbal::test::Api);
    testSuite.enqueueTestSet(new gimbal::test::Context());
    testSuite.enqueueTestSet(new gimbal::test::LinkedList());
    testSuite.enqueueTestSet(new gimbal::test::DoublyLinkedList());
    testSuite.enqueueTestSet(new gimbal::test::ArrayMap());
    testSuite.enqueueTestSet(new gimbal::test::Vector());
    testSuite.enqueueTestSet(new gimbal::test::TreeSet());
    testSuite.enqueueTestSet(new gimbal::test::Ref());
    testSuite.enqueueTestSet(new gimbal::test::ByteArray());
    testSuite.enqueueTestSet(new gimbal::test::Closure());
    testSuite.enqueueTestSet(new gimbal::test::StringView());
    testSuite.enqueueTestSet(new gimbal::test::StringRef());
    testSuite.enqueueTestSet(new gimbal::test::String());
 //   testSuite.enqueueTestSet(new gimbal::test::Quark());
    testSuite.enqueueTestSet(new gimbal::test::Algorithms());
    testSuite.enqueueTestSet(new gimbal::test::HashMap());
    //testSuite.enqueueTestSet(new gimbal::test::ByteArray());
    testSuite.enqueueTestSet(new gimbal::test::Type());
    testSuite.enqueueTestSet(new gimbal::test::Variant());
    testSuite.enqueueTestSet(new gimbal::test::Enum());
    testSuite.enqueueTestSet(new gimbal::test::Flags());
    testSuite.enqueueTestSet(new gimbal::test::Opaque());
    testSuite.enqueueTestSet(new gimbal::test::Boxed());
    testSuite.enqueueTestSet(new gimbal::test::CObject());
qDebug() << "DONEZ";
    return !testSuite.exec(argc, argv);
}
