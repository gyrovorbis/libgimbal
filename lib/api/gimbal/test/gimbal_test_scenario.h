/*! \file
 *  \brief GblTestScenario and related functions.
 *  \ingroup testing
 */
#ifndef GIMBAL_TEST_SCENARIO_H
#define GIMBAL_TEST_SCENARIO_H

#include "../objects/gimbal_context.h"

#define GBL_TEST_SCENARIO_TYPE                (GblTestScenario_type())
#define GBL_TEST_SCENARIO_STRUCT              GblTestScenario
#define GBL_TEST_SCENARIO_CLASS_STRUCT        GblTestScenarioClass

#define GBL_TEST_SCENARIO(inst)               (GBL_INSTANCE_CAST_PREFIX(inst, GBL_TEST_SCENARIO))
#define GBL_TEST_SCENARIO_CHECK(inst)         (GBL_INSTANCE_CHECK_PREFIX(inst, GBL_TEST_SCENARIO))
#define GBL_TEST_SCENARIO_TRY(inst)           (GBL_INSTANCE_TRY_PREFIX(inst, GBL_TEST_SCENARIO))

#define GBL_TEST_SCENARIO_CLASS(klass)        (GBL_CLASS_CAST_PREFIX(klass, GBL_TEST_SCENARIO))
#define GBL_TEST_SCENARIO_CLASS_CHECK(klass)  (GBL_CLASS_CHECK_PREFIX(klass, GBL_TEST_SCENARIO))
#define GBL_TEST_SCENARIO_CLASS_TRY(klass)    (GBL_CLASS_TRY_PREFIX(klass, GBL_TEST_SCENARIO))

#define GBL_TEST_SCENARIO_GET_CLASS(inst)     (GBL_INSTANCE_GET_CLASS_PREFIX(inst, GBL_TEST_SCENARIO))
#define GBL_TEST_SCENARIO_TRY_CLASS(inst)     (GBL_INSTANCE_TRY_CLASS_PREFIX(inst, GBL_TEST_SCENARIO))

#define SELF    GblTestScenario* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTestScenario);
GBL_FORWARD_DECLARE_STRUCT(GblTestSuite);

typedef struct GblTestScenarioClass {
    GblContextClass     base;
    GBL_RESULT          (*pFnBegin)     (SELF);
    GBL_RESULT          (*pFnEnd)       (SELF);
    GBL_RESULT          (*pFnRun)       (SELF, int argc, char* argv[]);
    GBL_RESULT          (*pFnSuiteBegin)(SELF, const GblTestSuite* pSuite);
    GBL_RESULT          (*pFnSuiteEnd)  (SELF, const GblTestSuite* pSuite);
} GblTestScenarioClass;

/*! \brief Top-level object representing a collection of GblTestSuite objects.
 *  \ingroup testing
 */
typedef struct GblTestScenario {
    union {
        GblTestScenarioClass*   pClass;
        GblContext              base;
    };
    GBL_RESULT                  result;
    GblSize                     suitesRun;
    GblSize                     suitesPassed;
    GblSize                     suitesFailed;
    GblSize                     suitesSkipped;
    GblSize                     caseCount;
    GblSize                     casesRun;
    GblSize                     casesPassed;
    GblSize                     casesFailed;
    GblSize                     casesSkipped;
    double                      totalTime;
} GblTestScenario;


typedef enum GBL_TEST_SCENARIO_PROPERTY_ID {
    GBL_TEST_SCENARIO_PROPERTY_ID_FIRST        = GBL_CONTEXT_PROPERTY_ID_COUNT,
    GBL_TEST_SCENARIO_PROPERTY_ID_RESULT       = GBL_TEST_SCENARIO_PROPERTY_ID_FIRST,
    GBL_TEST_SCENARIO_PROPERTY_ID_SUITE_COUNT,
    GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_RUN,
    GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_PASSED,
    GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_FAILED,
    GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_SKIPPED,
    GBL_TEST_SCENARIO_PROPERTY_ID_CASE_COUNT,
    GBL_TEST_SCENARIO_PROPERTY_ID_CASES_RUN,
    GBL_TEST_SCENARIO_PROPERTY_ID_CASES_PASSED,
    GBL_TEST_SCENARIO_PROPERTY_ID_CASES_FAILED,
    GBL_TEST_SCENARIO_PROPERTY_ID_CASES_SKIPPED,
    GBL_TEST_SCENARIO_PROPERTY_ID_COUNT
} GBL_TEST_SCENARIO_PROPERTY_ID;


// total time property
GBL_PROPERTY_TABLE_BEGIN(GBL_TEST_SCENARIO)
    GBL_PROPERTY_ENTRY("result",
                       "Result Code",
                       "Result code returned from test scenario run",
                       GBL_TEST_SCENARIO_PROPERTY_ID_RESULT,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("suiteCount",
                       "Suite Count",
                       "Number of test suites included within scenario",
                       GBL_TEST_SCENARIO_PROPERTY_ID_SUITE_COUNT,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("suitesRun",
                       "Suites Run",
                       "Number of test suites that ran",
                       GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_RUN,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("suitesPassed",
                       "Suites Passed",
                       "Number of test suites that passed",
                       GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_PASSED,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("suitesFailed",
                       "Suites Failed",
                       "Number of test suites that failed",
                       GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_FAILED,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("suitesSkipped",
                       "Suites Skipped",
                       "Number of test suites that were skipped",
                       GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_SKIPPED,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("caseCount",
                       "Case Count",
                       "Total number of test cases included within all suites",
                       GBL_TEST_SCENARIO_PROPERTY_ID_CASE_COUNT,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("casesRun",
                       "Cases Run",
                       "Number of test cases that ran",
                       GBL_TEST_SCENARIO_PROPERTY_ID_CASES_RUN,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("casesPassed",
                       "Cases Passed",
                       "Total number of test cases that passed",
                       GBL_TEST_SCENARIO_PROPERTY_ID_CASES_PASSED,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("casesFailed",
                       "Cases Failed",
                       "Total number of test cases that failed",
                       GBL_TEST_SCENARIO_PROPERTY_ID_CASES_FAILED,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
    GBL_PROPERTY_ENTRY("casesSkipped",
                       "Cases Skipped",
                       "Total number of test cases that were skipped",
                       GBL_TEST_SCENARIO_PROPERTY_ID_CASES_SKIPPED,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, SAVE))
GBL_PROPERTY_TABLE_END()


GBL_EXPORT GblType          GblTestScenario_type        (void)                              GBL_NOEXCEPT;
GBL_EXPORT GblTestScenario* GblTestScenario_create      (const char* pName)                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestScenario_destroy     (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestScenario_suiteEnqueue(SELF, const GblTestSuite* pSuite)  GBL_NOEXCEPT;
GBL_EXPORT GblTestSuite*    GblTestScenario_suiteCurrent(CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT GblTestSuite*    GblTestScenario_suiteFind   (CSELF, const char* pName)          GBL_NOEXCEPT;
GBL_EXPORT const char*      GblTestScenario_caseCurrent (CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestScenario_run         (SELF, int argc, char* argv[])      GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestScenario_ran         (CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestScenario_passed      (CSELF)                             GBL_NOEXCEPT;

GBL_DECLS_END

#undef CSELF
#undef SELF


#endif // GIMBAL_TEST_SCENARIO_H
