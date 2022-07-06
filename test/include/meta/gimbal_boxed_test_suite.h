#ifndef GIMBAL_BOXED_TEST_SUITE_H
#define GIMBAL_BOXED_TEST_SUITE_H

#include <gimbal/utils/gimbal_test_suite.h>

#define GBL_BOXED_TEST_SUITE_TYPE             (GblBoxedTestSuite_type())
#define GBL_BOXED_TEST_SUTE_STRUCT            GblBoxedTestSuite
#define GBL_BOXED_TEST_SUITE_CLASS_STRUCT     GblBoxedTestSuiteClass
#define GBL_BOXED_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_BOXED_TEST_SUITE))
#define GBL_BOXED_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_BOXED_TEST_SUITE))
#define GBL_BOXED_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_BOXED_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblBoxedTestSuiteClass {
    GblTestSuiteClass   base;
} GblBoxedTestSuiteClass;

typedef struct GblBoxedTestSuite {
    union {
        GblBoxedTestSuiteClass*    pClass;
        GblTestSuite               base;
    };
} GblBoxedTestSuite;

GBL_EXPORT GblType GblBoxedTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_BOXED_TEST_SUITE_H
