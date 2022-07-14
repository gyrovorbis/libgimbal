#ifndef GBIMAL_INTERFACE_TEST_SUITE_H
#define GBIMAL_INTERFACE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_INTERFACE_TEST_SUITE_TYPE             (GblInterfaceTestSuite_type())
#define GBL_INTERFACE_TEST_SUTE_STRUCT            GblInterfaceTestSuite
#define GBL_INTERFACE_TEST_SUITE_CLASS_STRUCT     GblInterfaceTestSuiteClass
#define GBL_INTERFACE_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_INTERFACE_TEST_SUITE))
#define GBL_INTERFACE_TEST_SUITE_CLASS(klass)     (GBL_INTERFACE_CAST_PREFIX(klass, GBL_INTERFACE_TEST_SUITE))
#define GBL_INTERFACE_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_INTERFACE_TEST_SUITE))

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

#endif // GBIMAL_INTERFACE_TEST_SUITE_H
