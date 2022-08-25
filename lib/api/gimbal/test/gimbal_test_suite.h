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

#define GBL_SELF    GblTestSuite* pSelf
#define GBL_CSELF   const GBL_SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblContext);
GBL_FORWARD_DECLARE_STRUCT(GblTestSuite);
GBL_FORWARD_DECLARE_STRUCT(GblTestScenario);

typedef GBL_RESULT (*GblTestCaseRunFn)(GBL_SELF, GblContext* pCtx);

typedef struct GblTestCase {
    const char*      pName;
    GblTestCaseRunFn pFnRun;
} GblTestCase;

typedef struct GblTestSuiteClassVTable {
    GBL_RESULT          (*pFnSuiteInit) (GBL_SELF, GblContext* pCtx);
    GBL_RESULT          (*pFnSuiteFinal)(GBL_SELF, GblContext* pCtx);
    GBL_RESULT          (*pFnCaseInit)  (GBL_SELF, GblContext* pCtx);
    GBL_RESULT          (*pFnCaseFinal) (GBL_SELF, GblContext* pCtx);
    const GblTestCase*  pCases;
} GblTestSuiteClassVTable;

GBL_CLASS_DERIVE(GblTestSuite, GblObject)
    const GblTestSuiteClassVTable* pVTable;
    GBL_RESULT  (*pFnSuiteName) (GBL_CSELF, const char** ppName);
    GBL_RESULT  (*pFnCaseCount) (GBL_CSELF, GblSize* pSize);
    GBL_RESULT  (*pFnCaseName)  (GBL_CSELF, GblSize index, const char** ppName);
    GBL_RESULT  (*pFnCaseRun)   (GBL_SELF, GblContext* pCtx, GblSize index);
GBL_CLASS_END

/*! \brief GblObject representing a collection of unit test cases
 *  \ingroup testing
 */
GBL_INSTANCE_DERIVE(GblTestSuite, GblObject)
    GblCallRecord   failingIssue;
    GblSize         casesRun;
    GblSize         casesPassed;
    GblSize         casesFailed;
    GblSize         casesSkipped;
GBL_INSTANCE_END

GBL_PROPERTIES(GblTestSuite,
    (runResult,      GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (failureMessage, GBL_GENERIC, (READ, SAVE), GBL_STRING_TYPE),
    (caseCount,      GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesRun,       GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesPassed,    GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesFailed,    GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesSkipped,   GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE)
)

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

GBL_EXPORT GBL_RESULT       GblTestSuite_destroy            (GBL_SELF)                                  GBL_NOEXCEPT;


GBL_EXPORT const char*      GblTestSuite_name               (GBL_CSELF)                                 GBL_NOEXCEPT;

GBL_EXPORT GblBool          GblTestSuite_ran                (GBL_CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestSuite_passed             (GBL_CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT GblTestScenario* GblTestSuite_scenario           (GBL_CSELF)                                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_caseAdd            (GBL_SELF,
                                                             const char* pName,
                                                             GblTestCaseRunFn pFnTest)                  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_casesAdd           (GBL_SELF,
                                                             const GblTestCase* pCases)                 GBL_NOEXCEPT;

GBL_EXPORT GblBool          GblTestSuite_caseRan            (GBL_CSELF, const char* pCaseName)          GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestSuite_casePassed         (GBL_CSELF, const char* pCaseName)          GBL_NOEXCEPT;
GBL_EXPORT GblSize          GblTestSuite_caseCount          (GBL_CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT const char*      GblTestSuite_caseName           (GBL_CSELF, GblSize index)                  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_caseInit           (GBL_SELF, GblContext* pCtx)                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestSuite_caseFinal          (GBL_SELF, GblContext* pCtx)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_caseRun            (GBL_SELF,
                                                             GblContext* pCtx,
                                                             GblSize index)                             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_suiteInit          (GBL_SELF, GblContext* pCtx)                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestSuite_suiteFinal         (GBL_SELF, GblContext* pCtx)                GBL_NOEXCEPT;


GBL_DECLS_END

#undef GBL_CSELF
#undef GBL_SELF

#endif // GIMBAL_TEST_SUITE_H
