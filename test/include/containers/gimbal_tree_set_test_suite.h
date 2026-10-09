#ifndef GIMBAL_TREE_SET_TEST_SUITE_H
#define GIMBAL_TREE_SET_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_TREE_SET_TEST_SUITE_TYPE           (GblTreeSetTestSuite_type())
#define GBL_TREE_SET_TEST_SUTE_STRUCT          GblTreeSetTestSuite
#define GBL_TREE_SET_TEST_SUITE_CLASS_STRUCT   GblTreeSetTestSuiteClass
#define GBL_TREE_SET_TEST_SUITE(inst)          (GBL_CAST(GblTreeSetTestSuite, inst))
#define GBL_TREE_SET_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblTreeSetTestSuite, klass))
#define GBL_TREE_SET_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblTreeSetTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblTreeSetTestSuiteClass {
    GblTestSuiteClass   base;
} GblTreeSetTestSuiteClass;

typedef struct GblTreeSetTestSuite {
    union {
        GblTreeSetTestSuiteClass*    pClass;
        GblTestSuite                  base;
    };
} GblTreeSetTestSuite;

GBL_EXPORT GblType GblTreeSetTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_TREE_SET_TEST_SUITE_H
