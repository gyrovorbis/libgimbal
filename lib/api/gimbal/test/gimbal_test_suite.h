/*! \file
 *  \brief   GblTestSuite structure and related functions
 *  \ingroup testing
 *
 *  \author    2023, 2024 Falco Girgis
 *  \copyright MIT License
 */

#ifndef GIMBAL_TEST_SUITE_H
#define GIMBAL_TEST_SUITE_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_TEST_SUITE_TYPE            (GBL_TYPEID(GblTestSuite))            //!< GblType UUID
#define GBL_TEST_SUITE(self)           (GBL_CAST(GblTestSuite, self))        //!< GblInstance cast operator
#define GBL_TEST_SUITE_CLASS(klass)    (GBL_CLASS_CAST(GblTestSuite, klass)) //!< GblClass cast operator
#define GBL_TEST_SUITE_GET_CLASS(self) (GBL_CLASSOF(GblTestSuite, self))     //!< GblInstance to GblClass operator
//! @}

//! Convenience macro for NULL-terminating the test case list
#define GBL_TEST_CASE_LAST  { GBL_NULL, GBL_NULL }

#define GBL_SELF_TYPE       GblTestSuite

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblContext);
GBL_FORWARD_DECLARE_STRUCT(GblTestSuite);
GBL_FORWARD_DECLARE_STRUCT(GblTestScenario);

/*! Function signature for a single test case
 *
 *  \param pSelf    The owning GblTestSuite object pointer.
 *  \param pCtx     The GblContext object for the test suite.
 *
 *  \returns        The result of the unit test case.
 */
typedef GBL_RESULT (*GblTestCaseFn)(GBL_SELF, GblContext* pCtx);

//! Represenst a single test case within a GblTestSuite
typedef struct GblTestCase {
    const char*   pName;     //!< Name of the test case
    GblTestCaseFn pFnRun;    //!< Function callback for the test case
} GblTestCase;

/*! Virtual table structure for a GblTestSuiteClass
 *
 *  External virtual table for providing custom initializers and
 *  finalizers along with a default list of test-cases to add to
 *  a GblTestSuiteClass without having to create a unique class for
 *  every semi-custom test suite behavior.
 *
 *  \sa GblTestSuiteClass
 */
typedef struct GblTestSuiteVTable {
    //! Called to set up the test suite before any test cases are run.
    GBL_RESULT (*pFnSuiteInit) (GBL_SELF, GblContext* pCtx);
    //! Called to finalize/clean-up the test suite after all test cases are run.
    GBL_RESULT (*pFnSuiteFinal)(GBL_SELF, GblContext* pCtx);
    //! Called before every individual test-case to do generic setup.
    GBL_RESULT (*pFnCaseInit)  (GBL_SELF, GblContext* pCtx);
    //! Called after every individual test-case to do generic cleanup.
    GBL_RESULT (*pFnCaseFinal) (GBL_SELF, GblContext* pCtx);
    //! Pointer to a NULL-terminated array of test cases to add to the suite.
    const GblTestCase* pCases;
} GblTestSuiteVTable;

/*! \struct  GblTestSuiteClass
 *  \extends GblObjectClass
 *  \brief   GblClass structure for GblTestSuite
 *
 *  Provides virtual methods for customizing/reimplementing
 *  a test suite's per-case logic.
 *
 *  \sa GblTestSuiteVTable, GblTestSuite
 */
GBL_CLASS_DERIVE(GblTestSuite, GblObject)
    //! Pointer to external virtual method table + default test case array.
    const GblTestSuiteVTable* pVTable;
    //! Virtual method for retrieving the test suite's name.
    GBL_RESULT (*pFnSuiteName)(GBL_CSELF, const char** ppName);
    //! Virtual method for retrieving the number of test cases within the suite.
    GBL_RESULT (*pFnCaseCount)(GBL_CSELF, size_t *pSize);
    //! Virtual method for retrieving the name of a test case by index.
    GBL_RESULT (*pFnCaseName) (GBL_CSELF, size_t index, const char** ppName);
    //! Virtual methodf or executing the test case at a particular index.
    GBL_RESULT (*pFnCaseRun)  (GBL_SELF, GblContext* pCtx, size_t index);
GBL_CLASS_END

/*! \struct  GblTestSuite
 *  \extends GblObject
 *  \brief   GblObject representing a collection of unit test cases
 *  \ingroup testing
 *
 *  GblTestSuite is a GblInstance type which encapsulates a single
 *  test suite--a collection of sequentially executing, related
 *  test cases.
 *
 *  \sa GblTestSuiteClass
 */
GBL_INSTANCE_DERIVE(GblTestSuite, GblObject)
    GblCallRecord lastRecord;   //!< Call record of the failing test case
    size_t        failingCase;  //!< Failing test case index
    size_t        casesRun;     //!< Number of test cases which have run
    size_t        casesPassed;  //!< Number of test cases which have passed
    size_t        casesFailed;  //!< Number of test cases which have failed
    size_t        casesSkipped; //!< Number of test cases which have been skipped
GBL_INSTANCE_END

//! \cond
GBL_PROPERTIES(GblTestSuite,
    (runResult,      GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (lastMessage,    GBL_GENERIC, (READ, SAVE), GBL_STRING_TYPE),
    (failureCase,    GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (caseCount,      GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesRun,       GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesPassed,    GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesFailed,    GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE),
    (casesSkipped,   GBL_GENERIC, (READ, SAVE), GBL_UINT32_TYPE)
)
//! \endcond

//! Returns the GblType UUID associated with GblTestSuite
GBL_EXPORT GblType GblTestSuite_type     (void) GBL_NOEXCEPT;
//! Registers a new GblType deriving from GBL_TEST_SUITE_TYPE
GBL_EXPORT GblType GblTestSuite_register (const char*               pName,
                                          const GblTestSuiteVTable* pVTable,
                                          size_t                    instanceSize,
                                          size_t                    instancePrivateSize,
                                          GblFlags                  typeFlags) GBL_NOEXCEPT;

/** \name Test Suite Creation
 *  \brief Methods for managing test suite lifetimes.
 *  @{
 */
//! Creates a new GblTestSuite instance with the given properties (or defaults if not specified).
GBL_EXPORT GblTestSuite* GblTestSuite_create   (GblType                   type,
                                                const char*               pName  /*=NULL*/,
                                                const GblTestSuiteVTable* pVTable/*=NULL*/,
                                                size_t                    size   /*=0*/,
                                                GblTestSuiteClass*        pClass /*=NULL*/) GBL_NOEXCEPT;
//! Returns a new reference to the GblTestSuite, incrementing its reference count by 1.
GBL_EXPORT GblTestSuite* GblTestSuite_ref      (GBL_SELF) GBL_NOEXCEPT;
//! Decrements the refcount of the given GblTestSuite by 1, destroying it if it reaches 0.
GBL_EXPORT GblRefCount   GblTestSuite_unref    (GBL_SELF) GBL_NOEXCEPT;
//! @}

/** \name  Test Suite Properties
 *  \brief Methods providing accessors for test suite properties.
 * @{
 */
//! Returns the string name of the test suite.
GBL_EXPORT const char*      GblTestSuite_name     (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the test suite has been run and GBL_FALSE otherwise.
GBL_EXPORT GblBool          GblTestSuite_ran      (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the test suite passed and GBL_FALSE otherwise.
GBL_EXPORT GblBool          GblTestSuite_passed   (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the parent test scenario object or NULL if there isn't one.
GBL_EXPORT GblTestScenario* GblTestSuite_scenario (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the top-level aggregate result for the overall test suite.
GBL_EXPORT GBL_RESULT       GblTestSuite_result   (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/** \name  Test Case Registration
 *  \brief Methods for adding test cases to the suite.
 *  @{
 */
//! Enqueues a single test case into the given test suite.
GBL_EXPORT GBL_RESULT GblTestSuite_addCase  (GBL_SELF,
                                             const char*   pName,
                                             GblTestCaseFn pFnTest)     GBL_NOEXCEPT;
//! Enqueues an array of cases, terminated by a final entry of all NULL values.
GBL_EXPORT GBL_RESULT GblTestSuite_addCases (GBL_SELF,
                                             const GblTestCase* pCases) GBL_NOEXCEPT;
//! @}

/** \name  Test Case Accessors
 *  \brief Methods for accessing test-case properties.
 *  @{
 */
//! Returns whether the test case with the given name has run.
GBL_EXPORT GblBool     GblTestSuite_caseRan    (GBL_CSELF, const char* pCaseName) GBL_NOEXCEPT;
//! Returns whether the test case with the given name has passed.
GBL_EXPORT GblBool     GblTestSuite_casePassed (GBL_CSELF, const char* pCaseName) GBL_NOEXCEPT;
//! Returns the number of test cases within the given test suite.
GBL_EXPORT size_t      GblTestSuite_caseCount  (GBL_CSELF)                        GBL_NOEXCEPT;
//! Returns the name of the test case at the given index within the test suite.
GBL_EXPORT const char* GblTestSuite_caseName   (GBL_CSELF, size_t index)          GBL_NOEXCEPT;
//! Returns the index of the test case with the given name within the test suite.
GBL_EXPORT size_t      GblTestSuite_caseIndex  (GBL_CSELF, const char* pCaseName) GBL_NOEXCEPT;
//! Returns the result of the test case at the given index within the test suite.
GBL_EXPORT GBL_RESULT  GblTestSuite_caseResult (GBL_CSELF, size_t index)          GBL_NOEXCEPT;
//! @}

/** \name  Test Execution
 *  \brief Methods for executing the test suite.
 *  @{
*/
//! Calls the top-level initialization function for the entire suite.
GBL_EXPORT GBL_RESULT GblTestSuite_initSuite  (GBL_SELF, GblContext* pCtx) GBL_NOEXCEPT;
//! Calls the top-level finalization function for the entire suite.
GBL_EXPORT GBL_RESULT GblTestSuite_finalSuite (GBL_SELF, GblContext* pCtx) GBL_NOEXCEPT;
//! Calls the case-level initialization function for the next test case.
GBL_EXPORT GBL_RESULT GblTestSuite_initCase   (GBL_SELF, GblContext* pCtx) GBL_NOEXCEPT;
//! Calls the case-level finalization function for the previous test case.
GBL_EXPORT GBL_RESULT GblTestSuite_finalCase  (GBL_SELF, GblContext* pCtx) GBL_NOEXCEPT;
//! Runs the test case at \p index within the given suite, returning its result code.
GBL_EXPORT GBL_RESULT GblTestSuite_runCase    (GBL_SELF,
                                               GblContext* pCtx,
                                               size_t      index)          GBL_NOEXCEPT;
//! Skips the test case at \p index within the given suite.
GBL_EXPORT GBL_RESULT GblTestSuite_skipCase   (GBL_SELF,
                                               GblContext* pCtx,
                                               size_t      index)          GBL_NOEXCEPT;
//! @}

//! \cond
#define GblTestSuite_create(...) \
    GblTestSuite_createDefault_(__VA_ARGS__)
#define GblTestSuite_createDefault_(...) \
    GblTestSuite_createDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL, 0, GBL_NULL)
#define GblTestSuite_createDefault__(type, name, vtable, size, klass, ...) \
    (GblTestSuite_create)(type, name, vtable, size, klass)
//! \endcond

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_TEST_SUITE_H
