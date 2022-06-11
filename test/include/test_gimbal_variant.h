#ifndef TEST_GIMBAL_VARIANT_H
#define TEST_GIMBAL_VARIANT_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/utils/gimbal_test.h>

#define GBL_VARIANT_TEST_SET(X)                 \
        X(GblVariant_test_testCaseInit)         \
        X(GblVariant_test_testCaseFinal)        \
        X(GblVariant_test_init)                 \
        X(GblVariant_test_nil)                  \
        X(GblVariant_test_nil_invalid)          \
        X(GblVariant_test_nil_conversions)      \
        X(GblVariant_test_bool)                 \
        X(GblVariant_test_bool_conversions)     \
        X(GblVariant_test_char)                 \
        X(GblVariant_test_char_conversions)     \
        X(GblVariant_test_u8)                   \
        X(GblVariant_test_u8_conversions)       \
        X(GblVariant_test_u16)                  \
        X(GblVariant_test_u16_conversions)      \
        X(GblVariant_test_i16)                  \
        X(GblVariant_test_i16_conversions)      \
        X(GblVariant_test_u32)                  \
        X(GblVariant_test_u32_conversions)      \
        X(GblVariant_test_i32)                  \
        X(GblVariant_test_i32_conversions)      \
        X(GblVariant_test_u64)                  \
        X(GblVariant_test_u64_conversions)      \
        X(GblVariant_test_i64)                  \
        X(GblVariant_test_i64_conversions)      \
        X(GblVariant_test_f32)                  \
        X(GblVariant_test_f32_conversions)      \
        X(GblVariant_test_f64)                  \
        X(GblVariant_test_f64_conversions)      \
        X(GblVariant_test_ptr)                  \
        X(GblVariant_test_ptr_conversions)      \
        X(GblVariant_test_string)               \
        X(GblVariant_test_string_conversions)   \
        X(GblVariant_test_type)                 \
        X(GblVariant_test_type_conversions)     \
        X(GblVariant_test_final)

GBL_DECLS_BEGIN
GBL_VARIANT_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_VARIANT_H
