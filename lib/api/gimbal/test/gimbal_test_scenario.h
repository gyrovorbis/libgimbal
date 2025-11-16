/*! \file
 *  \brief GblTestScenario and related functions.
 *  \ingroup testing
 *
 *  This file contains the type declaration for GblTestScenario
 *  and its related public API.
 *
 *  \author     2023, 2024, 2025 Falco Girgis
 *  \copyright  MIT License
 *
 *  \todo
 *      - removing GblTestSuites
 *      - Complex/Extended constructor like GblBox
 *      - Uniform argument parsing/handling?
 *      - abort the scenario
 *      - option for dumping failed cases at the end
 *      - suite count public API
 *      - expecting error
 *      - emit property changed signals
 */
#ifndef GIMBAL_TEST_SCENARIO_H
#define GIMBAL_TEST_SCENARIO_H

#include "../meta/instances/gimbal_context.h"
#include "../meta/signals/gimbal_signal.h"

/*! \name Type System
 *  \brief Type UUID and cast operators.
 *  @{
 */
#define GBL_TEST_SCENARIO_TYPE            GBL_TYPEID(GblTestScenario)             //!< GblType UUID for GblTestScenario
#define GBL_TEST_SCENARIO(self)           GBL_CAST(GblTestScenario, self)         //!< Casts a GblInstance to a GblTestScenario
#define GBL_TEST_SCENARIO_CLASS(klass)    GBL_CLASS_CAST(GblTestScenario, klass)  //!< Casts a GblClass to a GblTestScenarioClass
#define GBL_TEST_SCENARIO_CLASSOF(self)   GBL_CLASSOF(GblTestScenario, self)      //!< Retrieves a GblTestScenarioClass from a GblInstance
//! @}

#define GBL_SELF_TYPE GblTestScenario

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTestScenario);
GBL_FORWARD_DECLARE_STRUCT(GblTestSuite);

/*! \struct  GblTestScenarioClass
 *  \extends GblContextClass
 *  \brief   GblClass VTable structure for a GblTestScenario.
 *
 *  Provides a series of overridable virtual methods for looking
 *  up into individual GblTestSuites and executing them. This allows
 *  for mapping to a custom back-end that doesn't use GblTestSuite
 *  objects. The default implementations of these methods implement
 *  the built-in behavior of iterating through each child suite and
 *  executing its tests.
 *
 *  \sa GblTestScenario
 */
GBL_CLASS_DERIVE(GblTestScenario, GblContext)
    //! Virtual function which gets called just before a GblTestScenario begins execution.
    GBL_RESULT (*pFnBegin)     (GBL_SELF);
    //! Virtual function which gets called just after a GblTestScenario ends execution.
    GBL_RESULT (*pFnEnd)       (GBL_SELF);
    //! Virtual function which implements the actual GblTestScenario execution behavior.
    GBL_RESULT (*pFnRun)       (GBL_SELF, int argc, const char* argv[]);
    //! Virtual function which gets called just before a GblTestSuite begins execution.
    GBL_RESULT (*pFnSuiteBegin)(GBL_SELF, const GblTestSuite* pSuite);
    //! Virtual function which gets called just after a GblTestSuite ends execution.
    GBL_RESULT (*pFnSuiteEnd)  (GBL_SELF, const GblTestSuite* pSuite);
GBL_CLASS_END

/*! \struct  GblTestScenario
 *  \extends GblContext
 *  \ingroup testing
 *  \brief   Represents a single top-level test object.
 *
 *  GblTestScenario is the highest-level test object in the
 *  testing framework, and is logically divided into a series
 *  of related, yet independently executing GblTestSuite
 *  objects.
 *
 *  \sa GblTestScenarioClass, GblTestSuite
 */
GBL_INSTANCE_DERIVE(GblTestScenario, GblContext)
    GBL_RESULT result;          //!< Current overall result code from test scenario run.
    size_t     suitesRun;       //!< Total number of test suites which have run.
    size_t     suitesPassed;    //!< Total number of test suites which have passed during run.
    size_t     suitesFailed;    //!< Total number of test suites which have failed during run.
    size_t     suitesSkipped;   //!< Total number of test suites which have been skipped during run.
    size_t     caseCount;       //!< Total number of test cases contained within all test suites contained within the test scenario.
    size_t     casesRun;        //!< Total numer of test cases which have run.
    size_t     casesPassed;     //!< Total number of test cases which have passed.
    size_t     casesFailed;     //!< Total number of test cases which have failed.
    size_t     casesSkipped;    //!< Total number of test cases which have been skipped.
    double     totalTime;       //!< Total time spent executing tests, in milliseconds.
GBL_INSTANCE_END

// \cond
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
// \endcond

//! Returns the GblType UUID associated with GblTestScenario.
GBL_EXPORT GblType GblTestScenario_type (void) GBL_NOEXCEPT;

/*! \name  Lifetime Management
 *  \brief Creation and reference ownership methods.
 *  @{
 */
//! Creates a GblTestScenario with the given name, optionally as an extended subtype with the given instance structure size and floating class.
GBL_EXPORT GblTestScenario* GblTestScenario_create (const char* pName
#if 0
,
                                                    GblType     type/*=GBL_TEST_SCENARIO_TYPE*/,
                                                    size_t      size/*=sizeof(GblTestScenario)*/,
                                                    GBL_KLASS*  pClass=/*NULL*/
#endif
                                                    ) GBL_NOEXCEPT;
//! Returns a new reference to an existing GblTestScenario instance, incrementing its reference counter.
GBL_EXPORT GblTestScenario* GblTestScenario_ref    (GBL_SELF)                    GBL_NOEXCEPT;
//! Releases a reference to the given GblTestScenario, destroying it when the reference counter hits zero.
GBL_EXPORT GblRefCount      GblTestScenario_unref  (GBL_SELF)                    GBL_NOEXCEPT;
//! @}

/*! \name  Suite Management
 *  \brief Routines for managing child test suites.
 *  @{
 */
//! Enqueues the given GblTestSuite instance to be run as part of the given GblTestScenario's test plan.
GBL_EXPORT GBL_RESULT    GblTestScenario_enqueueSuite (GBL_SELF,
                                                       const GblTestSuite* pSuite) GBL_NOEXCEPT;
//! Finds a GblTestSuite by name which has been enqueued onto the given GblTestScenario.
GBL_EXPORT GblTestSuite* GblTestScenario_findSuite    (GBL_CSELF,
                                                       const char* pName)          GBL_NOEXCEPT;
//! @}

/*! \name  Current Tests
 *  \brief Routines for querying the active tests.
 *  @{
 */
//! Returns a pointer to the currently executing GblTestSuite within the given GblTestScenario, if there is an active one.
GBL_EXPORT GblTestSuite* GblTestScenario_currentSuite (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the index of the currently executing test case within the currently executing GblTestSuite within the given GblTestScenario, if there is an active one, or GBL_NPOS otherwise.
GBL_EXPORT size_t        GblTestScenario_currentCase  (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name  Execution
 *  \brief Routines for starting text execution.
 *  @{
 */
//! Begins execution of the given GblTestScenario instance, running through all of its enqueued GblTestSuites, possibly processing the given command-line arguments.
GBL_EXPORT GBL_RESULT GblTestScenario_run  (GBL_SELF, int argc, const char* argv[]) GBL_NOEXCEPT;
//! Variant of GblTestScenario_run() which releases the GblTestScenario after execution and returns a status code which can be directly returned from main().
GBL_EXPORT int        GblTestScenario_exec (GBL_SELF, int argc, const char* argv[]) GBL_NOEXCEPT;
//! @}

/*! \name  Results
 *  \brief Routines for querying the overall results.
 *  @{
 */
//! Returns whether or not the given GblTestScenario has run.
GBL_EXPORT GblBool GblTestScenario_ran    (GBL_CSELF) GBL_NOEXCEPT;
//! Returns whether or not the given GblTestScenario had any failing tests.
GBL_EXPORT GblBool GblTestScenario_passed (GBL_CSELF) GBL_NOEXCEPT;
//! @}

//! Signals to an executing GblTestScenario that the next error raised by the active test case is expected and should not be considered a test failure.
GBL_EXPORT void GblTestScenario_expectError (GBL_CSELF) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_TEST_SCENARIO_H
