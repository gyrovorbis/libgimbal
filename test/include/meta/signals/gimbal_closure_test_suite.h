#ifndef GIMBAL_CLOSURE_TEST_SUITE_H
#define GIMBAL_CLOSURE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_CLOSURE_TEST_SUITE_TYPE           (GblClosureTestSuite_type())
#define GBL_CLOSURE_TEST_SUTE_STRUCT          GblClosureTestSuite
#define GBL_CLOSURE_TEST_SUITE_CLASS_STRUCT   GblClosureTestSuiteClass
#define GBL_CLOSURE_TEST_SUITE(inst)          (GBL_CAST(GblClosureTestSuite, inst))
#define GBL_CLOSURE_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblClosureTestSuite, klass))
#define GBL_CLOSURE_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblClosureTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblClosureTestSuiteClass {
    GblTestSuiteClass   base;
} GblClosureTestSuiteClass;

typedef struct GblClosureTestSuite {
    union {
        GblClosureTestSuiteClass*   pClass;
        GblTestSuite                base;
    };
} GblClosureTestSuite;

GBL_EXPORT GblType GblClosureTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_CLOSURE_TEST_SUITE_H
