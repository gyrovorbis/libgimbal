#ifndef GIMBAL_SCANNER_TEST_SUITE_H
#define GIMBAL_SCANNER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_SCANNER_TEST_SUITE_TYPE             (GBL_TYPEID(GblScannerTestSuite))

#define GBL_SCANNER_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_SCANNER_TEST_SUITE_TYPE, GblScannerTestSuite))
#define GBL_SCANNER_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_SCANNER_TEST_SUITE_TYPE, GblScannerTestSuiteClass))
#define GBL_SCANNER_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_SCANNER_TEST_SUITE_TYPE, GblScannerTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblScannerTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblScannerTestSuite, GblTestSuite)

GBL_EXPORT GblType GblScannerTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_SCANNER_TEST_SUITE_H
