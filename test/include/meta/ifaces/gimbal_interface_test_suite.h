#ifndef GIMBAL_INTERFACE_TEST_SUITE_H
#define GIMBAL_INTERFACE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_INTERFACE_TEST_SUITE_TYPE           (GblInterfaceTestSuite_type())
#define GBL_INTERFACE_TEST_SUTE_STRUCT          GblInterfaceTestSuite
#define GBL_INTERFACE_TEST_SUITE_CLASS_STRUCT   GblInterfaceTestSuiteClass
#define GBL_INTERFACE_TEST_SUITE(inst)          (GBL_CAST(GblInterfaceTestSuite, inst))
#define GBL_INTERFACE_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblInterfaceTestSuite, klass))
#define GBL_INTERFACE_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblInterfaceTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblInterfaceTestSuiteClass {
    GblTestSuiteClass   base;
} GblInterfaceTestSuiteClass;

typedef struct GblInterfaceTestSuite {
    union {
        GblInterfaceTestSuiteClass*     pClass;
        GblTestSuite                base;
    };
} GblInterfaceTestSuite;

GBL_EXPORT GblType GblInterfaceTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_INTERFACE_TEST_SUITE_H
