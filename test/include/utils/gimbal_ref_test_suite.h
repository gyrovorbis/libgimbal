#ifndef GIMBAL_REF_TEST_SUITE_H
#define GIMBAL_REF_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_REF_TEST_SUITE_TYPE           (GblRefTestSuite_type())
#define GBL_REF_TEST_SUTE_STRUCT          GblRefTestSuite
#define GBL_REF_TEST_SUITE_CLASS_STRUCT   GblRefTestSuiteClass
#define GBL_REF_TEST_SUITE(inst)          (GBL_CAST(GblRefTestSuite, inst))
#define GBL_REF_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblRefTestSuite, klass))
#define GBL_REF_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblRefTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblRefTestSuiteClass {
    GblTestSuiteClass   base;
} GblRefTestSuiteClass;

typedef struct GblRefTestSuite {
    union {
        GblRefTestSuiteClass*    pClass;
        GblTestSuite                  base;
    };
} GblRefTestSuite;

GBL_EXPORT GblType GblRefTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_REF_TEST_SUITE_H
