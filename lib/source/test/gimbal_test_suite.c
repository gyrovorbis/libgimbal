#include <gimbal/test/gimbal_test_suite.h>
#include <gimbal/test/gimbal_test_scenario.h>
#include <gimbal/containers/gimbal_array_list.h>

#define GBL_TEST_SUITE_(inst)           (GBL_PRIVATE(GblTestSuite, inst))

typedef struct GblTestSuite_ {
    GblArrayList testCases;
} GblTestSuite_;

static GBL_RESULT GblTestSuiteClass_vTableDefault_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf && pCtx);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblTestSuiteClass_suiteName_(const GblTestSuite* pSelf, const char** ppName) {
    GBL_CTX_BEGIN(pSelf);
    GBL_CTX_VERIFY_POINTER(ppName);
    *ppName = GblObject_name(GBL_OBJECT(pSelf));
    GBL_CTX_END();
}

static GBL_RESULT GblTestSuiteClass_caseCount_(const GblTestSuite* pSelf, size_t * pSize) {
    GBL_CTX_BEGIN(pSelf);
    GBL_CTX_VERIFY_POINTER(pSize);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pSelf);
    *pSize = GblArrayList_size(&pSelf_->testCases);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblTestSuiteClass_caseName_(const GblTestSuite* pSelf, size_t  index, const char** ppName) {
    GBL_CTX_BEGIN(pSelf);
    GBL_CTX_VERIFY_POINTER(ppName);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pSelf);
    const GblTestCase* pCase = GblArrayList_at(&pSelf_->testCases, index);
    GBL_CTX_VERIFY_LAST_RECORD();
    *ppName = pCase->pName;
    GBL_CTX_END();
}

static GBL_RESULT GblTestSuiteClass_caseRun_(GblTestSuite* pSelf, GblContext* pCtx, size_t index) {
    GBL_CTX_BEGIN(pCtx);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pSelf);
    const GblTestCase* pCase = GblArrayList_at(&pSelf_->testCases, index);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_VERIFY_CALL(pCase->pFnRun(pSelf, pCtx));
    GBL_CTX_END_BLOCK();

    if(GBL_CTX_RESULT() == GBL_RESULT_SKIPPED) {
        ++pSelf->casesSkipped;
    } else {
        memcpy(&pSelf->failingIssue, &GBL_CTX_RECORD(), sizeof(GblCallRecord));

        ++pSelf->casesRun;
        if(!GBL_RESULT_ERROR(GBL_CTX_RESULT())) {
            ++pSelf->casesPassed;
        } else {
            ++pSelf->casesFailed;
        }
    }
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblTestSuiteClass_property_(const GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(pSelf);
    GblTestSuite* pSuite = GBL_TEST_SUITE(pSelf);
    GblTestSuite_* pSuite_ = GBL_TEST_SUITE_(pSelf);
    switch(pProp->id) {
    case GblTestSuite_Property_Id_runResult:
        GblVariant_setValueCopy(pValue, pProp->valueType, pSuite->failingIssue.result);
        break;
    case GblTestSuite_Property_Id_failureMessage:
        GblVariant_setValueCopy(pValue, pProp->valueType, pSuite->failingIssue.message);
        break;
    case GblTestSuite_Property_Id_caseCount:
        GblVariant_setValueCopy(pValue, pProp->valueType, GblArrayList_size(&pSuite_->testCases));
        break;
    case GblTestSuite_Property_Id_casesRun:
        GblVariant_setValueCopy(pValue, pProp->valueType, pSuite->casesRun);
        break;
    case GblTestSuite_Property_Id_casesPassed:
        GblVariant_setValueCopy(pValue, pProp->valueType, pSuite->casesPassed);
        break;
    case GblTestSuite_Property_Id_casesFailed:
        GblVariant_setValueCopy(pValue, pProp->valueType, pSuite->casesFailed);
        break;
    case GblTestSuite_Property_Id_casesSkipped:
        GblVariant_setValueCopy(pValue, pProp->valueType, pSuite->casesSkipped);
        break;
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "[GblTestSuite] Reading unhandled property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblTestSuiteClass_constructed_(GblObject* pSelf) {
    GBL_CTX_BEGIN(pSelf);

    GblObjectClass* pSuper = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));
    if(pSuper && pSuper->pFnConstructed) GBL_CTX_VERIFY_CALL(pSuper->pFnConstructed(pSelf));

    const char* pName = GblObject_name(pSelf);
    if(!pName) {
        GblObject_setName(pSelf, GblType_name(GblInstance_typeOf(GBL_INSTANCE(pSelf))));
    }

    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(pClass->pVTable && pClass->pVTable->pCases) {
        GBL_CTX_VERIFY_CALL(GblTestSuite_addCases(GBL_TEST_SUITE(pSelf),
                                                  pClass->pVTable->pCases));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblTestSuiteClass_destructor_(GblBox* pSelf) {
    GBL_CTX_BEGIN(pSelf);
    GblObjectClass* pSuper = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));
    if(pSuper && pSuper->base.pFnDestructor) GBL_CTX_VERIFY_CALL(pSuper->base.pFnDestructor(pSelf));

    GBL_CTX_VERIFY_CALL(GblArrayList_destruct(&GBL_TEST_SUITE_(pSelf)->testCases));
    GBL_CTX_END();
}

static GBL_RESULT GblTestSuiteClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(NULL);

    if(!GblType_classRefCount(GBL_TEST_SUITE_TYPE)) {
        GBL_PROPERTIES_REGISTER(GblTestSuite);
    }

    const static GblTestSuiteVTable defaultVTable = {
        .pFnSuiteInit   = GblTestSuiteClass_vTableDefault_,
        .pFnSuiteFinal  = GblTestSuiteClass_vTableDefault_,
        .pFnCaseInit    = GblTestSuiteClass_vTableDefault_,
        .pFnCaseFinal   = GblTestSuiteClass_vTableDefault_
    };

    GblTestSuiteClass* pSelf        = GBL_TEST_SUITE_CLASS(pClass);

    pSelf->pVTable                  = &defaultVTable;

    pSelf->pFnSuiteName             = GblTestSuiteClass_suiteName_;
    pSelf->pFnCaseRun               = GblTestSuiteClass_caseRun_;
    pSelf->pFnCaseCount             = GblTestSuiteClass_caseCount_;
    pSelf->pFnCaseName              = GblTestSuiteClass_caseName_;

    pSelf->base.pFnConstructed      = GblTestSuiteClass_constructed_;
    pSelf->base.base.pFnDestructor  = GblTestSuiteClass_destructor_;
    pSelf->base.pFnProperty         = GblTestSuiteClass_property_;

    GBL_CTX_END();
}

static GBL_RESULT GblTestSuiteClass_initDerived_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(NULL);

    GblTestSuiteClass* pSelf = GBL_TEST_SUITE_CLASS(pClass);
    pSelf->pVTable           = pUd;

    GBL_CTX_END();
}


GBL_EXPORT const char* GblTestSuite_name(const GblTestSuite* pSelf) {
    const char* pName = "";
    GBL_CTX_BEGIN(pSelf);
    GBL_VCALL(GblTestSuite, pFnSuiteName, pSelf, &pName);
    GBL_CTX_VERIFY_LAST_RECORD();
    if(!pName) pName = "";
    GBL_CTX_END_BLOCK();
    return pName;
}

GBL_EXPORT GBL_RESULT GblTestSuite_initSuite(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnSuiteInit)
        GBL_CTX_DONE();
    GBL_VCALL(GblTestSuite, pVTable->pFnSuiteInit, pSelf, pCtx);

    if(!GBL_RESULT_SUCCESS(GBL_CTX_RESULT())) {
         pSelf->casesSkipped += GblTestSuite_caseCount(pSelf);
         memcpy(&pSelf->failingIssue, &GBL_CTX_LAST_RECORD(), sizeof(GblCallRecord));
    }

    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_finalSuite(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnSuiteFinal)
        GBL_CTX_DONE();
    GBL_VCALL(GblTestSuite, pVTable->pFnSuiteFinal, pSelf, pCtx);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_initCase(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnCaseInit)
        GBL_CTX_DONE();
    GBL_VCALL(GblTestSuite, pVTable->pFnCaseInit, pSelf, pCtx);

    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_finalCase(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnCaseFinal)
        GBL_CTX_DONE();
    GBL_VCALL(GblTestSuite, pVTable->pFnCaseFinal, pSelf, pCtx);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT size_t  GblTestSuite_caseCount(const GblTestSuite* pSelf) {
    size_t  count = 0;
    GBL_CTX_BEGIN(pSelf);
    GBL_VCALL(GblTestSuite, pFnCaseCount, pSelf, &count);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return count;
}

GBL_EXPORT const char* GblTestSuite_caseName(const GblTestSuite* pSelf, size_t  index) {
    const char* pName = NULL;
    GBL_CTX_BEGIN(pSelf);
    GBL_VCALL(GblTestSuite, pFnCaseName, pSelf, index, &pName);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return pName;
}

GBL_EXPORT GBL_RESULT GblTestSuite_runCase(GblTestSuite* pSelf,
                                           GblContext* pCtx,
                                           size_t  index)
{
    GBL_CTX_BEGIN(pSelf);
    GBL_VCALL(GblTestSuite, pFnCaseRun, pSelf, pCtx, index);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblTestSuite_init_(GblInstance* pInstance) {
    GBL_CTX_BEGIN(NULL);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pInstance);
    GBL_CTX_VERIFY_CALL(GblArrayList_construct(&pSelf_->testCases, sizeof(GblTestCase)));
    GBL_CTX_END();
}

GBL_EXPORT GblType GblTestSuite_type(void) {

    static const GblTypeInfo typeInfo = {
        .pFnClassInit           = GblTestSuiteClass_init_,
        .classSize              = sizeof(GblTestSuiteClass),
        .pFnInstanceInit        = GblTestSuite_init_,
        .instanceSize           = sizeof(GblTestSuite),
        .instancePrivateSize    = sizeof(GblTestSuite_)
    };

    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblType_register(GblQuark_internStatic("GblTestSuite"),
                                      GBL_OBJECT_TYPE,
                                      &typeInfo,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

GBL_EXPORT GblTestSuite* GblTestSuite_createWithVTable(const char* pName, const GblTestSuiteVTable* pVTable) {
    GblTestSuite* pSuite = NULL;
    GBL_CTX_BEGIN(NULL);

    GblTestSuiteClass* pClass = GBL_TEST_SUITE_CLASS(GblClass_createFloating(GBL_TEST_SUITE_TYPE));
    pClass->pVTable = pVTable;

    pSuite = GBL_TEST_SUITE(GblObject_createWithClass(GBL_OBJECT_CLASS(pClass), "name", pName,
                                                                             NULL));
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return pSuite;
}

GBL_EXPORT GblTestSuite* GblTestSuite_create(GblType type) {
    GblTestSuite* pSuite = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY(GblType_check(type, GBL_TEST_SUITE_TYPE),
                   GBL_RESULT_ERROR_TYPE_MISMATCH);
    pSuite = GBL_TEST_SUITE(GblObject_create(type, NULL));
    GBL_CTX_END_BLOCK();
    return pSuite;
}

GBL_EXPORT GblRefCount GblTestSuite_destroy(GblTestSuite* pSelf) {
    return GBL_UNREF(pSelf);
}

GBL_EXPORT GBL_RESULT GblTestSuite_addCase(GblTestSuite* pSelf, const char* pName, GblTestCaseRunFn pFnRun) {
    GBL_CTX_BEGIN(pSelf);

    const GblTestCase tempCase = {
        .pName      = pName,
        .pFnRun     = pFnRun
    };

    GBL_CTX_VERIFY_POINTER(pName);
    GBL_CTX_VERIFY_POINTER(pFnRun);
    GBL_CTX_VERIFY_CALL(GblArrayList_pushBack(&GBL_TEST_SUITE_(pSelf)->testCases, &tempCase));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_addCases(GblTestSuite* pSelf, const GblTestCase* pCases) {
    const GblTestCase* pIt = pCases;
    GBL_CTX_BEGIN(pSelf);
    while(pIt && pIt->pName && pIt->pFnRun) {
        GBL_CTX_VERIFY_CALL(GblTestSuite_addCase(pSelf, pIt->pName, pIt->pFnRun));
        ++pIt;
    }
    GBL_CTX_END();
}

GBL_EXPORT GblBool GblTestSuite_ran(const GblTestSuite* pSelf) {
    return pSelf && pSelf->failingIssue.result != GBL_RESULT_UNKNOWN;
}

GBL_EXPORT GblBool GblTestSuite_passed(const GblTestSuite* pSelf) {
    return GblTestSuite_ran(pSelf) && !GBL_RESULT_ERROR(pSelf->failingIssue.result);
}

static size_t  GblTestSuite_caseIndex_(const GblTestSuite* pSelf, const char* pCaseName) {
    size_t  index = GBL_NPOS;
    GBL_CTX_BEGIN(NULL);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pSelf);
    const size_t  caseCount = GblArrayList_size(&pSelf_->testCases);
    for(size_t  i = 0; i < caseCount; ++i) {
        const GblTestCase* pCase = GblArrayList_at(&pSelf_->testCases, i);
        if(strcmp(pCase->pName, pCaseName) == 0) {
            index = i;
            break;
        }
    }
    GBL_CTX_END_BLOCK();
    return index;
}

GBL_EXPORT GblBool GblTestSuite_caseRan(const GblTestSuite* pSelf, const char* pCaseName) {
    GblBool ran = GBL_FALSE;
    GBL_CTX_BEGIN(NULL);
    size_t  caseIdx = GblTestSuite_caseIndex_(pSelf, pCaseName);
    if(caseIdx != GBL_NPOS) {
        ran = (caseIdx < pSelf->casesRun);
    }
    GBL_CTX_END_BLOCK();
    return ran;

}

GBL_EXPORT GblBool GblTestSuite_casePassed(const GblTestSuite* pSelf, const char* pCaseName) {
    GblBool passed = GBL_FALSE;
    GBL_CTX_BEGIN(NULL);
    size_t  caseIdx = GblTestSuite_caseIndex_(pSelf, pCaseName);
    if(caseIdx != GBL_NPOS) {
        if(caseIdx+1 < pSelf->casesRun)
            passed = GBL_TRUE;
        else if(caseIdx+1 == pSelf->casesRun)
            passed = GblTestSuite_passed(pSelf);
    }
    GBL_CTX_END_BLOCK();
    return passed;
}

GBL_EXPORT GblTestScenario* GblTestSuite_scenario(const GblTestSuite* pSelf) {
    return pSelf? GBL_TEST_SCENARIO(GblObject_findAncestorByType(GBL_OBJECT(pSelf), GBL_TEST_SCENARIO_TYPE)) : NULL;
}


GBL_EXPORT GblType GblTestSuite_register(const char*               pName,
                                         const GblTestSuiteVTable* pVTable,
                                         size_t                    instanceSize,
                                         size_t                    instancePrivateSize,
                                         GblFlags                  typeFlags)
{
    GblType type = GBL_INVALID_TYPE;
    type = GblType_register(GblQuark_internString(pName),
                            GBL_TEST_SUITE_TYPE,
                            &(const GblTypeInfo) {
                                .pFnClassInit         = GblTestSuiteClass_initDerived_,
                                .classSize            = sizeof(GblTestSuiteClass),
                                .pClassData           = pVTable,
                                .instanceSize         = instanceSize,
                                .instancePrivateSize  = instancePrivateSize
                            },
                            typeFlags);
    return type;
}
