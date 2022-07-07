#ifndef TEST_GIMBAL_TREE_SET_H
#define TEST_GIMBAL_TREE_SET_H


#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/test/gimbal_test.h>

#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#define GBL_TREE_SET_TEST_SET(X)                   \
    X(GblTreeSet_test_testCaseInit)                \
    X(GblTreeSet_test_testCaseFinal)               \
    X(GblTreeSet_test_init)                        \
    X(GblTreeSet_test_get_empty_invalid)           \
    X(GblTreeSet_test_get_empty)                   \
    X(GblTreeSet_test_set)                         \
    X(GblTreeSet_test_set_hint)                    \
    X(GblTreeSet_test_extract)                     \
    X(GblTreeSet_test_erase)                       \
    X(GblTreeSet_test_clear)                       \
    X(GblTreeSet_test_final)

GBL_DECLS_BEGIN
GBL_TREE_SET_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_TREE_SET_H
