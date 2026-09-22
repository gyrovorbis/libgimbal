#ifndef GIMBAL_POINTER_TEST_SUITE_H
#define GIMBAL_POINTER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_POINTER_TEST_SUITE_TYPE             (GblPointerTestSuite_type())

#define GBL_POINTER_TEST_SUITE(inst)          (GBL_CAST(GblPointerTestSuite, inst))
#define GBL_POINTER_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblPointerTestSuite, klass))
#define GBL_POINTER_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblPointerTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblPointerTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblPointerTestSuite, GblTestSuite)

GBL_EXPORT GblType GblPointerTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_POINTER_TEST_SUITE_H
