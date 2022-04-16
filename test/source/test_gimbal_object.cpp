#include <test_gimbal_object.hpp>
#include <gimbal/meta/gimbal_object.hpp>
#include "gimbal/meta/gimbal_object.h"
#include <gimbal/types/gimbal_variant.h>

#define TEST_OBJECT_TYPE                    (TestObject_type())
#define TEST_OBJECT(instance)               GBL_TYPE_INSTANCE_CAST(instance, TEST_OBJECT_TYPE, TestObject)
#define TEST_OBJECT_COMPATIBLE(instance)    GBL_TYPE_INSTANCE_IS_A(instance, TEST_OBJECT_TYPE)
#define TEST_OBJECT_CLASS(klass)            GBL_TYPE_CLASS_CAST(klass, TEST_OBJECT_TYPE, TestObjectClass)
#define TEST_OBJECT_CLASS_COMPATIBLE(klass) GBL_TYPE_CLASS_IS_A(klass, TEST_OBJECT_TYPE)
#define TEST_OBJECT_GET_CLASS(instance)     GBL_TYPE_INSTANCE_CLASS_CAST(instance, TEST_OBJECT_TYPE, TestObjectClass)

namespace gimbal::test {

static GblType TestObject_type(void);

struct TestObjectClass {
    GblObjectClass  base;
    int integral;
    char string[256];
};

struct TestObject {
    GblObject   base;
    float       floater;
    char        stringer[256];
};

static GBL_RESULT TestObject_constructor(GblObject* pSelf) {
    GBL_API_BEGIN(NULL);
    GblObjectClass* pParentClass = GBL_OBJECT_CLASS(GblInstance_parentOf(GBL_INSTANCE(pSelf)));
    pParentClass->pFnConstructor(pSelf);
    TestObject* pTest = TEST_OBJECT(pSelf);
    pTest->floater = -NAN;
    strcpy(pTest->stringer, "INVALID");
    GBL_API_END();
}

static GBL_RESULT TestObject_constructed(GblObject* pSelf) {
    GBL_API_BEGIN(NULL);
    GblObjectClass* pParentClass = GBL_OBJECT_CLASS(GblInstance_parentOf(GBL_INSTANCE(pSelf)));
    if(pParentClass->pFnConstructed) pParentClass->pFnConstructed(pSelf);
    GBL_API_END();
}

static GBL_RESULT TestObject_destructor(GblObject* pSelf) {
    GBL_API_BEGIN(NULL);
    GblObjectClass* pParentClass = GBL_OBJECT_CLASS(GblInstance_parentOf(GBL_INSTANCE(pSelf)));
    pParentClass->pFnDestructor(pSelf);
    GBL_API_END();
}

enum TEST_OBJECT_PROPERTIES {
    TEST_OBJECT_PROPERTY_FLOATER,
    TEST_OBJECT_PROPERTY_STRINGER,
    TEST_OBJECT_PROPERTY_COUNT
};

static GBL_RESULT TestObject_propertyGet(const GblObject* pSelf, GblUint slot, GblVariant* pValue, const GblProperty* pProp) {
    GBL_API_BEGIN(NULL);
    switch(slot) {
    case TEST_OBJECT_PROPERTY_FLOATER:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), TEST_OBJECT(pSelf)->floater);
        break;
    case TEST_OBJECT_PROPERTY_STRINGER:
        GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), TEST_OBJECT(pSelf)->stringer);
        break;
    default: GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Attempt to get unregistered property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_API_END();
}

static GBL_RESULT TestObject_propertySet(GblObject* pSelf, GblUint slot, const GblVariant* pValue, const GblProperty* pProp) {
    GBL_API_BEGIN(NULL);
    switch(slot) {
    case TEST_OBJECT_PROPERTY_FLOATER: {
        float value = NAN;
        GBL_API_CALL(GblVariant_getValueCopy(pValue, &value));
        TEST_OBJECT(pSelf)->floater = value;
        break;
    }
    case TEST_OBJECT_PROPERTY_STRINGER: {
        const char* pStr = NULL;
        GBL_API_CALL(GblVariant_getValueCopy(pValue, &pStr));
        if(pStr) strcpy(TEST_OBJECT(pSelf)->stringer, pStr);
        break;
    }
    default: GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Attempt to set unregistered property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_API_END();
}


static GblType TestObject_type(void) {
    static GblType type = GBL_TYPE_INVALID;
    if(type == GBL_TYPE_INVALID) {
        const GblTypeInfo info = {
            .pFnClassInit = [](GblClass* pClass,
                               const void* pData,
                               GblContext hCtx) {
                auto pTestClass = TEST_OBJECT_CLASS(pClass);
                GBL_API_BEGIN(hCtx);
                if(GBL_CLASS_TYPE(pTestClass) == TEST_OBJECT_TYPE) {
                    gblPropertyTableInsert(GBL_CLASS_TYPE(pTestClass),
                                           gblQuarkFromStringStatic("floater"),
                                           TEST_OBJECT_PROPERTY_FLOATER,
                                           GBL_TYPE_FLOAT,
                                           GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE | GBL_PROPERTY_FLAG_CONSTRUCT);
                    gblPropertyTableInsert(GBL_CLASS_TYPE(pTestClass),
                                           gblQuarkFromStringStatic("stringer"),
                                           TEST_OBJECT_PROPERTY_STRINGER,
                                           GBL_TYPE_STRING,
                                           GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE | GBL_PROPERTY_FLAG_CONSTRUCT);
                }
                pTestClass->integral = 77;
                strcpy(pTestClass->string, (const char*)pData);
                pTestClass->base.pFnConstructor = TestObject_constructor;
                pTestClass->base.pFnDestructor  = TestObject_destructor;
                pTestClass->base.pFnConstructed = TestObject_constructed;
                pTestClass->base.pFnPropertyGet = TestObject_propertyGet;
                pTestClass->base.pFnPropertySet = TestObject_propertySet;
                GBL_API_END();
            },
            .classSize      = sizeof(TestObjectClass),
            .classAlign     = alignof(TestObjectClass),
            .pClassData     = (void*)"Davey Havoc",
            .instanceSize   = sizeof(TestObject),
            .instanceAlign  = alignof(TestObject)
        };

        type = gblTypeRegisterStatic(GBL_TYPE_OBJECT,
                                     "TestObject",
                                    &info,
                                    GBL_TYPE_FLAGS_NONE);
    }
    return type;
}

void CObject::newDefault(void) {
    TestObject* pObj = TEST_OBJECT(GblObject_new(TEST_OBJECT_TYPE, NULL));
    pTestObj_ = pObj;

    QCOMPARE(gblTypeClassRefCount(TEST_OBJECT_TYPE), 1);

    // validate class
    TestObjectClass* pClass = TEST_OBJECT_GET_CLASS(pObj);
    QCOMPARE(gblTypeFromClass(GBL_CLASS(pClass)), TEST_OBJECT_TYPE);
    QCOMPARE(&pClass->base, GBL_OBJECT_CLASS(pClass));
    QCOMPARE(QString(pClass->string), "Davey Havoc");
    QCOMPARE(pClass->integral, 77);

    // validate interfaces
    GblIVariantIFace* pIVariantIFace = GBL_IVARIANT_GET_IFACE(pObj);
    QCOMPARE(gblTypeFromClass(GBL_CLASS(pIVariantIFace)), GBL_TYPE_IVARIANT);
    QCOMPARE(pClass, TEST_OBJECT_CLASS(pIVariantIFace));
    QCOMPARE(QString(pIVariantIFace->pGetValueFmt), QString("p"));

    GblITableIFace* pITableIFace = GBL_ITABLE_GET_IFACE(pObj);
    QCOMPARE(gblTypeFromClass(GBL_CLASS(pITableIFace)), GBL_TYPE_ITABLE);
    QCOMPARE(pClass, TEST_OBJECT_CLASS(pITableIFace));

    GblIEventHandlerIFace* pIEventHandlerIFace = GBL_IEVENT_HANDLER_GET_IFACE(pObj);
    QCOMPARE(gblTypeFromClass(GBL_CLASS(pIEventHandlerIFace)), GBL_TYPE_IEVENT_HANDLER);
    QCOMPARE(pClass, TEST_OBJECT_CLASS(pIEventHandlerIFace));

    GblIEventFilterIFace* pIEventFilterIFace = GBL_IEVENT_FILTER_GET_IFACE(pObj);
    QCOMPARE(gblTypeFromClass(GBL_CLASS(pIEventFilterIFace)), GBL_TYPE_IEVENT_FILTER);
    QCOMPARE(pClass, TEST_OBJECT_CLASS(pIEventFilterIFace));

    // validate sanity of random casts
    QCOMPARE(pITableIFace, GBL_ITABLE_IFACE(pIEventHandlerIFace));
    QCOMPARE(pIVariantIFace, GBL_IVARIANT_IFACE(pIEventFilterIFace));

    //validate some of the compatible checks
    QVERIFY(GBL_IVARIANT_IFACE_COMPATIBLE(pClass));
    QVERIFY(!TEST_OBJECT_CLASS_COMPATIBLE(pIVariantIFace));
    QVERIFY(GBL_OBJECT_CLASS_COMPATIBLE(pClass));
    QVERIFY(GBL_ITABLE_IFACE_COMPATIBLE(GBL_OBJECT_CLASS(pITableIFace)));

    //validate insanity
    QCOMPARE(pClass, TEST_OBJECT_CLASS(GBL_ITABLE_IFACE(GBL_OBJECT_CLASS(GBL_OBJECT_CLASS(GBL_IEVENT_FILTER_IFACE(TEST_OBJECT_GET_CLASS(pObj)))))));

    // validate instance checks and casts
    QVERIFY(GBL_IVARIANT_COMPATIBLE(pObj));
    QVERIFY(GBL_IEVENT_FILTER_COMPATIBLE(GBL_OBJECT(GBL_IEVENT_FILTER(pObj))));
    QCOMPARE(pObj, TEST_OBJECT(GBL_IVARIANT(GBL_OBJECT(GBL_IEVENT_FILTER(TEST_OBJECT(pObj))))));
}

void CObject::ref(void) {
    QCOMPARE(GblObject_refCount(GBL_OBJECT(pTestObj_)), 1);
    GblObject* pObj = GBL_OBJECT(GblObject_ref(GBL_OBJECT(pTestObj_)));
    QCOMPARE(GBL_OBJECT(pTestObj_), pObj);
    QCOMPARE(GblObject_refCount(pObj), 2);
}

void CObject::unref(void) {
    QCOMPARE(GblObject_unref(GBL_OBJECT(pTestObj_)), 1);
    QCOMPARE(GblObject_unref(GBL_OBJECT(pTestObj_)), 0);
    QCOMPARE(gblTypeClassRefCount(TEST_OBJECT_TYPE), 0);

    // K now replace what we deleted... lulz.
    pTestObj_ = nullptr;
}

void CObject::extendedData(void) {
    GblObject* pObj = GblObject_new(TEST_OBJECT_TYPE, NULL);

    QCOMPARE(GblObject_nameGet(pObj),           nullptr);
    QCOMPARE(GblObject_userdataGet(pObj),       nullptr);
    QCOMPARE(GblObject_parentGet(pObj),         nullptr);
    QCOMPARE(GblObject_childFirst(pObj),        nullptr);
    QCOMPARE(GblObject_siblingNext(pObj),       nullptr);
    QCOMPARE(GblObject_eventFilterCount(pObj),  0);

    GblObject_nameSet(pObj, "Sing the Sorrow");
    QCOMPARE(GblObject_nameGet(pObj), "Sing the Sorrow");

    GblObject_userdataSet(pObj, (void*)0xbadbeef);
    QCOMPARE(GblObject_userdataGet(pObj), (void*)0xbadbeef);

    // make sure we didn't screw up other extended data
    QCOMPARE(GblObject_parentGet(pObj),         nullptr);
    QCOMPARE(GblObject_childFirst(pObj),        nullptr);
    QCOMPARE(GblObject_siblingNext(pObj),       nullptr);
    QCOMPARE(GblObject_eventFilterCount(pObj),  0);

    QCOMPARE(GblObject_unref(pObj), 0);

}

void CObject::newVaArgs(void) {
    GblObject* pObj = GblObject_new(TEST_OBJECT_TYPE,
                                    "name",     "Fuckwad",
                                    "userdata", (void*)0xdeadbeef,
                                    NULL);

    QCOMPARE(GblObject_nameGet(pObj),           "Fuckwad");
    QCOMPARE(GblObject_userdataGet(pObj),       (void*)0xdeadbeef);
    QCOMPARE(GblObject_siblingNext(pObj),       nullptr);
    QCOMPARE(GblObject_eventFilterCount(pObj),  0);

    QCOMPARE(GblObject_unref(pObj), 0);
}

void CObject::newVariants(void) {
    GblVariant variants[2] = {
        GBL_VARIANT_INIT,
        GBL_VARIANT_INIT
    };
    const char* keys[2] = {
        "name",
        "userdata"
    };

    GblVariant_constructValueCopy(&variants[0], GBL_TYPE_STRING, "Fuckwad");
    GblVariant_constructValueCopy(&variants[1], GBL_TYPE_POINTER, (void*)0xdeadbeef);

    GblObject* pObj = GblObject_newVariants(TEST_OBJECT_TYPE, 2, keys, variants);

    QCOMPARE(GblObject_nameGet(pObj),           "Fuckwad");
    QCOMPARE(GblObject_userdataGet(pObj),       (void*)0xdeadbeef);
    QCOMPARE(GblObject_siblingNext(pObj),       nullptr);
    QCOMPARE(GblObject_eventFilterCount(pObj),  0);

    QCOMPARE(GblObject_unref(pObj), 0);
}

void CObject::propertyGet(void) {
    GblObject* pObj0 = GblObject_new(GBL_TYPE_OBJECT, NULL);
    QVERIFY(pObj0);

    GblObject* pObj1 = GblObject_new(TEST_OBJECT_TYPE,
                                    "name",     "Bulbasaur",
                                    "userdata", (void*)0xdeadbeef,
                                    "parent",   pObj0,
                                    "floater",  -77.7,
                                    "stringer", "Fuckin Inheritance!",
                                    nullptr);
    QVERIFY(pObj1);

    uint32_t    refCount    = 0;
    void*       pUserdata   = NULL;
    const char* pName       = NULL;
    GblObject*  pParent     = NULL;
    float       floater     = 0.0f;
    const char* pStringer   = NULL;

    GBL_RESULT result = GblObject_get(pObj1, "userdata", &pUserdata,
                                             "refCount", &refCount,
                                             "name",     &pName,
                                             "parent",   &pParent,
                                             "floater",  &floater,
                                             "stringer", &pStringer,
                                             nullptr);

    QCOMPARE(result,    GBL_RESULT_SUCCESS);
    QCOMPARE(pUserdata, (void*)0xdeadbeef);
    QCOMPARE(refCount,  1);
    QCOMPARE(pName,     "Bulbasaur");
    QCOMPARE(pParent,   pObj0);
    QCOMPARE(floater,   -77.7f);
    //QCOMPARE(pStringer, "Fuckin Inheritance!");

    QCOMPARE(GblObject_unref(pObj1), 0);
    QCOMPARE(GblObject_unref(pObj0), 0);
}

void CObject::propertySet(void) {

}

void CObject::parenting(void) {
    GblObject* pChild1 = GblObject_new(GBL_TYPE_OBJECT,
                                    "name",     "Child1",
                                    NULL);
    GblObject* pChild2 = GblObject_new(GBL_TYPE_OBJECT,
                                    "name",     "Child2",
                                    NULL);
    GblObject* pChild3 = GblObject_new(GBL_TYPE_OBJECT,
                                    "name",     "Child3",
                                    "parent",   pChild2,
                                    NULL);
    GblObject* pParent = GblObject_new(TEST_OBJECT_TYPE,
                                    "name",     "Parent",
                                    NULL);

    GblObject_parentSet(pChild1, pParent);
    QCOMPARE(GblObject_childFirst(pParent), pChild1);
    QCOMPARE(GblObject_parentGet(pChild1), pParent);

    GblObject_childAdd(pParent, pChild2);
    QCOMPARE(GblObject_parentGet(pChild2), pParent);
    QCOMPARE(GblObject_siblingNext(pChild1), pChild2);

    QCOMPARE(GblObject_parentGet(pChild3), pChild2);

    QCOMPARE(GblObject_ancestorFindByType(pChild3, TEST_OBJECT_TYPE), pParent);
    QCOMPARE(GblObject_ancestorFindByName(pChild2, "Parent"), pParent);
    QCOMPARE(GblObject_childFindByType(pChild2, GBL_TYPE_OBJECT), pChild3);
    QCOMPARE(GblObject_siblingFindByName(pChild1, "Child2"), pChild2);
    QCOMPARE(GblObject_childFindByName(pParent, "Child1"), pChild1);

    GblObject_childRemove(pParent, pChild1);
    QCOMPARE(GblObject_childFirst(pParent), pChild2);

    GblObject_childRemove(pChild2, pChild3);
    QCOMPARE(GblObject_childFirst(pChild2), nullptr);

    QCOMPARE(GblObject_unref(pChild3), 0);
    QCOMPARE(GblObject_unref(pChild1), 0);
    QCOMPARE(GblObject_unref(pParent), 0);
    QCOMPARE(GblObject_unref(pChild2), 0);
}

}
