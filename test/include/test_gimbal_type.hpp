#ifndef TEST_GIMBAL_TYPE_H
#define TEST_GIMBAL_TYPE_H

#include <gimbal/meta/gimbal_class.hpp>
#include <gimbal/meta/gimbal_instance.hpp>
#include <gimbal/meta/gimbal_type.hpp>
#include <gimbal/meta/gimbal_interface.hpp>
#include "test_gimbal.hpp"

namespace gimbal::test {

#define CINSTANCE_TYPE (CInstance_type())

class Type: public UnitTestSet {
    Q_OBJECT
private:

private slots:
    void reinit(void);
    void invalidType(void);
    void builtinTypes(void);
    void blankType(void);
    void reregisterType(void);
    void unregisterType(void);
    void cClassedType(void);
    void derivedType(void);
    void placementDerivedType(void);
    void interfaceType(void);
    void implementInterfaceType(void);

};

inline void Type::reinit(void) {
    QVERIFY(gimbal::Type::reinit(pCtx()).isSuccess());
    QCOMPARE(gimbal::Type::count(), GBL_TYPE_BUILTIN_COUNT);
    QVERIFY(!gimbal::Type::fromName("Lol").isValid());
}

inline void Type::invalidType(void) {
    gimbal::InvalidType invalidType;
    QVERIFY(!invalidType.isValid());
    QVERIFY(invalidType.isA(gimbal::InvalidType()));
    QVERIFY(invalidType.getFundamentalType() == InvalidType());
    QVERIFY(invalidType.getParentType() == InvalidType());
    QVERIFY(!invalidType.getInfo());
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

inline void Type::builtinTypes(void) {
    gimbal::Type nilType = GBL_TYPE_NIL;
    QCOMPARE(gimbal::Type::fromName("nil"), nilType);
    QVERIFY(nilType.isValid());
    QVERIFY(nilType.isFundamental());
    QVERIFY(!nilType.isInstantiable());
    QVERIFY(!nilType.isClassed());

    gimbal::Type interfaceType = GBL_TYPE_INTERFACE;
    QCOMPARE(gimbal::Type::fromName("Interface"), interfaceType);
    QVERIFY(interfaceType.isValid());
    QVERIFY(interfaceType.isFundamental());
    QVERIFY(interfaceType.isDerivable());
    QVERIFY(interfaceType.isClassed());
}

inline void Type::blankType(void) {
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

inline void Type::reregisterType(void) {
#if 0
    gimbal::Type type = GBL_TYPE_INVALID;
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

inline void Type::unregisterType(void) {
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
#define CINSTANCE(instance)                 GBL_TYPE_INSTANCE_TO(instance, CINSTANCE_TYPE, CInstance)
#define CINSTANCE_COMPATIBLE(instance)      GBL_TYPE_INSTANCE_IS_A(instance, CINSTANCE_TYPE)
#define CINSTANCE_CLASS(klass)              GBL_TYPE_CLASS_TO(klass, CINSTANCE_TYPE, CInstanceClass)
#define CINSTANCE_CLASS_COMPATIBLE(klass)   GBL_TYPE_CLASS_IS_A(klass, CINSTANCE_TYPE)
#define CINSTANCE_GET_CLASS(instance)       GBL_TYPE_INSTANCE_CLASS_TO(instance, CINSTANCE_TYPE, CInstanceClass)


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

static GBL_RESULT CInstanceClass_init(GblClass* pClass, const void* pData) {
    GBL_API_BEGIN(NULL);
    CInstanceClass* pSelf = reinterpret_cast<CInstanceClass*>(pClass);
    pSelf->pHeapInteger = (int*)malloc(sizeof(int));
    *pSelf->pHeapInteger = (uintptr_t)pData;
    pSelf->floater = 12.0f;
    pSelf->vPrint = CInstanceClass_vPrint;
    GBL_API_END();
}

static GBL_RESULT CInstanceClass_final(GblClass* pClass, const void* pData) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(pData);
    CInstanceClass* pSelf = reinterpret_cast<CInstanceClass*>(pClass);
    free(pSelf->pHeapInteger);
    GBL_API_END();
}

static GBL_RESULT CInstance_init(GblInstance* pInstance) {
    GBL_API_BEGIN(NULL);
    CInstance* pSelf = (CInstance*)pInstance;
    pSelf->floating = 10.0f;
    pSelf->integer = 2;
    strcpy(pSelf->string, "CInstance");
    GBL_API_END();
}

static GblType CInstance_type(void) {
    static GblType type = GBL_TYPE_INVALID;
    if(type == GBL_TYPE_INVALID) {
        GblTypeInfo info = {
            .pFnClassInit = CInstanceClass_init,
            .pFnClassFinalize = CInstanceClass_final,
            .classSize = sizeof(CInstanceClass),
            .classAlign = alignof(CInstanceClass),
            .pClassData = (void*)(uintptr_t)13,
            .pFnInstanceInit = CInstance_init,
            .instanceSize = sizeof(CInstance),
            .instanceAlign = alignof(CInstance)
        };

        type = gblTypeRegisterStatic(GBL_TYPE_INVALID, "CInstance",
                        &info,
                        GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED           |
                            GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE  |
                            GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE);

    }
    return type;
}

inline void Type::cClassedType(void) {
    // Verify MetaClass
    const gimbal::Type cType = CINSTANCE_TYPE;
    QVERIFY(cType.isValid());
    QCOMPARE(cType, gimbal::Type::fromName("CInstance"));
    QVERIFY(cType.isClassed());
    QVERIFY(cType.isDerivable());
    QVERIFY(cType.isFundamental());
    QVERIFY(cType.isInstantiable());

    // Verify Class
    QCOMPARE(cType.getClassRefCount(), 0);
    CInstanceClass* pClass = CINSTANCE_CLASS(cType.classReference());
    QCOMPARE(cType.getClassRefCount(), 1);
    QCOMPARE(pClass->base.typeId, cType);
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
#define CPPINSTANCE(instance)             GBL_TYPE_INSTANCE_TO(instance, CPPINSTANCE_TYPE, CppInstance)
#define CPPINSTANCE_CLASS(klass)          GBL_TYPE_CLASS_TO(klass, CPPINSTANCE_TYPE, CppInstanceClass)
#define CPPINSTANCE_GET_CLASS(instance)   GBL_TYPE_INSTANCE_CLASS_TO(instance, CPPINSTANCE_TYPE, CppInstanceClass)

struct CppInstanceClass: public CInstanceClass {
    QString qString;
};

struct CppInstance: public CInstance {
    QString qString;
};

inline GblType (*CppInstance_type)(void) = []() -> GblType {
    static gimbal::Type type;
    if(type == gimbal::InvalidType()) {
        type = gimbal::Type::registerStatic(CINSTANCE_TYPE,
            "CppInstance",
            gimbal::TypeInfo {
                {
                    .pFnClassInit = [](GblClass* pClass, const void*) {
                        auto* pSelf = (CppInstanceClass*)(pClass);
                        GBL_API_FUNCTION(NULL, ({
                            new (&pSelf->qString) QString("QSTRING CLASS");
                            pSelf->vPrint = [](const CInstance*, int value) -> int {
                                printf("virtual overload!!!");
                                return 4*value;
                            };
                        }));
                    },
                    .pFnClassFinalize = [](GblClass* pClass, const void*) {
                        GBL_API_FUNCTION(NULL, ({
                            reinterpret_cast<CppInstanceClass*>(pClass)->qString.~QString();
                        }));
                    },
                    .classSize = sizeof(CppInstanceClass),
                    .classAlign = alignof(CppInstanceClass),
                    .pClassData = (void*)(uintptr_t)44,
                    .pFnInstanceInit = [](GblInstance* pInstance) {
                        GBL_API_FUNCTION(NULL, ({
                            new (&reinterpret_cast<CppInstance*>(pInstance)->qString) QString("QSTRING INSTANCE");
                        }));
                    },
                    .instanceSize = sizeof(CppInstance),
                    .instanceAlign = alignof(CppInstance)
                }
            },
        {});
    }
    return type;
};

inline void Type::derivedType(void) {
    auto cppInstanceType = gimbal::Type(CPPINSTANCE_TYPE);

    QVERIFY(cppInstanceType.isValid());
    QCOMPARE(cppInstanceType.getName(), "CppInstance");
    QVERIFY(!cppInstanceType.isFundamental());
    QCOMPARE(cppInstanceType.getFundamentalType(),  gimbal::Type(CINSTANCE_TYPE));
    QCOMPARE(cppInstanceType.getParentType(),       gimbal::Type(CINSTANCE_TYPE));

    auto* pClass = CPPINSTANCE_CLASS(cppInstanceType.classReference());
    QVERIFY(pClass);
    QCOMPARE(pClass->base.typeId, cppInstanceType);
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

inline void Type::placementDerivedType(void) {
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
    QCOMPARE(pClass->base.typeId, type);
    QCOMPARE(pClass->qString, "QSTRING CLASS");
    QCOMPARE(pClass->floater, 12.0f);
    QCOMPARE(*pClass->pHeapInteger, 44);
    QCOMPARE(pClass->vPrint(NULL, 3), 12);

    type.instanceDestruct(&cppInstance);
    QCOMPARE(type.getClassRefCount(), 0);
}


#define CINTERFACE_TYPE                  (CInterface_type())
#define CINTERFACE(instance)             GBL_TYPE_INSTANCE_TO(instance, CINTERFACE_TYPE, CInterface)
#define CINTERFACE_CLASS(klass)          GBL_TYPE_CLASS_TO(klass, CINTERFACE_TYPE, CInterface)
#define CINTERFACE_GET_CLASS(instance)   GBL_TYPE_INSTANCE_CLASS_TO(instance, CINTERFACE_TYPE, CInterface)

struct CInterface {
    GblInterface        baseClass;
    int                 staticInt;
    const char*         (*pVClassStringer)(void);
    const char*         (*pVInstanceStringer)(const CInstance*);
};


inline GblType (*CInterface_type)(void) = []() -> GblType {
    static gimbal::Type type = GBL_TYPE_INVALID;

    if(type == GBL_TYPE_INVALID) {
        type = gimbal::Type::registerStatic(gimbal::InterfaceType(),
        "CInterface",
        gimbal::TypeInfo {
            {
                .pFnClassInit = [](GblClass* pClass, const void* pUd) {
                    auto* pSelf = reinterpret_cast<CInterface*>(pClass);
                    GBL_API_FUNCTION(NULL, ({
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
                .classAlign = alignof(CInterface),
                .pClassData = (void*)(uintptr_t)67,
            }
        },
        {});
    }
    return type;
};

inline void Type::interfaceType(void) {
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

struct ICppInstance: public CppInstance {
    float           instanceFloat;
};

#define ICPPINSTANCE_TYPE                  (ICppInstance_type())
#define ICPPINSTANCE(instance)             GBL_TYPE_INSTANCE_TO(instance, ICPPINSTANCE_TYPE, ICppInstance)
#define ICPPINSTANCE_CLASS(klass)          GBL_TYPE_CLASS_TO(klass, ICPPINSTANCE_TYPE, ICppInstanceClass)
#define ICPPINSTANCE_GET_CLASS(instance)   GBL_TYPE_INSTANCE_CLASS_TO(instance, ICPPINSTANCE_TYPE, ICppInstanceClass)

inline GblType (*ICppInstance_type)(void) = []() {
    static GblType type = GBL_TYPE_INVALID;
    if(type == GBL_TYPE_INVALID) {
        GblTypeInterfaceMapEntry entry = {
            gimbal::Type::fromName("CInterface"),
            offsetof(ICppInstanceClass, cInterfaceClass)
        };
        type = gimbal::Type::registerStatic(gimbal::Type::fromName("CppInstance"),
        "ICppInstance",
        gimbal::TypeInfo {
            {
                .pFnClassInit = [](GblClass* pClass, const void*) {
                    GBL_API_FUNCTION(NULL, {
                        auto* pSelf = reinterpret_cast<ICppInstanceClass*>(pClass);
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
                .classAlign = alignof(ICppInstanceClass),
                .interfaceCount = 1,
                .pInterfaceMap = &entry,
                .pFnInstanceInit = [](GblInstance* pInstance) {
                    GBL_API_FUNCTION(NULL, {
                        reinterpret_cast<ICppInstance*>(pInstance)->instanceFloat = 3.33f;
                    });
                },
                .instanceSize = sizeof(ICppInstance),
                .instanceAlign = alignof(ICppInstance),
            }
        },
        {});
    }
    return type;
};

inline void Type::implementInterfaceType(void) {
    // retrieve metatype from type system (will force registry)
    gimbal::Type type = ICPPINSTANCE_TYPE;

    // create a new instance from a metatype
    auto* pInstance = ICPPINSTANCE(type.instanceCreate());
    // validate derived instance variables
    QCOMPARE(pInstance->instanceFloat, 3.33f);
    // validate inherited instance variables
    QCOMPARE(pInstance->floating, 10.0f);
    QCOMPARE(pInstance->integer, 2);

    // grab pointer to class from instance
    auto* pClass = ICPPINSTANCE_GET_CLASS(pInstance);
    // validate static class variable
    QCOMPARE(pClass->staticFloat, -3.33f);
    // validate overriden inherited virtual function
    QCOMPARE(pClass->baseClass.vPrint(CINSTANCE(pInstance), 3), 12);

    // grab pointer to interface from instance
    auto* pInterface = CINTERFACE_GET_CLASS(pInstance);
    //verify static interface class variable
    QCOMPARE(pInterface->staticInt, 67);
    // verify class has overridden interface's virtual method
    QCOMPARE(pInterface->pVClassStringer(), "ICppInstanceClass::ClassStringer");

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

#endif // TEST_GIMBAL_TYPE_H
