#include <gimbal/utils/gimbal_test_scenario.h>
#include <gimbal/utils/gimbal_test_suite.h>
#include <time.h>

#define GBL_TEST_SCENARIO_(inst)    ((GblTestScenario_*)GBL_INSTANCE_PRIVATE(inst, GBL_TEST_SCENARIO_TYPE))

typedef struct GblTestScenario_ {
    GblTestSuite*   pCurSuite;
    const char*     pCurCase;
    clock_t         startTime;
} GblTestScenario_;


GblSize GblTestScenario_suiteCount_(const GblTestScenario* pSelf) {
    return pSelf? GblObject_childCount(GBL_OBJECT(pSelf)) : 0;
}

static GBL_RESULT GblTestScenarioClass_begin_(GblTestScenario* pSelf) {
    GBL_API_BEGIN(pSelf);
    const char* pName = GblObject_name(GBL_OBJECT(pSelf));
    GBL_API_INFO("[GblTestScenario] Beginning Scenario: [%s]", pName? pName : "");
    GBL_API_PUSH();
    GBL_API_END();
}


static GBL_RESULT GblTestScenarioClass_end_(GblTestScenario* pSelf) {
    GBL_API_BEGIN(pSelf);
    const char* pName = GblObject_name(GBL_OBJECT(pSelf));
    GBL_API_INFO("[GblTestScenario] Ending Scenario: [%s]", pName? pName : "");
    GBL_API_PUSH();

    GBL_API_INFO("Test Suite Totals");
    GBL_API_PUSH();
    GBL_API_INFO("%-20s: %20u", "Total",   GblTestScenario_suiteCount_(pSelf));
    GBL_API_INFO("%-20s: %20u", "Passed",  pSelf->suitesPassed);
    GBL_API_INFO("%-20s: %20u", "Skipped", pSelf->suitesSkipped);
    GBL_API_INFO("%-20s: %20u", "Failed",  pSelf->suitesFailed);
    GBL_API_POP(1);

    GBL_API_INFO("Test Case Totals");
    GBL_API_PUSH();
    GBL_API_INFO("%-20s: %20u", "Total",   pSelf->caseCount);
    GBL_API_INFO("%-20s: %20u", "Passed",  pSelf->casesPassed);
    GBL_API_INFO("%-20s: %20u", "Skipped", pSelf->casesSkipped);
    GBL_API_INFO("%-20s: %20u", "Failed",  pSelf->casesFailed);
    GBL_API_POP(1);

    GBL_API_INFO("%-20s: %20s", "Result",   gblResultString(pSelf->result));

    GBL_API_POP(1);
    GBL_API_END();
}


static GBL_RESULT GblTestScenarioClass_run_(GblTestScenario* pSelf, int argc, char* argv[]) {
    GBL_UNUSED(argc && argv);
    GBL_API_BEGIN(pSelf);

    GblContext* pCtx = GblObject_contextFind(GBL_OBJECT(pSelf));
    GblTestScenarioClass* pClass = GBL_TEST_SCENARIO_GET_CLASS(pSelf);
    GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pSelf);

    pSelf->result = GBL_RESULT_SUCCESS;
    GBL_API_VERIFY_CALL(pClass->pFnBegin(pSelf));

    for(GblTestSuite* pSuiteIt = GBL_TEST_SUITE_TRY(GblObject_childFirst(GBL_OBJECT(pSelf)));
        pSuiteIt != NULL;
        pSuiteIt = GBL_TEST_SUITE_TRY(GblObject_siblingNext(GBL_OBJECT(pSuiteIt))))
    {
        pSelf_->pCurSuite = pSuiteIt;
        pSelf->result = GBL_API_RESULT();

        GBL_API_VERIFY_CALL(pClass->pFnSuiteBegin(pSelf, pSuiteIt));

        GBL_API_CALL(GblTestSuite_suiteInit(pSuiteIt, pCtx));

        if(GBL_RESULT_ERROR(GBL_API_RESULT())) {
            GBL_API_ERROR("[GblTestSuite] Failed to initailize test suite[%s], skipping: %s",
                          GblTestSuite_name(pSuiteIt),
                          GBL_API_RECORD().message);

            ++pSelf->suitesSkipped;

        } else {

            ++pSelf->suitesRun;
            const GblSize caseCount = GblTestSuite_caseCount(pSuiteIt);
            GblBool suiteFailed = GBL_FALSE;

            for(GblSize idx = 0; idx < caseCount; ++idx) {

                pSelf_->pCurCase = GblTestSuite_caseName(pSuiteIt, idx);

                GBL_API_CALL(GblTestSuite_caseInit(pSuiteIt, pCtx));

                if(GBL_RESULT_ERROR(GBL_API_RESULT())) {
                    GBL_API_ERROR("[GblTestSuite] Failed to initailize test case[%s], skipping: %s",
                                  pSelf_->pCurCase,
                                  GBL_API_RECORD().message);
                    ++pSelf->casesSkipped;
                    suiteFailed = GBL_TRUE;
                } else {

                    ++pSelf->casesRun;
                    GBL_RESULT result = GblTestSuite_caseRun(pSuiteIt, pCtx, idx);
                    if(result == GBL_RESULT_SKIPPED) {
                        ++pSelf->casesSkipped;
                        GBL_API_INFO("%-12s: %s::%s", "[ SKIPPED ]",
                                     GblTestSuite_name(pSelf_->pCurSuite),
                                     pSelf_->pCurCase);
                    } else if(!GBL_RESULT_ERROR(result)) {
                        ++pSelf->casesPassed;
                        GBL_API_INFO("%-12s: %s::%s", "[ PASSED ]",
                                     GblTestSuite_name(pSelf_->pCurSuite),
                                     pSelf_->pCurCase);
                    } else {
                        ++pSelf->casesFailed;
                        GBL_API_INFO("%-12s: %s::%s", "[ FAILED ]",
                                     GblTestSuite_name(pSelf_->pCurSuite),
                                     pSelf_->pCurCase);
                        suiteFailed = GBL_TRUE;
                        break;
                    }

                    GBL_API_CALL(GblTestSuite_caseFinal(pSuiteIt, pCtx));
                    if(GBL_RESULT_ERROR(GBL_API_RESULT())) {
                        GBL_API_ERROR("[GblTestSuite] Failed to finalize test case[%s]: %s",
                                      pSelf_->pCurCase,
                                      GBL_API_RECORD().message);
                        suiteFailed = GBL_TRUE; // log as failed suite, but not case, continue
                    }
                }
            }

            GBL_API_CALL(GblTestSuite_suiteFinal(pSuiteIt, pCtx));

            if(GBL_RESULT_ERROR(GBL_API_RESULT())) {
                GBL_API_ERROR("[GblTestSuite] Failed to finalize test suite[%s], skipping: %s",
                              GblTestSuite_name(pSuiteIt),
                              GBL_API_RECORD().message);

                suiteFailed = GBL_TRUE;
            }

            if(!suiteFailed) ++pSelf->suitesPassed;
            else ++pSelf->suitesFailed;

            GBL_API_CALL(pClass->pFnSuiteEnd(pSelf, pSuiteIt));
        }
    }

    GBL_API_CALL(pClass->pFnEnd(pSelf));

    GBL_API_END();
}


static GBL_RESULT GblTestScenarioClass_suiteBegin_(GblTestScenario* pSelf, const GblTestSuite* pSuite) {
    GBL_API_BEGIN(pSelf);
    const char* pSuiteName = GblTestSuite_name(pSuite);
    GBL_API_INFO("********* Starting TestSuite [%s] *********", pSuiteName);
    GBL_TEST_SCENARIO_(pSelf)->startTime = clock();
    GBL_API_END();
}


static GBL_RESULT GblTestScenarioClass_suiteEnd_(GblTestScenario* pSelf, const GblTestSuite* pSuite) {
    GBL_API_BEGIN(pSelf);
    const char* pSuiteName = GblTestSuite_name(pSuite);
    const double msec = (double)(clock() - GBL_TEST_SCENARIO_(pSelf)->startTime) * 1000.0 / (double)CLOCKS_PER_SEC;
    GBL_API_INFO("Totals: %u passed, %u failed, %u skipped, %.3fms",
                 pSuite->casesPassed,
                 pSuite->casesFailed,
                 pSuite->casesSkipped,
                 msec);
    GBL_API_INFO("********* Finished TestSuite [%s] *********", pSuiteName);
    GBL_API_END();
}

static GBL_RESULT GblTestScenarioClass_propertyGet_(const GblObject* pSelf, GblSize id, GblVariant* pValue, const GblProperty* pProp) {
    GBL_API_BEGIN(pSelf);
    GblTestScenario* pScenario = GBL_TEST_SCENARIO(pSelf);
    switch(id) {
    case GBL_TEST_SCENARIO_PROPERTY_ID_RESULT:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pScenario->result);
        break;
    case GBL_TEST_SCENARIO_PROPERTY_ID_SUITE_COUNT:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), GblTestScenario_suiteCount_(pScenario));
        break;
    case GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_RUN:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pScenario->suitesRun);
        break;
    case GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_PASSED:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pScenario->suitesPassed);
        break;
    case GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_FAILED:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pScenario->suitesFailed);
        break;
    case GBL_TEST_SCENARIO_PROPERTY_ID_SUITES_SKIPPED:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pScenario->suitesSkipped);
        break;
    case GBL_TEST_SCENARIO_PROPERTY_ID_CASE_COUNT:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pScenario->caseCount);
        break;
    case GBL_TEST_SCENARIO_PROPERTY_ID_CASES_RUN:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pScenario->casesRun);
        break;
    case GBL_TEST_SCENARIO_PROPERTY_ID_CASES_PASSED:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pScenario->casesPassed);
        break;
    case GBL_TEST_SCENARIO_PROPERTY_ID_CASES_FAILED:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pScenario->casesFailed);
        break;
    case GBL_TEST_SCENARIO_PROPERTY_ID_CASES_SKIPPED:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pScenario->casesSkipped);
        break;
    default: GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "[GblTestScenario] Reading unhandled property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_API_END();
}



static GBL_RESULT GblTestScenarioClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);

    if(!GblType_classRefCount(GBL_TEST_SCENARIO_TYPE)) {
        GBL_PROPERTY_TABLE_REGISTER(GBL_TEST_SCENARIO, pClass);
    }

    GblTestScenarioClass* pSelfClass = GBL_TEST_SCENARIO_CLASS(pClass);
    pSelfClass->pFnBegin            = GblTestScenarioClass_begin_;
    pSelfClass->pFnEnd              = GblTestScenarioClass_end_;
    pSelfClass->pFnRun              = GblTestScenarioClass_run_;
    pSelfClass->pFnSuiteBegin       = GblTestScenarioClass_suiteBegin_;
    pSelfClass->pFnSuiteEnd         = GblTestScenarioClass_suiteEnd_;
    pSelfClass->base.pFnPropertyGet = GblTestScenarioClass_propertyGet_;
    GBL_API_END();
}

GBL_EXPORT GblType GblTestScenario_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo typeInfo = {
        .pFnClassInit           = GblTestScenarioClass_init_,
        .classSize              = sizeof(GblTestScenarioClass),
        .instanceSize           = sizeof(GblTestScenario),
        .instancePrivateSize    = sizeof(GblTestScenario_)
    };

    if(type == GBL_INVALID_TYPE) {

        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GBL_OBJECT_TYPE,
                                      GblQuark_internStringStatic("TestScenario"),
                                      &typeInfo,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);

        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();

    }
    return type;

}

GBL_EXPORT GblTestScenario* GblTestScenario_create(const char* pName) {
    GblTestScenario* pScenario = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pName);
    pScenario = GBL_TEST_SCENARIO(GblObject_new(GBL_TEST_SCENARIO_TYPE,
                                                "name", pName,
                                                NULL));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return pScenario;
}

GBL_EXPORT GBL_RESULT GblTestScenario_destroy(GblTestScenario* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);

    // not properly disposing of shit anything beyond the first entry!
    for(GblObject* pIt = GblObject_childFirst(GBL_OBJECT(pSelf));
        pIt != NULL;
        pIt = GblObject_siblingNext(pIt))
    {
        if(GblObject_unref(pIt) != 0) {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                               "[GblTestSuite] Destroy: Leaking unexpected existing references!");
        }
    }

    GBL_API_VERIFY(GblObject_unref(GBL_OBJECT(pSelf)) == 0,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "[GblTestScenario] Destroy: Leaking unexpected existing references!");
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestScenario_suiteEnqueue(GblTestScenario* pSelf, const GblTestSuite* pSuite) {
    GBL_API_BEGIN(pSelf);
    GblObject_childAdd(GBL_OBJECT(pSelf), GBL_OBJECT(pSuite));
    pSelf->caseCount += GblTestSuite_caseCount(pSuite);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GblTestSuite* GblTestScenario_suiteFind(const GblTestScenario* pSelf, const char* pName) {
    GblTestSuite* pSuite = NULL;
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY_POINTER(pName);
    pSuite = GBL_TEST_SUITE_TRY(GblObject_childFindByName(GBL_OBJECT(pSelf), pName));
    GBL_API_END_BLOCK();
    return pSuite;
}

GBL_EXPORT GblTestSuite* GblTestScenario_suiteCurrent(const GblTestScenario* pSelf) {
    return pSelf? GBL_TEST_SCENARIO_(pSelf)->pCurSuite : NULL;
}

GBL_EXPORT const char* GblTestScenario_caseCurrent(const GblTestScenario* pSelf) {
    return pSelf? GBL_TEST_SCENARIO_(pSelf)->pCurCase : NULL;
}

GBL_EXPORT GBL_RESULT GblTestScenario_run(GblTestScenario* pSelf, int argc, char* argv[]) {
    GBL_API_BEGIN(pSelf);
    GBL_INSTANCE_VCALL_PREFIX(GBL_TEST_SCENARIO, pFnRun, pSelf, argc, argv);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GblBool GblTestScenario_ran(const GblTestScenario* pSelf) {
    return pSelf && pSelf->result != GBL_RESULT_UNKNOWN;
}

GBL_EXPORT GblBool GblTestScenario_passed(const GblTestScenario* pSelf) {
    return GblTestScenario_ran(pSelf) && !GBL_RESULT_ERROR(pSelf->result);
}
