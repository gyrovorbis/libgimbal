#ifndef GIMBAL_OPAQUE_TEST_SUITE_H
#define GIMBAL_OPAQUE_TEST_SUITE_H

#include <gimbal/utils/gimbal_test_suite.h>

#define GBL_OPAQUE_TEST_SUITE_TYPE             (GblOpaqueTestSuite_type())
#define GBL_OPAQUE_TEST_SUTE_STRUCT            GblOpaqueTestSuite
#define GBL_OPAQUE_TEST_SUITE_CLASS_STRUCT     GblOpaqueTestSuiteClass
#define GBL_OPAQUE_TEST_SUITE(inst)            (GBL_INSTANCE_CAST_PREFIX(inst, GBL_OPAQUE_TEST_SUITE))
#define GBL_OPAQUE_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_OPAQUE_TEST_SUITE))
#define GBL_OPAQUE_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST_PREFIX(inst, GBL_OPAQUE_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblOpaqueTestSuiteClass {
    GblTestSuiteClass   base;
} GblOpaqueTestSuiteClass;

typedef struct GblOpaqueTestSuite {
    union {
        GblOpaqueTestSuiteClass*    pClass;
        GblTestSuite                base;
    };
} GblOpaqueTestSuite;

GBL_EXPORT GblType GblOpaqueTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_OPAQUE_TEST_SUITE_H
