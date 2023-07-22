/*! \file
 *  \brief GblTestSuite structure and related functions
 *  \ingroup testing
 *
 *  \author 2023 Falco Girgis
 *  \copyright MIT License
 */

#ifndef GIMBAL_TEST_SUITE_H
#define GIMBAL_TEST_SUITE_H

#include "../meta/instances/gimbal_object.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_TEST_SUITE_TYPE                (GBL_TYPEOF(GblTestSuite))
#define GBL_TEST_SUITE(instance)           (GBL_INSTANCE_CAST(instance, GblTestSuite))
#define GBL_TEST_SUITE_CLASS(klass)        (GBL_CLASS_CAST(klass, GblTestSuite))
#define GBL_TEST_SUITE_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GblTestSuite))
//! @}

#define GBL_TEST_CASE_LAST  { GBL_NULL, GBL_NULL }

#define GBL_SELF_TYPE       GblTestSuite

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblContext);
GBL_FORWARD_DECLARE_STRUCT(GblTestSuite);
GBL_FORWARD_DECLARE_STRUCT(GblTestScenario);

typedef GBL_RESULT (*GblTestCaseRunFn)(GBL_SELF, GblContext* pCtx);

//! Represenst a single test case within a GblTestSuite
typedef struct GblTestCase {
    const char*      pName;
    GblTestCaseRunFn pFnRun;
} GblTestCase;

//! Virtual table structure for a GblTestSuiteClass
typedef struct GblTestSuiteVTable {
    GBL_RESULT (*pFnSuiteInit) (GBL_SELF, GblContext* pCtx);
    GBL_RESULT (*pFnSuiteFinal)(GBL_SELF, GblContext* pCtx);
    GBL_RESULT (*pFnCaseInit)  (GBL_SELF, GblContext* pCtx);
    GBL_RESULT (*pFnCaseFinal) (GBL_SELF, GblContext* pCtx);
    const GblTestCase*  pCases;
} GblTestSuiteVTable;

/*! \struct GblTestSuiteClass
 *  \extends GblObjectClass
 *  \brief GblClass structure for GblTestSuite
 *
 *  Provides virtual methods for customizing/reimplementing
 *  a test suite's per-case logic.
 *
 *  \sa GblTestSuite
 */
GBL_CLASS_DERIVE(GblTestSuite, GblObject)
    const GblTestSuiteVTable* pVTable;

    GBL_RESULT (*pFnSuiteName)(GBL_CSELF, const char** ppName);
    GBL_RESULT (*pFnCaseCount)(GBL_CSELF, size_t * pSize);
    GBL_RESULT (*pFnCaseName) (GBL_CSELF, size_t  index, const char** ppName);
    GBL_RESULT (*pFnCaseRun)  (GBL_SELF, GblContext* pCtx, size_t  index);
GBL_CLASS_END

/*! \struct GblTestSuite
 *  \extends GblObject
 *  \brief GblObject representing a collection of unit test cases
 *  \ingroup testing
 *
 *  GblTestSuite is a GblInstance type which encapsulates a single
 *  test suite--a collection of sequentially executing, related
 *  test cases.
 *
 *  \sa GblTestSuiteClass
 */
GBL_INSTANCE_DERIVE(GblTestSuite, GblObject)
    GblCallRecord failingIssue;
    size_t        casesRun;
    size_t        casesPassed;
    size_t        casesFailed;
    size_t        casesSkipped;
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

GBL_EXPORT GblType       GblTestSuite_type             (void)                               GBL_NOEXCEPT;

GBL_EXPORT GblType       GblTestSuite_register         (const char* pName,
                                                        const GblTestSuiteVTable* pVTable,
                                                        size_t     instanceSize,
                                                        size_t     instancePrivateSize,
                                                        GblFlags   typeFlags)               GBL_NOEXCEPT;

GBL_EXPORT GblTestSuite* GblTestSuite_create           (GblType type)                       GBL_NOEXCEPT;

GBL_EXPORT GblTestSuite* GblTestSuite_createWithVTable (const char* pName,
                                                        const GblTestSuiteVTable* pVTable)  GBL_NOEXCEPT;

GBL_EXPORT GblRefCount   GblTestSuite_unref            (GBL_SELF)                           GBL_NOEXCEPT;

GBL_EXPORT const char*   GblTestSuite_name             (GBL_CSELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblTestSuite_ran              (GBL_CSELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblTestSuite_passed           (GBL_CSELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblTestScenario*
                         GblTestSuite_scenario         (GBL_CSELF)                          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblTestSuite_addCase          (GBL_SELF,
                                                        const char*      pName,
                                                        GblTestCaseRunFn pFnTest)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblTestSuite_addCases         (GBL_SELF,
                                                        const GblTestCase* pCases)          GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblTestSuite_caseRan          (GBL_CSELF, const char* pCaseName)   GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblTestSuite_casePassed       (GBL_CSELF, const char* pCaseName)   GBL_NOEXCEPT;
GBL_EXPORT size_t        GblTestSuite_caseCount        (GBL_CSELF)                          GBL_NOEXCEPT;
GBL_EXPORT const char*   GblTestSuite_caseName         (GBL_CSELF, size_t  index)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblTestSuite_initCase         (GBL_SELF, GblContext* pCtx)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblTestSuite_finalCase        (GBL_SELF, GblContext* pCtx)         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblTestSuite_runCase          (GBL_SELF,
                                                        GblContext* pCtx,
                                                        size_t      index)                  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblTestSuite_initSuite        (GBL_SELF, GblContext* pCtx)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblTestSuite_finalSuite       (GBL_SELF, GblContext* pCtx)         GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_TEST_SUITE_H
