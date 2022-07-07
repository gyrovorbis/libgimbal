#ifndef GIMBAL_STRING_VIEW_TEST_SUITE_H
#define GIMBAL_STRING_VIEW_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_STRING_VIEW_TEST_SUITE_TYPE             (GblStringViewTestSuite_type())
#define GBL_STRING_VIEW_TEST_SUTE_STRUCT            GblStringViewTestSuite
#define GBL_STRING_VIEW_TEST_SUITE_CLASS_STRUCT     GblStringViewTestSuiteClass
#define GBL_STRING_VIEW_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_STRING_VIEW_TEST_SUITE))
#define GBL_STRING_VIEW_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_STRING_VIEW_TEST_SUITE))
#define GBL_STRING_VIEW_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_STRING_VIEW_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblStringViewTestSuiteClass {
    GblTestSuiteClass   base;
} GblStringViewTestSuiteClass;

typedef struct GblStringViewTestSuite {
    union {
        GblStringViewTestSuiteClass*     pClass;
        GblTestSuite                base;
    };
} GblStringViewTestSuite;

GBL_EXPORT GblType GblStringViewTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_STRING_VIEW_TEST_SUITE_H
