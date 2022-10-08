/*! \file
 *  \brief Unit testing macro utilities
 *  \ingroup testing
 */

#ifndef GIMBAL_TEST_H
#define GIMBAL_TEST_H

#include "../preprocessor/gimbal_macro_utils.h"
#include <stdint.h>

#define GBL_TEST_VERIFY(expr)                        GBL_CTX_VERIFY_EXPRESSION(expr)

/// \cond
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_DFLT_(void) { return "Values differed"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_CHAR_(void) { return "Values differed [actual: %c, expected: %c]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_INT_ (void) { return "Values differed [actual: %d, expected: %d]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_UINT_(void) { return "Values differed [actual: %u, expected: %u]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_STR_ (void) { return "Values differed [actual: %s, expected: %s]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_FLT_ (void) { return "Values differed [actual: %f, expected: %f]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_PTR_ (void) { return "Values differed [actual: %p, expected: %p]"; }

#define GBL_TEST_COMPARE_FMT_TABLE_  (                      \
        GBL_TEST_COMPARE_FMT_DFLT_,                         \
        (                                                   \
            (char,          GBL_TEST_COMPARE_FMT_CHAR_),    \
            (int16_t,       GBL_TEST_COMPARE_FMT_INT_),     \
            (int32_t,       GBL_TEST_COMPARE_FMT_INT_),     \
            (int64_t,       GBL_TEST_COMPARE_FMT_INT_),     \
            (uint8_t,       GBL_TEST_COMPARE_FMT_UINT_),    \
            (uint16_t,      GBL_TEST_COMPARE_FMT_UINT_),    \
            (uint32_t,      GBL_TEST_COMPARE_FMT_UINT_),    \
            (uint64_t,      GBL_TEST_COMPARE_FMT_UINT_),    \
            (const char*,   GBL_TEST_COMPARE_FMT_STR_),     \
            (char*,         GBL_TEST_COMPARE_FMT_STR_),     \
            (float,         GBL_TEST_COMPARE_FMT_FLT_),     \
            (double,        GBL_TEST_COMPARE_FMT_FLT_),     \
            (const void*,   GBL_TEST_COMPARE_FMT_PTR_)      \
        )                                                   \
    )
#define GBL_TEST_COMPARE_FMT_(value) GBL_META_GENERIC_MACRO_GENERATE(GBL_TEST_COMPARE_FMT_TABLE_, value)()

GBL_INLINE GblBool GBL_TEST_COMPARE_CMP_UINTPTR_(uintptr_t actual, uintptr_t expected)       { return actual == expected; }
GBL_INLINE GblBool GBL_TEST_COMPARE_CMP_PTR_    (const void* pActual, const void* pExpected) { return pActual == pExpected; }
GBL_INLINE GblBool GBL_TEST_COMPARE_CMP_UINT32_ (uint32_t actual, uint32_t expected)         { return actual == expected; }
GBL_INLINE GblBool GBL_TEST_COMPARE_CMP_UINT64_ (uint64_t actual, uint64_t expected)         { return actual == expected; }
GBL_INLINE GblBool GBL_TEST_COMPARE_CMP_INT32_  (int32_t actual, int32_t expected)           { return actual == expected; }
GBL_INLINE GblBool GBL_TEST_COMPARE_CMP_INT64_  (int64_t actual, int64_t expected)           { return actual == expected; }
GBL_INLINE GblBool GBL_TEST_COMPARE_CMP_DBL_    (double actual, double expected)             { return actual == expected; }
GBL_INLINE GblBool GBL_TEST_COMPARE_CMP_STR_    (const char* pActual, const char* pExpected) { return pActual == pExpected ||
                                                                                                     (pActual && pExpected && \
                                                                                                     strcmp(pActual, pExpected) == 0); }

#ifdef __DREAMCAST__
#   define GBL_TEST_CMP_PLATFORM_ENTRIES()       \
        (GblSize, GBL_TEST_COMPARE_CMP_UINT32_), \
        (int,     GBL_TEST_COMPARE_CMP_INT32_),
# else
#   define GBL_TEST_CMP_PLATFORM_ENTRIES()
#endif

#define GBL_TEST_COMPARE_CMP_TABLE_  (                      \
        GBL_TEST_COMPARE_CMP_PTR_,                          \
        (                                                   \
            (char,          GBL_TEST_COMPARE_CMP_INT32_),   \
            (int16_t,       GBL_TEST_COMPARE_CMP_INT32_),   \
            (int32_t,       GBL_TEST_COMPARE_CMP_INT32_),   \
            (int64_t,       GBL_TEST_COMPARE_CMP_INT64_),   \
            (uint8_t,       GBL_TEST_COMPARE_CMP_UINT32_),  \
            (uint16_t,      GBL_TEST_COMPARE_CMP_UINT32_),  \
            (uint32_t,      GBL_TEST_COMPARE_CMP_UINT32_),  \
            (uint64_t,      GBL_TEST_COMPARE_CMP_UINT64_),  \
            GBL_TEST_CMP_PLATFORM_ENTRIES()                 \
            (const char*,   GBL_TEST_COMPARE_CMP_STR_),     \
            (char*,         GBL_TEST_COMPARE_CMP_STR_),     \
            (float,         GBL_TEST_COMPARE_CMP_DBL_),     \
            (double,        GBL_TEST_COMPARE_CMP_DBL_),     \
            (const void*,   GBL_TEST_COMPARE_CMP_PTR_)      \
        )                                                   \
    )
#define GBL_TEST_COMPARE_CMP_(actual, expected)      GBL_META_GENERIC_MACRO_GENERATE(GBL_TEST_COMPARE_CMP_TABLE_, actual)(actual, expected)
/// \endcond

#define GBL_TEST_COMPARE(actual, expected)           GBL_CTX_VERIFY_EXPRESSION(GBL_TEST_COMPARE_CMP_(actual, expected),      \
                                                                               GBL_TEST_COMPARE_FMT_(actual),                \
                                                                               actual, expected)

#define GBL_TEST_COMPARE_INT(actual, expected)       GBL_CTX_VERIFY_EXPRESSION(actual == expected,                           \
                                                                          "Values differed [expected: %d, actual: %d]",      \
                                                                          actual, expected)

#define GBL_TEST_COMPARE_UINT(actual, expected)      GBL_CTX_VERIFY_EXPRESSION(actual == expected,                           \
                                                                          "Values differed [expected: %u, actual: %u]",      \
                                                                          actual, expected)

#define GBL_TEST_COMPARE_STRING(actual, expected)    GBL_CTX_VERIFY_EXPRESSION(strcmp(actual, expected) == 0,                \
                                                                          "Values differed [expected: %s, actual: %s]",      \
                                                                          actual, expected)

#define GBL_TEST_COMPARE_FLOAT(actual, expected)     GBL_CTX_VERIFY_EXPRESSION(actual == expected,                           \
                                                                          "Values differed [expected: %f, actual: %f]",      \
                                                                          actual, expected)

#define GBL_TEST_COMPARE_POINTER(actual, expected)   GBL_CTX_VERIFY_EXPRESSION(actual == expected,                           \
                                                                          "Values differed [exptected: %p, actual: %p]",     \
                                                                          actual, expected)


#define GBL_TEST_SKIP(reason)                        GBL_CTX_VERIFY(GBL_FALSE,                                               \
                                                                    GBL_RESULT_SKIPPED,                                      \
                                                                    reason)

#define GBL_TEST_FAIL(reason)                        GBL_CTX_VERIFY(GBL_FALSE,                                               \
                                                                    GBL_RESULT_ERROR,                                        \
                                                                    reason)

#define GBL_TEST_EXPECT_ERROR()                         \
    GBL_CTX_VERIFY(!GBL_CONFIG_ASSERT_ERROR_ENABLED,    \
                   GBL_RESULT_SKIPPED,                  \
                   "Skipping test case due to GBL_CONFIG_ASSERT_ERROR_ENABLED.")



#endif // GIMBAL_TEST_H
