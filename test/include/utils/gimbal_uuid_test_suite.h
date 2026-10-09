#ifndef GIMBAL_UUID_TEST_SUITE_H
#define GIMBAL_UUID_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_UUID_TEST_SUITE_TYPE           (GblUuidTestSuite_type())
#define GBL_UUID_TEST_SUTE_STRUCT          GblUuidTestSuite
#define GBL_UUID_TEST_SUITE_CLASS_STRUCT   GblUuidTestSuiteClass
#define GBL_UUID_TEST_SUITE(inst)          (GBL_CAST(GblUuidTestSuite, inst))
#define GBL_UUID_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblUuidTestSuite, klass))
#define GBL_UUID_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblUuidTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblUuidTestSuiteClass {
    GblTestSuiteClass   base;
} GblUuidTestSuiteClass;

typedef struct GblUuidTestSuite {
    union {
        GblUuidTestSuiteClass*  pClass;
        GblTestSuite            base;
    };
} GblUuidTestSuite;

GBL_EXPORT GblType GblUuidTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_UUID_TEST_SUITE_H
