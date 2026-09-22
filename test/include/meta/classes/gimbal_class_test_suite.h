#ifndef GIMBAL_CLASS_TEST_SUITE_H
#define GIMBAL_CLASS_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_CLASS_TEST_SUITE_TYPE           (GblClassTestSuite_type())
#define GBL_CLASS_TEST_SUTE_STRUCT          GblClassTestSuite
#define GBL_CLASS_TEST_SUITE_CLASS_STRUCT   GblClassTestSuiteClass
#define GBL_CLASS_TEST_SUITE(inst)          (GBL_CAST(GblClassTestSuite, inst))
#define GBL_CLASS_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblClassTestSuite, klass))
#define GBL_CLASS_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblClassTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblClassTestSuiteClass {
    GblTestSuiteClass   base;
} GblClassTestSuiteClass;

typedef struct GblClassTestSuite {
    union {
        GblClassTestSuiteClass*     pClass;
        GblTestSuite                base;
    };
} GblClassTestSuite;

GBL_EXPORT GblType GblClassTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_CLASS_TEST_SUITE_H
