#ifndef GIMBAL_QUARK_TEST_SUITE_H
#define GIMBAL_QUARK_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_QUARK_TEST_SUITE_TYPE             (GblQuarkTestSuite_type())
#define GBL_QUARK_TEST_SUTE_STRUCT            GblQuarkTestSuite
#define GBL_QUARK_TEST_SUITE_CLASS_STRUCT     GblQuarkTestSuiteClass
#define GBL_QUARK_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_QUARK_TEST_SUITE))
#define GBL_QUARK_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_QUARK_TEST_SUITE))
#define GBL_QUARK_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_QUARK_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblQuarkTestSuiteClass {
    GblTestSuiteClass   base;
} GblQuarkTestSuiteClass;

typedef struct GblQuarkTestSuite {
    union {
        GblQuarkTestSuiteClass*     pClass;
        GblTestSuite                base;
    };
} GblQuarkTestSuite;

GBL_EXPORT GblType GblQuarkTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_QUARK_TEST_SUITE_H
