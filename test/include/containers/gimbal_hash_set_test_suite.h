#ifndef GIMBAL_HASH_SET_TEST_SUITE_H
#define GIMBAL_HASH_SET_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_HASH_SET_TEST_SUITE_TYPE             (GblHashSetTestSuite_type())
#define GBL_HASH_SET_TEST_SUTE_STRUCT            GblHashSetTestSuite
#define GBL_HASH_SET_TEST_SUITE_CLASS_STRUCT     GblHashSetTestSuiteClass
#define GBL_HASH_SET_TEST_SUITE(inst)            (GBL_CAST_PREFIX(inst, GBL_HASH_SET_TEST_SUITE))
#define GBL_HASH_SET_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_HASH_SET_TEST_SUITE))
#define GBL_HASH_SET_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST_PREFIX(inst, GBL_HASH_SET_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblHashSetTestSuiteClass {
    GblTestSuiteClass   base;
} GblHashSetTestSuiteClass;

typedef struct GblHashSetTestSuite {
    union {
        GblHashSetTestSuiteClass*   pClass;
        GblTestSuite                base;
    };
} GblHashSetTestSuite;

GBL_EXPORT GblType GblHashSetTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_HASH_SET_TEST_SUITE_H
