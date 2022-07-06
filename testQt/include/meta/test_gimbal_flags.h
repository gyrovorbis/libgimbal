#ifndef TEST_GIMBAL_FLAGS_H
#define TEST_GIMBAL_FLAGS_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/utils/gimbal_test.h>

#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#define GBL_FLAGS_TEST_SET(X)                 \
        X(GblFlags_test_testCaseInit)         \
        X(GblFlags_test_testCaseFinal)        \
        X(GblFlags_test_init)                 \
        X(GblFlags_test_register)             \
        X(GblFlags_test_class_name)           \
        X(GblFlags_test_class_nick)           \
        X(GblFlags_test_class_value)          \
        X(GblFlags_test_class_string)         \
        X(GblFlags_test_value_from)           \
        X(GblFlags_test_value_to)             \
        X(GblFlags_test_value_check)          \
        X(GblFlags_test_value_string)         \
        X(GblFlags_test_variant)              \
        X(GblFlags_test_variant_fail)         \
        X(GblFlags_test_variant_conversions)  \
        X(GblFlags_test_final)

GBL_DECLS_BEGIN
GBL_FLAGS_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_FLAGS_H
