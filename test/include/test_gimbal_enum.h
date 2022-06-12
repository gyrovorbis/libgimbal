#ifndef TEST_GIMBAL_ENUM_H
#define TEST_GIMBAL_ENUM_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/utils/gimbal_test.h>

#define GBL_ENUM_TEST_SET(X)                 \
        X(GblEnum_test_testCaseInit)         \
        X(GblEnum_test_testCaseFinal)        \
        X(GblEnum_test_init)                 \
        X(GblEnum_test_register)             \
        X(GblEnum_test_class_name)           \
        X(GblEnum_test_class_nick)           \
        X(GblEnum_test_class_value)          \
        X(GblEnum_test_value_from)           \
        X(GblEnum_test_value_to)             \
        X(GblEnum_test_value_check)          \
        X(GblEnum_test_variant)              \
        X(GblEnum_test_variant_fail)         \
        X(GblEnum_test_variant_conversions)  \
        X(GblEnum_test_final)

#if 0
        X(GblEnum_test_variant_construct)    \
        X(GblEnum_test_variant_value_to)     \
        X(GblEnum_test_variant_value_from)   \
        X(GblEnum_test_variant_operators)
#endif

GBL_DECLS_BEGIN
GBL_ENUM_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_ENUM_H
