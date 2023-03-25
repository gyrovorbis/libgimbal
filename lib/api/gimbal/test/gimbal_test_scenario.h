/*! \file
 *  \brief GblTestScenario and related functions.
 *  \ingroup testing
 */
#ifndef GIMBAL_TEST_SCENARIO_H
#define GIMBAL_TEST_SCENARIO_H

#include "../meta/instances/gimbal_context.h"

#define GBL_TEST_SCENARIO_TYPE                (GBL_TYPEOF(GblTestScenario))

#define GBL_TEST_SCENARIO(instance)           (GBL_INSTANCE_CAST(instance, GblTestScenario))
#define GBL_TEST_SCENARIO_CLASS(klass)        (GBL_CLASS_CAST(klass, GblTestScenario))
#define GBL_TEST_SCENARIO_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GblTestScenario))

#define GBL_SELF_TYPE GblTestScenario

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTestScenario);
GBL_FORWARD_DECLARE_STRUCT(GblTestSuite);

GBL_CLASS_DERIVE(GblTestScenario, GblContext)
    GBL_RESULT  (*pFnBegin)     (GBL_SELF);
    GBL_RESULT  (*pFnEnd)       (GBL_SELF);
    GBL_RESULT  (*pFnRun)       (GBL_SELF, int argc, char* argv[]);
    GBL_RESULT  (*pFnSuiteBegin)(GBL_SELF, const GblTestSuite* pSuite);
    GBL_RESULT  (*pFnSuiteEnd)  (GBL_SELF, const GblTestSuite* pSuite);
GBL_CLASS_END

/*! \brief Top-level object representing a collection of GblTestSuite objects.
 *  \ingroup testing
 */
GBL_INSTANCE_DERIVE(GblTestScenario, GblContext)
    GBL_RESULT result;
    size_t     suitesRun;
    size_t     suitesPassed;
    size_t     suitesFailed;
    size_t     suitesSkipped;
    size_t     caseCount;
    size_t     casesRun;
    size_t     casesPassed;
    size_t     casesFailed;
    size_t     casesSkipped;
    double     totalTime;
GBL_INSTANCE_END

GBL_PROPERTIES(GblTestScenario,
    (testResult,    GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (suiteCount,    GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (suitesRun,     GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (suitesPassed,  GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (suitesFailed,  GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (suitesSkipped, GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (caseCount,     GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesRun,      GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesPassed,   GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesFailed,   GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesSkipped,  GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE)
)

GBL_EXPORT GblType          GblTestScenario_type         (void)                                 GBL_NOEXCEPT;
GBL_EXPORT GblTestScenario* GblTestScenario_create       (const char* pName)                    GBL_NOEXCEPT;
GBL_EXPORT GblRefCount      GblTestScenario_unref        (GBL_SELF)                             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestScenario_enqueueSuite (GBL_SELF, const GblTestSuite* pSuite) GBL_NOEXCEPT;
GBL_EXPORT GblTestSuite*    GblTestScenario_currentSuite (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GblTestSuite*    GblTestScenario_findSuite    (GBL_CSELF, const char* pName)         GBL_NOEXCEPT;
GBL_EXPORT const char*      GblTestScenario_currentCase  (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestScenario_run          (GBL_SELF, int argc, char* argv[])     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestScenario_ran          (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestScenario_passed       (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT void             GblTestScenario_expectError  (GBL_CSELF)                            GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_TEST_SCENARIO_H
