/*! \file
 *  \brief GblTestScenario and related functions.
 *  \ingroup testing
 *
 *  \author     2023, 2024 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_TEST_SCENARIO_H
#define GIMBAL_TEST_SCENARIO_H

#include "../meta/instances/gimbal_context.h"
#include "../meta/signals/gimbal_signal.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_TEST_SCENARIO_TYPE            (GBL_TYPEID(GblTestScenario))
#define GBL_TEST_SCENARIO(self)           (GBL_CAST(GblTestScenario, self))
#define GBL_TEST_SCENARIO_CLASS(klass)    (GBL_CLASS_CAST(GblTestScenario, klass))
#define GBL_TEST_SCENARIO_GET_CLASS(self) (GBL_CLASSOF(GblTestScenario, self))
//! @}

#define GBL_SELF_TYPE GblTestScenario

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTestScenario);
GBL_FORWARD_DECLARE_STRUCT(GblTestSuite);

/*! \struct  GblTestScenarioClass
 *  \extends GblContextClass
 *  \brief   GblClass VTable structure for a GblTestScenario
 *
 *  Provides a series of overridable virtual methods for looking
 *  up into individual GblTestSuites and executing them. This allows
 *  for mapping to a custom back-end that doesn't use GblTestSuite
 *  objects.
 *
 *  \sa GblTestScenario
 */
GBL_CLASS_DERIVE(GblTestScenario, GblContext)
    GBL_RESULT (*pFnBegin)     (GBL_SELF);
    GBL_RESULT (*pFnEnd)       (GBL_SELF);
    GBL_RESULT (*pFnRun)       (GBL_SELF, int argc, char* argv[]);
    GBL_RESULT (*pFnSuiteBegin)(GBL_SELF, const GblTestSuite* pSuite);
    GBL_RESULT (*pFnSuiteEnd)  (GBL_SELF, const GblTestSuite* pSuite);
GBL_CLASS_END

/*! \struct  GblTestScenario
 *  \extends GblContext
 *  \ingroup testing
 *  \brief   Represents a single top-level test object
 *
 *  GblTestScenario is the highest-level test object in the
 *  testing framework, and is logically divided into a series
 *  of related, yet independently executing GblTestSuite
 *  objects.
 *
 *  \sa GblTestScenarioClass
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

GBL_SIGNALS(GblTestScenario,
    (began),
    (ended),
    (suiteBegan, (GBL_INSTANCE_TYPE, pSuite)),
    (suiteEnded, (GBL_INSTANCE_TYPE, pSuite)),
    (caseBegan,  (GBL_INSTANCE_TYPE, pSuite), (GBL_SIZE_TYPE, caseIndex)),
    (caseEnded,  (GBL_INSTANCE_TYPE, pSuite), (GBL_SIZE_TYPE, caseIndex))
)

GBL_EXPORT GblType          GblTestScenario_type         (void)                                 GBL_NOEXCEPT;
GBL_EXPORT GblTestScenario* GblTestScenario_create       (const char* pName)                    GBL_NOEXCEPT;
GBL_EXPORT GblRefCount      GblTestScenario_unref        (GBL_SELF)                             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestScenario_enqueueSuite (GBL_SELF, const GblTestSuite* pSuite) GBL_NOEXCEPT;
GBL_EXPORT GblTestSuite*    GblTestScenario_currentSuite (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GblTestSuite*    GblTestScenario_findSuite    (GBL_CSELF, const char* pName)         GBL_NOEXCEPT;
GBL_EXPORT size_t           GblTestScenario_currentCase  (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblTestScenario_run          (GBL_SELF, int argc, char* argv[])     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestScenario_ran          (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblTestScenario_passed       (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT void             GblTestScenario_expectError  (GBL_CSELF)                            GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_TEST_SCENARIO_H
