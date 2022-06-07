#ifndef TEST_GIMBAL_REF_H
#define TEST_GIMBAL_REF_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/utils/gimbal_test.h>

#define GBL_REF_TEST_SET(X)                 \
        X(GblRef_test_testCaseInit)         \
        X(GblRef_test_testCaseFinal)        \
        X(GblRef_test_init)                 \
        X(GblRef_test_allocEmpty)           \
        X(GblRef_test_alloc)                \
        X(GblRef_test_allocWithContext)     \
        X(GblRef_test_acquire)              \
        X(GblRef_test_release)              \
        X(GblRef_test_destructFail)         \
        X(GblRef_test_destruct)             \
        X(GblRef_test_final)

GBL_DECLS_BEGIN
GBL_REF_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_REF_H
