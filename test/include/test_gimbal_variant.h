#ifndef TEST_GIMBAL_VARIANT_H
#define TEST_GIMBAL_VARIANT_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/utils/gimbal_test.h>

#define GBL_VARIANT_TEST_SET(X)                 \
        X(GblVariant_test_testCaseInit)         \
        X(GblVariant_test_testCaseFinal)        \
        X(GblVariant_test_init)                 \
        X(GblVariant_test_final)

GBL_DECLS_BEGIN
GBL_VARIANT_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_VARIANT_H
