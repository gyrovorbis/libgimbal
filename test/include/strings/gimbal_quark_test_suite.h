#ifndef GIMBAL_QUARK_TEST_SUITE_H
#define GIMBAL_QUARK_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_QUARK_TEST_SUITE_TYPE           (GblQuarkTestSuite_type())
#define GBL_QUARK_TEST_SUTE_STRUCT          GblQuarkTestSuite
#define GBL_QUARK_TEST_SUITE_CLASS_STRUCT   GblQuarkTestSuiteClass
#define GBL_QUARK_TEST_SUITE(inst)          (GBL_CAST(GblQuarkTestSuite, inst))
#define GBL_QUARK_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblQuarkTestSuite, klass))
#define GBL_QUARK_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblQuarkTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblQuarkTestSuiteClass {
    GblTestSuiteClass   base;
} GblQuarkTestSuiteClass;

typedef struct GblQuarkTestSuite {
    union {
        GblQuarkTestSuiteClass*     pClass;
        GblTestSuite                base;
    };
} GblQuarkTestSuite;

GBL_EXPORT GblType GblQuarkTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_QUARK_TEST_SUITE_H
