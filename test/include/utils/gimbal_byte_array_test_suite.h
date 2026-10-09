#ifndef GIMBAL_BYTE_ARRAY_TEST_SUITE_H
#define GIMBAL_BYTE_ARRAY_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_BYTE_ARRAY_TEST_SUITE_TYPE           (GblByteArrayTestSuite_type())
#define GBL_BYTE_ARRAY_TEST_SUTE_STRUCT          GblByteArrayTestSuite
#define GBL_BYTE_ARRAY_TEST_SUITE_CLASS_STRUCT   GblByteArrayTestSuiteClass
#define GBL_BYTE_ARRAY_TEST_SUITE(inst)          (GBL_CAST(GblByteArrayTestSuite, inst))
#define GBL_BYTE_ARRAY_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblByteArrayTestSuite, klass))
#define GBL_BYTE_ARRAY_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblByteArrayTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblByteArrayTestSuiteClass {
    GblTestSuiteClass   base;
} GblByteArrayTestSuiteClass;

typedef struct GblByteArrayTestSuite {
    union {
        GblByteArrayTestSuiteClass*    pClass;
        GblTestSuite                  base;
    };
} GblByteArrayTestSuite;

GBL_EXPORT GblType GblByteArrayTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_BYTE_ARRAY_TEST_SUITE_H
