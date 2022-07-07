#ifndef TEST_GIMBAL_STRING_VIEW_H
#define TEST_GIMBAL_STRING_VIEW_H


#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/test/gimbal_test.h>

#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#define GBL_STRING_VIEW_TEST_SET(X)                 \
        X(GblStringView_test_testCaseInit)          \
        X(GblStringView_test_testCaseFinal)         \
        X(GblStringView_test_fromEmpty)             \
        X(GblStringView_test_fromString)            \
        X(GblStringView_test_fromStringSized)       \
        X(GblStringView_test_fromQuark)             \
        X(GblStringView_test_compare)               \
        X(GblStringView_test_compareIgnoreCase)     \
        X(GblStringView_test_equals)                \
        X(GblStringView_test_equalsIgnoreCase)      \
        X(GblStringView_test_copyInvalid)           \
        X(GblStringView_test_copy)                  \
        X(GblStringView_test_empty)                 \
        X(GblStringView_test_blank)                 \
        X(GblStringView_test_at_invalid)            \
        X(GblStringView_test_at)                    \
        X(GblStringView_test_first_invalid)         \
        X(GblStringView_test_first)                 \
        X(GblStringView_test_last_invalid)          \
        X(GblStringView_test_last)                  \
        X(GblStringView_test_removePrefix_invalid)  \
        X(GblStringView_test_removePrefix)          \
        X(GblStringView_test_removeSuffix_invalid)  \
        X(GblStringView_test_removeSuffix)          \
        X(GblStringView_test_chomp)                 \
        X(GblStringView_test_substr_invalid)        \
        X(GblStringView_test_substr)                \
        X(GblStringView_test_startsWith)            \
        X(GblStringView_test_endsWith)              \
        X(GblStringView_test_contains)              \
        X(GblStringView_test_count)                 \
        X(GblStringView_test_find_invalid)          \
        X(GblStringView_test_find)                  \
        X(GblStringView_test_rfind_invalid)         \
        X(GblStringView_test_rfind)                 \
        X(GblStringView_test_findFirstOf_invalid)   \
        X(GblStringView_test_findFirstOf)           \
        X(GblStringView_test_findFirstNotOf_invalid)\
        X(GblStringView_test_findFirstNotOf)        \
        X(GblStringView_test_findLastOf_invalid)    \
        X(GblStringView_test_findLastOf)            \
        X(GblStringView_test_findLastNotOf_invalid) \
        X(GblStringView_test_findLastNotOf)         \
        X(GblStringView_test_quark)                 \
        X(GblStringView_test_quarkTry)              \
        X(GblStringView_test_intern)                \
        X(GblStringView_test_hash)                  \
        X(GblStringView_test_toCString)             \
        X(GblStringView_test_toNil)                 \
        X(GblStringView_test_toBool)                \
        X(GblStringView_test_toInt)                 \
        X(GblStringView_test_toUint)                \
        X(GblStringView_test_toFloat)               \
        X(GblStringView_test_toDouble)              \
        X(GblStringView_test_toPointer_invalid)     \
        X(GblStringView_test_toPointer)

GBL_DECLS_BEGIN
GBL_STRING_VIEW_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END


#endif // TEST_GIMBAL_STRING_VIEW_H
