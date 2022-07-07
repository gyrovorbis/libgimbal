#ifndef GIMBAL_FLAGS_TEST_SUITE_H
#define GIMBAL_FLAGS_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_FLAGS_TEST_SUITE_TYPE             (GblFlagsTestSuite_type())
#define GBL_FLAGS_TEST_SUTE_STRUCT            GblFlagsTestSuite
#define GBL_FLAGS_TEST_SUITE_CLASS_STRUCT     GblFlagsTestSuiteClass
#define GBL_FLAGS_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_FLAGS_TEST_SUITE))
#define GBL_FLAGS_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_FLAGS_TEST_SUITE))
#define GBL_FLAGS_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_FLAGS_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblFlagsTestSuiteClass {
    GblTestSuiteClass   base;
} GblFlagsTestSuiteClass;

typedef struct GblFlagsTestSuite {
    union {
        GblFlagsTestSuiteClass*     pClass;
        GblTestSuite                base;
    };
} GblFlagsTestSuite;

GBL_EXPORT GblType GblFlagsTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_FLAGS_TEST_SUITE_H
