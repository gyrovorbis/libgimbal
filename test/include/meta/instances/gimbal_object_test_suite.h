#ifndef GIMBAL_OBJECT_TEST_SUITE_H
#define GIMBAL_OBJECT_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_OBJECT_TEST_SUITE_TYPE             (GblObjectTestSuite_type())
#define GBL_OBJECT_TEST_SUTE_STRUCT            GblObjectTestSuite
#define GBL_OBJECT_TEST_SUITE_CLASS_STRUCT     GblObjectTestSuiteClass
#define GBL_OBJECT_TEST_SUITE(inst)            (GBL_CAST_PREFIX(inst, GBL_OBJECT_TEST_SUITE))
#define GBL_OBJECT_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_OBJECT_TEST_SUITE))
#define GBL_OBJECT_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST_PREFIX(inst, GBL_OBJECT_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblObjectTestSuiteClass {
    GblTestSuiteClass   base;
} GblObjectTestSuiteClass;

typedef struct GblObjectTestSuite {
    union {
        GblObjectTestSuiteClass*   pClass;
        GblTestSuite               base;
    };
} GblObjectTestSuite;

GBL_EXPORT GblType GblObjectTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_OBJECT_TEST_SUITE_H
