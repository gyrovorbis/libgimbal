#ifndef GIMBAL_TYPE_TEST_SUITE_H
#define GIMBAL_TYPE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_TYPE_TEST_SUITE_TYPE             (GblTypeTestSuite_type())
#define GBL_TYPE_TEST_SUTE_STRUCT            GblTypeTestSuite
#define GBL_TYPE_TEST_SUITE_CLASS_STRUCT     GblTypeTestSuiteClass
#define GBL_TYPE_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_TYPE_TEST_SUITE))
#define GBL_TYPE_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_TYPE_TEST_SUITE))
#define GBL_TYPE_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_TYPE_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblTypeTestSuiteClass {
    GblTestSuiteClass   base;
} GblTypeTestSuiteClass;

typedef struct GblTypeTestSuite {
    union {
        GblTypeTestSuiteClass*  pClass;
        GblTestSuite            base;
    };
} GblTypeTestSuite;

GBL_EXPORT GblType GblTypeTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_TYPE_TEST_SUITE_H
