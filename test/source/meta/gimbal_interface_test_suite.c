#include "meta/gimbal_interface_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/meta/gimbal_interface.h>

#define GBL_INTERFACE_TEST_SUITE_(inst)     (GBL_INSTANCE_PRIVATE(inst, GBL_INTERFACE_TEST_SUITE_TYPE))

typedef enum TYPE_ {
    TYPE_IA_        = 0,
    TYPE_CA_        = 1,
    TYPE_A_COUNT_   = TYPE_CA_+1,
    TYPE_IB1_       = 2,
    TYPE_IB2_       = 3,
    TYPE_CB_        = 4,
    TYPE_B_COUNT_   = TYPE_CB_+1,
    TYPE_IC_BASE_   = 5,
    TYPE_IC_        = 6,
    TYPE_CC_        = 7,
    TYPE_C_COUNT_   = TYPE_CC_+1,
    TYPE_ID_MAPPED_ = 8,
    TYPE_ID_        = 9,
    TYPE_CD_        = 10,
    TYPE_D_COUNT_   = TYPE_CD_+1,
    TYPE_IE_BASE_   = 11,
    TYPE_IE_MAPPED_ = 12,
    TYPE_IE_        = 13,
    TYPE_CE_        = 14,
    TYPE_E_COUNT_   = TYPE_CE_+1,
    TYPE_COUNT_     = TYPE_E_COUNT_
} TYPE_;

typedef struct IDerived_ {
    GblInterface     base;
    TYPE_            type;
} IDerived_;

typedef struct IDeepDerived_ {
    IDerived_       base;
    TYPE_           type;
} IDeepDerived_;

typedef struct IMapped_ {
    GblInterface    base;
    IDerived_       iDerived;
    TYPE_           type;
} IMapped_;

typedef struct IDeepDerivedMapped_ {
    IDerived_       base;
    IDerived_       iMapped;
    TYPE_           type;
} IDeepDerivedMapped_;

typedef struct ClassA_ {
    GblClass    base;
    IDerived_   iA;
    TYPE_       type;
} ClassA_;

typedef struct ClassB_ {
    ClassA_     base;
    IDerived_   iB[2];
    TYPE_       type;
} ClassB_;

typedef struct ClassC_ {
    ClassB_         base;
    IDeepDerived_   iC;
    TYPE_           type;
} ClassC_;

typedef struct ClassD_ {
    ClassC_     base;
    IMapped_    iD;
    TYPE_       type;
} ClassD_;

typedef struct ClassE {
    ClassD_             base;
    IDeepDerivedMapped_ iE;
    TYPE_               type;
} ClassE_;

typedef struct ClassEntry_ {
    GblType     type;
    GblEnum     enumVal;
    GblSize     enumOffset;
} ClassEntry_;

typedef struct GblInterfaceTestSuite_ {
    GblSize             initialInterfaceRefCount;
    GblSize             initialStaticClassRefCount;
    GblSize             initialTypeCount;
    GblType             typeList[TYPE_COUNT_];
    GblType             typeRefCounts[TYPE_COUNT_];
    GblType             ifaceRefCount;
    const ClassEntry_*  pClassEntries;
} GblInterfaceTestSuite_;

static GBL_RESULT typeClassInit_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    struct {
        union {
            void*                   pPtr;
            IDerived_*              pIDerived;
            IDeepDerived_*          pIDeepDerived;
            IMapped_*               pIMapped;
            IDeepDerivedMapped_*    pIDeepDerivedMapped;
            ClassA_*                pClassA;
            ClassB_*                pClassB;
            ClassC_*                pClassC;
            ClassD_*                pClassD;
            ClassE_*                pClassE;
        };
    } klass = {
        .pPtr = pClass
    };

    const uintptr_t type = (uintptr_t)pUd;

    switch(type) {
    case TYPE_IA_:
    case TYPE_IB1_:
    case TYPE_IB2_:
    case TYPE_IC_BASE_:
    case TYPE_ID_MAPPED_:
    case TYPE_IE_BASE_:
    case TYPE_IE_MAPPED_:   klass.pIDerived             ->type = type; break;
    case TYPE_IC_:          klass.pIDeepDerived         ->type = type; break;
    case TYPE_ID_:          klass.pIMapped              ->type = type; break;
    case TYPE_IE_:          klass.pIDeepDerivedMapped   ->type = type; break;
    case TYPE_CA_:          klass.pClassA               ->type = type; break;
    case TYPE_CB_:          klass.pClassB               ->type = type; break;
    case TYPE_CC_:          klass.pClassC               ->type = type; break;
    case TYPE_CD_:          klass.pClassD               ->type = type; break;
    case TYPE_CE_:          klass.pClassE               ->type = type; break;
    }

    GBL_API_END();
}


static GBL_RESULT GblInterfaceTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblInterfaceTestSuite_));
    pSelf_->initialInterfaceRefCount = GblType_classRefCount(GBL_INTERFACE_TYPE);
    pSelf_->initialStaticClassRefCount = GblType_classRefCount(GBL_STATIC_CLASS_TYPE);
    pSelf_->initialTypeCount = GblType_registeredCount();

    GblTypeInfo info = {
        .classSize      = sizeof(IDerived_),
        .pFnClassInit   = typeClassInit_
    };

    info.pClassData = (const void*)(uintptr_t)TYPE_IA_;
    pSelf_->typeList[TYPE_IA_] = GblType_registerStatic("IA",
                                                        GBL_INTERFACE_TYPE,
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);
    info.pClassData = (const void*)(uintptr_t)TYPE_IB1_;
    pSelf_->typeList[TYPE_IB1_] = GblType_registerStatic("IB1",
                                                        GBL_INTERFACE_TYPE,
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);
    info.pClassData = (const void*)(uintptr_t)TYPE_IB2_;
    pSelf_->typeList[TYPE_IB2_] = GblType_registerStatic("IB2",
                                                        GBL_INTERFACE_TYPE,
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);
    info.pClassData = (const void*)(uintptr_t)TYPE_IC_BASE_;
    pSelf_->typeList[TYPE_IC_BASE_] = GblType_registerStatic("IC_BASE",
                                                        GBL_INTERFACE_TYPE,
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);
    info.pClassData = (const void*)(uintptr_t)TYPE_ID_MAPPED_;
    pSelf_->typeList[TYPE_ID_MAPPED_] = GblType_registerStatic("ID_MAPPED",
                                                        GBL_INTERFACE_TYPE,
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);
    info.pClassData = (const void*)(uintptr_t)TYPE_IE_BASE_;
    pSelf_->typeList[TYPE_IE_BASE_] = GblType_registerStatic("IE_BASE",
                                                        GBL_INTERFACE_TYPE,
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);
    info.pClassData = (const void*)(uintptr_t)TYPE_IE_MAPPED_;
    pSelf_->typeList[TYPE_IE_MAPPED_] = GblType_registerStatic("IE_MAPPED",
                                                        GBL_INTERFACE_TYPE,
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);
    info.classSize = sizeof(IDeepDerived_);
    info.pClassData = (const void*)(uintptr_t)TYPE_IC_;
    pSelf_->typeList[TYPE_IC_] = GblType_registerStatic("IC",
                                                        pSelf_->typeList[TYPE_IC_BASE_],
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);

    GblTypeInterfaceMapEntry intMapEntry[2] = {
        {
            .classOffset = offsetof(IMapped_, iDerived),
            .interfaceType = pSelf_->typeList[TYPE_ID_MAPPED_]
        }, {
            0
        }
    };

    info.classSize = sizeof(IMapped_);
    info.interfaceCount = 1;
    info.pInterfaceMap = intMapEntry;
    info.pClassData = (const void*)(uintptr_t)TYPE_ID_;
    pSelf_->typeList[TYPE_ID_] = GblType_registerStatic("ID",
                                                        GBL_INTERFACE_TYPE,
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);

    info.classSize = sizeof(IDeepDerivedMapped_);
    intMapEntry[0].classOffset = offsetof(IDeepDerivedMapped_, iMapped);
    intMapEntry[0].interfaceType = pSelf_->typeList[TYPE_IE_MAPPED_];
    info.pClassData = (const void*)(uintptr_t)TYPE_IE_;
    pSelf_->typeList[TYPE_IE_] = GblType_registerStatic("IE",
                                                        pSelf_->typeList[TYPE_IE_BASE_],
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);
    info.classSize = sizeof(ClassA_);
    intMapEntry[0].classOffset = offsetof(ClassA_, iA);
    intMapEntry[0].interfaceType = pSelf_->typeList[TYPE_IA_];
    info.pClassData = (const void*)(uintptr_t)TYPE_CA_;
    pSelf_->typeList[TYPE_CA_] = GblType_registerStatic("CA",
                                                        GBL_STATIC_CLASS_TYPE,
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);

    info.classSize = sizeof(ClassB_);
    info.interfaceCount = 2;
    intMapEntry[0].classOffset = offsetof(ClassB_, iB[0]);
    intMapEntry[0].interfaceType = pSelf_->typeList[TYPE_IB1_];
    intMapEntry[1].classOffset = offsetof(ClassB_, iB[1]);
    intMapEntry[1].interfaceType = pSelf_->typeList[TYPE_IB2_];
    info.pClassData = (const void*)(uintptr_t)TYPE_CB_;
    pSelf_->typeList[TYPE_CB_] = GblType_registerStatic("CB",
                                                        pSelf_->typeList[TYPE_CA_],
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);
    info.classSize = sizeof(ClassC_);
    info.interfaceCount = 1;
    intMapEntry[0].classOffset = offsetof(ClassC_, iC);
    intMapEntry[0].interfaceType = pSelf_->typeList[TYPE_IC_];
    info.pClassData = (const void*)(uintptr_t)TYPE_CC_;
    pSelf_->typeList[TYPE_CC_] = GblType_registerStatic("CC",
                                                        pSelf_->typeList[TYPE_CB_],
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);
    info.classSize = sizeof(ClassD_);
    info.interfaceCount = 1;
    intMapEntry[0].classOffset = offsetof(ClassD_, iD);
    intMapEntry[0].interfaceType = pSelf_->typeList[TYPE_ID_];
    info.pClassData = (const void*)(uintptr_t)TYPE_CD_;
    pSelf_->typeList[TYPE_CD_] = GblType_registerStatic("CD",
                                                        pSelf_->typeList[TYPE_CC_],
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);

    info.classSize = sizeof(ClassE_);
    info.interfaceCount = 1;
    intMapEntry[0].classOffset = offsetof(ClassE_, iE);
    intMapEntry[0].interfaceType = pSelf_->typeList[TYPE_IE_];
    info.pClassData = (const void*)(uintptr_t)TYPE_CE_;
    pSelf_->typeList[TYPE_CE_] = GblType_registerStatic("CE",
                                                        pSelf_->typeList[TYPE_CD_],
                                                        &info,
                                                        GBL_TYPE_FLAGS_NONE);

    ClassEntry_ classEntries[TYPE_COUNT_] = {
        {
            .type       = pSelf_->typeList[TYPE_CA_],
            .enumVal    = TYPE_CA_,
            .enumOffset = offsetof(ClassA_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_IA_],
            .enumVal    = TYPE_IA_,
            .enumOffset = offsetof(IDerived_, type)
        },
        {
            .type        = pSelf_->typeList[TYPE_CB_],
            .enumVal     = TYPE_CB_,
            .enumOffset  = offsetof(ClassB_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_IB1_],
            .enumVal    = TYPE_IB1_,
            .enumOffset = offsetof(IDerived_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_IB2_],
            .enumVal    = TYPE_IB2_,
            .enumOffset = offsetof(IDerived_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_CC_],
            .enumVal    = TYPE_CC_,
            .enumOffset = offsetof(ClassC_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_IC_BASE_],
            .enumVal    = TYPE_IC_BASE_,
            .enumOffset = offsetof(IDerived_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_IC_],
            .enumVal    = TYPE_IC_,
            .enumOffset = offsetof(IDeepDerived_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_CD_],
            .enumVal    = TYPE_CD_,
            .enumOffset = offsetof(ClassD_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_ID_MAPPED_],
            .enumVal    = TYPE_ID_MAPPED_,
            .enumOffset = offsetof(IDerived_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_ID_],
            .enumVal    = TYPE_ID_,
            .enumOffset = offsetof(IMapped_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_CE_],
            .enumVal    = TYPE_CE_,
            .enumOffset = offsetof(ClassE_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_IE_BASE_],
            .enumVal    = TYPE_IE_BASE_,
            .enumOffset = offsetof(IDerived_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_IE_MAPPED_],
            .enumVal    = TYPE_IE_MAPPED_,
            .enumOffset = offsetof(IDerived_, type)
        },
        {
            .type       = pSelf_->typeList[TYPE_IE_],
            .enumVal    = TYPE_IE_,
            .enumOffset = offsetof(IDeepDerivedMapped_, type)
        },
    };
    static ClassEntry_ staticClassEntries[TYPE_COUNT_];

    memcpy(staticClassEntries, classEntries, sizeof(ClassEntry_) * TYPE_COUNT_);

    pSelf_->pClassEntries = staticClassEntries;

    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblType_classRefCount(GBL_INTERFACE_TYPE), pSelf_->initialInterfaceRefCount);
    GBL_TEST_COMPARE(GblType_classRefCount(GBL_STATIC_CLASS_TYPE), pSelf_->initialStaticClassRefCount);


    for(int t = 0; t < TYPE_COUNT_; ++t) {
        GBL_API_VERIFY_CALL(GblType_unregister(pSelf_->typeList[t]));
    }

    GBL_TEST_COMPARE(GblType_registeredCount(), pSelf_->initialTypeCount);
    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_classRefDefault_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);
    GblClass* pClass = GblClass_refDefault(GBL_INTERFACE_TYPE);
    GBL_TEST_COMPARE(GBL_CLASS_TYPEOF(pClass), GBL_INTERFACE_TYPE);
    GBL_TEST_VERIFY(GblClass_isInterface(pClass));
    GBL_TEST_VERIFY(GblClass_isDefault(pClass));
    GBL_TEST_VERIFY(!GblClass_isInterfaceImpl(pClass));
    GBL_TEST_VERIFY(GblClass_check(pClass, GBL_INTERFACE_TYPE));
    GBL_TEST_COMPARE(GblClass_cast(pClass, GBL_INTERFACE_TYPE), pClass);
    GBL_TEST_COMPARE(GblClass_try(pClass, GBL_INTERFACE_TYPE), pClass);

    GBL_TEST_COMPARE(GblType_classRefCount(GBL_INTERFACE_TYPE),
                     pSelf_->initialInterfaceRefCount + 1);
    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_castDefault_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);
    GBL_TEST_COMPARE(GBL_INTERFACE(GblClass_weakRefDefault(GBL_INTERFACE_TYPE)),
                     GblClass_weakRefDefault(GBL_INTERFACE_TYPE));
    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_tryDefault_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);

    GBL_TEST_COMPARE(GBL_INTERFACE_TRY(GblClass_weakRefDefault(GBL_INTERFACE_TYPE)),
                     GblClass_weakRefDefault(GBL_INTERFACE_TYPE));

    GBL_TEST_COMPARE(GBL_INTERFACE(GblClass_weakRefDefault(GBL_INTERFACE_TYPE)),
                     GblClass_weakRefDefault(GBL_INTERFACE_TYPE));
    GBL_API_END();
}


static GBL_RESULT GblInterfaceTestSuite_outerClassNull_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblInterface_outerClass(NULL), NULL);

    GBL_TEST_COMPARE(GBL_INTERFACE_OUTER_CLASS(NULL), NULL);

    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_TYPE_MISMATCH);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_outerClassDefault_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblInterface_outerClass(GBL_INTERFACE(GblClass_weakRefDefault(GBL_INTERFACE_TYPE))),
                     GblClass_weakRefDefault(GBL_INTERFACE_TYPE));

    GBL_TEST_COMPARE(GBL_INTERFACE_OUTER_CLASS(GblClass_weakRefDefault(GBL_INTERFACE_TYPE)),
                     GblClass_weakRefDefault(GBL_INTERFACE_TYPE));

    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_outerMostClassNull_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblInterface_outerMostClass(NULL), NULL);

    GBL_TEST_COMPARE(GBL_INTERFACE_OUTER_MOST_CLASS(NULL), NULL);

    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_TYPE_MISMATCH);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}


static GBL_RESULT GblInterfaceTestSuite_outerMostClassDefault_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblInterface_outerMostClass(GBL_INTERFACE(GblClass_weakRefDefault(GBL_INTERFACE_TYPE))),
                     GblClass_weakRefDefault(GBL_INTERFACE_TYPE));

    GBL_TEST_COMPARE(GBL_INTERFACE_OUTER_MOST_CLASS(GblClass_weakRefDefault(GBL_INTERFACE_TYPE)),
                     GblClass_weakRefDefault(GBL_INTERFACE_TYPE));

    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_classUnrefDefault_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblClass_unrefDefault(GblClass_weakRefDefault(GBL_INTERFACE_TYPE)),
                     pSelf_->initialInterfaceRefCount);
    GBL_API_END();
}

static GBL_RESULT checkRefCountsBegin_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);

    pSelf_->ifaceRefCount = GblType_classRefCount(GBL_INTERFACE_TYPE);

    for(int t = 0; t < TYPE_COUNT_; ++t) {
        pSelf_->typeRefCounts[t] = GblType_classRefCount(pSelf_->typeList[t]);
    }

    GBL_API_END();
}

static GBL_RESULT checkRefCountsEnd_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblType_classRefCount(GBL_INTERFACE_TYPE), pSelf_->ifaceRefCount);

    for(int t = 0; t < TYPE_COUNT_; ++t) {
        GBL_TEST_COMPARE(GblType_classRefCount(pSelf_->typeList[t]), pSelf_->typeRefCounts[t]);
    }

    GBL_API_END();
}

typedef GBL_RESULT (*ClassVerifierFn)(GblClass*, GblTestSuite*, GblContext*);


static GBL_RESULT verifyCasts_(GblClass* pClass, GblSize count, const ClassEntry_* pEntries, GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);
    const ClassEntry_* pSrc = pEntries;
    GblSize srcCount = 0;

    while(srcCount++ < count) {
        GblClass* pSrcClass = GblClass_cast(pClass, pSrc->type);
        GBL_TEST_VERIFY(pSrcClass);
        GBL_TEST_COMPARE(*(GblEnum*)((uint8_t*)pSrcClass + pSrc->enumOffset), pSrc->enumVal);

        const ClassEntry_* pDst = pEntries;
        GblSize dstCount = 0;

        while(dstCount++ < count) {
            GblClass* pDstClass = GblClass_cast(pSrcClass, pDst->type);
            GBL_TEST_VERIFY(pDstClass);
            GBL_TEST_COMPARE(*(GblEnum*)((uint8_t*)pDstClass + pDst->enumOffset), pDst->enumVal);
            ++pDst;
        }
        ++pSrc;
    }

    GBL_API_END();
}

static GBL_RESULT verifyClass_(GblEnum type, GblSize count, const ClassEntry_* pEntries,  GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_API_VERIFY_CALL(checkRefCountsBegin_(pSelf, pCtx));

    GblClass* pClass = GblClass_createFloating(type);

    GBL_API_VERIFY_CALL(verifyCasts_(pClass, count, pEntries, pSelf, pCtx));

    GBL_API_VERIFY_CALL(GblClass_destroyFloating(pClass));

    GBL_API_VERIFY_CALL(checkRefCountsEnd_(pSelf, pCtx));

    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_classMappingInterface_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(verifyClass_(pSelf_->typeList[TYPE_CA_],
                                     TYPE_A_COUNT_,
                                     pSelf_->pClassEntries,
                                     pSelf,
                                     pCtx));
    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_derivedClassMappingInterfaces_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(verifyClass_(pSelf_->typeList[TYPE_CB_],
                                     TYPE_B_COUNT_,
                                     pSelf_->pClassEntries,
                                     pSelf,
                                     pCtx));

    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_deepDerivedClassMappingDeepDerivedInterface_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(verifyClass_(pSelf_->typeList[TYPE_CC_],
                                     TYPE_C_COUNT_,
                                     pSelf_->pClassEntries,
                                     pSelf,
                                     pCtx));
    GBL_API_END();
}


static GBL_RESULT GblInterfaceTestSuite_deepDerivedClassMappingInterfaceMappingInterface_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(verifyClass_(pSelf_->typeList[TYPE_CD_],
                                     TYPE_D_COUNT_,
                                     pSelf_->pClassEntries,
                                     pSelf,
                                     pCtx));
    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_deepDerivedClassMappingDeepDerivedInterfaceMappingInterface_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(verifyClass_(pSelf_->typeList[TYPE_CE_],
                                     TYPE_E_COUNT_,
                                     pSelf_->pClassEntries,
                                     pSelf,
                                     pCtx));
    GBL_API_END();
}

GBL_EXPORT GblType GblInterfaceTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "classRefDefault",    GblInterfaceTestSuite_classRefDefault_  },
        { "castDefault",        GblInterfaceTestSuite_castDefault_ },
        { "tryDefault",         GblInterfaceTestSuite_tryDefault_  },
        { "outerClassNull",  GblInterfaceTestSuite_outerClassNull_},
        { "outerClassDefault",  GblInterfaceTestSuite_outerClassDefault_},
        { "outerMostClassNull", GblInterfaceTestSuite_outerMostClassNull_ },
        { "outerMostClassDefault", GblInterfaceTestSuite_outerMostClassDefault_ },
        { "classUnrefDefault",  GblInterfaceTestSuite_classUnrefDefault_ },
        { "classMappingInterface", GblInterfaceTestSuite_classMappingInterface_},
        { "derivedClassMappingInterfaces", GblInterfaceTestSuite_derivedClassMappingInterfaces_ },
        { "deepDerivedClassMappingDeepDerivedInterface", GblInterfaceTestSuite_deepDerivedClassMappingDeepDerivedInterface_ },
        { "deepDerivedClassMappingInterfaceMappingInterface", GblInterfaceTestSuite_deepDerivedClassMappingInterfaceMappingInterface_ },
        { "deepDerivedClassMappingDeepDerivedInterfaceMappingInterface", GblInterfaceTestSuite_deepDerivedClassMappingDeepDerivedInterfaceMappingInterface_ },
        { NULL,     NULL    }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblInterfaceTestSuite_init_,
        .pFnSuiteFinal  = GblInterfaceTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("InterfaceTestSuite"),
                                     &vTable,
                                     sizeof(GblInterfaceTestSuite),
                                     sizeof(GblInterfaceTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}
