#include "types/gimbal_closure_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/types/gimbal_closure.h>
#include <gimbal/types/gimbal_ref.h>

#define GBL_CLOSURE_TEST_SUITE_(inst)   ((GblClosureTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_CLOSURE_TEST_SUITE_TYPE))

GBL_FORWARD_DECLARE_STRUCT(TestClosure_);

typedef struct GblClosureTestSuite_ {
    GblRefCount     startGlobalRefCount;
    TestClosure_*   pTestClosure;
    GblSize         testClosureDtorCount;
    void*           pTestClosureMarshalData;
    GblType         testClosureMarshalArgTypes[4];
    GblSize         testClosureMarshalArgCount;
    GblType         testClosureMarshalRetType;
    const char*     pCClosureArg;
    void*           pCClosureUserdata;
} GblClosureTestSuite_;

static GBL_RESULT GblClosureTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblClosureTestSuite_));
    pSelf_->startGlobalRefCount = GblRef_activeCount();
    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->startGlobalRefCount);
    GBL_API_END();
}

typedef struct TestClosure_ {
    GblClosure             base;
    GblClosureTestSuite_*  pTestSuite;
} TestClosure_;

static GBL_RESULT testClosureDestruct_(GblClosure* pClosure) {
    GBL_API_BEGIN(GblClosure_context(pClosure));
    TestClosure_* pSelf = (TestClosure_*)pClosure;
    ++pSelf->pTestSuite->testClosureDtorCount;
    GBL_API_END();
}

static GBL_RESULT testClosureMarshal_(GblClosure* pClosure, GblVariant* pRetValue, GblSize argCount, GblVariant* pArgs, GblPtr pMarshalData) {
    GBL_API_BEGIN(GblClosure_context(pClosure));
    TestClosure_* pSelf = (TestClosure_*)pClosure;
    pSelf->pTestSuite->pTestClosureMarshalData = pMarshalData.pData;
    pSelf->pTestSuite->testClosureMarshalArgCount = argCount;
    pSelf->pTestSuite->testClosureMarshalRetType = GblVariant_typeOf(pRetValue);

    for(GblSize a = 0; a < argCount; ++a) {
        pSelf->pTestSuite->testClosureMarshalArgTypes[a] = GblVariant_typeOf(&pArgs[a]);
    }

    GBL_API_END();
}

static GBL_RESULT testClosureMetaMarshal_(GblClosure* pClosure, GblVariant* pRetValue, GblSize argCount, GblVariant* pArgs, GblPtr pMarshalData) {
    GBL_UNUSED(pMarshalData);
    GBL_API_BEGIN(GblClosure_context(pClosure));

    GblPtr ptr = { .pData = (void*)0xdeadbeef };

    GBL_API_VERIFY_CALL(pClosure->pFnMarshal(pClosure, pRetValue, argCount, pArgs, ptr));
    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_closureCreate_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);
    pSelf_->pTestClosure = (TestClosure_*)GblClosure_createWithContext(sizeof(TestClosure_),
                                                                        NULL,
                                                                        testClosureDestruct_,
                                                                        pCtx);
    GBL_TEST_VERIFY(pSelf_->pTestClosure);
    pSelf_->pTestClosure->pTestSuite = pSelf_;
    GBL_TEST_COMPARE(GblClosure_context(&pSelf_->pTestClosure->base), pCtx);
    GBL_TEST_COMPARE(GblClosure_refCount(&pSelf_->pTestClosure->base), 1);

    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_closureSetUserdata_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblClosure_setUserdata(&pSelf_->pTestClosure->base, pSelf_->pTestClosure);
    GBL_TEST_COMPARE(pSelf_->pTestClosure->base.pUserdata, pSelf_->pTestClosure);
    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_closureSetMarshal_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblClosure_setMarshal(&pSelf_->pTestClosure->base, testClosureMarshal_);
    GBL_TEST_COMPARE(pSelf_->pTestClosure->base.pFnMarshal, testClosureMarshal_);
    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_closureInvokeMarshal_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblVariant ret;
    GblVariant args[2];

    GblVariant_constructBool(&ret, GBL_TRUE);
    GblVariant_constructString(&args[0], "Chrono Trigger");
    GblVariant_constructInt32(&args[1], 17);


    GBL_API_VERIFY_CALL(GblClosure_invoke(&pSelf_->pTestClosure->base,
                                          &ret,
                                          2,
                                          args));

    GBL_TEST_COMPARE(pSelf_->pTestClosureMarshalData, NULL);
    GBL_TEST_COMPARE(pSelf_->testClosureMarshalRetType, GBL_BOOL_TYPE);
    GBL_TEST_COMPARE(pSelf_->testClosureMarshalArgCount, 2);
    GBL_TEST_COMPARE(pSelf_->testClosureMarshalArgTypes[0], GBL_STRING_TYPE);
    GBL_TEST_COMPARE(pSelf_->testClosureMarshalArgTypes[1], GBL_INT32_TYPE);

    GblVariant_destruct(&ret);
    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_closureSetMetaMarshal_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblClosure_setMetaMarshal(&pSelf_->pTestClosure->base, testClosureMetaMarshal_, (void*)0xcafebabe);
    GBL_TEST_COMPARE(pSelf_->pTestClosure->base.pFnMetaMarshal, testClosureMetaMarshal_);
    GBL_TEST_COMPARE(pSelf_->pTestClosure->base.pMetaMarshalData, (void*)0xcafebabe);

    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_closureInvokeMetaMarshal_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblVariant ret;
    GblVariant args[3];

    GblVariant_constructString(&ret, "Secret of Mana");
    GblVariant_constructBool(&args[0], GBL_FALSE);
    GblVariant_constructFloat(&args[1], 17.777f);
    GblVariant_constructPointer(&args[2], (void*)0xdeadcafe);


    GBL_API_VERIFY_CALL(GblClosure_invoke(&pSelf_->pTestClosure->base,
                                          &ret,
                                          3,
                                          args));

    GBL_TEST_COMPARE(pSelf_->pTestClosureMarshalData, (void*)0xdeadbeef);
    GBL_TEST_COMPARE(pSelf_->testClosureMarshalRetType, GBL_STRING_TYPE);
    GBL_TEST_COMPARE(pSelf_->testClosureMarshalArgCount, 3);
    GBL_TEST_COMPARE(pSelf_->testClosureMarshalArgTypes[0], GBL_BOOL_TYPE);
    GBL_TEST_COMPARE(pSelf_->testClosureMarshalArgTypes[1], GBL_FLOAT_TYPE);
    GBL_TEST_COMPARE(pSelf_->testClosureMarshalArgTypes[2], GBL_POINTER_TYPE);

    GblVariant_destruct(&ret);
    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);
    GblVariant_destruct(&args[2]);

    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_closureRef_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblClosure_ref(&pSelf_->pTestClosure->base), pSelf_->pTestClosure);
    GBL_TEST_COMPARE(GblClosure_refCount(&pSelf_->pTestClosure->base), 2);

    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_closureUnref_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblClosure_unref(&pSelf_->pTestClosure->base), 1);
    GBL_TEST_COMPARE(pSelf_->testClosureDtorCount, 0);

    GBL_TEST_COMPARE(GblClosure_unref(&pSelf_->pTestClosure->base), 0);
    GBL_TEST_COMPARE(pSelf_->testClosureDtorCount, 1);

    GBL_API_END();
}

static void cClosureFunction_(void* pPointer, const char* pString) {
    GblClosureTestSuite_* pSelf_ = pPointer;
    pSelf_->pCClosureArg = pString;
}

static GBL_RESULT cClosureMarshal_(GblClosure* pClosure, GblVariant* pRetValue, GblSize argCount, GblVariant* pArgs, GblPtr pMarshalData) {
    GBL_UNUSED(pRetValue && argCount);
    GblCClosure* pCClosure = (GblCClosure*)pClosure;
    if(!pMarshalData.pFunc) pCClosure->pFnCCallback(GblVariant_toPointer(&pArgs[0]), GblVariant_toPointer(&pArgs[1]));
    else pMarshalData.pFunc(GblVariant_toPointer(&pArgs[0]), GblVariant_toPointer(&pArgs[1]));
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblClosureTestSuite_cClosure_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblClosure* pClosure = GblCClosure_createWithContext(GBL_CALLBACK(cClosureFunction_), NULL, pCtx);
    GblClosure_setMarshal(pClosure, cClosureMarshal_);

    GblVariant args[2];
    GblVariant_constructPointer(&args[0], pSelf_);
    GblVariant_constructPointer(&args[1], "Dragon Quest");

    GblClosure_invoke(pClosure, NULL, 2, args);

    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GblClosure_unref(pClosure);

    GBL_TEST_COMPARE(pSelf_->pCClosureArg, "Dragon Quest");

    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_classClosure_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    struct BullshitClass {
        GblClass base;
        void (*pFnMethod)(void* pPointer, const char* pString);
    } bullshitClass;

    GblClass_constructFloating(GBL_CLASS(&bullshitClass), GBL_INSTANCE_TYPE);
    bullshitClass.pFnMethod = cClosureFunction_;

    GblInstance* pInstance = GblInstance_createWithClass(GBL_CLASS(&bullshitClass));
    GblInstance_sinkClass(pInstance);


    GblClosure* pClosure = GblClassClosure_createWithContext(GBL_INSTANCE_TYPE,
                                                             offsetof(struct BullshitClass, pFnMethod),
                                                             pInstance,
                                                             pCtx);
    GblClosure_setMarshal(pClosure, cClosureMarshal_);

    GblVariant args[2];
    GblVariant_constructPointer(&args[0], pSelf_);
    GblVariant_constructPointer(&args[1], "Phantasy Star");

    GblClosure_invoke(pClosure, NULL, 2, args);

    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GblClosure_unref(pClosure);
    GblInstance_destroy(pInstance);

    GBL_TEST_COMPARE(pSelf_->pCClosureArg, "Phantasy Star");

    GBL_API_END();
}

static GBL_RESULT GblClosureTestSuite_captureNone_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblClosure_current(), NULL);
    GBL_API_END();
}

static void cClosureCaptureFunction_(void* pPointer, const char* pString) {
    GBL_UNUSED(pString);
    GblClosureTestSuite_* pSelf_ = pPointer;
    GblClosure* pClosure = GblClosure_current();
    pSelf_->pCClosureUserdata = pClosure->pUserdata;
}

static GBL_RESULT GblClosureTestSuite_capture_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblClosure_current(), NULL);


    GblClosure* pClosure = GblCClosure_createWithContext(GBL_CALLBACK(cClosureCaptureFunction_),
                                                         (void*)0xdeadbabe,
                                                         pCtx);
    GblClosure_setMarshal(pClosure, cClosureMarshal_);

    GblVariant args[2];
    GblVariant_constructPointer(&args[0], pSelf_);
    GblVariant_constructDefault(&args[1], GBL_POINTER_TYPE);

    GblClosure_invoke(pClosure, NULL, 2, args);
    GBL_TEST_COMPARE(GblClosure_current(), NULL);

    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GblClosure_unref(pClosure);

    GBL_TEST_COMPARE(pSelf_->pCClosureUserdata, (void*)0xdeadbabe);

    GBL_API_END();
}

static void cClosureCaptureClosureFunction_(GblClosureTestSuite_* pSelf_, const char* pString) {
    GBL_UNUSED(pString);
    GblClosure* pThisClosure = GblClosure_current();
    GblClosure* pInnerClosure = pThisClosure->pUserdata;

    GBL_API_BEGIN(GblClosure_context(pThisClosure));

    GblVariant args[2];
    GblVariant_constructPointer(&args[0], pSelf_);
    GblVariant_constructDefault(&args[1], GBL_POINTER_TYPE);

    GblClosure_invoke(pInnerClosure, NULL, 2, args);
    GBL_TEST_COMPARE(GblClosure_current(), pThisClosure);

    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GBL_API_END_BLOCK();
}


static GBL_RESULT GblClosureTestSuite_captureCapture_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);


    GblClosure* pClosureInner = GblCClosure_createWithContext(GBL_CALLBACK(cClosureCaptureFunction_),
                                                         (void*)0xdeadbeef,
                                                         pCtx);
    GblClosure_setMarshal(pClosureInner, cClosureMarshal_);

    GblClosure* pClosureOuter = GblCClosure_createWithContext(GBL_CALLBACK(cClosureCaptureClosureFunction_),
                                                              pClosureInner,
                                                              pCtx);
    GblClosure_setMarshal(pClosureOuter, cClosureMarshal_);

    GblVariant args[2];
    GblVariant_constructPointer(&args[0], pSelf_);
    GblVariant_constructDefault(&args[1], GBL_POINTER_TYPE);

    GblClosure_invoke(pClosureOuter, NULL, 2, args);
    GBL_TEST_COMPARE(GblClosure_current(), NULL);

    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GblClosure_unref(pClosureOuter);
    GblClosure_unref(pClosureInner);

    GBL_TEST_COMPARE(pSelf_->pCClosureUserdata, (void*)0xdeadbeef);

    GBL_API_END();
}


GBL_EXPORT GblType GblClosureTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "closureCreate",              GblClosureTestSuite_closureCreate_              },
        { "closureSetUserdata",         GblClosureTestSuite_closureSetUserdata_         },
        { "closureSetMarshal",          GblClosureTestSuite_closureSetMarshal_          },
        { "closureInvokeMarshal",       GblClosureTestSuite_closureInvokeMarshal_       },
        { "closureSetMetaMarshal",      GblClosureTestSuite_closureSetMetaMarshal_      },
        { "closureInvokeMetaMarshal",   GblClosureTestSuite_closureInvokeMetaMarshal_   },
        { "closureRef",                 GblClosureTestSuite_closureRef_                 },
        { "closureUnref",               GblClosureTestSuite_closureUnref_               },
        { "cClosure",                   GblClosureTestSuite_cClosure_                   },
        { "classClosure",               GblClosureTestSuite_classClosure_               },
        { "captureNone",                GblClosureTestSuite_captureNone_                },
        { "capture",                    GblClosureTestSuite_capture_                    },
        { "captureCapture",             GblClosureTestSuite_captureCapture_             },
        { NULL,                         NULL                                            }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblClosureTestSuite_init_,
        .pFnSuiteFinal  = GblClosureTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("ClosureTestSuite"),
                                     &vTable,
                                     sizeof(GblClosureTestSuite),
                                     sizeof(GblClosureTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}
