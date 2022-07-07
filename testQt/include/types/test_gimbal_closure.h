#ifndef TEST_GIMBAL_CLOSURE_H
#define TEST_GIMBAL_CLOSURE_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/test/gimbal_test.h>

#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#define GBL_CLOSURE_TEST_SET(X)         \
    X(GblClosure_test_testCaseInit)     \
    X(GblClosure_test_testCaseFinal)

GBL_DECLS_BEGIN
GBL_CLOSURE_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_CLOSURE_H
