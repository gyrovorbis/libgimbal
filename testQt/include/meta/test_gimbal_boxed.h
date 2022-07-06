#ifndef TEST_GIMBAL_BOXED_H
#define TEST_GIMBAL_BOXED_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/utils/gimbal_test.h>

#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#define GBL_BOXED_TEST_SET(X)                 \
        X(GblBoxed_test_testCaseInit)         \
        X(GblBoxed_test_testCaseFinal)        \
        X(GblBoxed_test_init)                 \
        X(GblBoxed_test_register)             \
        X(GblBoxed_test_variant)              \
        X(GblBoxed_test_conversions)          \
        X(GblBoxed_test_final)

GBL_DECLS_BEGIN
GBL_BOXED_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END


#endif // TEST_GIMBAL_BOXED_H
