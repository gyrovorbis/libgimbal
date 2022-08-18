#include <gimbal/test/gimbal_test_scenario.h>
#include "containers/gimbal_linked_list_test_suite.h"
#include "containers/gimbal_doubly_linked_list_test_suite.h"
#include "containers/gimbal_array_list_test_suite.h"
#include "containers/gimbal_array_map_test_suite.h"
#include "containers/gimbal_tree_set_test_suite.h"
#include "containers/gimbal_hash_set_test_suite.h"
#include "types/gimbal_ref_test_suite.h"
#include "containers/gimbal_byte_array_test_suite.h"
#include "strings/gimbal_quark_test_suite.h"
#include "strings/gimbal_string_view_test_suite.h"
#include "strings/gimbal_string_ref_test_suite.h"
#include "strings/gimbal_string_buffer_test_suite.h"
#include "meta/gimbal_type_test_suite.h"
#include "meta/gimbal_class_test_suite.h"
#include "meta/gimbal_interface_test_suite.h"
#include "meta/gimbal_instance_test_suite.h"
#include "types/gimbal_closure_test_suite.h"
#include "meta/gimbal_signal_test_suite.h"
#include "types/gimbal_variant_test_suite.h"
#include "meta/gimbal_enum_test_suite.h"
#include "meta/gimbal_flags_test_suite.h"
#include "meta/gimbal_opaque_test_suite.h"
#include "meta/gimbal_boxed_test_suite.h"
#include "objects/gimbal_object_test_suite.h"
#include "types/gimbal_uuid_test_suite.h"

#if defined(__DREAMCAST__) && !defined(NDEBUG)
#   include <arch/gdb.h>
#endif

int main(int argc, char* pArgv[]) {
#if defined(__DREAMCAST__) && !defined(NDEBUG)
    gdb_init();
#endif
    GblTestScenario* pScenario = GblTestScenario_create("libGimbalTests");

    GblContext_setLogFilter(GBL_CONTEXT(pScenario), GBL_LOG_LEVEL_INFO | GBL_LOG_LEVEL_WARNING | GBL_LOG_LEVEL_ERROR );

    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_LINKED_LIST_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_DOUBLY_LINKED_LIST_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_ARRAY_LIST_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_ARRAY_MAP_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_TREE_SET_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_HASH_SET_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_REF_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_BYTE_ARRAY_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_UUID_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_QUARK_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_STRING_VIEW_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_STRING_REF_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_STRING_BUFFER_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_TYPE_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_CLASS_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_INTERFACE_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_INSTANCE_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_CLOSURE_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_SIGNAL_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_VARIANT_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_ENUM_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_FLAGS_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_OPAQUE_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_BOXED_TEST_SUITE_TYPE));
    GblTestScenario_suiteEnqueue(pScenario,
                                 GblTestSuite_createFromType(GBL_OBJECT_TEST_SUITE_TYPE));

    GBL_RESULT result = GblTestScenario_run(pScenario, argc, pArgv);

    GblTestScenario_destroy(pScenario);

    return GBL_RESULT_SUCCESS(result)? 0 : -1;
}
