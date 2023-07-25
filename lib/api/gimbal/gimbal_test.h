/*! \file
 *  \brief Test module-wide include
 *  \ingroup testing
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_TEST_H
#define GIMBAL_TEST_H

#include "test/gimbal_test_macros.h"
#include "test/gimbal_test_scenario.h"
#include "test/gimbal_test_suite.h"

/*! \defgroup testing Unit Testing
    \brief Object-oriented, C-based unit testing framework.

    LibGimbal provides a built-in unit testing framework, which was architecturally inspired by QTest (which we had originally used for testing), with output
    similar to a combination between QTest and GTest. Since libGimbal runs everywhere, we wanted to ensure that our tests could also run anywhere as well.
    The testing framework leverages the meta type system and error management back-end to implement a powerful object-oriented testing framework. We use it
    extensively on both desktop enironments and for embedded platforms such as the Sega Dreamcast and Sony PSVita.

    A unit test program typically comprises a single top-level GblTestScenario object, to which GblTestSuite objects are added. A GblTestSuite represents a
    sequentially-executing set of test cases.

    \tableofcontents

    ##Features
    - Timing
    - Memory utilization
    - Memory leak tracing
    - Dynamic test construction/scheduling

    ## Code Samples

    ### Sample Test Case
    \code{.c}
        static GBL_RESULT GblUuidTestSuite_initV4_(GblTestSuite* pSelf, GblContext* pCtx) {
            GBL_UNUSED(pSelf);
            GBL_API_BEGIN(pCtx);

            GblUuid uuids[GBL_UUID_TEST_SUITE__V4_COUNT];

            for(int u = 0; u < GBL_UUID_TEST_SUITE__V4_COUNT; ++u) {
                GBL_API_VERIFY_CALL(GblUuid_genV4(&uuids[u]));

                GBL_TEST_COMPARE(GblUuid_version(&uuids[u]), 4);
                GBL_TEST_VERIFY(!GblUuid_isNil(&uuids[u]));

                for(int u2 = 0; u2 < u-1; ++u2) {
                    GBL_TEST_VERIFY(GblUuid_compare(&uuids[u], &uuids[u2]) != 0);
                }

                GBL_TEST_COMPARE(GblUuid_compare(&uuids[u], &uuids[u]), 0);
            }

            GBL_API_END();
        }
    \endcode

    ### Registering a Test Suite
    \code{.c}
        GBL_EXPORT GblType GblUuidTestSuite_type(void) {
            static GblType type = GBL_INVALID_TYPE;

            const static GblTestCase cases[] = {
                { "versionInvalid",         GblUuidTestSuite_versionInvalid_        },
                { "compareInvalid",         GblUuidTestSuite_compareInvalid_        },
                { "isNilInvalid",           GblUuidTestSuite_isNilInvalid_          },
                { "initNilInvalid",         GblUuidTestSuite_initNilInvalid_        },
                { "initNil",                GblUuidTestSuite_initNil_               },
                { "initV4",                 GblUuidTestSuite_initV4_                },
                { "initV4Invalid",          GblUuidTestSuite_initV4Invalid_         },
                { "initFromStringInvalid",  GblUuidTestSuite_initFromStringInvalid_ },
                { "initFromString",         GblUuidTestSuite_initFromString_        },
                { "toString",               GblUuidTestSuite_toString_              },
                { NULL,                     NULL                                    }
            };

            const static GblTestSuiteVTable vTable = {
                .pCases         = cases
                .pFnSuiteInit   = GblUuidTestSuite_init_, // any initialization logic before first test case runs
               . pFnSuiteFinal  = GblUuidTestSuite_final_ // any finalization logic after last test case runs
            };

            if(type == GBL_INVALID_TYPE) {
                type = GblTestSuite_register(GblQuark_internStringStatic("UuidTestSuite"),
                                             &vTable,
                                             sizeof(GblUuidTestSuite),
                                             0,
                                             GBL_TYPE_FLAGS_NONE);
            }
            return type;
        }
    \endcode
    ### Running A Test Scenario

    \code{.c}
        int main(int argc, char* pArgv[]) {
            // Create top-level test scenario
            GblTestScenario* pScenario = GblTestScenario_create("LibGimbalTests");
            // Set the log filter mask
            GblContext_setLogFilter(GBL_CONTEXT(pScenario), GBL_LOG_LEVEL_INFO | GBL_LOG_LEVEL_WARNING | GBL_LOG_LEVEL_ERROR );
            // Create a single test suite (its constructor registers itstest cases)
            GblUuidTestSuite* pSuite = GBL_OBJECT_CREATE(GblUuidTestSuite);
            // Enqueue the test suite
            GblTestScenario_enqueueSuite(pScenario, GBL_TEST_SUITE(pSuite));
            // Run the test scenario (which runs its test suites (which run their test cases))
            const GBL_RESULT result = GblTestScenario_run(pScenario, argc, pArgv);
            // Clean up after ourselves
            GblTestScenario_destroy(pScenario);
            // Check result
            return GBL_RESULT_SUCCESS(result)? 0 : -1;
        }
    \endcode

    ## Sample Output
    ### Completing a GblTestSuite:

        * ********* Starting TestSuite [RefTestSuite] *********
        * [ INIT      ]: RefTestSuite
        * [ RUN       ]: RefTestSuite::allocEmpty
        * [      PASS ]: RefTestSuite::allocEmpty (0.011 ms)
        * [ RUN       ]: RefTestSuite::alloc
        * [      PASS ]: RefTestSuite::alloc (0.016 ms)
        * [ RUN       ]: RefTestSuite::allocWithContext
        * [      PASS ]: RefTestSuite::allocWithContext (0.012 ms)
        * [ RUN       ]: RefTestSuite::acquire
        * [      PASS ]: RefTestSuite::acquire (0.010 ms)
        * [ RUN       ]: RefTestSuite::release
        * [      PASS ]: RefTestSuite::release (0.007 ms)
        * [ RUN       ]: RefTestSuite::destructFail
        * [      PASS ]: RefTestSuite::destructFail (0.016 ms)
        * [ RUN       ]: RefTestSuite::destruct
        * [      PASS ]: RefTestSuite::destruct (0.018 ms)
        * [ FINAL     ]: RefTestSuite
        * Totals: 7 passed, 0 failed, 0 skipped, 0.090ms, 0/2 leaked (0/544 bytes)
        * ********* Finished TestSuite [RefTestSuite] *********

    ### Completing the GblTestScenario:

        * [GblTestScenario] Ending Scenario: [libGimbalTests]
                * Test Suite Totals
                        * Total               :                   24
                        * Passed              :                   24
                        * Skipped             :                    0
                        * Failed              :                    0
                * Test Case Totals
                        * Total               :                  429
                        * Passed              :                  429
                        * Skipped             :                    0
                        * Failed              :                    0
                * Total Time          :               67.737ms
        * ********************* [   PASS   ] *********************

*/

#endif // GIMBAL_TEST_H
