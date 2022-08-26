/*! \file
 *  \brief GblTestScenario and related functions.
 *  \ingroup testing
 */
#ifndef GIMBAL_TEST_SCENARIO_H
#define GIMBAL_TEST_SCENARIO_H

#include "../meta/instances/gimbal_context.h"

#define GBL_TEST_SCENARIO_TYPE                (GblTestScenario_type())

#define GBL_TEST_SCENARIO(instance)           (GBL_INSTANCE_CAST(instance, GBL_TEST_SCENARIO_TYPE, GblTestScenario))
#define GBL_TEST_SCENARIO_CLASS(klass)        (GBL_CLASS_CAST(klass, GBL_TEST_SCENARIO_TYPE, GblTestScenarioClass))
#define GBL_TEST_SCENARIO_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GBL_TEST_SCENARIO_TYPE, GblTestScenarioClass))

#define GBL_SELF    GblTestScenario* pSelf
#define GBL_CSELF   const GBL_SELF

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
    GBL_RESULT  result;
    GblSize     suitesRun;
    GblSize     suitesPassed;
    GblSize     suitesFailed;
    GblSize     suitesSkipped;
    GblSize     caseCount;
    GblSize     casesRun;
    GblSize     casesPassed;
    GblSize     casesFailed;
    GblSize     casesSkipped;
    double      totalTime;
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

GBL_EXPORT GblType          GblTestScenario_type        (void)                                 GBL_NOEXCEPT;
GBL_EXPORT GblTestScenario* GblTestScenario_create      (const char* pName)                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestScenario_destroy     (GBL_SELF)                             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestScenario_suiteEnqueue(GBL_SELF, const GblTestSuite* pSuite) GBL_NOEXCEPT;
GBL_EXPORT GblTestSuite*    GblTestScenario_suiteCurrent(GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GblTestSuite*    GblTestScenario_suiteFind   (GBL_CSELF, const char* pName)         GBL_NOEXCEPT;
GBL_EXPORT const char*      GblTestScenario_caseCurrent (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestScenario_run         (GBL_SELF, int argc, char* argv[])     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestScenario_ran         (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestScenario_passed      (GBL_CSELF)                            GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_CSELF
#undef GBL_SELF


#endif // GIMBAL_TEST_SCENARIO_H
