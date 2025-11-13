/*! \file
 *  \brief Unit testing macro utilities
 *  \ingroup testing
 *
 *  \todo
 *      - need to make an actual exported set of C
 *        functions as the top-level entry-point into
 *        GBL_TEST_COMPARE(), so that language bindings
 *        without macros can use the test framework.
 *
 *  \author Falco Girgis
 */
#ifndef GIMBAL_TEST_MACROS_H
#define GIMBAL_TEST_MACROS_H

#include "../preprocessor/gimbal_macro_utils.h"
#include "gimbal_test_scenario.h"
#include <stdint.h>
#include <inttypes.h>

#define GBL_TEST_VERIFY(expr)                   GBL_CTX_VERIFY_EXPRESSION(expr)

/// \cond
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_DFLT_ (void) { return "Values differed"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_CHAR_ (void) { return "Values differed [actual: %c, expected: %c]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_INT_  (void) { return "Values differed [actual: %d, expected: %d]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_UINT_ (void) { return "Values differed [actual: %u, expected: %u]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_LINT_ (void) { return "Values differed [actual: %" PRId64 ", expected: %" PRId64 "]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_LUINT_(void) { return "Values differed [actual: %" PRIu64 ", expected: %" PRIu64 "]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_STR_  (void) { return "Values differed [actual: %s, expected: %s]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_FLT_  (void) { return "Values differed [actual: %f, expected: %f]"; }
GBL_INLINE const char* GBL_TEST_COMPARE_FMT_PTR_  (void) { return "Values differed [actual: %p, expected: %p]"; }

#ifndef __cplusplus
#   define GBL_TEST_COMPARE_FMT_TABLE_  (                       \
            GBL_TEST_COMPARE_FMT_PTR_,                          \
            (                                                   \
                (char,          GBL_TEST_COMPARE_FMT_CHAR_),    \
                (int16_t,       GBL_TEST_COMPARE_FMT_INT_),     \
                (int32_t,       GBL_TEST_COMPARE_FMT_INT_),     \
                (int64_t,       GBL_TEST_COMPARE_FMT_LINT_),    \
                (uint8_t,       GBL_TEST_COMPARE_FMT_UINT_),    \
                (uint16_t,      GBL_TEST_COMPARE_FMT_UINT_),    \
                (uint32_t,      GBL_TEST_COMPARE_FMT_UINT_),    \
                (uint64_t,      GBL_TEST_COMPARE_FMT_LUINT_),   \
                (const char*,   GBL_TEST_COMPARE_FMT_STR_),     \
                (char*,         GBL_TEST_COMPARE_FMT_STR_),     \
                (float,         GBL_TEST_COMPARE_FMT_FLT_),     \
                (double,        GBL_TEST_COMPARE_FMT_FLT_),     \
                (const void*,   GBL_TEST_COMPARE_FMT_PTR_)      \
            )                                                   \
        )

#   define GBL_TEST_COMPARE_FMT_(value)            GBL_META_GENERIC_MACRO_GENERATE(GBL_TEST_COMPARE_FMT_TABLE_, value)()
#else

inline const char* GBL_TEST_COMPARE_FMT_(char value)      { return GBL_TEST_COMPARE_FMT_CHAR_();  }
inline const char* GBL_TEST_COMPARE_FMT_(int32_t value)   { return GBL_TEST_COMPARE_FMT_INT_();   }
inline const char* GBL_TEST_COMPARE_FMT_(int64_t value)   { return GBL_TEST_COMPARE_FMT_LINT_();  }
inline const char* GBL_TEST_COMPARE_FMT_(uint32_t value)  { return GBL_TEST_COMPARE_FMT_UINT_();  }
inline const char* GBL_TEST_COMPARE_FMT_(uint64_t value)  { return GBL_TEST_COMPARE_FMT_LUINT_(); }
inline const char* GBL_TEST_COMPARE_FMT_(const char* str) { return GBL_TEST_COMPARE_FMT_STR_();   }
inline const char* GBL_TEST_COMPARE_FMT_(double value)    { return GBL_TEST_COMPARE_FMT_FLT_();   }
inline const char* GBL_TEST_COMPARE_FMT_(auto* ptr)       { return GBL_TEST_COMPARE_FMT_PTR_();   }

#endif


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

#ifndef __cplusplus
#if     defined(GBL_DREAMCAST) || defined(GBL_GAMECUBE)
#    define GBL_TEST_CMP_PLATFORM_ENTRIES()      \
        (size_t , GBL_TEST_COMPARE_CMP_UINT32_), \
        (int,     GBL_TEST_COMPARE_CMP_INT32_),
#elif defined(GBL_PSP)
#    define GBL_TEST_CMP_PLATFORM_ENTRIES()        \
        (size_t ,   GBL_TEST_COMPARE_CMP_UINT32_), \
        (int,       GBL_TEST_COMPARE_CMP_INT32_),
#elif (defined(__APPLE__) && defined(GBL_64BIT))
#    define GBL_TEST_CMP_PLATFORM_ENTRIES()      \
        (size_t , GBL_TEST_COMPARE_CMP_UINT64_), \
        (long,    GBL_TEST_COMPARE_CMP_INT64_),
#elif defined(__EMSCRIPTEN__)
#    define GBL_TEST_CMP_PLATFORM_ENTRIES()      \
        (size_t , GBL_TEST_COMPARE_CMP_UINT64_), \
        (int8_t,  GBL_TEST_COMPARE_CMP_UINT32_),
#   else
#       define GBL_TEST_CMP_PLATFORM_ENTRIES()
#   endif

#   define GBL_TEST_COMPARE_CMP_TABLE_  (                       \
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

#   define GBL_TEST_COMPARE_CMP_(actual, expected) GBL_META_GENERIC_MACRO_GENERATE(GBL_TEST_COMPARE_CMP_TABLE_, actual)(actual, expected)

#else
inline GblBool GBL_TEST_COMPARE_CMP_(auto actual, auto expected) { return actual == expected; }
inline GblBool GBL_TEST_COMPARE_CMP_(const char* pActual, const char *pExpected) { return GBL_TEST_COMPARE_CMP_STR_(pActual, pExpected); }

#endif
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

typedef enum GBL_TEST_ERROR_COMPARISON {
    GBL_TEST_ERROR_ABSOLUTE,
    GBL_TEST_ERROR_RELATIVE,
    GBL_TEST_ERROR_FUZZY
} GBL_TEST_ERROR_COMPARISON;

#define GBL_TEST_ERROR(actual, precise, epsilon, cmp)                           \
    GBL_STMT_START {                                                            \
        double relative = gblRelativeError((actual), (precise));                \
        double absolute = gblAbsoluteError((actual), (precise));                \
        if(((cmp) == GBL_TEST_ERROR_ABSOLUTE && absolute > (epsilon)) ||        \
           ((cmp) == GBL_TEST_ERROR_RELATIVE && relative > (epsilon)) ||        \
           (absolute > (epsilon) && relative > (epsilon)))                      \
            GBL_TEST_FAIL("(actual == %.9f) != (expected == %.9f)\n"            \
                          "\t    ERROR: Rel == %.9f, Abs == %.9f, Max == %.9f", \
                          (actual), (precise), relative, absolute, (epsilon));  \
    } GBL_STMT_END


#define GBL_TEST_CALL                                GBL_CTX_VERIFY_CALL

#define GBL_TEST_SKIP(reason)                        GBL_CTX_VERIFY(GBL_FALSE,                                               \
                                                                    GBL_RESULT_SKIPPED,                                      \
                                                                    reason)

#define GBL_TEST_FAIL(...)                           GBL_CTX_VERIFY(GBL_FALSE,                                               \
                                                                    GBL_RESULT_ERROR,                                        \
                                                                    __VA_ARGS__)

#define GBL_TEST_EXPECT_ERROR()                                                         \
    GBL_STMT_START {                                                                    \
        GBL_CTX_VERIFY(!GBL_CONFIG_ASSERT_ERROR_ENABLED,                                \
                       GBL_RESULT_SKIPPED,                                              \
                       "Skipping test case due to GBL_CONFIG_ASSERT_ERROR_ENABLED.");   \
        GblTestScenario* pScenario = GBL_TEST_SCENARIO(GBL_CTX_CONTEXT());              \
        if(pScenario) GblTestScenario_expectError(pScenario);                           \
    } GBL_STMT_END


#ifndef GBL_SELF_TYPE_NAME
#   define GBL_SELF_TYPE_NAME pSelf
#endif

#ifndef GBL_TEST_SUITE_CONTEXT_NAME
#   define GBL_TEST_SUITE_CONTEXT_NAME pCtx
#endif

#ifndef GBL_SELF_TYPE_PRIVATE_NAME
#   define GBL_SELF_TYPE_PRIVATE_NAME pFixture
#endif

#ifndef GBL_SELF_TYPE_PRIVATE
#   define GBL_SELF_TYPE_PRIVATE  GBL_GLUE(GBL_SELF_TYPE, _)
#endif

#define GBL_TEST_CASE(name) \
    static GBL_RESULT GBL_GLUE(GBL_GLUE(GBL_GLUE(GBL_SELF_TYPE, _), name), _) \
                               (GblTestSuite* GBL_SELF_TYPE_NAME, \
                                GblContext*   GBL_TEST_SUITE_CONTEXT_NAME) { \
        GBL_CTX_BEGIN(GBL_TEST_SUITE_CONTEXT_NAME); \
        GBL_SELF_TYPE_PRIVATE* GBL_SELF_TYPE_PRIVATE_NAME = \
                (GBL_SELF_TYPE_PRIVATE*) \
                    GBL_PRIVATE(GBL_SELF_TYPE, pSelf); \
        GBL_UNUSED(GBL_SELF_TYPE_NAME, \
                   GBL_TEST_SUITE_CONTEXT_NAME, \
                   GBL_SELF_TYPE_PRIVATE_NAME);

#define GBL_TEST_CASE_END   \
    GBL_CTX_END(); }

#define GBL_TEST_FIXTURE_TYPE \
    GBL_SELF_TYPE_PRIVATE

#define GBL_TEST_FIXTURE \
    struct GBL_TEST_FIXTURE_TYPE;   \
    typedef struct GBL_TEST_FIXTURE_TYPE GblTestFixture; \
    GBL_DECLARE_STRUCT(GBL_TEST_FIXTURE_TYPE)

#ifdef __cplusplus
#   define GBL_TEST_REGISTER(...) \
        extern "C" GBL_TEST_REGISTER_(__VA_ARGS__)
#else
#   define GBL_TEST_REGISTER(...) \
        GBL_TEST_REGISTER_(__VA_ARGS__)
#endif

#define GBL_TEST_REGISTER_(...) \
    GBL_EXPORT GblType GBL_GLUE(GBL_GLUE(GBL_SELF_TYPE, _), type)(void) GBL_NOEXCEPT { \
    static GblType type = GBL_INVALID_TYPE;                                   \
    const static GblTestCase cases[] = { \
        GBL_TUPLE_FOREACH(GBL_TEST_CASE_PAIR, GBL_GLUE(GBL_SELF_TYPE,_), (__VA_ARGS__)) \
        { NULL, NULL } \
    }; \
    const static GblTestSuiteVTable vTable = {         \
        .pFnSuiteInit  = GBL_GLUE(GBL_GLUE(GBL_GLUE(GBL_SELF_TYPE, _), init), _), \
        .pFnSuiteFinal = GBL_GLUE(GBL_GLUE(GBL_GLUE(GBL_SELF_TYPE, _), final), _), \
        .pCases        = cases             \
    };  \
    if(type == GBL_INVALID_TYPE) {  \
        GBL_CTX_BEGIN(NULL);    \
        type = GblTestSuite_register(GblQuark_internStatic(GBL_STRINGIFY(GBL_SELF_TYPE)), \
                                     &vTable, \
                                     sizeof(GBL_SELF_TYPE), \
                                     sizeof(GBL_SELF_TYPE_PRIVATE), \
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
#define GBL_TEST_INIT_NONE \
    GBL_TEST_INIT() \
    GBL_TEST_CASE_END
#define GBL_TEST_FINAL_NONE \
    GBL_TEST_FINAL() \
    GBL_TEST_CASE_END
#define GBL_TEST_FIXTURE_NONE \
    GBL_TEST_FIXTURE { uintptr_t compat; };
#define GBL_TEST_INIT_END  GBL_TEST_CASE_END
#define GBL_TEST_FINAL_END GBL_TEST_CASE_END

#endif // GIMBAL_TEST_MACROS_H
