#ifndef TEST_GIMBAL_QUARK_H
#define TEST_GIMBAL_QUARK_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/utils/gimbal_test.h>

#define GBL_QUARK_TEST_SET(X)                 \
        X(GblQuark_test_testCaseInit)         \
        X(GblQuark_test_testCaseFinal)        \
        X(GblQuark_test_init)                 \
        X(GblQuark_test_fromStringStatic)     \
        X(GblQuark_test_fromString)           \
        X(GblQuark_test_fromStringSized)      \
        X(GblQuark_test_tryString)            \
        X(GblQuark_test_tryStringSized)       \
        X(GblQuark_test_extraPage)            \
        X(GblQuark_test_toString)             \
        X(GblQuark_test_internString)         \
        X(GblQuark_test_internStringSized)    \
        X(GblQuark_test_internStringStatic)   \
        X(GblQuark_test_final)

GBL_DECLS_BEGIN
GBL_QUARK_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END


#endif // TEST_GIMBAL_QUARK_H
