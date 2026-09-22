#ifndef GIMBAL_SIGNAL_TEST_SUITE_H
#define GIMBAL_SIGNAL_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_SIGNAL_TEST_SUITE_TYPE           (GblSignalTestSuite_type())
#define GBL_SIGNAL_TEST_SUTE_STRUCT          GblSignalTestSuite
#define GBL_SIGNAL_TEST_SUITE_CLASS_STRUCT   GblSignalTestSuiteClass
#define GBL_SIGNAL_TEST_SUITE(inst)          (GBL_CAST(GblSignalTestSuite, inst))
#define GBL_SIGNAL_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblSignalTestSuite, klass))
#define GBL_SIGNAL_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblSignalTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblSignalTestSuiteClass {
    GblTestSuiteClass   base;
} GblSignalTestSuiteClass;

typedef struct GblSignalTestSuite {
    union {
        GblSignalTestSuiteClass*    pClass;
        GblTestSuite                base;
    };
} GblSignalTestSuite;

GBL_EXPORT GblType GblSignalTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_SIGNAL_TEST_SUITE_H
