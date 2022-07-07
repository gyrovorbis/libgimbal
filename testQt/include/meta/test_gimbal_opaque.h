#ifndef TEST_GIMBAL_OPAQUE_H
#define TEST_GIMBAL_OPAQUE_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/test/gimbal_test.h>

#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#define GBL_OPAQUE_TEST_SET(X)                 \
        X(GblOpaque_test_testCaseInit)         \
        X(GblOpaque_test_testCaseFinal)        \
        X(GblOpaque_test_init)                 \
        X(GblOpaque_test_register)             \
        X(GblOpaque_test_variant)              \
        X(GblOpaque_test_conversions)          \
        X(GblOpaque_test_final)

GBL_DECLS_BEGIN
GBL_OPAQUE_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_OPAQUE_H
