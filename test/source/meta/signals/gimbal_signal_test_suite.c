#include "meta/signals/gimbal_signal_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/meta/signals/gimbal_signal.h>
#include <gimbal/meta/signals/gimbal_closure.h>
#include <gimbal/meta/signals/gimbal_c_closure.h>

#define GBL_SIGNAL_TEST_SUITE_(inst)    (GBL_PRIVATE(GblSignalTestSuite, inst))

typedef enum TYPE_ {
    TYPE_I_A_,
    TYPE_C_A_,
    TYPE_I_B_1_,
    TYPE_I_B_BASE_,
    TYPE_I_B_,
    TYPE_C_B_,
    TYPE_COUNT_,
} TYPE_;

typedef enum INSTANCE_ {
    INSTANCE_1_,
    INSTANCE_2_,
    INSTANCE_3_,
    INSTANCE_4_,
    INSTANCE_5_,
    INSTANCE_COUNT_
} INSTANCE_;

typedef enum SIGNAL_ {
    SIGNAL_I_A_,
    SIGNAL_C_A_,
    SIGNAL_I_B_1_,
    SIGNAL_I_B_BASE_,
    SIGNAL_1_C_B_,
    SIGNAL_2_C_B_,
    SIGNAL_COUNT_
} SIGNAL_;

typedef struct SignalInterface_ {
    GblInterface    base;
    uintptr_t       id;
} SignalInterface_;

struct SignalInstance_;

typedef struct SignalInterfaceB_ {
    SignalInterface_    base;
    SignalInterface_    iB_1;
    uintptr_t           id;
    void                (*pFnS_IBBase_Slot_)(struct SignalInstance_* pSelf, void* pArg);
} SignalInterfaceB_;

typedef struct SignalClassA_ {
    GblClass            base;
    SignalInterface_    iA;
    uintptr_t           id;
} SignalClassA_;

struct GblSignalTestSuite_;

typedef struct SignalClassB_ {
    SignalClassA_               base;
    SignalInterfaceB_           iB;
    uintptr_t                   id;
} SignalClassB_;

typedef struct SignalInstance_ {
    GblInstance     base;
    uintptr_t       id;
    struct GblSignalTestSuite_* pSelf_;
} SignalInstance_;

typedef struct GblSignalTestSuite_ {
    GblType             types[TYPE_COUNT_];
    SignalInstance_*    pInstances[INSTANCE_COUNT_];
    size_t              signalEmissions[SIGNAL_COUNT_];
    uintptr_t           signalReceivers[SIGNAL_COUNT_][INSTANCE_COUNT_];
    uintptr_t           signalArgs[SIGNAL_COUNT_][INSTANCE_COUNT_];
    GblInstance*        pSignalCurrentReceivers[SIGNAL_COUNT_][INSTANCE_COUNT_];
    GblInstance*        pSignalCurrentEmitters[SIGNAL_COUNT_][INSTANCE_COUNT_];
    void*               pClosureUserdata;
    GblClosure*         pClosure;
} GblSignalTestSuite_;

void s_IA_Slot_(SignalInstance_* pReceiver, GblBool arg) {
    pReceiver->pSelf_->signalReceivers[SIGNAL_I_A_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_A_]]   = pReceiver->id;
    pReceiver->pSelf_->pSignalCurrentReceivers[SIGNAL_I_A_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_A_]] = GblSignal_receiver();
    pReceiver->pSelf_->pSignalCurrentEmitters[SIGNAL_I_A_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_A_]] =  GblSignal_emitter();
    pReceiver->pSelf_->signalArgs[SIGNAL_I_A_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_A_]++] = (uintptr_t)arg;
}

void s_CA_Slot_(SignalInstance_* pReceiver, uint32_t arg) {
    pReceiver->pSelf_->signalReceivers[SIGNAL_C_A_][pReceiver->pSelf_->signalEmissions[SIGNAL_C_A_]]   = pReceiver->id;
    pReceiver->pSelf_->pSignalCurrentReceivers[SIGNAL_C_A_][pReceiver->pSelf_->signalEmissions[SIGNAL_C_A_]] = GblSignal_receiver();
    pReceiver->pSelf_->pSignalCurrentEmitters[SIGNAL_C_A_][pReceiver->pSelf_->signalEmissions[SIGNAL_C_A_]] =  GblSignal_emitter();
    pReceiver->pSelf_->signalArgs     [SIGNAL_C_A_][pReceiver->pSelf_->signalEmissions[SIGNAL_C_A_]++] = (uintptr_t)arg;
}

void s_IB_1_Slot_(SignalInstance_* pReceiver, const char* pArg) {
    pReceiver->pSelf_->signalReceivers[SIGNAL_I_B_1_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_B_1_]]   = pReceiver->id;
    pReceiver->pSelf_->pSignalCurrentReceivers[SIGNAL_I_B_1_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_B_1_]] = GblSignal_receiver();
    pReceiver->pSelf_->pSignalCurrentEmitters[SIGNAL_I_B_1_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_B_1_]] =  GblSignal_emitter();
    pReceiver->pSelf_->signalArgs     [SIGNAL_I_B_1_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_B_1_]++] = (uintptr_t)GblStringView_strdup(GBL_STRV(pArg));
}

void s_IB_Base_Slot_(SignalInstance_* pReceiver, void* pArg) {
    pReceiver->pSelf_->signalReceivers[SIGNAL_I_B_BASE_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_B_BASE_]]   = pReceiver->id;
    pReceiver->pSelf_->pSignalCurrentReceivers[SIGNAL_I_B_BASE_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_B_BASE_]] = GblSignal_receiver();
    pReceiver->pSelf_->pSignalCurrentEmitters[SIGNAL_I_B_BASE_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_B_BASE_]] =  GblSignal_emitter();
    pReceiver->pSelf_->signalArgs     [SIGNAL_I_B_BASE_][pReceiver->pSelf_->signalEmissions[SIGNAL_I_B_BASE_]++] = (uintptr_t)pArg;
}

void s_1_CB_Slot_(SignalInstance_* pReceiver, float arg) {
    pReceiver->pSelf_->signalReceivers[SIGNAL_1_C_B_][pReceiver->pSelf_->signalEmissions[SIGNAL_1_C_B_]]   = pReceiver->id;
    pReceiver->pSelf_->pSignalCurrentReceivers[SIGNAL_1_C_B_][pReceiver->pSelf_->signalEmissions[SIGNAL_1_C_B_]] = GblSignal_receiver();
    pReceiver->pSelf_->pSignalCurrentEmitters[SIGNAL_1_C_B_][pReceiver->pSelf_->signalEmissions[SIGNAL_1_C_B_]] =  GblSignal_emitter();
    pReceiver->pSelf_->signalArgs     [SIGNAL_1_C_B_][pReceiver->pSelf_->signalEmissions[SIGNAL_1_C_B_]++] = (uintptr_t)arg;
}

void s_2_CB_Slot_(SignalInstance_* pReceiver, double arg) {
    pReceiver->pSelf_->signalReceivers[SIGNAL_2_C_B_][pReceiver->pSelf_->signalEmissions[SIGNAL_2_C_B_]]   = pReceiver->id;
    pReceiver->pSelf_->pSignalCurrentReceivers[SIGNAL_2_C_B_][pReceiver->pSelf_->signalEmissions[SIGNAL_2_C_B_]] = GblSignal_receiver();
    pReceiver->pSelf_->pSignalCurrentEmitters[SIGNAL_2_C_B_][pReceiver->pSelf_->signalEmissions[SIGNAL_2_C_B_]] =  GblSignal_emitter();
    pReceiver->pSelf_->signalArgs     [SIGNAL_2_C_B_][pReceiver->pSelf_->signalEmissions[SIGNAL_2_C_B_]++] = (uintptr_t)arg;
    pReceiver->pSelf_->pClosureUserdata = GblBox_userdata(GBL_BOX(GblClosure_current()));
}

static GBL_RESULT classInit_(GblClass* pClass, const void* pUd) {
    GBL_CTX_BEGIN(NULL);

    union {
        GblClass*           pClass;
        SignalInterface_*   pSignalIFace;
        SignalClassA_*      pSignalClassA;
        SignalInterfaceB_*  pSignalIFaceB;
        SignalClassB_*      pSignalClassB;
    } class;

    class.pClass = pClass;
    const TYPE_ id = (TYPE_)(uintptr_t)pUd;

    switch(id) {
    case TYPE_I_A_:
    case TYPE_I_B_1_:
    case TYPE_I_B_BASE_:    class.pSignalIFace->id  = id; break;
    case TYPE_C_A_:         class.pSignalClassA->id = id; break;
    case TYPE_I_B_:         class.pSignalIFaceB->id = id; class.pSignalIFaceB->pFnS_IBBase_Slot_ = s_IB_Base_Slot_; break;
    case TYPE_C_B_:         class.pSignalClassB->id = id; break;
    default: break;
    }

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(NULL);
    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblSignalTestSuite_));


    GblTypeInfo info = {
        .classSize      = sizeof(SignalInterface_),
        .pFnClassInit   = classInit_,
    };

    info.pClassData = (void*)TYPE_I_A_;
    pSelf_->types[TYPE_I_A_] = GblType_register("IA",
                                                      GBL_INTERFACE_TYPE,
                                                      &info,
                                                      GBL_TYPE_FLAGS_NONE);

    info.pClassData = (void*)TYPE_I_B_BASE_;
    pSelf_->types[TYPE_I_B_BASE_] = GblType_register("IBBase",
                                                           GBL_INTERFACE_TYPE,
                                                           &info,
                                                           GBL_TYPE_FLAGS_NONE);

    info.pClassData = (void*)TYPE_I_B_1_;
    pSelf_->types[TYPE_I_B_1_] = GblType_register("IB_1",
                                                        GBL_INTERFACE_TYPE,
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);

    GblInterfaceImpl entry = {
        .interfaceType = pSelf_->types[TYPE_I_A_],
        .classOffset = offsetof(SignalClassA_, iA)
    };

    info.pClassData = (void*)TYPE_C_A_;
    info.classSize = sizeof(SignalClassA_);
    info.instanceSize = sizeof(SignalInstance_);
    info.interfaceCount = 1;
    info.pInterfaceImpls = &entry;

    pSelf_->types[TYPE_C_A_] = GblType_register("CA",
                                                      GBL_INSTANCE_TYPE,
                                                      &info,
                                                      GBL_TYPE_FLAGS_NONE);

    info.pClassData = (void*)TYPE_I_B_;
    info.classSize = sizeof(SignalInterfaceB_);
    info.instanceSize = 0;
    entry.interfaceType = pSelf_->types[TYPE_I_B_1_];
    entry.classOffset = offsetof(SignalInterfaceB_, iB_1);

    pSelf_->types[TYPE_I_B_] = GblType_register("IB",
                                                      pSelf_->types[TYPE_I_B_BASE_],
                                                      &info,
                                                      GBL_TYPE_FLAGS_NONE);

    info.pClassData = (void*)TYPE_C_B_;
    info.classSize = sizeof(SignalClassB_);
    info.instanceSize = sizeof(SignalInstance_);
    entry.interfaceType = pSelf_->types[TYPE_I_B_];
    entry.classOffset = offsetof(SignalClassB_, iB);

    pSelf_->types[TYPE_C_B_] = GblType_register("CB",
                                                      pSelf_->types[TYPE_C_A_],
                                                      &info,
                                                      GBL_TYPE_FLAGS_NONE);

    for(int i = 0; i < INSTANCE_COUNT_; ++i) {
        pSelf_->pInstances[i]           = (SignalInstance_*)GblInstance_create(pSelf_->types[TYPE_C_B_]);
        pSelf_->pInstances[i]->id       = i;
        pSelf_->pInstances[i]->pSelf_   = pSelf_;
    }

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    for(int i = 0; i < INSTANCE_COUNT_; ++i) {
        GblInstance_destroy(GBL_INSTANCE(pSelf_->pInstances[i]));
    }

    for(int t = TYPE_COUNT_-1; t >= 0; --t) {
        GblType_unregister(pSelf_->types[t]);
    }

    GBL_CTX_END();
}


static GBL_RESULT GblSignalTestSuite_emitterNone_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblSignal_emitter(), NULL);
    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_receiverNone_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblSignal_receiver(), NULL);
    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_installInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblSignal_install(GBL_INVALID_TYPE, "lolol", NULL, 1, GBL_INVALID_TYPE),
                     GBL_RESULT_ERROR_INVALID_TYPE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_install(GBL_INSTANCE_TYPE, NULL, NULL, 0),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_install(GBL_INSTANCE_TYPE,
                                       "lolol",
                                       NULL,
                                       2,
                                       GBL_INT32_TYPE,
                                       GBL_INVALID_TYPE),
                     GBL_RESULT_ERROR_INVALID_TYPE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_install_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_install(pSelf_->types[TYPE_I_A_],
                                       "S_IA",
                                       GblMarshal_CClosure_VOID__INSTANCE_BOOL,
                                       1,
                                       GBL_BOOL_TYPE),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_install(pSelf_->types[TYPE_C_A_],
                                       "S_CA",
                                       GblMarshal_CClosure_VOID__INSTANCE_UINT32,
                                       1,
                                       GBL_UINT32_TYPE),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_install(pSelf_->types[TYPE_I_B_1_],
                                       "S_IB_1",
                                       GblMarshal_CClosure_VOID__INSTANCE_STRING,
                                       1,
                                       GBL_STRING_TYPE),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_install(pSelf_->types[TYPE_I_B_BASE_],
                                       "S_IBBase",
                                       GblMarshal_CClosure_VOID__INSTANCE_POINTER,
                                       1,
                                       GBL_POINTER_TYPE),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_install(pSelf_->types[TYPE_C_B_],
                                       "S_1_CB",
                                       GblMarshal_CClosure_VOID__INSTANCE_FLOAT,
                                       1,
                                       GBL_FLOAT_TYPE),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_install(pSelf_->types[TYPE_C_B_],
                                       "S_2_CB",
                                       GblMarshal_CClosure_VOID__INSTANCE_DOUBLE,
                                       1,
                                       GBL_DOUBLE_TYPE),
                     GBL_RESULT_SUCCESS);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_connectInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GblInstance instance;
    GblInstance_construct(&instance, GBL_INSTANCE_TYPE);

    GBL_TEST_COMPARE(GblSignal_connect(NULL,
                                       "S_IA",
                                       GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       GBL_CALLBACK(GblSignalTestSuite_connectInvalid_)),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connect(&instance,
                                       NULL,
                                       &instance,
                                       GBL_CALLBACK(GblSignalTestSuite_connectInvalid_)),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       "S_IA",
                                       NULL,
                                       GBL_CALLBACK(GblSignalTestSuite_connectInvalid_)),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       "S_IA",
                                       &instance,
                                       NULL),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       "lolol",
                                       GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       GBL_CALLBACK(GblSignalTestSuite_connectInvalid_)),
                     GBL_RESULT_ERROR_INVALID_HANDLE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GblInstance_destruct(&instance);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_connect_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_connect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       "S_IA",
                                       GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       GBL_CALLBACK(s_IA_Slot_)),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                               "S_IA"),
                     1);

    GBL_TEST_COMPARE(GblSignal_connect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       "S_CA",
                                       GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]),
                                       GBL_CALLBACK(s_CA_Slot_)),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                               "S_CA"),
                     1);


    GBL_TEST_COMPARE(GblSignal_connect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       "S_IB_1",
                                       GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       GBL_CALLBACK(s_IB_1_Slot_)),
                     GBL_RESULT_SUCCESS);


    GBL_TEST_COMPARE(GblSignal_connect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       "S_IB_1",
                                       GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]),
                                       GBL_CALLBACK(s_IB_1_Slot_)),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                               "S_IB_1"),
                     2);

    GBL_TEST_COMPARE(GblSignal_connect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                       "S_1_CB",
                                       GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]),
                                       GBL_CALLBACK(s_1_CB_Slot_)),
                     GBL_RESULT_SUCCESS);


    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                               "S_1_CB"),
                     1);

    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), NULL), 5);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_connectClassInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GblInstance instance;
    GblInstance_construct(&instance, GBL_INSTANCE_TYPE);

    GBL_TEST_COMPARE(GblSignal_connectClass(NULL,
                                            "lolol",
                                            &instance,
                                            GBL_INSTANCE_TYPE,
                                            sizeof(GblClass)),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connectClass(&instance,
                                            NULL,
                                            &instance,
                                            GBL_INSTANCE_TYPE,
                                            sizeof(GblClass)),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connectClass(&instance,
                                            "lolol",
                                            NULL,
                                            GBL_INSTANCE_TYPE,
                                            sizeof(GblClass)),
                     GBL_RESULT_ERROR_TYPE_MISMATCH);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connectClass(&instance,
                                            "lolol",
                                            &instance,
                                            GBL_INVALID_TYPE,
                                            sizeof(GblClass)),
                     GBL_RESULT_ERROR_TYPE_MISMATCH);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connectClass(&instance,
                                            "lolol",
                                            &instance,
                                            GBL_INSTANCE_TYPE,
                                            0),
                     GBL_RESULT_ERROR_INVALID_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();

    GblInstance_destruct(&instance);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_connectClass_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_connectClass(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                            "S_IBBase",
                                            GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]),
                                            pSelf_->types[TYPE_I_B_],
                                            offsetof(SignalInterfaceB_, pFnS_IBBase_Slot_)),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                               "S_IBBase"),
                     1);

    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), NULL), 6);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_connectSignalInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GblInstance instance;
    GblInstance_construct(&instance, GBL_INSTANCE_TYPE);

    GBL_TEST_COMPARE(GblSignal_connectSignal(NULL,
                                             "SI_CB",
                                             GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                             "SI_CB"),
                     GBL_RESULT_ERROR_INVALID_HANDLE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connectSignal(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                             NULL,
                                             GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                             "SI_CB"),
                     GBL_RESULT_ERROR_INVALID_HANDLE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connectSignal(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                             "SI_CB",
                                             NULL,
                                             "SI_CB"),
                     GBL_RESULT_ERROR_INVALID_HANDLE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connectSignal(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                             "SI_CB",
                                             GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                             NULL),
                    GBL_RESULT_ERROR_INVALID_HANDLE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_connectSignal(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                             "SI_CB",
                                             GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                             "lolol"),
                    GBL_RESULT_ERROR_INVALID_HANDLE);
    GBL_CTX_CLEAR_LAST_RECORD();


    GblInstance_destruct(&instance);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_connectSignal_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_connectSignal(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]),
                                             "S_1_CB",
                                             GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                             "S_1_CB"),
                     GBL_RESULT_SUCCESS);


    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]),
                                               "S_1_CB"),
                     1);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_connectClosureInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblSignal_connectClosure(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]),
                                              "S_1_CB",
                                              GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                              NULL),
                     GBL_RESULT_ERROR_INVALID_POINTER);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_connectClosure_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GblClosure* pClosure = GBL_CLOSURE(GblCClosure_create(GBL_CALLBACK(s_2_CB_Slot_),
                                                          (void*)0xdeadbeef));

    pSelf_->pClosure = pClosure;

    GBL_TEST_COMPARE(GblSignal_connectClosure(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                              "S_2_CB",
                                              GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]),
                                              pClosure),
                     GBL_RESULT_SUCCESS);


    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                                               "S_2_CB"),
                     1);

    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), NULL), 7);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_emitInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblSignal_emit(NULL, "S_1_CB", 1),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();


    GBL_TEST_COMPARE(GblSignal_emit(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), NULL, 1),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_emitSelf_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblSignal_emit(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "S_IA", GBL_TRUE));

    const size_t  emissionIndex = pSelf_->signalEmissions[SIGNAL_I_A_] - 1;
    GBL_TEST_COMPARE(emissionIndex, 0);
    GBL_TEST_COMPARE((GblBool)pSelf_->signalArgs[SIGNAL_I_A_][emissionIndex], GBL_TRUE);
    GBL_TEST_COMPARE(pSelf_->signalReceivers[SIGNAL_I_A_][emissionIndex], INSTANCE_1_);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentEmitters[SIGNAL_I_A_][emissionIndex], pSelf_->pInstances[INSTANCE_1_]);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentReceivers[SIGNAL_I_A_][emissionIndex], pSelf_->pInstances[INSTANCE_1_]);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_emitOther_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblSignal_emit(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "S_CA", 17));

    const size_t  emissionIndex = pSelf_->signalEmissions[SIGNAL_C_A_] - 1;
    GBL_TEST_COMPARE(emissionIndex, 0);
    GBL_TEST_COMPARE(pSelf_->signalArgs[SIGNAL_C_A_][emissionIndex], 17);
    GBL_TEST_COMPARE(pSelf_->signalReceivers[SIGNAL_C_A_][emissionIndex], INSTANCE_2_);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentEmitters[SIGNAL_C_A_][emissionIndex], pSelf_->pInstances[INSTANCE_1_]);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentReceivers[SIGNAL_C_A_][emissionIndex], pSelf_->pInstances[INSTANCE_2_]);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_emitMulti_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblSignal_emit(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "S_IB_1", "C Marshals"));

    const size_t  emissionCount = pSelf_->signalEmissions[SIGNAL_I_B_1_];
    GBL_TEST_COMPARE(emissionCount, 2);

    GBL_TEST_COMPARE((const char*)pSelf_->signalArgs[SIGNAL_I_B_1_][0], "C Marshals");
    GBL_CTX_FREE((void*)pSelf_->signalArgs[SIGNAL_I_B_1_][0]);
    GBL_TEST_COMPARE(pSelf_->signalReceivers[SIGNAL_I_B_1_][0], INSTANCE_1_);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentEmitters[SIGNAL_I_B_1_][0], pSelf_->pInstances[INSTANCE_1_]);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentReceivers[SIGNAL_I_B_1_][0], pSelf_->pInstances[INSTANCE_1_]);


    GBL_TEST_COMPARE((const char*)pSelf_->signalArgs[SIGNAL_I_B_1_][1], "C Marshals");
    GBL_CTX_FREE((void*)pSelf_->signalArgs[SIGNAL_I_B_1_][1]);
    GBL_TEST_COMPARE(pSelf_->signalReceivers[SIGNAL_I_B_1_][1], INSTANCE_2_);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentEmitters[SIGNAL_I_B_1_][1], pSelf_->pInstances[INSTANCE_1_]);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentReceivers[SIGNAL_I_B_1_][1], pSelf_->pInstances[INSTANCE_2_]);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_emitOtherClass_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblSignal_emit(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "S_IBBase", (void*)0xdeadbabe));

    const size_t  emissionIndex = pSelf_->signalEmissions[SIGNAL_I_B_BASE_] - 1;
    GBL_TEST_COMPARE(emissionIndex, 0);
    GBL_TEST_COMPARE(pSelf_->signalArgs[SIGNAL_I_B_BASE_][emissionIndex], 0xdeadbabe);
    GBL_TEST_COMPARE(pSelf_->signalReceivers[SIGNAL_I_B_BASE_][emissionIndex], INSTANCE_2_);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentEmitters[SIGNAL_I_B_BASE_][emissionIndex], pSelf_->pInstances[INSTANCE_1_]);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentReceivers[SIGNAL_I_B_BASE_][emissionIndex], pSelf_->pInstances[INSTANCE_2_]);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_emitVaListInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_emitVaList(NULL, "S_1_CB", NULL),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();


    GBL_TEST_COMPARE(GblSignal_emit(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), NULL, NULL),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT emitVaList_(GblInstance* pEmitter, const char* pSignalName, ...) {
    va_list varArgs;
    va_start(varArgs, pSignalName);

    GBL_RESULT result = GblSignal_emitVaList(pEmitter, pSignalName, &varArgs);
    va_end(varArgs);
    return result;
}

static GBL_RESULT GblSignalTestSuite_emitVaListOtherSignal_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);


    GBL_CTX_VERIFY_CALL(emitVaList_(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]), "S_1_CB", 17.0f));

    const size_t  emissionIndex = pSelf_->signalEmissions[SIGNAL_1_C_B_] - 1;
    GBL_TEST_COMPARE(emissionIndex, 0);
    GBL_TEST_COMPARE(pSelf_->signalArgs[SIGNAL_1_C_B_][emissionIndex], (uintptr_t)17.0f);
    GBL_TEST_COMPARE(pSelf_->signalReceivers[SIGNAL_1_C_B_][emissionIndex], INSTANCE_2_);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentEmitters[SIGNAL_1_C_B_][emissionIndex], pSelf_->pInstances[INSTANCE_1_]);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentReceivers[SIGNAL_1_C_B_][emissionIndex], pSelf_->pInstances[INSTANCE_2_]);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_emitVariantsInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_emitVariants(NULL, "S_1_CB", NULL),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();


    GBL_TEST_COMPARE(GblSignal_emitVariants(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), NULL, NULL),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_emitVariantsOtherClosure_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GblVariant v;
    GblVariant_constructDouble(&v, 33.3);

    GBL_CTX_VERIFY_CALL(GblSignal_emitVariants(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "S_2_CB", &v));

    const size_t  emissionIndex = pSelf_->signalEmissions[SIGNAL_2_C_B_] - 1;
    GBL_TEST_COMPARE(emissionIndex, 0);
    GBL_TEST_COMPARE(pSelf_->signalArgs[SIGNAL_2_C_B_][emissionIndex], (uintptr_t)33.3);
    GBL_TEST_COMPARE(pSelf_->signalReceivers[SIGNAL_2_C_B_][emissionIndex], INSTANCE_2_);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentEmitters[SIGNAL_2_C_B_][emissionIndex], pSelf_->pInstances[INSTANCE_1_]);
    GBL_TEST_COMPARE(pSelf_->pSignalCurrentReceivers[SIGNAL_2_C_B_][emissionIndex], pSelf_->pInstances[INSTANCE_2_]);
    GBL_TEST_COMPARE(pSelf_->pClosureUserdata, (void*)0xdeadbeef);

    GblVariant_destruct(&v);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_blockInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_block(NULL, "trolo", GBL_TRUE), GBL_FALSE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_block(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "lola", GBL_TRUE), GBL_FALSE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_block_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_block(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "S_IB_1", GBL_TRUE), GBL_FALSE);
    GBL_CTX_VERIFY_CALL(GblSignal_emit(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "S_IB_1", "Final Pheasantry"));

    GBL_TEST_COMPARE(pSelf_->signalEmissions[SIGNAL_I_B_1_], 2);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_blockBeforeConnecting_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_block(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_3_]), "S_IA", GBL_TRUE), GBL_FALSE);

    GblSignal_connect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_3_]),
                      "S_IA",
                      GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                      GBL_CALLBACK(s_IA_Slot_));

    GblSignal_emit(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_3_]),
                   "S_IA",
                   GBL_FALSE);

    GBL_TEST_COMPARE(pSelf_->signalEmissions[SIGNAL_I_A_], 1);


    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_blockAllInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblSignal_blockAll(NULL, GBL_TRUE), GBL_FALSE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_blockAll_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_blockAll(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), GBL_TRUE), GBL_FALSE);

    GblSignal_emit(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                   "S_IA",
                   GBL_FALSE);

    GBL_TEST_COMPARE(pSelf_->signalEmissions[SIGNAL_I_B_1_], 2);
    GBL_TEST_COMPARE(pSelf_->signalEmissions[SIGNAL_I_A_], 1);
    GBL_TEST_COMPARE(pSelf_->signalEmissions[SIGNAL_C_A_], 1);
    GBL_TEST_COMPARE(pSelf_->signalEmissions[SIGNAL_1_C_B_], 1);
    GBL_TEST_COMPARE(pSelf_->signalEmissions[SIGNAL_2_C_B_], 1);
    GBL_TEST_COMPARE(pSelf_->signalEmissions[SIGNAL_I_B_BASE_], 1);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_blockAllBeforeConnecting_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_blockAll(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_4_]), GBL_TRUE), GBL_FALSE);

    GblSignal_connect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_4_]),
                      "S_IA",
                      GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]),
                      GBL_CALLBACK(s_IA_Slot_));

    GblSignal_emit(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_4_]),
                   "S_IA",
                   GBL_FALSE);

    GBL_TEST_COMPARE(pSelf_->signalEmissions[SIGNAL_I_A_], 1);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_disconnectInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblSignal_disconnect(NULL, "trololo", NULL, NULL), 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_disconnect(NULL, "trololo", NULL, (void*)0xdeadbeef), 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_disconnect(NULL, NULL, NULL, (void*)0xdeadbeef), 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_disconnect(NULL, NULL, NULL, NULL), 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_disconnect_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_disconnect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_4_]), NULL, NULL, NULL), 1);
    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_4_]), "S_IA"), 0);

    GBL_TEST_COMPARE(GblSignal_disconnect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_3_]), "S_IA", NULL, NULL), 1);
    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_3_]), "S_IA"), 0);

    GBL_TEST_COMPARE(GblSignal_disconnect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]), NULL, GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), NULL), 1);
    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_2_]), "S_1_CB"), 0);

    GBL_TEST_COMPARE(GblSignal_disconnect(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), NULL, GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), pSelf_->pClosure), 1);
    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "S_2_CB"), 0);

    GBL_TEST_COMPARE(GblSignal_disconnect(NULL, "S_IB_1", GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), NULL), 1);
    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "S_IB_1"), 1);

    GBL_TEST_COMPARE(GblSignal_disconnect(NULL, NULL, GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), NULL), 1);
    GBL_TEST_COMPARE(GblSignal_connectionCount(GBL_INSTANCE(pSelf_->pInstances[INSTANCE_1_]), "S_IA"), 0);

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_uninstallInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblSignal_uninstall(GBL_INVALID_TYPE, "lolol"),
                     GBL_RESULT_ERROR_INVALID_HANDLE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblSignal_uninstall(GBL_INTERFACE_TYPE, "lolol"),
                    GBL_RESULT_ERROR_INVALID_HANDLE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblSignalTestSuite_uninstall_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblSignalTestSuite_* pSelf_ = GBL_SIGNAL_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblSignal_uninstall(pSelf_->types[TYPE_I_A_],
                                         "S_IA"),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_uninstall(pSelf_->types[TYPE_C_A_],
                                         "S_CA"),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_uninstall(pSelf_->types[TYPE_I_B_1_],
                                        "S_IB_1"),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_uninstall(pSelf_->types[TYPE_I_B_BASE_],
                                         "S_IBBase"),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_uninstall(pSelf_->types[TYPE_C_B_],
                                         "S_1_CB"),
                     GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GblSignal_uninstall(pSelf_->types[TYPE_C_B_],
                                         "S_2_CB"),
                     GBL_RESULT_SUCCESS);

    GBL_CTX_END();
}


GBL_EXPORT GblType GblSignalTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "emitterNone",                GblSignalTestSuite_emitterNone_             },
        { "receiverNone",               GblSignalTestSuite_receiverNone_            },
        { "installInvalid",             GblSignalTestSuite_installInvalid_          },
        { "install",                    GblSignalTestSuite_install_                 },
        { "connectInvalid",             GblSignalTestSuite_connectInvalid_          },
        { "connect",                    GblSignalTestSuite_connect_                 },
        { "connectClassInvalid",        GblSignalTestSuite_connectClassInvalid_     },
        { "connectClass",               GblSignalTestSuite_connectClass_            },
        { "connectSignalInvalid",       GblSignalTestSuite_connectSignalInvalid_    },
        { "connectSignal",              GblSignalTestSuite_connectSignal_           },
        { "connectClosureInvalid",      GblSignalTestSuite_connectClosureInvalid_   },
        { "connectClosure",             GblSignalTestSuite_connectClosure_          },
        { "emitInvalid",                GblSignalTestSuite_emitInvalid_             },
        { "emitSelf",                   GblSignalTestSuite_emitSelf_                },
        { "emitOther",                  GblSignalTestSuite_emitOther_               },
        { "emitMulti",                  GblSignalTestSuite_emitMulti_               },
        { "emitOtherClass",             GblSignalTestSuite_emitOtherClass_          },
        { "emitVaListInvalid",          GblSignalTestSuite_emitVaListInvalid_       },
        { "emitVaListOtherSignal",      GblSignalTestSuite_emitVaListOtherSignal_   },
        { "emitVariantsInvalid",        GblSignalTestSuite_emitVariantsInvalid_     },
        { "emitVariantsOtherClosure",   GblSignalTestSuite_emitVariantsOtherClosure_},
        { "blockInvalid",               GblSignalTestSuite_blockInvalid_            },
        { "block",                      GblSignalTestSuite_block_                   },
        { "blockBeforeConnecting",      GblSignalTestSuite_blockBeforeConnecting_   },
        { "blockAllInvalid",            GblSignalTestSuite_blockAllInvalid_         },
        { "blockAll",                   GblSignalTestSuite_blockAll_                },
        { "blockAllBeforeConnecting",   GblSignalTestSuite_blockAllBeforeConnecting_},
        { "disconnectInvalid",          GblSignalTestSuite_disconnectInvalid_       },
        { "disconnect",                 GblSignalTestSuite_disconnect_              },
        { "uninstallInvalid",           GblSignalTestSuite_uninstallInvalid_        },
        { "uninstall",                  GblSignalTestSuite_uninstall_               },
        { NULL,                         NULL                                        }
    };

    const static GblTestSuiteVTable vTable = {
        .pFnSuiteInit   = GblSignalTestSuite_init_,
        .pFnSuiteFinal  = GblSignalTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("SignalTestSuite"),
                                     &vTable,
                                     sizeof(GblSignalTestSuite),
                                     sizeof(GblSignalTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}
