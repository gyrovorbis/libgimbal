#ifndef GIMBAL_ARRAY_MAP_TEST_SUITE_H
#define GIMBAL_ARRAY_MAP_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ARRAY_MAP_TEST_SUITE_TYPE             (GblArrayMapTestSuite_type())
#define GBL_ARRAY_MAP_TEST_SUTE_STRUCT            GblArrayMapTestSuite
#define GBL_ARRAY_MAP_TEST_SUITE_CLASS_STRUCT     GblArrayMapTestSuiteClass
#define GBL_ARRAY_MAP_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_ARRAY_MAP_TEST_SUITE))
#define GBL_ARRAY_MAP_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_ARRAY_MAP_TEST_SUITE))
#define GBL_ARRAY_MAP_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_ARRAY_MAP_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblArrayMapTestSuiteClass {
    GblTestSuiteClass   base;
} GblArrayMapTestSuiteClass;

typedef struct GblArrayMapTestSuite {
    union {
        GblArrayMapTestSuiteClass*    pClass;
        GblTestSuite                  base;
    };
} GblArrayMapTestSuite;

GBL_EXPORT GblType GblArrayMapTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_ARRAY_MAP_TEST_SUITE_H
