#ifndef TEST_GIMBAL_STRING_REF_H
#define TEST_GIMBAL_STRING_REF_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/test/gimbal_test.h>

#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#define GBL_STRING_REF_TEST_SET(X)                        \
        X(GblStringRef_test_testCaseInit)                 \
        X(GblStringRef_test_testCaseFinal)                \
        X(GblStringRef_test_init)                         \
        X(GblStringRef_test_null)                         \
        X(GblStringRef_test_null_at)                      \
        X(GblStringRef_test_createEmpty)                  \
        X(GblStringRef_test_empty_at)                     \
        X(GblStringRef_test_create)                       \
        X(GblStringRef_test_createWithContext)            \
        X(GblStringRef_test_createFromViewEmpty)          \
        X(GblStringRef_test_createFromView)               \
        X(GblStringRef_test_createFromViewWithContext)    \
        X(GblStringRef_test_acquire)                      \
        X(GblStringRef_test_release)                      \
        X(GblStringRef_test_final)

GBL_DECLS_BEGIN
GBL_STRING_REF_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_STRING_REF_H
