#include "test_gimbal_type.hpp"
#include <gimbal/meta/gimbal_class.hpp>
#include <gimbal/meta/gimbal_instance.hpp>
#include <gimbal/meta/gimbal_type.hpp>
#include <gimbal/meta/gimbal_interface.hpp>

#define CINSTANCE_TYPE (CInstance_type())

namespace gimbal::test {

 void Type::init_(void) {
    QVERIFY(gimbal::Type::init(pCtx()));
    QCOMPARE(gimbal::Type::count(), GBL_TYPE_BUILTIN_COUNT);
    QVERIFY(!gimbal::Type::fromName("Lol").isValid());
}

 void Type::invalidType(void) {
    gimbal::InvalidType invalidType;
    QVERIFY(!invalidType.isValid());
    QVERIFY(invalidType.isA(gimbal::InvalidType()));
    QVERIFY(invalidType.getFundamentalType() == InvalidType());
    QVERIFY(invalidType.getParentType() == InvalidType());
    QVERIFY(!invalidType.getInfo());
    QCOMPARE(invalidType.getDepth(), 0);
    QCOMPARE(invalidType.getName(), "Invalid");
    QVERIFY(invalidType.isFundamental());
    QVERIFY(!invalidType.isClassed());
    QVERIFY(!invalidType.isInstantiable());
    QVERIFY(!invalidType.isDerivable());
    QVERIFY(!invalidType.isDeepDerivable());
    QVERIFY(!invalidType.isAbstract());
    QVERIFY(!invalidType.isInterface());
    QVERIFY(!invalidType.isFinal());
}

 void Type::builtinTypes(void) {
    gimbal::Type nilType = GBL_NIL_TYPE;
    QCOMPARE(gimbal::Type::fromName("nil"), nilType);
    QVERIFY(nilType.isValid());
    QCOMPARE(nilType.getDepth(), 0);
    QVERIFY(nilType.isFundamental());
    QVERIFY(!nilType.isInstantiable());
    QVERIFY(nilType.isClassed());
    QVERIFY(!nilType.isDerivable());

    gimbal::Type interfaceType = GBL_INTERFACE_TYPE;
    QCOMPARE(gimbal::Type::fromName("Interface"), interfaceType);
    QVERIFY(interfaceType.isValid());
    QVERIFY(interfaceType.isFundamental());
    QVERIFY(interfaceType.isDeepDerivable());
    QVERIFY(interfaceType.isDerivable());
    QVERIFY(interfaceType.isClassed());
}

 void Type::blankType(void) {
    ContextAllocScopeGuard scopeGuard(pCtx(), 1);
    auto type = gimbal::Type::registerStatic(gimbal::InvalidType(),
                                            "Blank",
                                             {},
                                             {});
    QCOMPARE(gimbal::Type::count(), GBL_TYPE_BUILTIN_COUNT+1);
    QVERIFY(type.isValid());
    QCOMPARE(type.getName(), "Blank");
    QCOMPARE(gimbal::Type::fromName("Blank"), type);
    QVERIFY(type.isFundamental());
    QVERIFY(type.isA(type));
    QVERIFY(!type.isA(gimbal::InvalidType()));
    QCOMPARE(type.getFundamentalType(), type);
    QCOMPARE(type.getInfo()->classSize,     0);
    QCOMPARE(type.getInfo()->instanceSize,  0);
    QCOMPARE(type.getParentType(), gimbal::InvalidType());
    QCOMPARE(type.getClassRefCount(), 0);
}

 void Type::reregisterType(void) {
#if 0
    gimbal::Type type = GBL_INVALID_TYPE;
    auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "Duplicate Type") {

        type = gimbal::Type::registerStatic(gimbal::InvalidType(),
                                                "Blank",
                                                 {},
                                                 {});
    };
    QVERIFY(test.getLastCallRecord().getResult().isError());
    QCOMPARE(type, InvalidType());
    QVERIFY(!type.isValid());
#else
    QSKIP("uncomment me to watch it shit its pants on assertion");
#endif
}

 void Type::unregisterType(void) {
    ContextAllocScopeGuard scopeGuard(pCtx(), -1);
    auto type = gimbal::Type::fromName("Blank");
    QVERIFY(type.isValid());
    QCOMPARE(gimbal::Type::count(), GBL_TYPE_BUILTIN_COUNT+1);
    gimbal::Type::unregister(type);
    QCOMPARE(gimbal::Type::count(), GBL_TYPE_BUILTIN_COUNT);
    QCOMPARE(gimbal::Type::fromName("Blank"), gimbal::InvalidType());
}


struct CInstance;

#define CINSTANCE_TYPE                      (CInstance_type())
#define CINSTANCE(instance)                 GBL_INSTANCE_CAST(instance, CINSTANCE_TYPE, CInstance)
#define CINSTANCE_COMPATIBLE(instance)      GBL_INSTANCE_CHECK(instance, CINSTANCE_TYPE)
#define CINSTANCE_CLASS(klass)              GBL_CLASS_CAST(klass, CINSTANCE_TYPE, CInstanceClass)
#define CINSTANCE_CLASS_COMPATIBLE(klass)   GBL_CLASS_CHECK(klass, CINSTANCE_TYPE)
#define CINSTANCE_GET_CLASS(instance)       GBL_INSTANCE_CAST_CLASS(instance, CINSTANCE_TYPE, CInstanceClass)


struct CInstanceClass {
    GblClass base;
    int* pHeapInteger;
    float floater;
    int (*vPrint)(const CInstance* pSelf, int value);
};

struct CInstance {
    union {
        CInstanceClass*  pClass;
        GblInstance     baseInstance;
    };
    int integer;
    int floating;
    char string[100];
};


static int CInstanceClass_vPrint(const CInstance* pSelf, int value) {
    printf("Hello, fucker from: %s", pSelf? pSelf->string : "NO INSTANCE");
    return value * (pSelf? pSelf->integer: 2);
}

static GBL_RESULT CInstanceClass_init(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    CInstanceClass* pSelf = reinterpret_cast<CInstanceClass*>(pClass);
    pSelf->pHeapInteger = (int*)malloc(sizeof(int));
    *pSelf->pHeapInteger = (uintptr_t)pData;
    pSelf->floater = 12.0f;
    pSelf->vPrint = CInstanceClass_vPrint;
    GBL_API_END();
}

static GBL_RESULT CInstanceClass_final(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_UNUSED(pData);
    CInstanceClass* pSelf = reinterpret_cast<CInstanceClass*>(pClass);
    free(pSelf->pHeapInteger);
    GBL_API_END();
}

static GBL_RESULT CInstance_init(GblInstance* pInstance, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    CInstance* pSelf = (CInstance*)pInstance;
    pSelf->floating = 10.0f;
    pSelf->integer = 2;
    strcpy(pSelf->string, "CInstance");
    GBL_API_END();
}

static GblType CInstance_type(void) {
    static GblType type = GBL_INVALID_TYPE;
    if(type == GBL_INVALID_TYPE) {
        GblTypeInfo info = {
            .pFnClassInit = CInstanceClass_init,
            .pFnClassFinal = CInstanceClass_final,
            .classSize = sizeof(CInstanceClass),
            .pClassData = (void*)(uintptr_t)13,
            .pFnInstanceInit = CInstance_init,
            .instanceSize = sizeof(CInstance),
        };

        type = GblType_registerStatic(GBL_INVALID_TYPE, "CInstance",
                        &info,
                        GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE      |
                        GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);

    }
    return type;
}

 void Type::cClassedType(void) {
    // Verify MetaClass
    const gimbal::Type cType = CINSTANCE_TYPE;
    QVERIFY(cType.isValid());
    QCOMPARE(cType, gimbal::Type::fromName("CInstance"));
    QVERIFY(cType.isClassed());
    QVERIFY(cType.isDerivable());
    QVERIFY(cType.isFundamental());
    QVERIFY(cType.isInstantiable());
    QCOMPARE(cType.getDepth(), 0);

    // Verify Class
    QCOMPARE(cType.getClassRefCount(), 0);
    CInstanceClass* pClass = CINSTANCE_CLASS(cType.classReference());
    QCOMPARE(cType.getClassRefCount(), 1);
    QCOMPARE(GBL_CLASS_TYPE(pClass), cType);
    QCOMPARE(*pClass->pHeapInteger, 13);
    QCOMPARE(pClass->floater, 12.0f);
    QCOMPARE(pClass->vPrint(NULL, 3), 6);
    // force class to be destroyed and recreated again with instance!
    cType.classUnreference(pClass);
    QCOMPARE(cType.getClassRefCount(), 0);

    // Verify Instance
    CInstance* pInstance = CINSTANCE(cType.instanceCreate());
    QCOMPARE(cType.getClassRefCount(), 1);
    QVERIFY(pInstance);
    QCOMPARE(pInstance->floating, 10.0f);
    QCOMPARE(pInstance->integer, 2);
    QCOMPARE(pInstance->string, "CInstance");
    cType.instanceDestroy(pInstance);
    QCOMPARE(cType.getClassRefCount(), 0);
}

#define CPPINSTANCE_TYPE                  (CppInstance_type())
#define CPPINSTANCE(instance)             GBL_INSTANCE_CAST(instance, CPPINSTANCE_TYPE, CppInstance)
#define CPPINSTANCE_CLASS(klass)          GBL_CLASS_CAST(klass, CPPINSTANCE_TYPE, CppInstanceClass)
#define CPPINSTANCE_GET_CLASS(instance)   GBL_INSTANCE_CAST_CLASS(instance, CPPINSTANCE_TYPE, CppInstanceClass)

struct CppInstanceClass: public CInstanceClass {
    QString qString;
};

struct CppInstance: public CInstance {
    QString qString;
};

 GblType (*CppInstance_type)(void) = []() -> GblType {
    static gimbal::Type type;
    if(type == gimbal::InvalidType()) {
        type = gimbal::Type::registerStatic(CINSTANCE_TYPE,
            "CppInstance",
            gimbal::TypeInfo {
                {
                    .pFnClassInit = [](GblClass* pClass, const void*, GblContext* pCtx) {
                        auto* pSelf = (CppInstanceClass*)(pClass);
                        GBL_API_FUNCTION(pCtx, ({
                            new (&pSelf->qString) QString("QSTRING CLASS");
                            pSelf->vPrint = [](const CInstance*, int value) -> int {
                                printf("virtual overload!!!");
                                return 4*value;
                            };
                        }));
                    },
                    .pFnClassFinal = [](GblClass* pClass, const void*, GblContext* pCtx) {
                        GBL_API_FUNCTION(pCtx, ({
                            reinterpret_cast<CppInstanceClass*>(pClass)->qString.~QString();
                        }));
                    },
                    .classSize = sizeof(CppInstanceClass),
                    .pClassData = (void*)(uintptr_t)44,
                    .pFnInstanceInit = [](GblInstance* pInstance, GblContext* pCtx) {
                        GBL_API_FUNCTION(pCtx, ({
                            new (&reinterpret_cast<CppInstance*>(pInstance)->qString) QString("QSTRING INSTANCE");
                        }));
                    },
                    .instanceSize = sizeof(CppInstance),
                }
            },
        {});
    }
    return type;
};

 void Type::derivedType(void) {
    auto cppInstanceType = gimbal::Type(CPPINSTANCE_TYPE);

    QVERIFY(cppInstanceType.isValid());
    QCOMPARE(cppInstanceType.getName(), "CppInstance");
    QVERIFY(!cppInstanceType.isFundamental());
    QCOMPARE(cppInstanceType.getFundamentalType(),  gimbal::Type(CINSTANCE_TYPE));
    QCOMPARE(cppInstanceType.getParentType(),       gimbal::Type(CINSTANCE_TYPE));
    QCOMPARE(cppInstanceType.getDepth(), 1);

    auto* pClass = CPPINSTANCE_CLASS(cppInstanceType.classReference());
    QVERIFY(pClass);
    QCOMPARE(GBL_CLASS_TYPE(pClass), cppInstanceType);
    QCOMPARE(pClass->qString, "QSTRING CLASS");
    QCOMPARE(pClass->floater, 12.0f);
    QCOMPARE(*pClass->pHeapInteger, 44);
    QCOMPARE(pClass->vPrint(NULL, 3), 12);
    cppInstanceType.classUnreference(pClass);

    auto* pInstance = CPPINSTANCE(cppInstanceType.instanceCreate());
    QCOMPARE(pInstance->qString, "QSTRING INSTANCE");
    QCOMPARE(pInstance->floating, 10.0f);
    QCOMPARE(pInstance->integer, 2);
    cppInstanceType.instanceDestroy(pInstance);

    QCOMPARE(gimbal::Type(CINSTANCE_TYPE).getClassRefCount(), 0);
}

 void Type::placementDerivedType(void) {
    auto type = gimbal::Type::fromName("CppInstance");
    QVERIFY(type.isValid());
    QCOMPARE(type.getClassRefCount(), 0);

    CppInstance cppInstance;
    type.instanceConstruct(&cppInstance);
    QCOMPARE(type.getClassRefCount(), 1);
    QCOMPARE(cppInstance.qString, "QSTRING INSTANCE");
    QCOMPARE(cppInstance.floating, 10.0f);
    QCOMPARE(cppInstance.integer, 2);

    auto* pClass = CPPINSTANCE_CLASS(cppInstance.pClass);
    QVERIFY(pClass);
    QCOMPARE(GBL_CLASS_TYPE(pClass), type);
    QCOMPARE(pClass->qString, "QSTRING CLASS");
    QCOMPARE(pClass->floater, 12.0f);
    QCOMPARE(*pClass->pHeapInteger, 44);
    QCOMPARE(pClass->vPrint(NULL, 3), 12);

    type.instanceDestruct(&cppInstance);
    QCOMPARE(type.getClassRefCount(), 0);
}


#define CINTERFACE_TYPE                  (CInterface_type())
#define CINTERFACE(instance)             GBL_INSTANCE_CAST(instance, CINTERFACE_TYPE, CInterface)
#define CINTERFACE_CLASS(klass)          GBL_CLASS_CAST(klass, CINTERFACE_TYPE, CInterface)
#define CINTERFACE_GET_CLASS(instance)   GBL_INSTANCE_CAST_CLASS(instance, CINTERFACE_TYPE, CInterface)

struct CInterface {
    GblInterface        baseClass;
    int                 staticInt;
    const char*         (*pVClassStringer)(void);
    const char*         (*pVInstanceStringer)(const CInstance*);
};


 GblType (*CInterface_type)(void) = []() -> GblType {
    static gimbal::Type type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        type = gimbal::Type::registerStatic(gimbal::InterfaceType(),
        "CInterface",
        gimbal::TypeInfo {
            {
                .pFnClassInit = [](GblClass* pClass, const void* pUd, GblContext* pCtx) {
                    auto* pSelf = reinterpret_cast<CInterface*>(pClass);
                    GBL_API_FUNCTION(pCtx, ({
                        pSelf->staticInt = (int)(uintptr_t)pUd;
                        pSelf->pVClassStringer = [](void) {
                            return "CInterface::classStringer";
                        };
                        pSelf->pVInstanceStringer = [](const CInstance* pInstance) {
                            return pInstance->string;
                        };
                    }));
                },
                .classSize = sizeof(CInterface),
                .pClassData = (void*)(uintptr_t)67,
            }
        },
        {});
    }
    return type;
};

 void Type::interfaceType(void) {
    gimbal::Type interfaceType = CINTERFACE_TYPE;
    QVERIFY(interfaceType.isValid());
    QCOMPARE(interfaceType.getParentType(), gimbal::InterfaceType());
    QCOMPARE(interfaceType.getFundamentalType(), gimbal::InterfaceType());

    auto* pIface = CINTERFACE_CLASS(interfaceType.classReference());
    QCOMPARE(pIface->staticInt, 67);
    QCOMPARE(pIface->pVClassStringer(), "CInterface::classStringer");
    interfaceType.classUnreference(pIface);
}

struct ICppInstanceClass {
    CppInstanceClass    baseClass;
    CInterface          cInterfaceClass;
    float               staticFloat;
};

struct ICppClassPrivate {
    char buffer[64];
};

struct ICppInstancePrivate {
    char buffer[200];
};

struct ICppInstance: public CppInstance {
    float           instanceFloat;
};

#define ICPPINSTANCE_TYPE                  (ICppInstance_type())
#define ICPPINSTANCE(instance)             GBL_INSTANCE_CAST(instance, ICPPINSTANCE_TYPE, ICppInstance)
#define ICPPINSTANCE_CLASS(klass)          GBL_CLASS_CAST(klass, ICPPINSTANCE_TYPE, ICppInstanceClass)
#define ICPPINSTANCE_GET_CLASS(instance)   GBL_INSTANCE_CAST_CLASS(instance, ICPPINSTANCE_TYPE, ICppInstanceClass)
#define ICPPINSTANCE_PRIVATE(instance)     (ICppInstancePrivate*)GBL_INSTANCE_PRIVATE(instance, ICPPINSTANCE_TYPE)
#define ICPPINSTANCE_CLASS_PRIVATE(instance)((ICppClassPrivate*)GBL_CLASS_PRIVATE(pClass, ICPPINSTANCE_TYPE))

 GblType (*ICppInstance_type)(void) = []() {
    static GblType type = GBL_INVALID_TYPE;
    if(type == GBL_INVALID_TYPE) {
        GblTypeInterfaceMapEntry entry = {
            gimbal::Type::fromName("CInterface"),
            offsetof(ICppInstanceClass, cInterfaceClass)
        };
        type = gimbal::Type::registerStatic(gimbal::Type::fromName("CppInstance"),
        "ICppInstance",
        gimbal::TypeInfo {
            {
                .pFnClassInit = [](GblClass* pClass, const void*, GblContext* pCtx) {
                    GBL_API_FUNCTION(pCtx, {
                        auto* pSelf = reinterpret_cast<ICppInstanceClass*>(pClass);
                        ICppClassPrivate* pPriv = (ICppClassPrivate*)GblClass_private(pClass, ICPPINSTANCE_TYPE);
                        strcpy(pPriv->buffer, "lookit my privates!");
                        pSelf->staticFloat = -3.33f;
                        pSelf->cInterfaceClass.pVClassStringer = [](void) {
                            return "ICppInstanceClass::ClassStringer";
                        };
                        pSelf->cInterfaceClass.pVInstanceStringer = [](const CInstance*) {
                            return "OVERRIDDEN";
                        };
                    });
                },
                .classSize = sizeof(ICppInstanceClass),
                .classPrivateSize = sizeof(ICppClassPrivate),
                .interfaceCount = 1,
                .pInterfaceMap = &entry,
                .pFnInstanceInit = [](GblInstance* pInstance, GblContext* pCtx) {
                    GBL_API_FUNCTION(pCtx, {
                        reinterpret_cast<ICppInstance*>(pInstance)->instanceFloat = 3.33f;
                        ICppInstancePrivate* pPriv = ICPPINSTANCE_PRIVATE(pInstance);
                        strcpy(pPriv->buffer, "FUCKWEASEL!");
                    });
                },
                .instanceSize = sizeof(ICppInstance),
                .instancePrivateSize = sizeof(ICppInstancePrivate)
            }
        },
        {});
    }
    return type;
};

 void Type::implementInterfaceType(void) {
    // retrieve metatype from type system (will force registry)
    gimbal::Type type = ICPPINSTANCE_TYPE;
    QCOMPARE(type.getDepth(), 2);

    // create a new instance from a metatype
    auto* pInstance = ICPPINSTANCE(type.instanceCreate());
    // validate derived instance variables
    QCOMPARE(pInstance->instanceFloat, 3.33f);
    // validate inherited instance variables
    QCOMPARE(pInstance->floating, 10.0f);
    QCOMPARE(pInstance->integer, 2);

    auto* pPrivate = ICPPINSTANCE_PRIVATE(pInstance);
    QCOMPARE(pPrivate->buffer, "FUCKWEASEL!");

    // grab pointer to class from instance
    auto* pClass = ICPPINSTANCE_GET_CLASS(pInstance);
    // validate static class variable
    QCOMPARE(pClass->staticFloat, -3.33f);
    // validate overriden inherited virtual function
    QCOMPARE(pClass->baseClass.vPrint(CINSTANCE(pInstance), 3), 12);

    auto* pClassPrivate = ICPPINSTANCE_CLASS_PRIVATE(pClass);
    QCOMPARE(pClassPrivate->buffer, "lookit my privates!");

    // grab pointer to interface from instance
    auto* pInterface = CINTERFACE_GET_CLASS(pInstance);
    //verify static interface class variable
    QCOMPARE(pInterface->staticInt, 67);
    // verify class has overridden interface's virtual method
    QCOMPARE(pInterface->pVClassStringer(), "ICppInstanceClass::ClassStringer");

    // test casting operatprs between interface and class types
    auto* pClass2 = ICPPINSTANCE_CLASS(pClass);
    QCOMPARE(pClass2, pClass);

    auto* pInterface2 = CINTERFACE_CLASS(pInterface);
    QCOMPARE(pInterface2, pInterface);

    auto* pClass3 = ICPPINSTANCE_CLASS(pInterface);
    QCOMPARE(pClass3, pClass);

    auto* pInterface3 = CINTERFACE_CLASS(pInterface2);
    QCOMPARE(pInterface3, pInterface2);

    type.instanceDestroy(pInstance);
}

}
