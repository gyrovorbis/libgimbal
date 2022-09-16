/*! \file
 *  \brief GblTestSuite structure and related functions
 *  \ingroup testing
 */

#ifndef GIMBAL_TEST_SUITE_H
#define GIMBAL_TEST_SUITE_H

#include "../meta/instances/gimbal_object.h"

#define GBL_TEST_SUITE_TYPE                (GBL_TYPEOF(GblTestSuite))

#define GBL_TEST_SUITE(instance)           (GBL_INSTANCE_CAST(instance, GblTestSuite))
#define GBL_TEST_SUITE_CLASS(klass)        (GBL_CLASS_CAST(klass, GblTestSuite))
#define GBL_TEST_SUITE_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GblTestSuite))

#define GBL_TEST_CASE_LAST  { GBL_NULL, GBL_NULL }
#define GBL_SELF_TYPE       GblTestSuite

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
    GBL_RESULT (*pFnSuiteInit) (GBL_SELF, GblContext* pCtx);
    GBL_RESULT (*pFnSuiteFinal)(GBL_SELF, GblContext* pCtx);
    GBL_RESULT (*pFnCaseInit)  (GBL_SELF, GblContext* pCtx);
    GBL_RESULT (*pFnCaseFinal) (GBL_SELF, GblContext* pCtx);
    const GblTestCase*  pCases;
} GblTestSuiteClassVTable;

GBL_CLASS_DERIVE(GblTestSuite, GblObject)
    const GblTestSuiteClassVTable* pVTable;

    GBL_RESULT  (*pFnSuiteName)(GBL_CSELF, const char** ppName);
    GBL_RESULT  (*pFnCaseCount)(GBL_CSELF, GblSize* pSize);
    GBL_RESULT  (*pFnCaseName) (GBL_CSELF, GblSize index, const char** ppName);
    GBL_RESULT  (*pFnCaseRun)  (GBL_SELF, GblContext* pCtx, GblSize index);
GBL_CLASS_END

/*! \brief GblObject representing a collection of unit test cases
 *  \ingroup testing
 */
GBL_INSTANCE_DERIVE(GblTestSuite, GblObject)
    GblCallRecord failingIssue;
    GblSize       casesRun;
    GblSize       casesPassed;
    GblSize       casesFailed;
    GblSize       casesSkipped;
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

GBL_EXPORT GblType          GblTestSuite_type             (void)                                   GBL_NOEXCEPT;

GBL_EXPORT GblType          GblTestSuite_register         (const char* pName,
                                                           const GblTestSuiteClassVTable* pVTable,
                                                           GblSize    instanceSize,
                                                           GblSize    instancePrivateSize,
                                                           GblFlags   typeFlags)                   GBL_NOEXCEPT;

GBL_EXPORT GblTestSuite*    GblTestSuite_create           (const char* pName)                      GBL_NOEXCEPT;

GBL_EXPORT GblTestSuite*    GblTestSuite_createWithVTable (const char* pName,
                                                           const GblTestSuiteClassVTable* pVTable) GBL_NOEXCEPT;

GBL_EXPORT GblTestSuite*    GblTestSuite_createFromType   (GblType type)                           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_destroy          (GBL_SELF)                               GBL_NOEXCEPT;

GBL_EXPORT const char*      GblTestSuite_name             (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestSuite_ran              (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestSuite_passed           (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblTestScenario* GblTestSuite_scenario         (GBL_CSELF)                              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_addCase          (GBL_SELF,
                                                           const char* pName,
                                                           GblTestCaseRunFn pFnTest)               GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_addCases         (GBL_SELF,
                                                           const GblTestCase* pCases)              GBL_NOEXCEPT;

GBL_EXPORT GblBool          GblTestSuite_caseRan          (GBL_CSELF, const char* pCaseName)       GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestSuite_casePassed       (GBL_CSELF, const char* pCaseName)       GBL_NOEXCEPT;
GBL_EXPORT GblSize          GblTestSuite_caseCount        (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT const char*      GblTestSuite_caseName         (GBL_CSELF, GblSize index)               GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_initCase         (GBL_SELF, GblContext* pCtx)             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestSuite_finalCase        (GBL_SELF, GblContext* pCtx)             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_runCase          (GBL_SELF,
                                                           GblContext* pCtx,
                                                           GblSize     index)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblTestSuite_initSuite        (GBL_SELF, GblContext* pCtx)             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestSuite_finalSuite       (GBL_SELF, GblContext* pCtx)             GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_TEST_SUITE_H
