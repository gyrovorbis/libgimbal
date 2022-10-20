#include <gimbal/test/gimbal_test_scenario.h>
#include "algorithms/gimbal_sort_test_suite.h"
#include "containers/gimbal_linked_list_test_suite.h"
#include "containers/gimbal_doubly_linked_list_test_suite.h"
#include "containers/gimbal_nary_tree_test_suite.h"
#include "containers/gimbal_array_list_test_suite.h"
#include "containers/gimbal_ring_list_test_suite.h"
#include "containers/gimbal_ring_buffer_test_suite.h"
#include "containers/gimbal_array_map_test_suite.h"
#include "containers/gimbal_tree_set_test_suite.h"
#include "containers/gimbal_hash_set_test_suite.h"
#include "containers/gimbal_array_deque_test_suite.h"
#include "containers/gimbal_array_heap_test_suite.h"
#include "allocators/gimbal_arena_allocator_test_suite.h"
#include "allocators/gimbal_pool_allocator_test_suite.h"
#include "core/gimbal_ref_test_suite.h"
#include "containers/gimbal_byte_array_test_suite.h"
#include "strings/gimbal_quark_test_suite.h"
#include "strings/gimbal_string_view_test_suite.h"
#include "strings/gimbal_string_ref_test_suite.h"
#include "strings/gimbal_string_buffer_test_suite.h"
#include "strings/gimbal_pattern_matcher_test_suite.h"
#include "meta/types/gimbal_type_test_suite.h"
#include "meta/classes/gimbal_class_test_suite.h"
#include "meta/ifaces/gimbal_interface_test_suite.h"
#include "meta/instances/gimbal_instance_test_suite.h"
#include "meta/signals/gimbal_closure_test_suite.h"
#include "meta/signals/gimbal_signal_test_suite.h"
#include "meta/types/gimbal_variant_test_suite.h"
#include "meta/classes/gimbal_enum_test_suite.h"
#include "meta/classes/gimbal_flags_test_suite.h"
#include "meta/classes/gimbal_opaque_test_suite.h"
#include "meta/types/gimbal_pointer_test_suite.h"
#include "meta/instances/gimbal_object_test_suite.h"
#include "utils/gimbal_uuid_test_suite.h"

#include <math.h>

#if defined(__DREAMCAST__) && !defined(NDEBUG)
#   include <arch/gdb.h>
#endif

int main(int argc, char* pArgv[]) {
#if defined(__DREAMCAST__) && !defined(NDEBUG)
    gdb_init();
#endif
    GblTestScenario* pScenario = GblTestScenario_create("LibGimbalTests");

    GblContext_setLogFilter(GBL_CONTEXT(pScenario), GBL_LOG_LEVEL_INFO | GBL_LOG_LEVEL_WARNING | GBL_LOG_LEVEL_ERROR );

    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_SORT_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_LINKED_LIST_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_DOUBLY_LINKED_LIST_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_NARY_TREE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_ARRAY_LIST_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_RING_LIST_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_RING_BUFFER_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_ARRAY_MAP_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_TREE_SET_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_HASH_SET_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_ARRAY_DEQUE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_ARRAY_HEAP_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_ARENA_ALLOCATOR_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_POOL_ALLOCATOR_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_REF_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_BYTE_ARRAY_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_UUID_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_QUARK_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_STRING_VIEW_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_STRING_REF_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_STRING_BUFFER_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_PATTERN_MATCHER_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_TYPE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_CLASS_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_INTERFACE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_INSTANCE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_CLOSURE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_SIGNAL_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_VARIANT_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_ENUM_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_FLAGS_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_POINTER_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_OPAQUE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_createFromType(GBL_OBJECT_TEST_SUITE_TYPE));

    const GBL_RESULT result = GblTestScenario_run(pScenario, argc, pArgv);

    GblTestScenario_destroy(pScenario);

    return GBL_RESULT_SUCCESS(result)? 0 : -1;
}
