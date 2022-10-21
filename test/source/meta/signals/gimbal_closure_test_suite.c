#include "meta/signals/gimbal_closure_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/meta/signals/gimbal_closure.h>
#include <gimbal/meta/signals/gimbal_c_closure.h>
#include <gimbal/meta/signals/gimbal_class_closure.h>
#include <gimbal/utils/gimbal_ref.h>

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
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblClosureTestSuite_));
    pSelf_->startGlobalRefCount = GblType_instanceRefCount(GBL_BOX_TYPE);
    GBL_CTX_END();
}

static GBL_RESULT GblClosureTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblType_instanceRefCount(GBL_BOX_TYPE), pSelf_->startGlobalRefCount);
    GBL_CTX_END();
}

typedef struct TestClosure_ {
    GblClosure             base;
    GblClosureTestSuite_*  pTestSuite;
} TestClosure_;

static GBL_RESULT testClosureDestruct_(const GblArrayMap* pMap, uintptr_t key, void* pClosure) {
    GBL_UNUSED(pMap, key);
    GBL_CTX_BEGIN(NULL);
    TestClosure_* pSelf = (TestClosure_*)GBL_CLOSURE(pClosure);
    ++pSelf->pTestSuite->testClosureDtorCount;
    GBL_CTX_END();
}

static GBL_RESULT testClosureMarshal_(GblClosure* pClosure, GblVariant* pRetValue, GblSize argCount, GblVariant* pArgs, GblPtr pMarshalData) {
    GBL_CTX_BEGIN(NULL);
    TestClosure_* pSelf = (TestClosure_*)pClosure;
    pSelf->pTestSuite->pTestClosureMarshalData = pMarshalData.pData;
    pSelf->pTestSuite->testClosureMarshalArgCount = argCount;
    pSelf->pTestSuite->testClosureMarshalRetType = GblVariant_typeOf(pRetValue);

    for(GblSize a = 0; a < argCount; ++a) {
        pSelf->pTestSuite->testClosureMarshalArgTypes[a] = GblVariant_typeOf(&pArgs[a]);
    }

    GBL_CTX_END();
}

static GBL_RESULT testClosureMetaMarshal_(GblClosure* pClosure, GblVariant* pRetValue, GblSize argCount, GblVariant* pArgs, GblPtr pMarshalData) {
    GBL_UNUSED(pMarshalData);
    GBL_CTX_BEGIN(NULL);

    GblPtr ptr = { .pData = (void*)0xdeadbeef };

    GBL_CTX_VERIFY_CALL(GBL_PRIV_REF(pClosure).pFnMarshal(pClosure, pRetValue, argCount, pArgs, ptr));
    GBL_CTX_END();
}

static GBL_RESULT GblClosureTestSuite_closureCreate_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);
    pSelf_->pTestClosure = (TestClosure_*)GBL_CLOSURE(GblClosure_create(GBL_CLOSURE_TYPE,
                                                                        sizeof(TestClosure_),
                                                                        (void*)0xdeadbeef,
                                                                        testClosureDestruct_));
    GBL_TEST_VERIFY(pSelf_->pTestClosure);
    pSelf_->pTestClosure->pTestSuite = pSelf_;
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(&pSelf_->pTestClosure->base)), 1);
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(&pSelf_->pTestClosure->base)), (void*)0xdeadbeef);

    GBL_CTX_END();
}

static GBL_RESULT GblClosureTestSuite_closureSetMarshal_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblClosure_setMarshal(&pSelf_->pTestClosure->base, testClosureMarshal_);
    GBL_TEST_COMPARE(GBL_PRIV(pSelf_->pTestClosure->base).pFnMarshal, testClosureMarshal_);
    GBL_CTX_END();
}

static GBL_RESULT GblClosureTestSuite_closureInvokeMarshal_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblVariant ret;
    GblVariant args[2];

    GblVariant_constructBool(&ret, GBL_TRUE);
    GblVariant_constructString(&args[0], "Chrono Trigger");
    GblVariant_constructInt32(&args[1], 17);


    GBL_CTX_VERIFY_CALL(GblClosure_invoke(&pSelf_->pTestClosure->base,
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

    GBL_CTX_END();
}

static GBL_RESULT GblClosureTestSuite_closureSetMetaMarshal_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblClosure_setMetaMarshal(&pSelf_->pTestClosure->base, testClosureMetaMarshal_);
    GBL_TEST_VERIFY(GblClosure_hasMetaMarshal(&pSelf_->pTestClosure->base));

    GBL_CTX_END();
}

static GBL_RESULT GblClosureTestSuite_closureInvokeMetaMarshal_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblVariant ret;
    GblVariant args[3];

    GblVariant_constructString(&ret, "Secret of Mana");
    GblVariant_constructBool(&args[0], GBL_FALSE);
    GblVariant_constructFloat(&args[1], 17.777f);
    GblVariant_constructPointer(&args[2], GBL_POINTER_TYPE, (void*)0xdeadcafe);


    GBL_CTX_VERIFY_CALL(GblClosure_invoke(&pSelf_->pTestClosure->base,
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

    GBL_CTX_END();
}

static GBL_RESULT GblClosureTestSuite_closureRef_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GBL_BOX_REF(&pSelf_->pTestClosure->base), pSelf_->pTestClosure);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(&pSelf_->pTestClosure->base)), 2);

    GBL_CTX_END();
}
static GBL_RESULT GblClosureTestSuite_closureUnref_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GBL_BOX_UNREF(&pSelf_->pTestClosure->base), 1);
    GBL_TEST_COMPARE(pSelf_->testClosureDtorCount, 0);

    GBL_TEST_COMPARE(GBL_BOX_UNREF(&pSelf_->pTestClosure->base), 0);
    GBL_TEST_COMPARE(pSelf_->testClosureDtorCount, 1);

    GBL_CTX_END();
}

static void cClosureFunction_(void* pPointer, const char* pString) {
    GblClosureTestSuite_* pSelf_ = pPointer;
    pSelf_->pCClosureArg = pString;
}

static GBL_RESULT cClosureMarshal_(GblClosure* pClosure, GblVariant* pRetValue, GblSize argCount, GblVariant* pArgs, GblPtr pMarshalData) {
    GBL_UNUSED(pRetValue && argCount);
    if(!pMarshalData.pFunc) {
        GblCClosure* pCClosure = GBL_C_CLOSURE(pClosure);
        GBL_PRIV_REF(pCClosure).pFnCallback(GblVariant_toPointer(&pArgs[0]), GblVariant_toPointer(&pArgs[1]));
    }
    else pMarshalData.pFunc(GblVariant_toPointer(&pArgs[0]), GblVariant_toPointer(&pArgs[1]));
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblClosureTestSuite_cClosure_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GblCClosure* pCClosure = GblCClosure_create(cClosureFunction_, NULL);

    GblClosure_setMarshal(GBL_CLOSURE(pCClosure), cClosureMarshal_);

    GblVariant args[2];
    GblVariant_constructPointer(&args[0], GBL_POINTER_TYPE, pSelf_);
    GblVariant_constructPointer(&args[1], GBL_POINTER_TYPE, "Dragon Quest");

    GblClosure_invoke(GBL_CLOSURE(pCClosure), NULL, 2, args);

    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GBL_BOX_UNREF(pCClosure);

    GBL_TEST_COMPARE(pSelf_->pCClosureArg, "Dragon Quest");

    GBL_CTX_END();
}

static GBL_RESULT GblClosureTestSuite_classClosure_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    struct BullshitClass {
        GblClass base;
        void (*pFnMethod)(void* pPointer, const char* pString);
    } bullshitClass;

    GblClass_constructFloating(GBL_CLASS(&bullshitClass), GBL_INSTANCE_TYPE);
    bullshitClass.pFnMethod = cClosureFunction_;

    GblInstance* pInstance = GblInstance_createWithClass(GBL_CLASS(&bullshitClass));
    GblInstance_sinkClass(pInstance);


    GblClosure* pClosure = GBL_CLOSURE(GblClassClosure_create(GBL_INSTANCE_TYPE,
                                                              offsetof(struct BullshitClass, pFnMethod),
                                                              pInstance,
                                                              NULL));

    GblClosure_setMarshal(pClosure, cClosureMarshal_);

    GblVariant args[2];
    GblVariant_constructPointer(&args[0], GBL_POINTER_TYPE, pSelf_);
    GblVariant_constructPointer(&args[1], GBL_POINTER_TYPE, "Phantasy Star");

    GBL_CTX_VERIFY_CALL(GblClosure_invoke(pClosure, NULL, 2, args));

    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GBL_BOX_UNREF(pClosure);
    GblInstance_destroy(pInstance);

    GBL_TEST_COMPARE(pSelf_->pCClosureArg, "Phantasy Star");

    GBL_CTX_END();
}

static GBL_RESULT GblClosureTestSuite_captureNone_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblClosure_current(), NULL);
    GBL_CTX_END();
}

static void cClosureCaptureFunction_(void* pPointer, const char* pString) {
    GBL_UNUSED(pString);
    GblClosureTestSuite_* pSelf_ = pPointer;
    GblClosure* pClosure = GblClosure_current();
    pSelf_->pCClosureUserdata = (void*)GblBox_userdata(GBL_BOX(pClosure));
}

static GBL_RESULT GblClosureTestSuite_capture_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblClosure_current(), NULL);

    GblClosure* pClosure = GBL_CLOSURE(GblCClosure_create(GBL_CALLBACK(cClosureCaptureFunction_), (void*)0xdeadbabe));

    GblClosure_setMarshal(pClosure, cClosureMarshal_);

    GblVariant args[2];
    GblVariant_constructPointer(&args[0], GBL_POINTER_TYPE, pSelf_);
    GblVariant_constructDefault(&args[1], GBL_POINTER_TYPE);

    GblClosure_invoke(pClosure, NULL, 2, args);
    GBL_TEST_COMPARE(GblClosure_current(), NULL);

    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GBL_BOX_UNREF(pClosure);

    GBL_TEST_COMPARE(pSelf_->pCClosureUserdata, (void*)0xdeadbabe);

    GBL_CTX_END();
}

static void cClosureCaptureClosureFunction_(GblClosureTestSuite_* pSelf_, const char* pString) {
    GBL_UNUSED(pString);
    GblClosure* pThisClosure = GblClosure_current();
    GblClosure* pInnerClosure = GBL_CLOSURE(GblBox_userdata(GBL_BOX(pThisClosure)));

    GBL_CTX_BEGIN(NULL);

    GblVariant args[2];
    GblVariant_constructPointer(&args[0], GBL_POINTER_TYPE, pSelf_);
    GblVariant_constructDefault(&args[1], GBL_POINTER_TYPE);

    GblClosure_invoke(pInnerClosure, NULL, 2, args);
    GBL_TEST_COMPARE(GblClosure_current(), pThisClosure);

    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GBL_CTX_END_BLOCK();
}


static GBL_RESULT GblClosureTestSuite_captureCapture_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblClosureTestSuite_* pSelf_ = GBL_CLOSURE_TEST_SUITE_(pSelf);


    GblClosure* pClosureInner = GBL_CLOSURE(GblCClosure_create(GBL_CALLBACK(cClosureCaptureFunction_),
                                                               (void*)0xdeadbeef));
    GblClosure_setMarshal(pClosureInner, cClosureMarshal_);

    GblClosure* pClosureOuter = GBL_CLOSURE(GblCClosure_create(GBL_CALLBACK(cClosureCaptureClosureFunction_),
                                                              pClosureInner));
    GblClosure_setMarshal(pClosureOuter, cClosureMarshal_);

    GblVariant args[2];
    GblVariant_constructPointer(&args[0], GBL_POINTER_TYPE, pSelf_);
    GblVariant_constructDefault(&args[1], GBL_POINTER_TYPE);

    GblClosure_invoke(pClosureOuter, NULL, 2, args);
    GBL_TEST_COMPARE(GblClosure_current(), NULL);

    GblVariant_destruct(&args[0]);
    GblVariant_destruct(&args[1]);

    GBL_BOX_UNREF(pClosureOuter);
    GBL_BOX_UNREF(pClosureInner);

    GBL_TEST_COMPARE(pSelf_->pCClosureUserdata, (void*)0xdeadbeef);

    GBL_CTX_END();
}


GBL_EXPORT GblType GblClosureTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "closureCreate",              GblClosureTestSuite_closureCreate_              },
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
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("ClosureTestSuite"),
                                     &vTable,
                                     sizeof(GblClosureTestSuite),
                                     sizeof(GblClosureTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}
