#ifndef GIMBAL_INSTANCE_TEST_SUITE_H
#define GIMBAL_INSTANCE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_INSTANCE_TEST_SUITE_TYPE             (GblInstanceTestSuite_type())
#define GBL_INSTANCE_TEST_SUTE_STRUCT            GblInstanceTestSuite
#define GBL_INSTANCE_TEST_SUITE_CLASS_STRUCT     GblInstanceTestSuiteClass
#define GBL_INSTANCE_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_INSTANCE_TEST_SUITE))
#define GBL_INSTANCE_TEST_SUITE_CLASS(klass)     (GBL_INSTANCE_CAST_PREFIX(klass, GBL_INSTANCE_TEST_SUITE))
#define GBL_INSTANCE_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_INSTANCE_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblInstanceTestSuiteClass {
    GblTestSuiteClass   base;
} GblInstanceTestSuiteClass;

typedef struct GblInstanceTestSuite {
    union {
        GblInstanceTestSuiteClass*     pClass;
        GblTestSuite                base;
    };
} GblInstanceTestSuite;

GBL_EXPORT GblType GblInstanceTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_INSTANCE_TEST_SUITE_H
