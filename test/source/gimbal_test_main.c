#include <gimbal/test/gimbal_test_scenario.h>
#include "algorithms/gimbal_sort_test_suite.h"
#include "algorithms/gimbal_hash_test_suite.h"
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
#include "allocators/gimbal_scope_allocator_test_suite.h"
#include "utils/gimbal_ref_test_suite.h"
#include "utils/gimbal_byte_array_test_suite.h"
#include "strings/gimbal_quark_test_suite.h"
#include "strings/gimbal_string_view_test_suite.h"
#include "strings/gimbal_string_ref_test_suite.h"
#include "strings/gimbal_string_buffer_test_suite.h"
#include "strings/gimbal_string_list_test_suite.h"
#include "strings/gimbal_pattern_test_suite.h"
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
#include "meta/ifaces/gimbal_itable_variant_test_suite.h"
#include "utils/gimbal_uuid_test_suite.h"
#include "utils/gimbal_version_test_suite.h"
#include "utils/gimbal_option_group_test_suite.h"
#include "utils/gimbal_cmd_parser_test_suite.h"
#include "utils/gimbal_date_time_test_suite.h"
#include "utils/gimbal_bit_view_test_suite.h"
#include "core/gimbal_module_test_suite.h"
#include "core/gimbal_thread_test_suite.h"
#include "utils/gimbal_scanner_test_suite.h"

#include <math.h>

#if defined(__DREAMCAST__) && !defined(NDEBUG)
#   include <arch/gdb.h>
#endif

#ifdef __ANDROID__
#include <android/log.h>
static int pfd[2];
static pthread_t thr;
static const char *tag = "myapp";

static void *thread_func(void*)
{
    ssize_t rdsz;
    char buf[128];
    while((rdsz = read(pfd[0], buf, sizeof buf - 1)) > 0) {
        if(buf[rdsz - 1] == '\n') --rdsz;
        buf[rdsz] = 0;  /* add null-terminator */
        __android_log_write(ANDROID_LOG_VERBOSE, "", buf);
    }
    return 0;
}

int start_logger(const char *app_name)
{
    tag = app_name;

    /* make stdout line-buffered and stderr unbuffered */
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);

    /* create the pipe and redirect stdout and stderr */
    pipe(pfd);
    dup2(pfd[1], 1);
    dup2(pfd[1], 2);

    /* spawn the logging thread */
    if(pthread_create(&thr, 0, thread_func, 0) == -1)
        return -1;
    pthread_detach(thr);
    return 0;
}


#endif

#ifdef __DREAMCAST__
#include <arch/arch.h>

//    assert_msg(false, "exec call failed");

    KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);

    /* This function will override the default stack protector handler that is
       defined in Newlib. This is not necessary to enable the stack protector,
       but is nice for being able to draw the error message to the screen or
       whatnot (not that we do any of that here). */
    void __stack_chk_fail(void) {
        unsigned int pr = (unsigned int)arch_get_ret_addr();
        GBL_CTX_ERROR("Stack smashed: [%x], aborting...");
        exit(-1);
    }

#endif

int main(int argc, char* pArgv[]) {
#if defined(__DREAMCAST__) && !defined(NDEBUG)
    gdb_init();
#elif defined(__ANDROID__)
    start_logger("");
#endif
    GblTestScenario* pScenario = GblTestScenario_create("libGimbalTests");

    GblContext_setLogFilter(GBL_CONTEXT(pScenario), GBL_LOG_LEVEL_INFO    |
                                                    GBL_LOG_LEVEL_WARNING |
                                                    GBL_LOG_LEVEL_ERROR);

    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_SORT_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_HASH_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_LINKED_LIST_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_DOUBLY_LINKED_LIST_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_NARY_TREE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_ARRAY_LIST_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_RING_LIST_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_RING_BUFFER_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_ARRAY_MAP_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_TREE_SET_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_HASH_SET_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_ARRAY_DEQUE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_ARRAY_HEAP_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_ARENA_ALLOCATOR_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_POOL_ALLOCATOR_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_SCOPE_ALLOCATOR_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_REF_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_BYTE_ARRAY_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_UUID_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_QUARK_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_STRING_VIEW_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_STRING_REF_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_STRING_BUFFER_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_PATTERN_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_STRING_LIST_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_TYPE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_CLASS_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_INTERFACE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_INSTANCE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_CLOSURE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_SIGNAL_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_VARIANT_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_ENUM_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_FLAGS_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_POINTER_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_OPAQUE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_OBJECT_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_VERSION_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_OPTION_GROUP_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_CMD_PARSER_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_DATE_TIME_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_BIT_VIEW_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_SCANNER_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_MODULE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_THREAD_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GBL_ITABLE_VARIANT_TEST_SUITE_TYPE));

    const GBL_RESULT result = GblTestScenario_run(pScenario, argc, pArgv);

    GblTestScenario_unref(pScenario);

    return GBL_RESULT_SUCCESS(result)? EXIT_SUCCESS : EXIT_FAILURE;
}
