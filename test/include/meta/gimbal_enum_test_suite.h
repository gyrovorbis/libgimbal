#ifndef GIMBAL_ENUM_TEST_SUITE_H
#define GIMBAL_ENUM_TEST_SUITE_H

#include <gimbal/utils/gimbal_test_suite.h>

#define GBL_ENUM_TEST_SUITE_TYPE             (GblEnumTestSuite_type())
#define GBL_ENUM_TEST_SUTE_STRUCT            GblEnumTestSuite
#define GBL_ENUM_TEST_SUITE_CLASS_STRUCT     GblEnumTestSuiteClass
#define GBL_ENUM_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_ENUM_TEST_SUITE))
#define GBL_ENUM_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_ENUM_TEST_SUITE))
#define GBL_ENUM_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_ENUM_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblEnumTestSuiteClass {
    GblTestSuiteClass   base;
} GblEnumTestSuiteClass;

typedef struct GblEnumTestSuite {
    union {
        GblEnumTestSuiteClass*      pClass;
        GblTestSuite                base;
    };
} GblEnumTestSuite;

GBL_EXPORT GblType GblEnumTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_ENUM_TEST_SUITE_H
