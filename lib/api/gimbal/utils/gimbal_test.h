#ifndef GIMBAL_TEST_H
#define GIMBAL_TEST_H

#define GBL_VERIFY(expr)    GBL_API_VERIFY_EXPRESSION(expr)
#define GBL_COMPARE(l, r)   GBL_API_VERIFY_EXPRESSION((l == r),         \
                                                      "Values differed")
#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);


#define GBL_TEST_EXPECT_ERROR()                         \
    GBL_API_VERIFY(!GBL_CONFIG_ASSERT_ERROR_ENABLED,    \
                   GBL_RESULT_SKIPPED,                  \
                    "Skipping due to assert-on-error enabled")



#endif // GIMBAL_TEST_H
