#include <gimbal/test/gimbal_test_suite.h>
#include <gimbal/test/gimbal_test_scenario.h>
#include <gimbal/containers/gimbal_array_list.h>

#define GBL_TEST_SUITE_(inst)           ((GblTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_TEST_SUITE_TYPE))

typedef struct GblTestSuite_ {
    GblArrayList       testCases;
} GblTestSuite_;

static GBL_RESULT GblTestSuiteClass_vTableDefault_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf && pCtx);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblTestSuiteClass_suiteName_(const GblTestSuite* pSelf, const char** ppName) {
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY_POINTER(ppName);
    *ppName = GblObject_name(GBL_OBJECT(pSelf));
    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_caseCount_(const GblTestSuite* pSelf, GblSize* pSize) {
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY_POINTER(pSize);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pSelf);
    *pSize = GblArrayList_size(&pSelf_->testCases);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_caseName_(const GblTestSuite* pSelf, GblSize index, const char** ppName) {
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY_POINTER(ppName);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pSelf);
    const GblTestCase* pCase = GblArrayList_at(&pSelf_->testCases, index);
    GBL_API_VERIFY_LAST_RECORD();
    *ppName = pCase->pName;
    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_caseRun_(GblTestSuite* pSelf, GblContext* pCtx, GblSize index) {
    GBL_API_BEGIN(pCtx);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pSelf);
    const GblTestCase* pCase = GblArrayList_at(&pSelf_->testCases, index);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_VERIFY_CALL(pCase->pFnRun(pSelf, pCtx));
    GBL_API_END_BLOCK();

    if(GBL_API_RESULT() == GBL_RESULT_SKIPPED) {
        ++pSelf->casesSkipped;
    } else {
        memcpy(&pSelf->failingIssue, &GBL_API_RECORD(), sizeof(GblCallRecord));

        ++pSelf->casesRun;
        if(!GBL_RESULT_ERROR(GBL_API_RESULT())) {
            ++pSelf->casesPassed;
        } else {
            ++pSelf->casesFailed;
        }
    }
    return GBL_API_RESULT();
}

static GBL_RESULT GblTestSuiteClass_property_(const GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue) {
    GBL_API_BEGIN(pSelf);
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
    default: GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "[GblTestSuite] Reading unhandled property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_constructed_(GblObject* pSelf) {
    GBL_API_BEGIN(pSelf);

    GblObjectClass* pSuper = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));
    if(pSuper && pSuper->pFnConstructed) GBL_API_VERIFY_CALL(pSuper->pFnConstructed(pSelf));

    const char* pName = GblObject_name(pSelf);
    if(!pName) {
        GblObject_setName(pSelf, GblType_name(GblInstance_typeOf(GBL_INSTANCE(pSelf))));
    }

    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(pClass->pVTable && pClass->pVTable->pCases) {
        GBL_API_VERIFY_CALL(GblTestSuite_addCases(GBL_TEST_SUITE(pSelf),
                                                  pClass->pVTable->pCases));
    }

    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_destructor_(GblBox* pSelf) {
    GBL_API_BEGIN(pSelf);
    GblObjectClass* pSuper = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));
    if(pSuper && pSuper->base.pFnDestructor) GBL_API_VERIFY_CALL(pSuper->base.pFnDestructor(pSelf));

    GBL_API_VERIFY_CALL(GblArrayList_destruct(&GBL_TEST_SUITE_(pSelf)->testCases));
    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);

    if(!GblType_classRefCount(GBL_TEST_SUITE_TYPE)) {
        GBL_PROPERTIES_REGISTER(GblTestSuite);
    }

    const static GblTestSuiteClassVTable defaultVTable = {
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

    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_initDerived_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);

    GblTestSuiteClass* pSelf        = GBL_TEST_SUITE_CLASS(pClass);

    pSelf->pVTable                  = pUd;

    GBL_API_END();
}


GBL_EXPORT const char* GblTestSuite_name(const GblTestSuite* pSelf) {
    const char* pName = "";
    GBL_API_BEGIN(pSelf);
    GBL_INSTANCE_VCALL(GBL_TEST_SUITE_TYPE, GblTestSuiteClass, pFnSuiteName, pSelf, &pName);
    GBL_API_VERIFY_LAST_RECORD();
    if(!pName) pName = "";
    GBL_API_END_BLOCK();
    return pName;
}

GBL_EXPORT GBL_RESULT GblTestSuite_initSuite(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnSuiteInit)
        GBL_API_DONE();
    GBL_INSTANCE_VCALL(GBL_TEST_SUITE_TYPE, GblTestSuiteClass, pVTable->pFnSuiteInit, pSelf, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_finalSuite(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnSuiteFinal)
        GBL_API_DONE();
    GBL_INSTANCE_VCALL(GBL_TEST_SUITE_TYPE, GblTestSuiteClass, pVTable->pFnSuiteFinal, pSelf, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_initCase(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnCaseInit)
        GBL_API_DONE();
    GBL_INSTANCE_VCALL(GBL_TEST_SUITE_TYPE, GblTestSuiteClass, pVTable->pFnCaseInit, pSelf, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_finalCase(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnCaseFinal)
        GBL_API_DONE();
    GBL_INSTANCE_VCALL(GBL_TEST_SUITE_TYPE, GblTestSuiteClass, pVTable->pFnCaseFinal, pSelf, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GblSize GblTestSuite_caseCount(const GblTestSuite* pSelf) {
    GblSize count = 0;
    GBL_API_BEGIN(pSelf);
    GBL_INSTANCE_VCALL(GBL_TEST_SUITE_TYPE, GblTestSuiteClass, pFnCaseCount, pSelf, &count);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT const char* GblTestSuite_caseName(const GblTestSuite* pSelf, GblSize index) {
    const char* pName = NULL;
    GBL_API_BEGIN(pSelf);
    GBL_INSTANCE_VCALL(GBL_TEST_SUITE_TYPE, GblTestSuiteClass, pFnCaseName, pSelf, index, &pName);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return pName;
}

GBL_EXPORT GBL_RESULT GblTestSuite_runCase(GblTestSuite* pSelf,
                                           GblContext* pCtx,
                                           GblSize index)
{
    GBL_API_BEGIN(pSelf);
    GBL_INSTANCE_VCALL(GBL_TEST_SUITE_TYPE, GblTestSuiteClass, pFnCaseRun, pSelf, pCtx, index);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

static GBL_RESULT GblTestSuite_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pInstance);
    GBL_API_VERIFY_CALL(GblArrayList_construct(&pSelf_->testCases, sizeof(GblTestCase)));
    GBL_API_END();
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
        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("TestSuite"),
                                      GBL_OBJECT_TYPE,
                                      &typeInfo,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

GBL_EXPORT GblTestSuite* GblTestSuite_create(const char* pName) {
    GblTestSuite* pSuite = NULL;

    GBL_API_BEGIN(NULL);
    pSuite = GBL_TEST_SUITE(GblObject_create(GBL_TEST_SUITE_TYPE, "name", pName,
                                                               NULL));

    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return pSuite;
}

GBL_EXPORT GblTestSuite* GblTestSuite_createWithVTable(const char* pName, const GblTestSuiteClassVTable* pVTable) {
    GblTestSuite* pSuite = NULL;
    GBL_API_BEGIN(NULL);

    GblTestSuiteClass* pClass = GBL_TEST_SUITE_CLASS(GblClass_createFloating(GBL_TEST_SUITE_TYPE));
    pClass->pVTable = pVTable;

    pSuite = GBL_TEST_SUITE(GblObject_createWithClass(GBL_OBJECT_CLASS(pClass), "name", pName,
                                                                             NULL));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return pSuite;
}

GBL_EXPORT GblTestSuite* GblTestSuite_createFromType(GblType type) {
    GblTestSuite* pSuite = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY(GblType_check(type, GBL_TEST_SUITE_TYPE),
                   GBL_RESULT_ERROR_TYPE_MISMATCH);
    pSuite = GBL_TEST_SUITE(GblObject_create(type, NULL));
    GBL_API_END_BLOCK();
    return pSuite;
}

GBL_EXPORT GBL_RESULT GblTestSuite_destroy(GblTestSuite* pSelf) {
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY(GblTestSuite_scenario(pSelf) == NULL,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Tried to destroy a GblTestSuite owned by a scenario!");
    GBL_API_VERIFY(GblBox_unref(GBL_BOX(pSelf)) == 0,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Tried to destroy GblTestSuite, has other references!");
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_addCase(GblTestSuite* pSelf, const char* pName, GblTestCaseRunFn pFnRun) {
    GBL_API_BEGIN(pSelf);

    const GblTestCase tempCase = {
        .pName      = pName,
        .pFnRun     = pFnRun
    };

    GBL_API_VERIFY_POINTER(pName);
    GBL_API_VERIFY_POINTER(pFnRun);
    GBL_API_VERIFY_CALL(GblArrayList_pushBack(&GBL_TEST_SUITE_(pSelf)->testCases, &tempCase));

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_addCases(GblTestSuite* pSelf, const GblTestCase* pCases) {
    const GblTestCase* pIt = pCases;
    GBL_API_BEGIN(pSelf);
    while(pIt && pIt->pName && pIt->pFnRun) {
        GBL_API_VERIFY_CALL(GblTestSuite_addCase(pSelf, pIt->pName, pIt->pFnRun));
        ++pIt;
    }
    GBL_API_END();
}

GBL_EXPORT GblBool GblTestSuite_ran(const GblTestSuite* pSelf) {
    return pSelf && pSelf->failingIssue.result != GBL_RESULT_UNKNOWN;
}

GBL_EXPORT GblBool GblTestSuite_passed(const GblTestSuite* pSelf) {
    return GblTestSuite_ran(pSelf) && !GBL_RESULT_ERROR(pSelf->failingIssue.result);
}

static GblSize GblTestSuite_caseIndex_(const GblTestSuite* pSelf, const char* pCaseName) {
    GblSize index = GBL_NPOS;
    GBL_API_BEGIN(NULL);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pSelf);
    const GblSize caseCount = GblArrayList_size(&pSelf_->testCases);
    for(GblSize i = 0; i < caseCount; ++i) {
        const GblTestCase* pCase = GblArrayList_at(&pSelf_->testCases, i);
        if(strcmp(pCase->pName, pCaseName) == 0) {
            index = i;
            break;
        }
    }
    GBL_API_END_BLOCK();
    return index;
}

GBL_EXPORT GblBool GblTestSuite_caseRan(const GblTestSuite* pSelf, const char* pCaseName) {
    GblBool ran = GBL_FALSE;
    GBL_API_BEGIN(NULL);
    GblSize caseIdx = GblTestSuite_caseIndex_(pSelf, pCaseName);
    if(caseIdx != GBL_NPOS) {
        ran = (caseIdx < pSelf->casesRun);
    }
    GBL_API_END_BLOCK();
    return ran;

}

GBL_EXPORT GblBool GblTestSuite_casePassed(const GblTestSuite* pSelf, const char* pCaseName) {
    GblBool passed = GBL_FALSE;
    GBL_API_BEGIN(NULL);
    GblSize caseIdx = GblTestSuite_caseIndex_(pSelf, pCaseName);
    if(caseIdx != GBL_NPOS) {
        if(caseIdx+1 < pSelf->casesRun)
            passed = GBL_TRUE;
        else if(caseIdx+1 == pSelf->casesRun)
            passed = GblTestSuite_passed(pSelf);
    }
    GBL_API_END_BLOCK();
    return passed;
}

GBL_EXPORT GblTestScenario* GblTestSuite_scenario(const GblTestSuite* pSelf) {
    return pSelf? GBL_TEST_SCENARIO(GblObject_findAncestorByType(GBL_OBJECT(pSelf), GBL_TEST_SCENARIO_TYPE)) : NULL;
}


GBL_EXPORT GblType GblTestSuite_register(const char* pName,
                                         const GblTestSuiteClassVTable* pVTable,
                                         GblSize instanceSize,
                                         GblSize instancePrivateSize,
                                         GblFlags typeFlags)
{
    GblType type = GBL_INVALID_TYPE;
    GBL_API_BEGIN(NULL);
    type = GblType_registerStatic(GblQuark_internString(pName),
                                  GBL_TEST_SUITE_TYPE,
                                  &(const GblTypeInfo) {
                                      .pFnClassInit         = GblTestSuiteClass_initDerived_,
                                      .classSize            = sizeof(GblTestSuiteClass),
                                      .pClassData           = pVTable,
                                      .instanceSize         = instanceSize,
                                      .instancePrivateSize  = instancePrivateSize
                                  },
                                  typeFlags);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return type;
}
