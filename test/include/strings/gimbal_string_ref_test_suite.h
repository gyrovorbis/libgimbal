#ifndef GIMBAL_STRING_REF_TEST_SUITE_H
#define GIMBAL_STRING_REF_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_STRING_REF_TEST_SUITE_TYPE           (GblStringRefTestSuite_type())
#define GBL_STRING_REF_TEST_SUTE_STRUCT          GblStringRefTestSuite
#define GBL_STRING_REF_TEST_SUITE_CLASS_STRUCT   GblStringRefTestSuiteClass
#define GBL_STRING_REF_TEST_SUITE(inst)          (GBL_CAST(GblStringRefTestSuite, inst))
#define GBL_STRING_REF_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblStringRefTestSuite, klass))
#define GBL_STRING_REF_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblStringRefTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblStringRefTestSuiteClass {
    GblTestSuiteClass   base;
} GblStringRefTestSuiteClass;

typedef struct GblStringRefTestSuite {
    union {
        GblStringRefTestSuiteClass*     pClass;
        GblTestSuite                base;
    };
} GblStringRefTestSuite;

GBL_EXPORT GblType GblStringRefTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_STRING_REF_TEST_SUITE_H
