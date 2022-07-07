#ifndef GIMBAL_VECTOR_TEST_SUITE_H
#define GIMBAL_VECTOR_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_VECTOR_TEST_SUITE_TYPE             (GblVectorTestSuite_type())
#define GBL_VECTOR_TEST_SUTE_STRUCT            GblVectorTestSuite
#define GBL_VECTOR_TEST_SUITE_CLASS_STRUCT     GblVectorTestSuiteClass
#define GBL_VECTOR_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_VECTOR_TEST_SUITE))
#define GBL_VECTOR_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_VECTOR_TEST_SUITE))
#define GBL_VECTOR_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_VECTOR_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblVectorTestSuiteClass {
    GblTestSuiteClass   base;
} GblVectorTestSuiteClass;

typedef struct GblVectorTestSuite {
    union {
        GblVectorTestSuiteClass*   pClass;
        GblTestSuite               base;
    };
} GblVectorTestSuite;

GBL_EXPORT GblType GblVectorTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_VECTOR_TEST_SUITE_H
