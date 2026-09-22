#ifndef GIMBAL_ENUM_TEST_SUITE_H
#define GIMBAL_ENUM_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ENUM_TEST_SUITE_TYPE           (GblEnumTestSuite_type())
#define GBL_ENUM_TEST_SUTE_STRUCT          GblEnumTestSuite
#define GBL_ENUM_TEST_SUITE_CLASS_STRUCT   GblEnumTestSuiteClass
#define GBL_ENUM_TEST_SUITE(inst)          (GBL_CAST(GblEnumTestSuite, inst))
#define GBL_ENUM_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblEnumTestSuite, klass))
#define GBL_ENUM_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblEnumTestSuite, inst))

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
