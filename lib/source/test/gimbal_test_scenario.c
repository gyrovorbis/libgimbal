#include <gimbal/test/gimbal_test_scenario.h>
#include <gimbal/test/gimbal_test_suite.h>
#include <gimbal/utils/gimbal_timer.h>
#include <gimbal/utils/gimbal_cmd_parser.h>
#include <gimbal/algorithms/gimbal_random.h>
#include <gimbal/allocators/gimbal_allocation_tracker.h>
#include <gimbal/meta/signals/gimbal_marshal.h>
#include <gimbal/meta/classes/gimbal_enum.h>
#include <gimbal/containers/gimbal_array_list.h>
#include <gimbal/containers/gimbal_array_map.h>
#include <gimbal/strings/gimbal_string_ref.h>
#include <time.h>

static const char* GBL_TERM_GREEN_  = "\x1b[32m";
static const char* GBL_TERM_YELLOW_ = "\x1b[33m";
static const char* GBL_TERM_RED_    = "\x1b[31m";
static const char* GBL_TERM_GRAY_   = "\x1b[2m";
static const char* GBL_TERM_BLINK_  = "\x1b[5m";
static const char* GBL_TERM_RESET_  = "\x1b[0m";

#define GBL_TEST_SCENARIO_(inst)    (GBL_PRIVATE(GblTestScenario, inst))

typedef struct GblTestResultCount_ {
    size_t count;
    size_t suiteStart;
} GblTestResultCount_;

typedef struct GblTestResultGroupKey_ {
    GblQuark name;
    GblType  enumType;
} GblTestResultGroupKey_;

typedef struct GblTestResultGroup_ {
    GblTestResultGroupKey_ key;
    GblEnumClass*          pEnumClass;
    GblArrayList           counts;
} GblTestResultGroup_;

typedef struct GblTestFailure_ {
    const GblTestSuite* pSuite;
    const char*         pCaseName;
    const char*         pPhase;
    GblCallRecord       record;
} GblTestFailure_;

typedef struct GblTestScenario_ {
    GblAllocationTracker*   pAllocTracker;
    GblTestSuite*           pCurSuite;
    size_t                  curCase;
    double                  suiteMs;
    GblAllocationCounters   suiteAllocCounters;
    GblBool                 runningCase;
    GblBool                 expectError;
    GblBool                 enableColor;
    GblArrayMap*            pResultGroups;
    GblArrayList            failures;
} GblTestScenario_;

static int GblTestScenario_resultGroupCompare_(const GblArrayMap* pMap,
                                               uintptr_t          key1,
                                               uintptr_t          key2) {
    GBL_UNUSED(pMap);

    const GblTestResultGroupKey_* pKey1 = (const GblTestResultGroupKey_*)key1;
    const GblTestResultGroupKey_* pKey2 = (const GblTestResultGroupKey_*)key2;

    if(pKey1->name != pKey2->name)
        return (pKey1->name < pKey2->name)? -1 : 1;

    return (pKey1->enumType < pKey2->enumType)? -1 :
           (pKey1->enumType > pKey2->enumType);

}

static GblTestResultGroup_* GblTestScenario_resultGroup_(const GblTestScenario* pSelf,
                                                         GblQuark               name,
                                                         GblType                enumType) {
    const GblTestResultGroupKey_ key = { name, enumType };
    return (GblTestResultGroup_*)GblArrayMap_getValue(&GBL_TEST_SCENARIO_(pSelf)->pResultGroups, (uintptr_t)&key);
}

static GBL_RESULT GblTestScenario_resultGroupDestruct_(const GblArrayMap* pMap,
                                                       uintptr_t          key,
                                                       void*              pData) {
    GBL_UNUSED(pMap, key);
    GBL_CTX_BEGIN(NULL);

    GblTestResultGroup_* pGroup = pData;

    GBL_CTX_CALL(GblArrayList_destruct(&pGroup->counts));
    GblClass_unrefDefault(GBL_CLASS(pGroup->pEnumClass));

    GBL_CTX_FREE(pGroup);
    GBL_CTX_END();
}

static GBL_RESULT GblTestScenario_logResults_(GblTestScenario* pSelf, GblBool suite) {
    GBL_CTX_BEGIN(pSelf);
    const GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pSelf);

    for(size_t groupIdx = 0; groupIdx < GblArrayMap_size(&pSelf_->pResultGroups); ++groupIdx) {
        const GblTestResultGroup_* pGroup = (GblTestResultGroup_*)GblArrayMap_probeValue(&pSelf_->pResultGroups, groupIdx);
        size_t total = 0;

        for(uint16_t valueIdx = 0; valueIdx < pGroup->pEnumClass->entryCount; ++valueIdx) {
            const GblTestResultCount_* pCount = GblArrayList_at(&pGroup->counts, valueIdx);
            total += pCount->count - (suite? pCount->suiteStart : 0);
        }

        if(suite && !total) continue;

        if(suite) {
            GblStringBuffer summary;
            GblStringBuffer_construct(&summary);
            GblStringBuffer_appendPrintf(&summary, "%s: %zu total", GblQuark_toString(pGroup->key.name), total);

            for(uint16_t valueIdx = 0; valueIdx < pGroup->pEnumClass->entryCount; ++valueIdx) {
                const char* pLabel = GblEnumClass_nickFromIndex(pGroup->pEnumClass, valueIdx);
                if(!pLabel || !pLabel[0])
                    pLabel = GblEnumClass_nameFromIndex(pGroup->pEnumClass, valueIdx);
                const GblTestResultCount_* pCount = GblArrayList_at(&pGroup->counts, valueIdx);
                GblStringBuffer_appendPrintf(&summary, ", %zu %s", pCount->count - pCount->suiteStart, pLabel);
            }

            GBL_CTX_INFO("%s", GblStringBuffer_cString(&summary));
            GblStringBuffer_destruct(&summary);
            continue;
        }

        GBL_CTX_INFO("%s", GblQuark_toString(pGroup->key.name));
        GBL_CTX_PUSH();
        GBL_CTX_INFO("%-20s: %20zu", "Total", total);

        for(uint16_t valueIdx = 0; valueIdx < pGroup->pEnumClass->entryCount; ++valueIdx) {
            const char* pLabel = GblEnumClass_nickFromIndex(pGroup->pEnumClass, valueIdx);

            if(!pLabel || !pLabel[0])
                pLabel = GblEnumClass_nameFromIndex(pGroup->pEnumClass, valueIdx);

            const GblTestResultCount_* pCount = GblArrayList_at(&pGroup->counts, valueIdx);

            GBL_CTX_INFO("%-20s: %20zu", pLabel, pCount->count);
        }

        GBL_CTX_POP(1);
    }

    GBL_CTX_END();
}

static GBL_RESULT GblTestScenario_recordFailure_(GblTestScenario*     pSelf,
                                                 const GblTestSuite*  pSuite,
                                                 const char*          pCaseName,
                                                 const char*          pPhase,
                                                 const GblCallRecord* pRecord) {
    GBL_CTX_BEGIN(pSelf);

    GblTestFailure_ failure = { .pSuite = pSuite, .pCaseName = pCaseName, .pPhase = pPhase };
    if(pRecord) failure.record = *pRecord;

    GBL_CTX_VERIFY_CALL(GblArrayList_pushBack(&GBL_TEST_SCENARIO_(pSelf)->failures, &failure));
    GBL_CTX_END();
}


size_t  GblTestScenario_suiteCount_(const GblTestScenario* pSelf) {
    return pSelf? GblObject_childCount(GBL_OBJECT(pSelf)) : 0;
}

static GBL_RESULT GblTestScenarioClass_begin_(GblTestScenario* pSelf) {
    GBL_CTX_BEGIN(pSelf);
    const char* pName = GblObject_name(GBL_OBJECT(pSelf));
    GBL_CTX_INFO("[GblTestScenario] Beginning Scenario: [%s]", pName? pName : "");
    GBL_CTX_PUSH();
    GblContext_logBuildInfo(GBL_CONTEXT(pSelf));

    GBL_EMIT(pSelf, "began");

    GBL_CTX_END();
}

static GBL_RESULT GblTestScenarioClass_end_(GblTestScenario* pSelf) {
    GBL_CTX_BEGIN(pSelf);
    const char* pName = GblObject_name(GBL_OBJECT(pSelf));
    GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pSelf);
    const char* pGreen  = (pSelf_->enableColor)? GBL_TERM_GREEN_ : "";
    const char* pRed    = (pSelf_->enableColor)? GBL_TERM_RED_   : "";
    const char* pBlink  = (pSelf_->enableColor)? GBL_TERM_BLINK_ : "";
    const char* pReset  = (pSelf_->enableColor)? GBL_TERM_RESET_ : "";
    GBL_CTX_INFO("[GblTestScenario] Ending Scenario: [%s]", pName? pName : "");
    GBL_CTX_PUSH();

    GBL_CTX_INFO("Runtime Statistics");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %20.3f", "Total Time (ms)",     pSelf->totalTime);
    GBL_CTX_INFO("%-20s: %20u",   "Max Allocations",     pSelf_->pAllocTracker->maxAllocations);
    GBL_CTX_INFO("%-20s: %20u",   "Max Allocation Size", pSelf_->pAllocTracker->maxAllocationSize);
    GBL_CTX_INFO("%-20s: %20u",   "Max Allocated Bytes", pSelf_->pAllocTracker->maxBytes);
    GBL_CTX_INFO("%-20s: %20u",   "Remaining Allocs",    pSelf_->pAllocTracker->counters.allocsActive);
    GBL_CTX_INFO("%-20s: %20u",   "Remaining Bytes",     pSelf_->pAllocTracker->counters.bytesActive);
    GBL_CTX_INFO("%-20s: %20u",   "Seed",                gblSeed(0));
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Test Suite Totals");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %20u", "Total",   GblTestScenario_suiteCount_(pSelf));
    GBL_CTX_INFO("%-20s: %20u", "Passed",  pSelf->suitesPassed);
    GBL_CTX_INFO("%-20s: %20u", "Skipped", pSelf->suitesSkipped);
    GBL_CTX_INFO("%-20s: %20u", "Failed",  pSelf->suitesFailed);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Test Case Totals");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %20u", "Total",   pSelf->caseCount);
    GBL_CTX_INFO("%-20s: %20u", "Passed",  pSelf->casesPassed);
    GBL_CTX_INFO("%-20s: %20u", "Skipped", pSelf->casesSkipped);
    GBL_CTX_INFO("%-20s: %20u", "Failed",  pSelf->casesFailed);
    GBL_CTX_POP(1);

    GBL_CTX_CALL(GblTestScenario_logResults_(pSelf, GBL_FALSE));

    if(!GblArrayList_empty(&pSelf_->failures)) {
        size_t nameWidth  = 0;
        size_t phaseWidth = 0;
        for(size_t idx = 0; idx < GblArrayList_size(&pSelf_->failures); ++idx) {
            const GblTestFailure_* pFailure = GblArrayList_at(&pSelf_->failures, idx);

            size_t nameLength = strlen(GblTestSuite_name(pFailure->pSuite));
            if(pFailure->pCaseName)
                nameLength += 2 + strlen(pFailure->pCaseName);

            nameWidth  = GBL_MAX(nameWidth, nameLength);
            const size_t phaseLength = strlen(pFailure->pPhase);
            phaseWidth = GBL_MAX(phaseWidth, phaseLength);
        }

        GBL_CTX_INFO("Failures");
        GBL_CTX_PUSH();
        for(size_t idx = 0; idx < GblArrayList_size(&pSelf_->failures); ++idx) {
            const GblTestFailure_* pFailure = GblArrayList_at(&pSelf_->failures, idx);

            const char*  pSuiteName = GblTestSuite_name(pFailure->pSuite);
            const size_t nameLength = strlen(pSuiteName) +
                                      ((pFailure->pCaseName)? 2 + strlen(pFailure->pCaseName) : 0);

            const int   namePadding  = (int)(nameWidth - nameLength + 1);
            const int   phasePadding = (int)(phaseWidth - strlen(pFailure->pPhase));
            const char* pMessage     = (pFailure->record.message[0])?
                                        pFailure->record.message : gblResultString(pFailure->record.result);

            GBL_CTX_INFO("%s%s%s%*s(%s)%*s: %s", pSuiteName,
                         pFailure->pCaseName? "::" : "",
                         pFailure->pCaseName? pFailure->pCaseName : "",
                         namePadding, "", pFailure->pPhase, phasePadding, "", pMessage);
        }
        GBL_CTX_POP(1);
    }

    GBL_CTX_POP(1);

    GBL_CTX_INFO("%s%s********************* %s *********************%s", pBlink,
                 !GBL_RESULT_SUCCESS(pSelf->result)? pRed : pGreen,
                 !GBL_RESULT_SUCCESS(pSelf->result)? "[   FAIL   ]" : "[   PASS   ]",
                 pReset);

    //GBL_CTX_VERIFY_CALL(GblAllocationTracker_logActive(pSelf_->pAllocTracker));

    GBL_EMIT(pSelf, "ended");

    GBL_CTX_END();
}

static GblBool GblTestScenario_stringListIterFn_(GblStringRef* pRef, void* pClosure) {
    return GblStringView_containsIgnoreCase(GBL_STRV((const char*)pClosure), pRef);
}

static void GblTestScenario_logHelp_(const GblCmdParser* pParser) {
    GBL_CTX_BEGIN(NULL);

    GblStringBuffer help;
    GblStringBuffer_construct(&help);
    GblCmdParser_formatHelp(pParser, &help);

    GBL_CTX_INFO("%s", GblStringBuffer_cString(&help));

    GblStringBuffer_destruct(&help);
    GBL_CTX_END_BLOCK();
}

static GBL_RESULT GblTestScenarioClass_run_begin_(GblTestScenario* pSelf) {
    GBL_CTX_BEGIN(pSelf);
    GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pSelf);

    for(size_t idx = 0; idx < GblArrayMap_size(&pSelf_->pResultGroups); ++idx) {
        GblTestResultGroup_* pGroup = (GblTestResultGroup_*)GblArrayMap_probeValue(&pSelf_->pResultGroups, idx);
        GBL_CTX_VERIFY_CALL(GblArrayList_assign(&pGroup->counts, NULL,
                                                pGroup->pEnumClass->entryCount));
    }

    GBL_CTX_VERIFY_CALL(GblArrayList_resize(&pSelf_->failures, 0));
    GBL_CTX_END();
}

static GblCmdParser* GblTestScenario_createCmdParser_(const char** ppOnly,
                                                      const char** ppSkip,
                                                      GblBool*     pEnableColor) {
    return GBL_NEW(GblCmdParser,
        "mainOptionGroup",  GBL_NEW(GblOptionGroup,
            "name", "Test Suite",
            "prefix", "test",
            "options", GblRingList_create(
                &(GblOption) { "only",  'o', GBL_OPTION_TYPE_STRING, ppOnly,
                               "Only runs suites matching case-insensitive name substrings. Use a quoted, space-separated list.\ne.g. --only \"CmdParser OptionGroup\".",
                               "test", GBL_OPTION_FLAG_NONE },
                &(GblOption) { "skip",  's', GBL_OPTION_TYPE_STRING, ppSkip,
                               "Skips suites matching case-insensitive name substrings. Use a quoted, space-separated list.\ne.g. --skip \"CmdParser OptionGroup\".",
                               "test", GBL_OPTION_FLAG_NONE },
                &(GblOption) { "color", 'c', GBL_OPTION_TYPE_BOOL, pEnableColor,
                               "Enables colored output",
                               "enableColor",  GBL_OPTION_FLAG_BOOL_NO_VALUE }
            )
        )
    );
}

static GBL_RESULT GblTestScenarioClass_run_(GblTestScenario* pSelf, int argc, const char* argv[]) {
    GBL_CTX_BEGIN(pSelf);

    GblCmdParser*  pParser    = NULL;
    GblStringList* pOnlyList_ = NULL;
    GblStringList* pSkipList_ = NULL;
    GBL_RESULT     result     = GBL_RESULT_SUCCESS;

    GblContext*           pCtx   = GblObject_findContext(GBL_OBJECT(pSelf));
    GblTestScenarioClass* pClass = GBL_TEST_SCENARIO_CLASSOF(pSelf);
    GblTestScenario_*     pSelf_ = GBL_TEST_SCENARIO_(pSelf);

    GBL_CTX_VERIFY_CALL(GblTestScenarioClass_run_begin_(pSelf));

    const char* pOnly_      = NULL;
    const char* pSkip_      = NULL;
    GblBool     enableColor = GBL_FALSE;

    pParser = GblTestScenario_createCmdParser_(&pOnly_, &pSkip_, &enableColor);

    if(!pParser) {
        result = GBL_RESULT_ERROR_INVALID_POINTER;
        goto cleanup;
    }

    result = GblCmdParser_parse(pParser, GblStringList_createWithArray(argv, argc));

    if(!GBL_RESULT_SUCCESS(result) || GblCmdParser_helpRequested(pParser)) {
        GblTestScenario_logHelp_(pParser);
        goto cleanup;
    }

    pSelf_->enableColor = enableColor;

    const char* pGreen  = (pSelf_->enableColor)? GBL_TERM_GREEN_  : "";
    const char* pYellow = (pSelf_->enableColor)? GBL_TERM_YELLOW_ : "";
    const char* pRed    = (pSelf_->enableColor)? GBL_TERM_RED_    : "";
    const char* pBlink  = (pSelf_->enableColor)? GBL_TERM_BLINK_  : "";
    const char* pReset  = (pSelf_->enableColor)? GBL_TERM_RESET_  : "";

    pSelf->result = GBL_RESULT_SUCCESS;
    GBL_CTX_VERIFY_CALL(pClass->pFnBegin(pSelf));

    pOnlyList_ = GblStringList_createSplit(pOnly_, " ");
    pSkipList_ = GblStringList_createSplit(pSkip_, " ");

    for(GblTestSuite* pSuiteIt = GBL_AS(GblTestSuite, GblObject_childFirst(GBL_OBJECT(pSelf)));
        pSuiteIt              != NULL;
        pSuiteIt               = GBL_AS(GblTestSuite, GblObject_siblingNext(GBL_OBJECT(pSuiteIt))))
    {
        pSelf_->pCurSuite = pSuiteIt;

        GBL_CTX_RESULT() = GBL_RESULT_SUCCESS;

        if((pOnly_ && !GblStringList_foreach(pOnlyList_, GblTestScenario_stringListIterFn_, GblTestSuite_name(pSuiteIt))) ||
           (pSkip_ &&  GblStringList_foreach(pSkipList_, GblTestScenario_stringListIterFn_, GblTestSuite_name(pSuiteIt)))) {
            pSelf->casesSkipped += GblTestSuite_caseCount(pSuiteIt);
            ++pSelf->suitesSkipped;
            continue;
        }

        GBL_CTX_VERIFY_CALL(pClass->pFnSuiteBegin(pSelf, pSuiteIt));

        GBL_CTX_INFO("%-12s: %s", "[ INIT      ]",
                     GblTestSuite_name(pSelf_->pCurSuite));

        GBL_CTX_PUSH();
        GBL_CTX_CALL(GblTestSuite_initSuite(pSuiteIt, pCtx));
        if(GBL_RESULT_ERROR(GBL_CTX_RESULT()))
            GblTestScenario_recordFailure_(pSelf, pSuiteIt, NULL,
                                                 "suite init", &GBL_CTX_LAST_RECORD());
        GBL_CTX_CLEAR_LAST_RECORD();
        GBL_CTX_POP(1);

        if(GBL_CTX_RESULT() == GBL_RESULT_SKIPPED) {
            GBL_CTX_INFO("%s%s%-12s: %s%s", pBlink, pYellow, "[      SKIP ]", pReset,
                         GblTestSuite_name(pSelf_->pCurSuite));

            pSelf->casesSkipped += GblTestSuite_caseCount(pSuiteIt);
            ++pSelf->suitesSkipped;

        } else if(GBL_RESULT_ERROR(GBL_CTX_RESULT())) {
            GBL_CTX_INFO("%s%s%-12s: %s%s", pBlink, pRed, "[      FAIL ]", pReset,
                         GblTestSuite_name(pSelf_->pCurSuite));

            ++pSelf->suitesRun;
            pSelf->casesSkipped += GblTestSuite_caseCount(pSuiteIt);
            ++pSelf->suitesFailed;

        } else {

            ++pSelf->suitesRun;
            const size_t caseCount = GblTestSuite_caseCount(pSuiteIt);
            GblBool suiteFailed = GBL_FALSE;

            for(size_t idx = 0; idx < caseCount; ++idx) {

                pSelf_->curCase = idx;
                const char* pCurCase = GblTestSuite_caseName(pSuiteIt, idx);

                GBL_CTX_PUSH();
                GBL_CTX_RESULT() = GBL_RESULT_SUCCESS;
                GBL_CTX_CALL(GblTestSuite_initCase(pSuiteIt, pCtx));
                if(GBL_RESULT_ERROR(GBL_CTX_RESULT()))
                    GblTestScenario_recordFailure_(pSelf, pSuiteIt, pCurCase,
                                                   "case init", &GBL_CTX_LAST_RECORD());
                GBL_CTX_CLEAR_LAST_RECORD();
                GBL_CTX_POP(1);

                GBL_EMIT(pSelf, "caseBegan", pSuiteIt, idx);

                if(GBL_RESULT_ERROR(GBL_CTX_RESULT())) {
                    GBL_CTX_ERROR("[GblTestSuite] Failed to initialize test case[%s]: SKIPPING",
                                  pCurCase);
                    ++pSelf->casesSkipped;
                    suiteFailed = GBL_TRUE;
                } else {
                    GblTimer caseTimer;
                    GBL_CTX_INFO("%-12s: %s::%s", "[ RUN       ]",
                                 GblTestSuite_name(pSelf_->pCurSuite),
                                 pCurCase);

                    ++pSelf->casesRun;
                    GBL_CTX_PUSH();

                    GblTimer_start(&caseTimer);
                    pSelf_->runningCase = GBL_TRUE;

                    GBL_RESULT result = GblTestSuite_runCase(pSuiteIt, pCtx, idx);
                    const GblCallRecord caseRecord = GBL_CTX_LAST_RECORD();

                    pSelf_->expectError = GBL_FALSE;
                    pSelf_->runningCase = GBL_FALSE;
                    GblTimer_stop(&caseTimer);
                    pSelf_->suiteMs += GblTimer_elapsedMs(&caseTimer);

                    GBL_CTX_CLEAR_LAST_RECORD();
                    GBL_CTX_POP(1);

                    if(result == GBL_RESULT_SKIPPED) {
                        ++pSelf->casesSkipped;
                        GBL_CTX_INFO("%s%s%-12s: %s%s::%s", pBlink, pYellow, "[      SKIP ]", pReset,
                                     GblTestSuite_name(pSelf_->pCurSuite),
                                     pCurCase);
                    } else if(!GBL_RESULT_ERROR(result)) {
                        ++pSelf->casesPassed;
                        GBL_CTX_INFO("%s%s%-12s: %s%s::%s (%.3f ms)", pBlink, pGreen, "[      PASS ]", pReset,
                                     GblTestSuite_name(pSelf_->pCurSuite),
                                     pCurCase,
                                     GblTimer_elapsedMs(&caseTimer));
                    } else {
                        ++pSelf->casesFailed;
                        GblTestScenario_recordFailure_(pSelf, pSuiteIt, pCurCase,
                                                             "case", &caseRecord);
                        GBL_CTX_INFO("%s%s%-12s: %s%s::%s (%.3f ms)", pBlink, pRed, "[      FAIL ]", pReset,
                                     GblTestSuite_name(pSelf_->pCurSuite),
                                     pCurCase,
                                     GblTimer_elapsedMs(&caseTimer));

                        suiteFailed = GBL_TRUE;
                        pSelf->casesSkipped += caseCount - idx - 1;

                        GBL_EMIT(pSelf, "caseEnded", pSuiteIt, idx);

                        // Manually skip remaining cases in suite + fire signals for them all
                        for(size_t rem_idx = idx + 1; rem_idx < caseCount; ++rem_idx) {
                            GblTestSuite_skipCase(pSuiteIt, pCtx, rem_idx);
                            GBL_EMIT(pSelf, "caseEnded", pSuiteIt, rem_idx);
                        }

                        break;
                    }

                    GBL_CTX_RESULT() = GBL_RESULT_SUCCESS;
                    GBL_CTX_CALL(GblTestSuite_finalCase(pSuiteIt, pCtx));
                    if(GBL_RESULT_ERROR(GBL_CTX_RESULT()))
                        GblTestScenario_recordFailure_(pSelf, pSuiteIt, pCurCase,
                                                       "case final", &GBL_CTX_LAST_RECORD());
                    GBL_CTX_CLEAR_LAST_RECORD();

                    GBL_EMIT(pSelf, "caseEnded", pSuiteIt, idx);

                    if(GBL_RESULT_ERROR(GBL_CTX_RESULT())) {
                        GBL_CTX_ERROR("[GblTestSuite] Failed to finalize test case: [%s]",
                                      pCurCase);
                        suiteFailed = GBL_TRUE; // log as failed suite, but not case, continue
                    }
                }
            }

            GBL_CTX_INFO("%-12s: %s", "[ FINAL     ]",
                         GblTestSuite_name(pSelf_->pCurSuite));
            GBL_CTX_PUSH();
            GBL_CTX_RESULT() = GBL_RESULT_SUCCESS;
            GBL_CTX_CALL(GblTestSuite_finalSuite(pSuiteIt, pCtx));
            if(GBL_RESULT_ERROR(GBL_CTX_RESULT()))
                GblTestScenario_recordFailure_(pSelf, pSuiteIt, NULL,
                                               "suite final", &GBL_CTX_LAST_RECORD());
            GBL_CTX_CLEAR_LAST_RECORD();
            GBL_CTX_POP(1);

            if(GBL_RESULT_ERROR(GBL_CTX_RESULT())) {

                GBL_CTX_INFO("%s%s%-12s: %s%s", pBlink, pRed, "[      FAIL ]", pReset,
                             GblTestSuite_name(pSelf_->pCurSuite));

                suiteFailed = GBL_TRUE;
            }

            if(!suiteFailed) ++pSelf->suitesPassed;
            else ++pSelf->suitesFailed;
        }

        GBL_CTX_CALL(pClass->pFnSuiteEnd(pSelf, pSuiteIt));
    }

    pSelf->result = (pSelf->casesFailed || pSelf->suitesFailed)? GBL_RESULT_ERROR : GBL_RESULT_SUCCESS;
    GBL_CTX_CALL(pClass->pFnEnd(pSelf));

    result = pSelf->result;
cleanup:
    GBL_CTX_END_BLOCK();
    if(GBL_RESULT_ERROR(GBL_CTX_RESULT())) result = GBL_CTX_RESULT();
    GblCmdParser_unref(pParser);
    GblStringList_unref(pOnlyList_);
    GblStringList_unref(pSkipList_);
    return result;
}

static GBL_RESULT GblTestScenarioClass_suiteBegin_(GblTestScenario* pSelf, const GblTestSuite* pSuite) {
    GBL_CTX_BEGIN(pSelf);

    GblTestScenario_* pSelf_     = GBL_TEST_SCENARIO_(pSelf);
    const char*       pSuiteName = GblTestSuite_name(pSuite);
    const char*       pGray      = (pSelf_->enableColor)? GBL_TERM_GRAY_  : "";
    const char*       pReset     = (pSelf_->enableColor)? GBL_TERM_RESET_ : "";

    GblAllocationTracker_captureCounters(pSelf_->pAllocTracker,
                                        &pSelf_->suiteAllocCounters);

    for(size_t groupIdx = 0; groupIdx < GblArrayMap_size(&pSelf_->pResultGroups); ++groupIdx) {
        GblTestResultGroup_* pGroup = (GblTestResultGroup_*)GblArrayMap_probeValue(&pSelf_->pResultGroups, groupIdx);

        for(uint16_t valueIdx = 0; valueIdx < pGroup->pEnumClass->entryCount; ++valueIdx) {
            GblTestResultCount_* pCount = GblArrayList_at(&pGroup->counts, valueIdx);
            pCount->suiteStart = pCount->count;
        }
    }

    pSelf_->suiteMs = 0.0;

    GBL_CTX_INFO("%s********* Starting TestSuite [%s] *********%s", pGray, pSuiteName, pReset);

    GBL_EMIT(pSelf, "suiteBegan", pSuite);

    GBL_CTX_END();
}


static GBL_RESULT GblTestScenarioClass_suiteEnd_(GblTestScenario* pSelf, const GblTestSuite* pSuite) {
    GBL_CTX_BEGIN(pSelf);

    GblTestScenario_* pSelf_     = GBL_TEST_SCENARIO_(pSelf);
    const char*       pSuiteName = GblTestSuite_name(pSuite);
    const char*       pGray      = (pSelf_->enableColor)? GBL_TERM_GRAY_  : "";
    const char*       pReset     = (pSelf_->enableColor)? GBL_TERM_RESET_ : "";
    GblAllocationCounters  diffCounters;

    GblAllocationTracker_diffCounters(pSelf_->pAllocTracker,
                                      &pSelf_->suiteAllocCounters,
                                      &diffCounters);

    pSelf->totalTime += pSelf_->suiteMs;

    GBL_CTX_INFO("Totals: %u passed, %u failed, %u skipped, %.3fms, %d/%u leaked (%d/%u bytes)",
                 pSuite->casesPassed,
                 pSuite->casesFailed,
                 pSuite->casesSkipped,
                 pSelf_->suiteMs,
                 diffCounters.allocsActive,
                 diffCounters.allocEvents,
                 diffCounters.bytesActive,
                 diffCounters.bytesAllocated);
    GBL_CTX_CALL(GblTestScenario_logResults_(pSelf, GBL_TRUE));
    GBL_CTX_INFO("%s********* Finished TestSuite [%s] *********%s", pGray, pSuiteName, pReset);

    GBL_EMIT(pSelf, "suiteEnded", pSuite);

    GBL_CTX_END();
}

static GBL_RESULT GblTestScenarioClass_property_(const GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(pSelf);
    GblTestScenario* pScenario = GBL_TEST_SCENARIO(pSelf);
    switch(pProp->id) {
    case GblTestScenario_Property_Id_testResult:
        GblVariant_setValueCopy(pValue, pProp->valueType, pScenario->result);
        break;
    case GblTestScenario_Property_Id_suiteCount:
        GblVariant_setValueCopy(pValue, pProp->valueType, GblTestScenario_suiteCount_(pScenario));
        break;
    case GblTestScenario_Property_Id_suitesRun:
        GblVariant_setValueCopy(pValue, pProp->valueType, pScenario->suitesRun);
        break;
    case GblTestScenario_Property_Id_suitesPassed:
        GblVariant_setValueCopy(pValue, pProp->valueType, pScenario->suitesPassed);
        break;
    case GblTestScenario_Property_Id_suitesFailed:
        GblVariant_setValueCopy(pValue, pProp->valueType, pScenario->suitesFailed);
        break;
    case GblTestScenario_Property_Id_suitesSkipped:
        GblVariant_setValueCopy(pValue, pProp->valueType, pScenario->suitesSkipped);
        break;
    case GblTestScenario_Property_Id_caseCount:
        GblVariant_setValueCopy(pValue, pProp->valueType, pScenario->caseCount);
        break;
    case GblTestScenario_Property_Id_casesRun:
        GblVariant_setValueCopy(pValue, pProp->valueType, pScenario->casesRun);
        break;
    case GblTestScenario_Property_Id_casesPassed:
        GblVariant_setValueCopy(pValue, pProp->valueType, pScenario->casesPassed);
        break;
    case GblTestScenario_Property_Id_casesFailed:
        GblVariant_setValueCopy(pValue, pProp->valueType, pScenario->casesFailed);
        break;
    case GblTestScenario_Property_Id_casesSkipped:
        GblVariant_setValueCopy(pValue, pProp->valueType, pScenario->casesSkipped);
        break;
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "[GblTestScenario] Reading unhandled property: %s",
                                GblProperty_name(pProp));
    }
    GBL_CTX_END();
}



static GBL_RESULT GblTestScenarioClass_IAllocator_alloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, size_t  size, size_t  align, const char* pDbgStr, void** ppData) GBL_NOEXCEPT {
    GblContext* pParentCtx = GblContext_parentContext((GblContext*)pIAllocator);
    GBL_CTX_BEGIN(pParentCtx);
    GblContextClass* pCtxClass = GBL_CONTEXT_CLASS(GblClass_weakRefDefault(GBL_CONTEXT_TYPE));
    GblTestScenario* pSelf = (GblTestScenario*)pIAllocator;
    GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pSelf);
    GBL_CTX_VERIFY_CALL(pCtxClass->GblIAllocatorImpl.pFnAlloc(pIAllocator, pFrame, size, align, pDbgStr, ppData));
    GBL_CTX_VERIFY_CALL(GblAllocationTracker_allocEvent(pSelf_->pAllocTracker, *ppData, size, align, pDbgStr, pFrame->record.srcLocation));
    GBL_CTX_END();
}

static GBL_RESULT GblTestScenarioClass_IAllocator_realloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData, size_t  newSize, size_t  newAlign, void** ppNewData) GBL_NOEXCEPT {
    GblContext* pParentCtx = GblContext_parentContext((GblContext*)pIAllocator);
    GBL_CTX_BEGIN(pParentCtx);
    GblContextClass* pCtxClass = GBL_CONTEXT_CLASS(GblClass_weakRefDefault(GBL_CONTEXT_TYPE));
    GblTestScenario* pSelf = (GblTestScenario*)pIAllocator;
    GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pSelf);
    GBL_CTX_VERIFY_CALL(pCtxClass->GblIAllocatorImpl.pFnRealloc(pIAllocator, pFrame, pData, newSize, newAlign, ppNewData));
    GBL_CTX_VERIFY_CALL(GblAllocationTracker_reallocEvent(pSelf_->pAllocTracker, pData, *ppNewData, newSize, newAlign, pFrame->record.srcLocation));
    GBL_CTX_END();
}

static GBL_RESULT GblTestScenarioClass_IAllocator_free_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData) GBL_NOEXCEPT {
    GblContext* pParentCtx = GblContext_parentContext((GblContext*)pIAllocator);
    GBL_CTX_BEGIN(pParentCtx);
    GblContextClass* pCtxClass = GBL_CONTEXT_CLASS(GblClass_weakRefDefault(GBL_CONTEXT_TYPE));
    GblTestScenario* pSelf = (GblTestScenario*)pIAllocator;
    GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pSelf);
    GBL_CTX_VERIFY_CALL(pCtxClass->GblIAllocatorImpl.pFnFree(pIAllocator, pFrame, pData));
    GBL_CTX_VERIFY_CALL(GblAllocationTracker_freeEvent(pSelf_->pAllocTracker, pData, pFrame->record.srcLocation));
    GBL_CTX_END();
}

static GBL_RESULT GblTestScenario_init_(GblInstance* pInstance) {
    GblObject* pObject = GBL_OBJECT(pInstance);

    GBL_CTX_BEGIN(pObject);
    GblContextClass* pCtxClass = GBL_CONTEXT_CLASS(GblClass_weakRefDefault(GBL_CONTEXT_TYPE));

    GblTestScenario*  pSelf  = GBL_TEST_SCENARIO(pObject);
    GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pSelf);

    GblContext* pParentCtx = GblContext_parentContext(GBL_CONTEXT(pObject));
    pSelf_->pAllocTracker  = GblAllocationTracker_create(pParentCtx);
    pSelf_->pResultGroups  = NULL;
    GBL_CTX_VERIFY_CALL(GblArrayList_construct(&pSelf_->failures,
                                               sizeof(GblTestFailure_)));

    GBL_CTX_END();
}

static GBL_RESULT GblTestScenarioClass_destructor_(GblBox* pRecord) {
    GBL_CTX_BEGIN(pRecord);

    GblTestScenario*  pSelf  = GBL_TEST_SCENARIO(pRecord);
    GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pSelf);

    GBL_CTX_CALL(GblArrayMap_destroy(&pSelf_->pResultGroups));
    GBL_CTX_CALL(GblArrayList_destruct(&pSelf_->failures));
    GBL_CTX_VERIFY_CALL(GblAllocationTracker_destroy(pSelf_->pAllocTracker));

    GblContextClass* pCtxClass = GBL_CONTEXT_CLASS(GblClass_weakRefDefault(GBL_CONTEXT_TYPE));
    GBL_CTX_VERIFY_CALL(pCtxClass->base.base.pFnDestructor(pRecord));
    GBL_CTX_END();
}


static GBL_RESULT GblTestScenarioClass_ILogger_write_(GblILogger* pILogger, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs){
    GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pILogger);
    if(pSelf_->expectError && (level & (GBL_LOG_LEVEL_WARNING|GBL_LOG_LEVEL_ERROR)))
        return GBL_RESULT_SUCCESS;

    GblTimer logTime;
    GblTimer_start(&logTime);

    GblContextClass* pClass = GBL_CONTEXT_CLASS(GblClass_weakRefDefault(GBL_CONTEXT_TYPE));

    const GBL_RESULT result = pClass->GblILoggerImpl.pFnWrite(pILogger, pFrame, level, pFmt, varArgs);

    if(pSelf_->runningCase) {
        GblTimer_stop(&logTime);
        pSelf_->suiteMs -= GblTimer_elapsedMs(&logTime);
    }

    return result;
}


static GBL_RESULT GblTestScenarioClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(NULL);

    if(!GblType_classRefCount(GBL_TEST_SCENARIO_TYPE)) {
        GBL_PROPERTIES_REGISTER(GblTestScenario);

        GblSignal_install(GBL_TEST_SCENARIO_TYPE,
                          "began",
                          GblMarshal_CClosure_VOID__INSTANCE,
                          0);

        GblSignal_install(GBL_TEST_SCENARIO_TYPE,
                          "ended",
                          GblMarshal_CClosure_VOID__INSTANCE,
                          0);

        GblSignal_install(GBL_TEST_SCENARIO_TYPE,
                          "suiteBegan",
                          GblMarshal_CClosure_VOID__INSTANCE_INSTANCE,
                          1,
                          GBL_POINTER_TYPE);

        GblSignal_install(GBL_TEST_SCENARIO_TYPE,
                          "suiteEnded",
                          GblMarshal_CClosure_VOID__INSTANCE_INSTANCE,
                          1,
                          GBL_POINTER_TYPE);

        GblSignal_install(GBL_TEST_SCENARIO_TYPE,
                          "caseBegan",
                          GblMarshal_CClosure_VOID__INSTANCE_INSTANCE_SIZE,
                          2,
                          GBL_POINTER_TYPE,
                          GBL_SIZE_TYPE);

        GblSignal_install(GBL_TEST_SCENARIO_TYPE,
                          "caseEnded",
                          GblMarshal_CClosure_VOID__INSTANCE_INSTANCE_SIZE,
                          2,
                          GBL_POINTER_TYPE,
                          GBL_SIZE_TYPE);
    }

    GblTestScenarioClass* pSelfClass = GBL_TEST_SCENARIO_CLASS(pClass);
    pSelfClass->pFnBegin                          = GblTestScenarioClass_begin_;
    pSelfClass->pFnEnd                            = GblTestScenarioClass_end_;
    pSelfClass->pFnRun                            = GblTestScenarioClass_run_;
    pSelfClass->pFnSuiteBegin                     = GblTestScenarioClass_suiteBegin_;
    pSelfClass->pFnSuiteEnd                       = GblTestScenarioClass_suiteEnd_;
    pSelfClass->base.base.base.pFnDestructor      = GblTestScenarioClass_destructor_;
    pSelfClass->base.base.pFnProperty             = GblTestScenarioClass_property_;
    pSelfClass->base.GblILoggerImpl.pFnWrite      = GblTestScenarioClass_ILogger_write_;
    pSelfClass->base.GblIAllocatorImpl.pFnAlloc   = GblTestScenarioClass_IAllocator_alloc_;
    pSelfClass->base.GblIAllocatorImpl.pFnRealloc = GblTestScenarioClass_IAllocator_realloc_;
    pSelfClass->base.GblIAllocatorImpl.pFnFree    = GblTestScenarioClass_IAllocator_free_;
    GBL_CTX_END();
}

GBL_EXPORT GblTestScenario* GblTestScenario_create(const char* pName) {
    GblTestScenario* pScenario = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pName);
    pScenario = GBL_TEST_SCENARIO(GblObject_create(GBL_TEST_SCENARIO_TYPE,
                                                   "name", pName,
                                                   NULL));
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return pScenario;
}

GBL_EXPORT GblTestScenario* GblTestScenario_ref(GblTestScenario* pSelf) {
    return GBL_TEST_SCENARIO(GBL_REF(pSelf));
}

GBL_EXPORT GblRefCount GblTestScenario_unref(GblTestScenario* pSelf) {
    GblRefCount retVal = 0;

    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);
    GblTestScenario_*   pSelf_  = GBL_TEST_SCENARIO_(pSelf);

    GblObject* pNext;
    // not properly disposing of shit anything beyond the first entry!
    for(GblObject* pIt = GblObject_childFirst(GBL_OBJECT(pSelf));
        pIt != NULL;
        pIt = pNext)
    {
        pNext = GblObject_siblingNext(pIt);
        if(GblBox_unref(GBL_BOX(pIt)) != 0) {
            GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                               "[GblTestSuite] Destroy: Leaking unexpected existing references!");
        }
    }

    GBL_CTX_VERIFY_CALL(GblAllocationTracker_logActive(pSelf_->pAllocTracker));

    retVal = GBL_UNREF(pSelf);
    GBL_CTX_END_BLOCK();
    return retVal;
}

GBL_EXPORT GBL_RESULT GblTestScenario_enqueueSuite(GblTestScenario* pSelf, const GblTestSuite* pSuite) {
    GBL_CTX_BEGIN(pSelf);
    GblObject_addChild(GBL_OBJECT(pSelf), GBL_OBJECT(pSuite));
    pSelf->caseCount += GblTestSuite_caseCount(pSuite);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GblTestSuite* GblTestScenario_findSuite(const GblTestScenario* pSelf, const char* pName) {
    GblTestSuite* pSuite = NULL;
    GBL_CTX_BEGIN(pSelf);
    GBL_CTX_VERIFY_POINTER(pName);
    pSuite = GBL_AS(GblTestSuite, GblObject_findChildByName(GBL_OBJECT(pSelf), pName));
    GBL_CTX_END_BLOCK();
    return pSuite;
}

GBL_EXPORT GBL_RESULT GblTestScenario_registerResultGroup(GblTestScenario* pSelf,
                                                          GblQuark         name,
                                                          GblType          enumType) {
    GBL_CTX_BEGIN(pSelf);
    GBL_CTX_VERIFY_POINTER(pSelf);

    const char* pName = GblQuark_toString(name);
    GBL_CTX_VERIFY_POINTER(pName);
    GBL_CTX_VERIFY_ARG(pName[0]);

    GBL_CTX_VERIFY_TYPE(enumType, GBL_ENUM_TYPE);

    GBL_CTX_VERIFY(!GblTestScenario_resultGroup_(pSelf, name, enumType),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Result group already registered: %s (%s)", pName, GblType_name(enumType));

    GblTestScenario_* pSelf_ = GBL_TEST_SCENARIO_(pSelf);
    if(!pSelf_->pResultGroups)
        pSelf_->pResultGroups = GblArrayMap_create(GblTestScenario_resultGroupCompare_, GBL_TRUE, NULL);

    GblTestResultGroup_* pGroup = GBL_CTX_MALLOC(sizeof(GblTestResultGroup_));
    pGroup->key.name            = name;
    pGroup->key.enumType        = enumType;
    pGroup->pEnumClass          = GBL_ENUM_CLASS(GblClass_refDefault(enumType));

    GBL_CTX_CALL(GblArrayList_construct(&pGroup->counts,
                                        sizeof(GblTestResultCount_),
                                        pGroup->pEnumClass->entryCount));

    GBL_CTX_CALL(GblArrayMap_setUserdata(&pSelf_->pResultGroups,
                                         (uintptr_t)&pGroup->key,
                                         (uintptr_t)pGroup,
                                         GblTestScenario_resultGroupDestruct_));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblTestScenario_recordResult(GblTestScenario* pSelf,
                                                   GblQuark         name,
                                                   GblType          enumType,
                                                   GblEnum          value) {
    GBL_CTX_BEGIN(pSelf);
    GBL_CTX_VERIFY_POINTER(pSelf);

    GblTestResultGroup_* pGroup = GblTestScenario_resultGroup_(pSelf, name, enumType);
    GBL_CTX_VERIFY(pGroup, GBL_RESULT_ERROR_INVALID_TYPE,
                   "Unregistered result group: %s (%s)", GblQuark_toString(name), GblType_name(enumType));

    const uint16_t index = GblEnumClass_indexFromValue(pGroup->pEnumClass, value);
    GBL_CTX_VERIFY(index != UINT16_MAX, GBL_RESULT_ERROR_OUT_OF_RANGE,
                   "Invalid result value: %u", value);

    GblTestResultCount_* pCount = GblArrayList_at(&pGroup->counts, index);
    ++pCount->count;

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblTestScenario_reportResult(GblTestScenario* pSelf,
                                                   GblQuark         name,
                                                   GblType          enumType,
                                                   GblEnum          value,
                                                   const char*      pFmt,
                                                   ...) {
    GBL_CTX_BEGIN(pSelf);
    GBL_CTX_VERIFY_CALL(GblTestScenario_recordResult(pSelf, name, enumType, value));

    const GblEnumClass* pEnumClass = GBL_ENUM_CLASS(GblClass_weakRefDefault(enumType));
    const char* pLabel             = GblEnumClass_nickFromValue(pEnumClass, value);

    if(!pLabel || !pLabel[0])
        pLabel = GblEnumClass_nameFromValue(pEnumClass, value);

    const GblBool runningCase = GBL_TEST_SCENARIO_(pSelf)->runningCase;
    if(runningCase) {
        GblThd_logPop(NULL, 1);
        GblContext_logPop_(GBL_CONTEXT(pSelf), GBL_CTX_FRAME(), 1);
    }

    if(pFmt && pFmt[0]) {
        GblStringBuffer details;
        GblStringBuffer_construct(&details);

        va_list varArgs;
        va_start(varArgs, pFmt);
        GBL_CTX_CALL(GblStringBuffer_appendVPrintf(&details, pFmt, varArgs));
        va_end(varArgs);

        GBL_CTX_INFO("[   %-6s  ]: %s", pLabel, GblStringBuffer_cString(&details));
        GblStringBuffer_destruct(&details);
    } else {
        GBL_CTX_INFO("[   %-6s  ]", pLabel);
    }

    if(runningCase) {
        GblThd_logPush(NULL);
        GblContext_logPush_(GBL_CONTEXT(pSelf), GBL_CTX_FRAME());
    }
    GBL_CTX_END();
}

GBL_EXPORT size_t GblTestScenario_resultCount(const GblTestScenario* pSelf,
                                              GblQuark               name,
                                              GblType                enumType,
                                              GblEnum                value) {
    GBL_CTX_BEGIN(pSelf);
    GBL_CTX_VERIFY_POINTER(pSelf);

    size_t count = 0;

    const GblTestResultGroup_* pGroup = GblTestScenario_resultGroup_(pSelf, name, enumType);
    GBL_CTX_VERIFY(pGroup, GBL_RESULT_ERROR_INVALID_TYPE,
                   "Unregistered result group: %s (%s)", GblQuark_toString(name), GblType_name(enumType));

    const uint16_t index = GblEnumClass_indexFromValue(pGroup->pEnumClass, value);
    GBL_CTX_VERIFY(index != UINT16_MAX, GBL_RESULT_ERROR_OUT_OF_RANGE,
                   "Invalid result value: %u", value);

    const GblTestResultCount_* pCount = GblArrayList_at(&pGroup->counts, index);
    count = pCount->count;

    GBL_CTX_END_BLOCK();
    return count;
}

GBL_EXPORT GblTestSuite* GblTestScenario_currentSuite(const GblTestScenario* pSelf) {
    return pSelf? GBL_TEST_SCENARIO_(pSelf)->pCurSuite : NULL;
}

GBL_EXPORT size_t GblTestScenario_currentCase(const GblTestScenario* pSelf) {
    return pSelf? GBL_TEST_SCENARIO_(pSelf)->curCase : GBL_NPOS;
}

GBL_EXPORT GBL_RESULT GblTestScenario_run(GblTestScenario* pSelf, int argc, const char* argv[]) {
    GBL_CTX_BEGIN(pSelf);

    GblContext* pOldGlobalCtx = GblContext_global();
    GblContext_setGlobal(GBL_CONTEXT(pSelf));

    GBL_VCALL(GblTestScenario, pFnRun, pSelf, argc, argv);

    GblContext_setLogFilter(pOldGlobalCtx, GBL_LOG_LEVEL_WARNING|GBL_LOG_LEVEL_ERROR);
    GblContext_setGlobal(pOldGlobalCtx);

    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT int GblTestScenario_exec(GblTestScenario* pSelf, int argc, const char* argv[]) {
    const GBL_RESULT result = GblTestScenario_run(pSelf, argc, argv);
    GblTestScenario_unref(pSelf);
    return GBL_RESULT_ERROR(result)? EXIT_FAILURE : EXIT_SUCCESS;
}

GBL_EXPORT GblBool GblTestScenario_ran(const GblTestScenario* pSelf) {
    return pSelf && pSelf->result != GBL_RESULT_UNKNOWN;
}

GBL_EXPORT GblBool GblTestScenario_passed(const GblTestScenario* pSelf) {
    return GblTestScenario_ran(pSelf) && !GBL_RESULT_ERROR(pSelf->result);
}

GBL_EXPORT void GblTestScenario_expectError(const GblTestScenario* pSelf) {
    GBL_TEST_SCENARIO_(pSelf)->expectError = GBL_TRUE;
}

GBL_EXPORT GblType GblTestScenario_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo typeInfo = {
        .pFnClassInit           = GblTestScenarioClass_init_,
        .classSize              = sizeof(GblTestScenarioClass),
        .pFnInstanceInit        = GblTestScenario_init_,
        .instanceSize           = sizeof(GblTestScenario),
        .instancePrivateSize    = sizeof(GblTestScenario_)
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblTestScenario"),
                                GBL_CONTEXT_TYPE,
                                &typeInfo,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}
