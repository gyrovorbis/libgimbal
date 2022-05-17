#ifndef GIMBAL_TEST_H
#define GIMBAL_TEST_H

#define GBL_VERIFY(expr)    GBL_API_VERIFY_EXPRESSION(expr)
#define GBL_COMPARE(l, r)   GBL_API_VERIFY_EXPRESSION((l == r),         \
                                                      "Values differed")
#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#endif // GIMBAL_TEST_H
