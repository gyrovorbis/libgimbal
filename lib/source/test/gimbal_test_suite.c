#include <gimbal/test/gimbal_test_suite.h>
#include <gimbal/test/gimbal_test_scenario.h>
#include <gimbal/containers/gimbal_vector.h>

#define GBL_TEST_SUITE_(inst)           ((GblTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_TEST_SUITE_TYPE))

typedef struct GblTestSuite_ {
    GblVector       testCases;
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
    *pSize = GblVector_size(&pSelf_->testCases);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_caseName_(const GblTestSuite* pSelf, GblSize index, const char** ppName) {
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY_POINTER(ppName);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pSelf);
    const GblTestCase* pCase = GblVector_at(&pSelf_->testCases, index);
    GBL_API_VERIFY_LAST_RECORD();
    *ppName = pCase->pName;
    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_caseRun_(GblTestSuite* pSelf, GblContext* pCtx, GblSize index) {
    GBL_API_BEGIN(pCtx);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pSelf);
    const GblTestCase* pCase = GblVector_at(&pSelf_->testCases, index);
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

static GBL_RESULT GblTestSuiteClass_propertyGet_(const GblObject* pSelf, GblSize id, GblVariant* pValue, const GblProperty* pProp) {
    GBL_API_BEGIN(pSelf);
    GblTestSuite* pSuite = GBL_TEST_SUITE(pSelf);
    GblTestSuite_* pSuite_ = GBL_TEST_SUITE_(pSelf);
    switch(id) {
    case GBL_TEST_SUITE_PROPERTY_ID_RESULT:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pSuite->failingIssue.result);
        break;
    case GBL_TEST_SUITE_PROPERTY_ID_FAILURE_MSG:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pSuite->failingIssue.message);
        break;
    case GBL_TEST_SUITE_PROPERTY_ID_CASE_COUNT:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), GblVector_size(&pSuite_->testCases));
        break;
    case GBL_TEST_SUITE_PROPERTY_ID_CASES_RUN:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pSuite->casesRun);
        break;
    case GBL_TEST_SUITE_PROPERTY_ID_CASES_PASSED:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pSuite->casesPassed);
        break;
    case GBL_TEST_SUITE_PROPERTY_ID_CASES_FAILED:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pSuite->casesFailed);
        break;
    case GBL_TEST_SUITE_PROPERTY_ID_CASES_SKIPPED:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), pSuite->casesSkipped);
        break;
    default: GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "[GblTestSuite] Reading unhandled property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_constructed_(GblObject* pSelf) {
    GBL_API_BEGIN(pSelf);

    GblObjectClass* pSuper = GBL_OBJECT_CLASS(GblClass_peek(GBL_OBJECT_TYPE));
    if(pSuper && pSuper->pFnConstructed) GBL_API_VERIFY_CALL(pSuper->pFnConstructed(pSelf));

    const char* pName = GblObject_name(pSelf);
    if(!pName) {
        GblObject_nameSet(pSelf, GblInstance_typeName(GBL_INSTANCE(pSelf)));
    }

    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(pClass->pVTable && pClass->pVTable->pCases) {
        GBL_API_VERIFY_CALL(GblTestSuite_casesAdd(GBL_TEST_SUITE(pSelf),
                                                  pClass->pVTable->pCases));
    }

    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_destructor_(GblObject* pSelf) {
    GBL_API_BEGIN(pSelf);
    GblObjectClass* pSuper = GBL_OBJECT_CLASS(GblClass_peek(GBL_OBJECT_TYPE));
    if(pSuper && pSuper->pFnDestructor) GBL_API_VERIFY_CALL(pSuper->pFnDestructor(pSelf));

    GBL_API_VERIFY_CALL(GblVector_destruct(&GBL_TEST_SUITE_(pSelf)->testCases));
    GBL_API_END();
}

static GBL_RESULT GblTestSuiteClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);

    if(!GblType_classRefCount(GBL_TEST_SUITE_TYPE)) {
        GBL_PROPERTY_TABLE_REGISTER(GBL_TEST_SUITE, pClass);
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
    pSelf->base.pFnDestructor       = GblTestSuiteClass_destructor_;
    pSelf->base.pFnPropertyGet      = GblTestSuiteClass_propertyGet_;

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
    GBL_INSTANCE_VCALL_PREFIX(GBL_TEST_SUITE, pFnSuiteName, pSelf, &pName);
    GBL_API_VERIFY_LAST_RECORD();
    if(!pName) pName = "";
    GBL_API_END_BLOCK();
    return pName;
}

GBL_EXPORT GBL_RESULT GblTestSuite_suiteInit(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnSuiteInit)
        GBL_API_DONE();
    GBL_INSTANCE_VCALL_PREFIX(GBL_TEST_SUITE, pVTable->pFnSuiteInit, pSelf, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_suiteFinal(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnSuiteFinal)
        GBL_API_DONE();
    GBL_INSTANCE_VCALL_PREFIX(GBL_TEST_SUITE, pVTable->pFnSuiteFinal, pSelf, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_caseInit(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnCaseInit)
        GBL_API_DONE();
    GBL_INSTANCE_VCALL_PREFIX(GBL_TEST_SUITE, pVTable->pFnCaseInit, pSelf, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_caseFinal(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTestSuiteClass* pClass = GBL_TEST_SUITE_GET_CLASS(pSelf);
    if(!pClass->pVTable || !pClass->pVTable->pFnCaseFinal)
        GBL_API_DONE();
    GBL_INSTANCE_VCALL_PREFIX(GBL_TEST_SUITE, pVTable->pFnCaseFinal, pSelf, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GblSize GblTestSuite_caseCount(const GblTestSuite* pSelf) {
    GblSize count = 0;
    GBL_API_BEGIN(pSelf);
    GBL_INSTANCE_VCALL_PREFIX(GBL_TEST_SUITE, pFnCaseCount, pSelf, &count);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT const char* GblTestSuite_caseName(const GblTestSuite* pSelf, GblSize index) {
    const char* pName = NULL;
    GBL_API_BEGIN(pSelf);
    GBL_INSTANCE_VCALL_PREFIX(GBL_TEST_SUITE, pFnCaseName, pSelf, index, &pName);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return pName;
}

GBL_EXPORT GBL_RESULT GblTestSuite_caseRun(GblTestSuite* pSelf,
                                           GblContext* pCtx,
                                           GblSize index)
{
    GBL_API_BEGIN(pSelf);
    GBL_INSTANCE_VCALL_PREFIX(GBL_TEST_SUITE, pFnCaseRun, pSelf, pCtx, index);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

static GBL_RESULT GblTestSuite_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTestSuite_* pSelf_ = GBL_TEST_SUITE_(pInstance);
    GBL_API_VERIFY_CALL(GblVector_construct(&pSelf_->testCases, sizeof(GblTestCase)));
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
        type = GblType_registerStatic(GBL_OBJECT_TYPE,
                                      "TestSuite",
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
    pSuite = GBL_TEST_SUITE(GblObject_new(GBL_TEST_SUITE_TYPE, "name", pName,
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

    pSuite = GBL_TEST_SUITE(GblObject_newWithClass(GBL_OBJECT_CLASS(pClass), "name", pName,
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
    pSuite = GBL_TEST_SUITE(GblObject_new(type, NULL));
    GBL_API_END_BLOCK();
    return pSuite;
}

GBL_EXPORT GBL_RESULT GblTestSuite_destroy(GblTestSuite* pSelf) {
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY(GblTestSuite_scenario(pSelf) == NULL,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Tried to destroy a GblTestSuite owned by a scenario!");
    GBL_API_VERIFY(GblObject_unref(GBL_OBJECT(pSelf)) == 0,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Tried to destroy GblTestSuite, has other references!");
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_caseAdd(GblTestSuite* pSelf, const char* pName, GblTestCaseRunFn pFnRun) {
    GBL_API_BEGIN(pSelf);

    const GblTestCase tempCase = {
        .pName      = pName,
        .pFnRun     = pFnRun
    };

    GBL_API_VERIFY_POINTER(pName);
    GBL_API_VERIFY_POINTER(pFnRun);
    GBL_API_VERIFY_CALL(GblVector_pushBack(&GBL_TEST_SUITE_(pSelf)->testCases, &tempCase));

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblTestSuite_casesAdd(GblTestSuite* pSelf, const GblTestCase* pCases) {
    const GblTestCase* pIt = pCases;
    GBL_API_BEGIN(pSelf);
    while(pIt && pIt->pName && pIt->pFnRun) {
        GBL_API_VERIFY_CALL(GblTestSuite_caseAdd(pSelf, pIt->pName, pIt->pFnRun));
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
    const GblSize caseCount = GblVector_size(&pSelf_->testCases);
    for(GblSize i = 0; i < caseCount; ++i) {
        const GblTestCase* pCase = GblVector_at(&pSelf_->testCases, i);
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
    return pSelf? GBL_TEST_SCENARIO(GblObject_ancestorFindByType(GBL_OBJECT(pSelf), GBL_TEST_SCENARIO_TYPE)) : NULL;
}


GBL_EXPORT GblType GblTestSuite_register(const char* pName,
                                         const GblTestSuiteClassVTable* pVTable,
                                         GblSize instanceSize,
                                         GblSize instancePrivateSize,
                                         GblFlags typeFlags)
{
    GblType type = GBL_INVALID_TYPE;
    GBL_API_BEGIN(NULL);
    type = GblType_registerStatic(GBL_TEST_SUITE_TYPE,
                                  GblQuark_internString(pName),
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
