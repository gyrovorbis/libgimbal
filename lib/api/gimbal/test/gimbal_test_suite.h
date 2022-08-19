/*! \file
 *  \brief GblTestSuite structure and related functions
 *  \ingroup testing
 */

#ifndef GIMBAL_TEST_SUITE_H
#define GIMBAL_TEST_SUITE_H

#include "../meta/instances/gimbal_object.h"

#define GBL_TEST_SUITE_TYPE                (GblTestSuite_type())
#define GBL_TEST_SUITE_STRUCT              GblTestSuite
#define GBL_TEST_SUITE_CLASS_STRUCT        GblTestSuiteClass

#define GBL_TEST_SUITE(inst)               (GBL_INSTANCE_CAST_PREFIX(inst, GBL_TEST_SUITE))
#define GBL_TEST_SUITE_CHECK(inst)         (GBL_INSTANCE_CHECK_PREFIX(inst, GBL_TEST_SUITE))
#define GBL_TEST_SUITE_TRY(inst)           (GBL_INSTANCE_TRY_PREFIX(inst, GBL_TEST_SUITE))

#define GBL_TEST_SUITE_CLASS(klass)        (GBL_CLASS_CAST_PREFIX(klass, GBL_TEST_SUITE))
#define GBL_TEST_SUITE_CLASS_CHECK(klass)  (GBL_CLASS_CHECK_PREFIX(klass, GBL_TEST_SUITE))
#define GBL_TEST_SUITE_CLASS_TRY(klass)    (GBL_CLASS_TRY_PREFIX(klass, GBL_TEST_SUITE))

#define GBL_TEST_SUITE_GET_CLASS(inst)     (GBL_INSTANCE_GET_CLASS_PREFIX(inst, GBL_TEST_SUITE))
#define GBL_TEST_SUITE_TRY_CLASS(inst)     (GBL_INSTANCE_TRY_CLASS_PREFIX(inst, GBL_TEST_SUITE))

#define GBL_TEST_CASE_LAST  { GBL_NULL, GBL_NULL }

#define SELF    GblTestSuite* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblContext);
GBL_FORWARD_DECLARE_STRUCT(GblTestSuite);
GBL_FORWARD_DECLARE_STRUCT(GblTestScenario);

typedef GBL_RESULT (*GblTestCaseRunFn)(SELF, GblContext* pCtx);

typedef struct GblTestCase {
    const char*      pName;
    GblTestCaseRunFn pFnRun;
} GblTestCase;

typedef struct GblTestSuiteClassVTable {
    GBL_RESULT          (*pFnSuiteInit) (SELF, GblContext* pCtx);
    GBL_RESULT          (*pFnSuiteFinal)(SELF, GblContext* pCtx);
    GBL_RESULT          (*pFnCaseInit)  (SELF, GblContext* pCtx);
    GBL_RESULT          (*pFnCaseFinal) (SELF, GblContext* pCtx);
    const GblTestCase*  pCases;
} GblTestSuiteClassVTable;

GBL_CLASS_DERIVE(GblTestSuiteClass, GblObjectClass)
    const GblTestSuiteClassVTable*  pVTable;

    GBL_RESULT                      (*pFnSuiteName) (CSELF, const char** ppName);
    GBL_RESULT                      (*pFnCaseCount) (CSELF, GblSize* pSize);
    GBL_RESULT                      (*pFnCaseName)  (CSELF, GblSize index, const char** ppName);
    GBL_RESULT                      (*pFnCaseRun)   (SELF, GblContext* pCtx, GblSize index);
GBL_CLASS_END

/*! \brief GblObject representing a collection of unit test cases
 *  \ingroup testing
 */
GBL_INSTANCE_DERIVE(GblTestSuite, GblObject, GblTestSuiteClass)
    GblCallRecord   failingIssue;
    GblSize         casesRun;
    GblSize         casesPassed;
    GblSize         casesFailed;
    GblSize         casesSkipped;
GBL_INSTANCE_END

typedef enum GBL_TEST_SUITE_PROPERTY_ID {
    GBL_TEST_SUITE_PROPERTY_ID_FIRST        = GBL_OBJECT_PROPERTY_ID_COUNT,
    GBL_TEST_SUITE_PROPERTY_ID_RESULT       = GBL_TEST_SUITE_PROPERTY_ID_FIRST,
    GBL_TEST_SUITE_PROPERTY_ID_FAILURE_MSG,
    GBL_TEST_SUITE_PROPERTY_ID_CASE_COUNT,
    GBL_TEST_SUITE_PROPERTY_ID_CASES_RUN,
    GBL_TEST_SUITE_PROPERTY_ID_CASES_PASSED,
    GBL_TEST_SUITE_PROPERTY_ID_CASES_FAILED,
    GBL_TEST_SUITE_PROPERTY_ID_CASES_SKIPPED,
    GBL_TEST_SUITE_PROPERTY_ID_COUNT
} GBL_TEST_SUITE_PROPERTY_ID;

GBL_PROPERTY_TABLE_BEGIN(GBL_TEST_SUITE)
    GBL_PROPERTY_ENTRY("result",
                       "Result Code",
                       "Result code returned from test suite run",
                       GBL_TEST_SUITE_PROPERTY_ID_RESULT,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("failureMessage",
                       "Failure Message",
                       "Error message returned from failing test cases (if any)",
                       GBL_TEST_SUITE_PROPERTY_ID_FAILURE_MSG,
                       GBL_STRING_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("caseCount",
                       "Case Count",
                       "Number of test cases included within suite",
                       GBL_TEST_SUITE_PROPERTY_ID_CASE_COUNT,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("casesRun",
                       "Cases Run",
                       "Total number of cases that have run",
                       GBL_TEST_SUITE_PROPERTY_ID_CASES_RUN,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("casesPassed",
                       "Cases Passed",
                       "Number of test cases that passed",
                       GBL_TEST_SUITE_PROPERTY_ID_CASES_PASSED,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("casesFailed",
                       "Cases Failed",
                       "Number of test cases that failed",
                       GBL_TEST_SUITE_PROPERTY_ID_CASES_FAILED,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("casesSkipped",
                       "Cases Skipped",
                       "Number of test cases that were skipped",
                       GBL_TEST_SUITE_PROPERTY_ID_CASES_SKIPPED,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
GBL_PROPERTY_TABLE_END()


GBL_EXPORT GblType          GblTestSuite_type               (void)                                      GBL_NOEXCEPT;

GBL_EXPORT GblType          GblTestSuite_register           (const char* pName,
                                                             const GblTestSuiteClassVTable* pVTable,
                                                             GblSize instanceSize,
                                                             GblSize instancePrivateSize,
                                                             GblFlags typeFlags)                        GBL_NOEXCEPT;

GBL_EXPORT GblTestSuite*    GblTestSuite_create             (const char* pName)                         GBL_NOEXCEPT;

GBL_EXPORT GblTestSuite*    GblTestSuite_createWithVTable   (const char* pName,
                                                             const GblTestSuiteClassVTable* pVTable)    GBL_NOEXCEPT;

GBL_EXPORT GblTestSuite*    GblTestSuite_createFromType     (GblType type)                              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_destroy            (SELF)                                      GBL_NOEXCEPT;


GBL_EXPORT const char*      GblTestSuite_name               (CSELF)                                     GBL_NOEXCEPT;

GBL_EXPORT GblBool          GblTestSuite_ran                (CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestSuite_passed             (CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT GblTestScenario* GblTestSuite_scenario           (CSELF)                                     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_caseAdd            (SELF,
                                                             const char* pName,
                                                             GblTestCaseRunFn pFnTest)                  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_casesAdd           (SELF,
                                                             const GblTestCase* pCases)                 GBL_NOEXCEPT;

GBL_EXPORT GblBool          GblTestSuite_caseRan            (CSELF, const char* pCaseName)              GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestSuite_casePassed         (CSELF, const char* pCaseName)              GBL_NOEXCEPT;
GBL_EXPORT GblSize          GblTestSuite_caseCount          (CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT const char*      GblTestSuite_caseName           (CSELF, GblSize index)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_caseInit           (SELF, GblContext* pCtx)                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestSuite_caseFinal          (SELF, GblContext* pCtx)                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_caseRun            (SELF,
                                                             GblContext* pCtx,
                                                             GblSize index)                             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_suiteInit          (SELF, GblContext* pCtx)                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestSuite_suiteFinal         (SELF, GblContext* pCtx)                    GBL_NOEXCEPT;


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_TEST_SUITE_H
