/*! \file
 *  \brief Unit testing macro utilities
 *  \ingroup testing
 */

#ifndef GIMBAL_TEST_H
#define GIMBAL_TEST_H

#include "../preprocessor/gimbal_macro_utils.h"
#include "gimbal_test_scenario.h"
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
#    define GBL_TEST_CMP_PLATFORM_ENTRIES()      \
        (GblSize, GBL_TEST_COMPARE_CMP_UINT32_), \
        (int,     GBL_TEST_COMPARE_CMP_INT32_),
#elif (defined(__clang__) && defined(__APPLE__) && defined(GBL_64BIT)) || defined(__EMSCRIPTEN__)
#    define GBL_TEST_CMP_PLATFORM_ENTRIES() \
        (GblSize, GBL_TEST_COMPARE_CMP_UINT64_),
#else
#    define GBL_TEST_CMP_PLATFORM_ENTRIES()
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

#define GBL_TEST_CALL                                GBL_CTX_VERIFY_CALL

#define GBL_TEST_SKIP(reason)                        GBL_CTX_VERIFY(GBL_FALSE,                                               \
                                                                    GBL_RESULT_SKIPPED,                                      \
                                                                    reason)

#define GBL_TEST_FAIL(reason)                        GBL_CTX_VERIFY(GBL_FALSE,                                               \
                                                                    GBL_RESULT_ERROR,                                        \
                                                                    reason)

#define GBL_TEST_EXPECT_ERROR()                                                         \
    GBL_STMT_START {                                                                    \
        GBL_CTX_VERIFY(!GBL_CONFIG_ASSERT_ERROR_ENABLED,                                \
                       GBL_RESULT_SKIPPED,                                              \
                       "Skipping test case due to GBL_CONFIG_ASSERT_ERROR_ENABLED.");   \
        GblTestScenario* pScenario = GBL_TEST_SCENARIO(GBL_CTX_CONTEXT());              \
        if(pScenario) GblTestScenario_expectError(pScenario);                           \
    } GBL_STMT_END


#ifndef GBL_TEST_SUITE_SELF_NAME
#   define GBL_TEST_SUITE_SELF_NAME pSelf
#endif

#ifndef GBL_TEST_SUITE_CONTEXT_NAME
#   define GBL_TEST_SUITE_CONTEXT_NAME pCtx
#endif

#ifndef GBL_TEST_SUITE_SELF_PRIVATE_NAME
#   define GBL_TEST_SUITE_SELF_PRIVATE_NAME pFixture
#endif

#ifndef GBL_TEST_SUITE_SELF_PRIVATE
#   define GBL_TEST_SUITE_SELF_PRIVATE  GBL_GLUE(GBL_TEST_SUITE_SELF, _)
#endif

#if 0
#define GBL_TEST_CASE(name, body)                      \
    GBL_TEST_CASE_BEGIN(GBL_TEST_SUITE_SELF, name)     \
        body                                           \
    GBL_TEST_CASE_END
#endif
#define GBL_TEST_CASE(name)                                                         \
    static GBL_RESULT GBL_GLUE(GBL_GLUE(GBL_GLUE(GBL_TEST_SUITE_SELF, _), name), _) \
                               (GblTestSuite* GBL_TEST_SUITE_SELF_NAME,             \
                                GblContext*   GBL_TEST_SUITE_CONTEXT_NAME) {        \
        GBL_CTX_BEGIN(GBL_TEST_SUITE_CONTEXT_NAME);                                 \
        GBL_TEST_SUITE_SELF_PRIVATE* GBL_TEST_SUITE_SELF_PRIVATE_NAME =             \
                (GBL_TEST_SUITE_SELF_PRIVATE*)                                      \
                    GBL_INSTANCE_PRIVATE(pSelf, GBL_TYPEOF(GBL_TEST_SUITE_SELF));   \
        GBL_UNUSED(GBL_TEST_SUITE_SELF_NAME,                                        \
                   GBL_TEST_SUITE_CONTEXT_NAME,                                     \
                   GBL_TEST_SUITE_SELF_PRIVATE_NAME);

#define GBL_TEST_CASE_END   \
    GBL_CTX_END(); }

#define GBL_TEST_FIXTURE \
    GBL_DECLARE_STRUCT(GBL_TEST_SUITE_SELF_PRIVATE)

#define GBL_TEST_REGISTER(...) \
    GBL_EXPORT GblType GBL_GLUE(GBL_GLUE(GBL_TEST_SUITE_SELF, _), type)(void) { \
    static GblType type = GBL_INVALID_TYPE;                                   \
    const static GblTestCase cases[] = { \
        GBL_TUPLE_FOREACH(GBL_TEST_CASE_PAIR, GBL_GLUE(GBL_TEST_SUITE_SELF,_), (__VA_ARGS__)) \
        { NULL, NULL } \
    }; \
    const static GblTestSuiteClassVTable vTable = {         \
        .pFnSuiteInit  = GBL_GLUE(GBL_GLUE(GBL_GLUE(GBL_TEST_SUITE_SELF, _), init), _), \
        .pFnSuiteFinal = GBL_GLUE(GBL_GLUE(GBL_GLUE(GBL_TEST_SUITE_SELF, _), final), _), \
        .pCases        = cases             \
    };  \
    if(type == GBL_INVALID_TYPE) {  \
        GBL_CTX_BEGIN(NULL);    \
        type = GblTestSuite_register(GblQuark_internStringStatic(GBL_STRINGIFY(GBL_TEST_SUITE_SELF)), \
                                     &vTable, \
                                     sizeof(GBL_TEST_SUITE_SELF), \
                                     sizeof(GBL_TEST_SUITE_SELF_PRIVATE), \
                                     GBL_TYPE_FLAGS_NONE); \
        GBL_CTX_VERIFY_LAST_RECORD(); \
        GBL_CTX_END_BLOCK();\
    } \
    return type; \
}

#define GBL_TEST_CASE_PAIR(suiteName, caseName) \
    { GBL_STRINGIFY(caseName), GBL_GLUE(GBL_GLUE(suiteName, caseName), _) },

#define GBL_TEST_INIT()  GBL_TEST_CASE(init)
#define GBL_TEST_FINAL() GBL_TEST_CASE(final)
#define GBL_TEST_NO_INIT \
    GBL_TEST_INIT() \
    GBL_TEST_CASE_END
#define GBL_TEST_NO_FINAL \
    GBL_TEST_FINAL() \
    GBL_TEST_CASE_END
#define GBL_TEST_INIT_END  GBL_TEST_CASE_END
#define GBL_TEST_FINAL_END GBL_TEST_CASE_END

#endif // GIMBAL_TEST_H
