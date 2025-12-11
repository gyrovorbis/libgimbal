#include "meta/instances/gimbal_object_test_suite.h"
#include <gimbal/containers/gimbal_ring_list.h>
#include <gimbal/meta/instances/gimbal_object.h>
#include <gimbal/meta/instances/gimbal_event.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_ref.h>
#include <math.h>

#define GBL_SELF_TYPE GblObjectTestSuite

#define TEST_OBJECT_TYPE                    (GBL_TYPEID(TestObject))
#define TEST_OBJECT(instance)               (GBL_CAST(TestObject, instance))
#define TEST_OBJECT_CHECK(instance)         (GBL_TYPECHECK(instance, TestObject))
#define TEST_OBJECT_CLASS(klass)            (GBL_CLASS_CAST(TestObject, klass))
#define TEST_OBJECT_CLASS_CHECK(klass)      (GBL_CLASS_TYPECHECK(klass, TestObject))
#define TEST_OBJECT_GET_CLASS(instance)     (GBL_CLASSOF(TestObject, instance))

GBL_FORWARD_DECLARE_STRUCT(TestObject);
static GBL_DECLARE_TYPE(TestObject);

GBL_TEST_FIXTURE {
    TestObject* pTestObj;
    size_t      refCount;
    size_t      classRefCount;
    size_t      instanceCount;
};

typedef struct TestObjectClass {
    GblObjectClass  base;
    int32_t         staticInt32;
    char            string[256];
} TestObjectClass;

GBL_INSTANCE_DERIVE(TestObject, GblObject)
    float               floater;
    char                stringer[256];
    unsigned            propertyChangedCounter;
    unsigned            floaterChangedCounter;
    unsigned            stringerChangedCounter;
    unsigned            parentChangedCounter;
    unsigned            nameChangedCounter;
    unsigned            otherChangedCounter;
    int                 eventHandlerCount;
    GblType             eventHandlerLastType;
    GblBool             eventHandlerAccept;
    int                 eventFilterCount;
    GblType             eventFilterLastType;
    GblBool             eventFilterAccept;
    GblIEventReceiver*  eventFilterLastTarget;
GBL_INSTANCE_END

GBL_PROPERTIES(TestObject,
    (floater,     GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE),
    (stringer,    GBL_GENERIC, (READ, WRITE), GBL_STRING_TYPE),
    (staticInt32, GBL_GENERIC, (READ),        GBL_INT32_TYPE)
)

GBL_TEST_INIT()
    pFixture->classRefCount = GblType_classRefCount(GBL_OBJECT_TYPE);
    pFixture->instanceCount = GblType_instanceCount(GBL_OBJECT_TYPE);
    pFixture->refCount      = GblRef_activeCount();
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_TEST_COMPARE(GblType_instanceCount(GBL_OBJECT_TYPE), pFixture->instanceCount);
    GBL_TEST_COMPARE(GblType_classRefCount(GBL_OBJECT_TYPE), pFixture->classRefCount);
    GBL_TEST_COMPARE(GblRef_activeCount(), pFixture->refCount);
    GBL_TEST_COMPARE(GblType_classRefCount(TEST_OBJECT_TYPE), 0);
    GBL_TEST_COMPARE(GblType_instanceCount(TEST_OBJECT_TYPE), 0);
    GblType_unregister(TEST_OBJECT_TYPE);
GBL_TEST_CASE_END

GBL_TEST_CASE(newDefault)
    TestObject* pObj = GBL_NEW(TestObject);
    pFixture->pTestObj = pObj;

    GBL_TEST_COMPARE(GblType_classRefCount(TEST_OBJECT_TYPE), 1);

    // validate class
    TestObjectClass* pClass = TEST_OBJECT_GET_CLASS(pObj);
    GBL_TEST_COMPARE(GBL_CLASS_TYPEOF(pClass), TEST_OBJECT_TYPE);
    GBL_TEST_COMPARE(&pClass->base, GBL_OBJECT_CLASS(pClass));
    GBL_TEST_COMPARE(pClass->string, "Davey Havoc");
    GBL_TEST_COMPARE(pClass->staticInt32, 77);

    // validate interfaces
    GblIVariantClass* pIVariantIFace = GBL_IVARIANT_GET_CLASS(pObj);
    GBL_TEST_COMPARE(GBL_CLASS_TYPEOF(pIVariantIFace), GBL_IVARIANT_TYPE);
    GBL_TEST_COMPARE(pClass, TEST_OBJECT_CLASS(pIVariantIFace));
    GBL_TEST_COMPARE(pIVariantIFace->pVTable->pGetValueFmt, "p");

    GblIEventReceiverClass* pIEventHandlerIFace = GBL_IEVENT_RECEIVER_CLASSOF(pObj);
    GBL_TEST_COMPARE(GBL_CLASS_TYPEOF(pIEventHandlerIFace), GBL_IEVENT_RECEIVER_TYPE);
    GBL_TEST_COMPARE(pClass, TEST_OBJECT_CLASS(pIEventHandlerIFace));

    GblIEventReceiverClass* pIEventFilterIFace = GBL_IEVENT_RECEIVER_CLASSOF(pObj);
    GBL_TEST_COMPARE(GBL_CLASS_TYPEOF(pIEventFilterIFace), GBL_IEVENT_RECEIVER_TYPE);
    GBL_TEST_COMPARE(pClass, TEST_OBJECT_CLASS(pIEventFilterIFace));

    // validate sanity of random casts
    GBL_TEST_COMPARE(pIVariantIFace, GBL_IVARIANT_CLASS(pIEventFilterIFace));

    //validate some of the compatible checks
    //GBL_TEST_VERIFY(GBL_IVARIANT_IFACE_CHECK(pClass));
    //GBL_TEST_VERIFY(!TEST_OBJECT_CLASS_CHECK(pIVariantIFace));
    //GBL_TEST_VERIFY(GBL_OBJECT_CLASS_CHECK(pClass));
    //GBL_TEST_VERIFY(GBL_ITABLE_IFACE_CHECK(GBL_OBJECT_CLASS(pITableIFace)));

    //validate insanity
    GBL_TEST_COMPARE(pClass, TEST_OBJECT_CLASS(GBL_OBJECT_CLASS(GBL_IEVENT_RECEIVER_CLASS(TEST_OBJECT_GET_CLASS(pObj)))));

    // validate instance checks and casts
    //GBL_TEST_VERIFY(GBL_IVARIANT_CHECK(pObj));
    //GBL_TEST_VERIFY(GBL_IEVENT_FILTER_CHECK(GBL_OBJECT(GBL_IEVENT_FILTER(pObj))));
    GBL_TEST_COMPARE(pObj, TEST_OBJECT(GBL_IVARIANT(GBL_OBJECT(GBL_IEVENT_RECEIVER(TEST_OBJECT(pObj))))));
GBL_TEST_CASE_END

GBL_TEST_CASE(name)
    GblObject_setName(GBL_OBJECT(pFixture->pTestObj), "TestName");
    GBL_TEST_COMPARE(GblObject_name(GBL_OBJECT(pFixture->pTestObj)), "TestName");
    GblObject_setNameRef(GBL_OBJECT(pFixture->pTestObj), GblStringRef_create("TestName2"));
    GBL_TEST_COMPARE(GblObject_name(GBL_OBJECT(pFixture->pTestObj)), "TestName2");
GBL_TEST_CASE_END

GBL_TEST_CASE(ref)
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pFixture->pTestObj)), 1);
    GblObject* pObj = GBL_OBJECT(GblBox_ref(GBL_BOX(pFixture->pTestObj)));
    GBL_TEST_COMPARE(GBL_OBJECT(pFixture->pTestObj), pObj);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pObj)), 2);
GBL_TEST_CASE_END

GBL_TEST_CASE(unref)
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pFixture->pTestObj)), 1);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pFixture->pTestObj)), 0);
    GBL_TEST_COMPARE(GblType_classRefCount(TEST_OBJECT_TYPE), 0);

    // K now replace what we deleted... lulz.
    pFixture->pTestObj = NULL;
GBL_TEST_CASE_END

GBL_TEST_CASE(extendedData)
    GblObject* pObj = GBL_OBJECT(GBL_NEW(TestObject));

    GBL_TEST_COMPARE(GblObject_name(pObj),           NULL);
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(pObj)),       NULL);
    GBL_TEST_COMPARE(GblObject_parent(pObj),         NULL);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(pObj),  0);

    GblObject_setName(pObj, "Sing the Sorrow");
    GBL_TEST_COMPARE(GblObject_name(pObj), "Sing the Sorrow");

    GblBox_setUserdata(GBL_BOX(pObj), (void*)0xbadbeef);
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(pObj)), (void*)0xbadbeef);

    // make sure we didn't screw up other extended data
    GBL_TEST_COMPARE(GblObject_parent(pObj),            NULL);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(pObj),  0);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj)), 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(newVaArgs)
    GblObject* pObj = GBL_OBJECT(GBL_NEW(TestObject,
                                        "name",     "truckwad",
                                        "userdata", (void*)0xdeadbeef));

    GBL_TEST_COMPARE(GblObject_name(pObj), "truckwad");
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(pObj)), (void*)0xdeadbeef);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(pObj),  0);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj)), 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(newInPlaceVaArgs)
    TestObject obj;

    GBL_TEST_CALL(GblObject_construct((GblObject*)&obj, TEST_OBJECT_TYPE,
                                       "name",     "truckwad",
                                       "userdata", (void*)0xdeadbeef,
                                        NULL));

    GBL_TEST_COMPARE(GblObject_name(GBL_OBJECT(&obj)), "truckwad");
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(&obj)), (void*)0xdeadbeef);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(GBL_OBJECT(&obj)),  0);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(&obj)), 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(newVaArgsWithClass)
    GblObjectClass* pClass = GBL_OBJECT_CLASS(GblClass_createFloating(TEST_OBJECT_TYPE));

    GblObject* pObj = GblObject_createWithClass(pClass,
                                    "name",     "truckwad",
                                    "userdata", (void*)0xdeadbeef,
                                    NULL);

    GBL_TEST_COMPARE(GblObject_name(pObj), "truckwad");
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(pObj)), (void*)0xdeadbeef);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(pObj),  0);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj)), 0);
    GBL_TEST_CALL(GblClass_destroyFloating(GBL_CLASS(pClass)));
GBL_TEST_CASE_END

GBL_TEST_CASE(newInPlaceVaArgsWithClass)
    GblObjectClass* pClass = GBL_OBJECT_CLASS(GblClass_createFloating(TEST_OBJECT_TYPE));
    TestObject obj;
    GBL_TEST_CALL(GblObject_constructWithClass((GblObject*)&obj, pClass,
                                                "name",     "truckwad",
                                                "userdata", (void*)0xdeadbeef,
                                                NULL));

    GBL_TEST_COMPARE(GblObject_name(GBL_OBJECT(&obj)),        "truckwad");
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(&obj)),          (void*)0xdeadbeef);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(GBL_OBJECT(&obj)),  0);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(&obj)), 0);
    GBL_TEST_CALL(GblClass_destroyFloating(GBL_CLASS(pClass)));
GBL_TEST_CASE_END

GBL_TEST_CASE(newInPlaceVaArgsWithClassInPlace)
    TestObjectClass klass;
    GBL_TEST_CALL(GblClass_constructFloating(GBL_CLASS(&klass), TEST_OBJECT_TYPE));

    TestObject obj;
    GBL_TEST_CALL(GblObject_constructWithClass((GblObject*)&obj, GBL_OBJECT_CLASS(&klass),
                                                        "name",     "truckwad",
                                                        "userdata", (void*)0xdeadbeef,
                                                        NULL));

    GBL_TEST_CALL(GblInstance_sinkClass(GBL_INSTANCE(&obj)));

    GBL_TEST_COMPARE(GblObject_name(GBL_OBJECT(&obj)),       "truckwad");
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(&obj)),          (void*)0xdeadbeef);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(GBL_OBJECT(&obj)),  0);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(&obj)), 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(newVariants)
    GblVariant variants[2] = {
        GBL_VARIANT_INIT,
        GBL_VARIANT_INIT
    };
    const char* keys[2] = {
        "name",
        "userdata"
    };

    GBL_TEST_CALL(GblVariant_constructValueCopy(&variants[0], GBL_STRING_TYPE, "truckwad"));
    GBL_TEST_CALL(GblVariant_constructValueCopy(&variants[1], GBL_POINTER_TYPE, (void*)0xdeadbeef));

    GblObject* pObj = GblObject_createVariants(TEST_OBJECT_TYPE, 2, keys, variants);

    GBL_TEST_COMPARE(GblObject_name(pObj),       "truckwad");
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(pObj)),          (void*)0xdeadbeef);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(pObj),  0);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj)), 0);

    GBL_TEST_CALL(GblVariant_destruct(&variants[0]));
    GBL_TEST_CALL(GblVariant_destruct(&variants[1]));
GBL_TEST_CASE_END

GBL_TEST_CASE(newVariantsWithClass)
    GblVariant variants[2] = {
        GBL_VARIANT_INIT,
        GBL_VARIANT_INIT
    };
    const char* keys[2] = {
        "name",
        "userdata"
    };

    GBL_TEST_CALL(GblVariant_constructValueCopy(&variants[0], GBL_STRING_TYPE, "truckwad"));
    GBL_TEST_CALL(GblVariant_constructValueCopy(&variants[1], GBL_POINTER_TYPE, (void*)0xdeadbeef));

    GblObjectClass* pClass = GBL_OBJECT_CLASS(GblClass_createFloating(TEST_OBJECT_TYPE));

    GblObject* pObj = GblObject_createVariantsWithClass(pClass, 2, keys, variants);

    GBL_TEST_COMPARE(GblObject_name(pObj),       "truckwad");
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(pObj)),          (void*)0xdeadbeef);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(pObj),  0);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj)), 0);
    GBL_TEST_CALL(GblClass_destroyFloating(GBL_CLASS(pClass)));

    GBL_TEST_CALL(GblVariant_destruct(&variants[0]));
    GBL_TEST_CALL(GblVariant_destruct(&variants[1]));
GBL_TEST_CASE_END


GBL_TEST_CASE(newInPlaceVariants)
    GblVariant variants[2] = {
        GBL_VARIANT_INIT,
        GBL_VARIANT_INIT
    };
    const char* keys[2] = {
        "name",
        "userdata"
    };

    GBL_TEST_CALL(GblVariant_constructValueCopy(&variants[0], GBL_STRING_TYPE, "truckwad"));
    GBL_TEST_CALL(GblVariant_constructValueCopy(&variants[1], GBL_POINTER_TYPE, (void*)0xdeadbeef));

    TestObject obj;
    GblObject* pObj = &obj.base;

    GBL_TEST_CALL((GblObject_constructVariants(pObj, TEST_OBJECT_TYPE, 2, keys, variants)));

    GBL_TEST_COMPARE(GblObject_name(pObj), "truckwad");
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(pObj)), (void*)0xdeadbeef);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(pObj),  0);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj)), 0);

    GBL_TEST_CALL(GblVariant_destruct(&variants[0]));
    GBL_TEST_CALL(GblVariant_destruct(&variants[1]));
GBL_TEST_CASE_END

GBL_TEST_CASE(newInPlaceVariantsWithClass)
    GblVariant variants[2] = {
        GBL_VARIANT_INIT,
        GBL_VARIANT_INIT
    };
    const char* keys[2] = {
        "name",
        "userdata"
    };

    GBL_TEST_CALL(GblVariant_constructValueCopy(&variants[0], GBL_STRING_TYPE, "truckwad"));
    GBL_TEST_CALL(GblVariant_constructValueCopy(&variants[1], GBL_POINTER_TYPE, (void*)0xdeadbeef));

    TestObject obj;
    GblObject* pObj = &obj.base;

    GblObjectClass* pClass = GBL_OBJECT_CLASS(GblClass_createFloating(TEST_OBJECT_TYPE));

    GBL_TEST_CALL(GblObject_constructVariantsWithClass(pObj, pClass, 2, keys, variants));

    GBL_TEST_COMPARE(GblObject_name(pObj), "truckwad");
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(pObj)), (void*)0xdeadbeef);
    GBL_TEST_COMPARE(GblObject_eventFilterCount(pObj),  0);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj)), 0);
    GBL_TEST_CALL(GblClass_destroyFloating(GBL_CLASS(pClass)));

    GBL_TEST_CALL(GblVariant_destruct(&variants[0]));
    GBL_TEST_CALL(GblVariant_destruct(&variants[1]));
GBL_TEST_CASE_END

GBL_TEST_CASE(propertyGet)
    GblObject* pObj0 = GBL_NEW(GblObject);
    GBL_TEST_VERIFY(pObj0);

    TestObject* pObj1 = GBL_NEW(TestObject,
                                "name",     "Bulbasaur",
                                "userdata", (void*)0xdeadbeef,
                                "parent",   pObj0,
                                "floater",  -77.7);
                            //     "stringer", "truckin Inheritance!");
    GBL_TEST_VERIFY(pObj1);

    uint16_t    refCount    = 0;
    void*       pUserdata   = NULL;
    const char* pName       = NULL;
    GblObject*  pParent     = NULL;
    float       floater     = 0.0f;
   // const char* pStringer   = NULL;

    GBL_RESULT result = GblObject_properties(GBL_OBJECT(pObj1),
                                             "userdata", &pUserdata,
                                             "refCount", &refCount,
                                             "name",     &pName,
                                             "parent",   &pParent,
                                             "floater",  &floater,
                                     //        "stringer", &pStringer,
                                             NULL);

    GBL_TEST_COMPARE(result,         GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(pUserdata,      (void*)0xdeadbeef);
    GBL_TEST_COMPARE(refCount,       1);
    GBL_TEST_COMPARE(pName,          "Bulbasaur");
    GBL_TEST_COMPARE(pParent,        pObj0);
    GBL_TEST_COMPARE(floater,        -77.7f);
   // GBL_TEST_COMPARE(pStringer, "truckin Inheritance!");

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj1)), 0);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj0)), 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(propertySet)
    GblObject* pObj = GBL_OBJECT(GBL_NEW(TestObject,
                                    "name",     "Bulbasaur",
                                    "userdata", (void*)0xdeadbeef,
                                    "parent",   NULL,
                                    "floater",  -77.7,
                                    "stringer", "truckin Inheritance!"));
    GblVariant variant;
    void* pUd;
    GBL_TEST_CALL(GblVariant_constructValueCopy(&variant, GBL_POINTER_TYPE, (void*)0xcafebabe));
    GBL_TEST_CALL(GblObject_setPropertyVariant(pObj, "userdata",  &variant));

    GBL_TEST_CALL(GblObject_property(pObj, "userdata", &pUd));
    GBL_TEST_COMPARE(pUd, (void*)0xcafebabe);

    GBL_TEST_CALL(GblObject_setProperty(pObj, "userdata", (void*)0xbadf00d));
    GBL_TEST_CALL(GblObject_property(pObj, "userdata", &pUd));
    GBL_TEST_COMPARE(pUd, (void*)0xbadf00d);

    GBL_TEST_CALL(GblObject_setProperties(pObj, "userdata", (void*)0x012345,
                                                      "floater", 33.33f,
                                                       NULL));
    float floater = 0.0f;
    GBL_TEST_CALL(GblObject_properties(pObj, "userdata", &pUd,
                                                  "floater", &floater,
                                                   NULL));

    GBL_TEST_COMPARE(pUd, (void*)0x12345);
    GBL_TEST_COMPARE(floater, 33.33f);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj)), 0);
GBL_TEST_CASE_END

static void GblObject_onPropertyChange_(TestObject* pSelf, GblProperty* pProp) {
    ++pSelf->propertyChangedCounter;

    if(GblProperty_objectType(pProp) == TEST_OBJECT_TYPE) {
        switch(pProp->id) {
        case TestObject_Property_Id_floater:
            ++pSelf->floaterChangedCounter; break;
        case TestObject_Property_Id_stringer:
            ++pSelf->stringerChangedCounter; break;
        default:
            ++pSelf->otherChangedCounter; break;
        }
    } else if(GblProperty_objectType(pProp) == GBL_OBJECT_TYPE) {
        switch(pProp->id) {
        case GblObject_Property_Id_name:
            ++pSelf->nameChangedCounter; break;
        case GblObject_Property_Id_parent:
            ++pSelf->parentChangedCounter; break;
        default:
            ++pSelf->otherChangedCounter; break;
        }
    }
}

GBL_TEST_CASE(propertyChange)
    TestObject* pObj = GBL_NEW(TestObject,
                               "name",     "Bulbasaur",
                               "userdata", (void*)0xdeadbeef,
                               "parent",   NULL,
                               "floater",  -77.7,
                               "stringer", "truckin Inheritance!");

    GBL_CONNECT(pObj, "propertyChange", GblObject_onPropertyChange_);

    GblObject_setProperty(GBL_OBJECT(pObj), "userdata", (void*)0xdeadbeef);
    GBL_TEST_COMPARE(pObj->propertyChangedCounter, 0);

    GblObject_setProperty(GBL_OBJECT(pObj), "userdata", (void*)0xcafebabe);
    GBL_TEST_COMPARE(pObj->propertyChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->floaterChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->stringerChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->parentChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->otherChangedCounter, 1);

    GblObject_setProperty(GBL_OBJECT(pObj), "name", "Bulbasaur");
    GBL_TEST_COMPARE(pObj->propertyChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->floaterChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->stringerChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->parentChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->otherChangedCounter, 1);

    GblObject_setProperty(GBL_OBJECT(pObj), "name", "Squirtle");
    GBL_TEST_COMPARE(pObj->propertyChangedCounter, 2);
    GBL_TEST_COMPARE(pObj->floaterChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->stringerChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->parentChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->nameChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->otherChangedCounter, 1);

    GblObject_setProperty(GBL_OBJECT(pObj), "floater", -77.7);
    GBL_TEST_COMPARE(pObj->propertyChangedCounter, 2);
    GBL_TEST_COMPARE(pObj->floaterChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->stringerChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->parentChangedCounter, 0);

    GblObject_setProperty(GBL_OBJECT(pObj), "floater", -67.7);
    GBL_TEST_COMPARE(pObj->propertyChangedCounter, 3);
    GBL_TEST_COMPARE(pObj->floaterChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->stringerChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->parentChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->nameChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->otherChangedCounter, 1);

    GblObject_emitPropertyChange(GBL_OBJECT(pObj), "parent");
    GBL_TEST_COMPARE(pObj->propertyChangedCounter, 4);
    GBL_TEST_COMPARE(pObj->floaterChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->stringerChangedCounter, 0);
    GBL_TEST_COMPARE(pObj->parentChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->nameChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->otherChangedCounter, 1);

    GblObject_emitPropertyChangeByQuark(GBL_OBJECT(pObj), GblQuark_tryString("stringer"));
    GBL_TEST_COMPARE(pObj->propertyChangedCounter, 5);
    GBL_TEST_COMPARE(pObj->floaterChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->stringerChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->parentChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->nameChangedCounter, 1);
    GBL_TEST_COMPARE(pObj->otherChangedCounter, 1);

    GBL_UNREF(pObj);
GBL_TEST_CASE_END

GBL_TEST_CASE(parenting)
    GblObject* pChild1 = GBL_NEW(GblObject, "name", "Child1");
    GblObject* pChild2 = GBL_NEW(GblObject, "name", "Child2");
    GblObject* pChild3 = GBL_NEW(GblObject, "name", "Child3",
                                            "parent", pChild2);
    GblObject* pParent = GBL_OBJECT(GBL_NEW(TestObject, "name", "Parent"));

    GblObject_setParent(pChild1, pParent);
    GBL_TEST_COMPARE(GblObject_findChildByIndex(pParent, 0), pChild1);
    GBL_TEST_COMPARE(GblObject_parent(pChild1), pParent);

    GblObject_addChild(pParent, pChild2);
    GBL_TEST_COMPARE(GblObject_parent(pChild2), pParent);
    GBL_TEST_COMPARE(GblObject_siblingNext(pChild1), pChild2);

    GBL_TEST_COMPARE(GblObject_parent(pChild3), pChild2);

    GBL_TEST_COMPARE(GblObject_findAncestorByType(pChild3, TEST_OBJECT_TYPE), pParent);
    GBL_TEST_COMPARE(GblObject_findAncestorByName(pChild2, "Parent"), pParent);
    GBL_TEST_COMPARE(GblObject_findChildByType(pChild2, GBL_OBJECT_TYPE), pChild3);
    GBL_TEST_COMPARE(GblObject_findSiblingByName(pChild1, "Child2"), pChild2);
    GBL_TEST_COMPARE(GblObject_findChildByName(pParent, "Child1"), pChild1);

    GblObject_removeChild(pParent, pChild1);
    GBL_TEST_COMPARE(GblObject_findChildByIndex(pParent, 0), pChild2);

    GblObject_removeChild(pChild2, pChild3);
    GBL_TEST_COMPARE(GblObject_findChildByIndex(pChild2, 0), NULL);

    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pChild3)), 0);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pChild1)), 0);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pChild2)), 0);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pParent)), 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(classSwizzle)
    // Create new object with name
    GblObject* pObj = GBL_OBJECT(GBL_NEW(TestObject, "name", "DickWhisp"));

    // "staticInt32" is a class-level property which is initialized to 77, fetch + verify
    int32_t value;
    GBL_TEST_VERIFY(GBL_RESULT_SUCCESS(GblObject_property(pObj, "staticInt32", &value)));
    GBL_TEST_COMPARE(value, 77);

    // pull a new, floating TestObjectClass out of our ass, preinitialized, ready-to-go
    TestObjectClass* pClass = TEST_OBJECT_CLASS(GblClass_createFloating(TEST_OBJECT_TYPE));

    // overwrite the default value
    pClass->staticInt32 = -666;

    // overwrite the existing class for the given object
    GBL_TEST_CALL(GblInstance_swizzleClass(GBL_INSTANCE(pObj), GBL_CLASS(pClass)));

    // sink swizzled class for the instance to claim ownership
    GBL_TEST_CALL(GblInstance_sinkClass(GBL_INSTANCE(pObj)));

    // now check the value of the static class property has changed accordingly
    GBL_TEST_CALL(GblObject_property(pObj, "staticInt32", &value));
    GBL_TEST_COMPARE(value, -666);

    // clean up our shit
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj)), 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(eventNotify)
    TestObject* pObj = GBL_NEW(TestObject, "name", "DickWhisp");
    GblEvent event;
    GBL_TEST_CALL(GblEvent_construct(&event, GBL_EVENT_TYPE));

    GBL_TEST_COMPARE(pObj->eventHandlerCount, 0);
    GBL_TEST_COMPARE(pObj->eventHandlerLastType, GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblEvent_state(&event), GBL_EVENT_STATE_PENDING);

    GBL_TEST_CALL(GblObject_notifyEvent(GBL_OBJECT(pObj), &event));

    GBL_TEST_COMPARE(pObj->eventHandlerCount, 1);
    GBL_TEST_COMPARE(pObj->eventHandlerLastType, GBL_EVENT_TYPE);
    GBL_TEST_COMPARE(GblEvent_state(&event), GBL_EVENT_STATE_PENDING);

    pObj->eventHandlerAccept = GBL_TRUE;

    GBL_TEST_CALL(GblObject_notifyEvent(GBL_OBJECT(pObj), &event));

    GBL_TEST_COMPARE(pObj->eventHandlerCount, 2);
    GBL_TEST_COMPARE(pObj->eventHandlerLastType, GBL_EVENT_TYPE);
    GBL_TEST_COMPARE(GblEvent_state(&event), GBL_EVENT_STATE_ACCEPTED);

    GBL_UNREF(&event);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pObj)), 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(eventNotifyAncestors)
    TestObject* pGrand  = GBL_NEW(TestObject, "name", "Grandparent");
    TestObject* pParent = GBL_NEW(TestObject, "name", "Parent", "parent", pGrand);
    TestObject* pChild  = GBL_NEW(TestObject, "name", "Child",  "parent", pParent);

    GblEvent event;
    GBL_TEST_CALL(GblEvent_construct(&event, GBL_EVENT_TYPE));

    GBL_TEST_CALL(GblObject_notifyEvent(GBL_OBJECT(pChild), &event));

    GBL_TEST_COMPARE(pChild->eventHandlerCount, 1);
    GBL_TEST_COMPARE(pParent->eventHandlerCount, 1);
    GBL_TEST_COMPARE(pGrand->eventHandlerCount, 1);

    pParent->eventHandlerAccept = GBL_TRUE;

    GBL_TEST_CALL(GblObject_notifyEvent(GBL_OBJECT(pChild), &event));

    GBL_TEST_COMPARE(pChild->eventHandlerCount, 2);
    GBL_TEST_COMPARE(pParent->eventHandlerCount, 2);
    GBL_TEST_COMPARE(pGrand->eventHandlerCount, 1);

    GBL_UNREF(&event);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pChild)), 0);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pParent)), 0);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pGrand)), 0);
 GBL_TEST_CASE_END

static GBL_RESULT GblObjectTestSuite_eventSendAncestorsEmit(TestObject* pObj, GblEvent* pEvent) {
    GBL_CTX_BEGIN(pObj);
    GBL_CTX_EVENT(pEvent);
    GBL_CTX_END();
}

GBL_TEST_CASE(eventSendAncestors)
    TestObject* pGrand  = GBL_NEW(TestObject, "name", "Grandparent");
    TestObject* pParent = GBL_NEW(TestObject, "name", "Parent", "parent", pGrand);
    TestObject* pChild  = GBL_NEW(TestObject, "name", "Child",  "parent", pParent);

    GblEvent event;
    GBL_TEST_CALL(GblEvent_construct(&event, GBL_EVENT_TYPE));
    pGrand->eventHandlerAccept = GBL_TRUE;

    GBL_TEST_CALL(GblObjectTestSuite_eventSendAncestorsEmit(pChild, &event));

    GBL_TEST_COMPARE(pChild->eventHandlerCount, 0);
    GBL_TEST_COMPARE(pParent->eventHandlerCount, 1);
    GBL_TEST_COMPARE(pGrand->eventHandlerCount, 1);
    GBL_TEST_COMPARE(GblEvent_state(&event), GBL_EVENT_STATE_ACCEPTED);

    GBL_UNREF(&event);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pChild)), 0);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pParent)), 0);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pGrand)), 0);
GBL_TEST_CASE_END


GBL_TEST_CASE(eventFilters)
    TestObject* pGrand  = GBL_NEW(TestObject, "name", "Grandparent");
    TestObject* pParent = GBL_NEW(TestObject, "name", "Parent", "parent", pGrand);
    TestObject* pChild  = GBL_NEW(TestObject, "name", "Child",  "parent", pParent);

    GBL_TEST_CALL(GblObject_installEventFilter(GBL_OBJECT(pParent), GBL_IEVENT_RECEIVER(pChild)));

    GblEvent event;
    GBL_TEST_CALL(GblEvent_construct(&event, GBL_EVENT_TYPE));

    GBL_TEST_CALL(GblObject_notifyEvent(GBL_OBJECT(pChild), &event));

    GBL_TEST_COMPARE(pChild->eventHandlerCount, 1);
    GBL_TEST_COMPARE(pParent->eventHandlerCount, 1);
    GBL_TEST_COMPARE(pGrand->eventHandlerCount, 1);
    GBL_TEST_COMPARE(pChild->eventFilterCount, 1);
    GBL_TEST_COMPARE(pChild->eventFilterLastType, GBL_EVENT_TYPE);
    GBL_TEST_COMPARE(pChild->eventFilterLastTarget, GBL_IEVENT_RECEIVER(pParent));

    pChild->eventFilterAccept = GBL_TRUE;

    GBL_TEST_CALL(GblObject_notifyEvent(GBL_OBJECT(pChild), &event));
    GBL_TEST_COMPARE(pChild->eventHandlerCount, 2);
    GBL_TEST_COMPARE(pParent->eventHandlerCount, 1);
    GBL_TEST_COMPARE(pGrand->eventHandlerCount, 1);
    GBL_TEST_COMPARE(pChild->eventFilterCount, 2);

    GBL_TEST_CALL(GblObject_uninstallEventFilter(GBL_OBJECT(pParent), GBL_IEVENT_RECEIVER(pChild)));

    GblEvent_reset(&event);
    GBL_TEST_CALL(GblObject_notifyEvent(GBL_OBJECT(pChild), &event));
    GBL_TEST_COMPARE(pChild->eventHandlerCount, 3);
    GBL_TEST_COMPARE(pParent->eventHandlerCount, 2);
    GBL_TEST_COMPARE(pGrand->eventHandlerCount, 2);
    GBL_TEST_COMPARE(pChild->eventFilterCount, 2);

    GBL_UNREF(&event);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pChild)), 0);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pParent)), 0);
    GBL_TEST_COMPARE(GblBox_unref(GBL_BOX(pGrand)), 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantITableCount)
    GblVariant v;

    GBL_TEST_CALL(GblVariant_constructValueMove(&v, GBL_OBJECT_TYPE, GBL_NEW(GblObject)));
    GBL_TEST_COMPARE(GblVariant_count(&v), 5);
    GBL_TEST_CALL(GblVariant_destruct(&v));

GBL_TEST_CASE_END

GBL_TEST_CASE(variantITableIndex)
    GblVariant t = GBL_VARIANT_INIT;
    GblVariant v = GBL_VARIANT_INIT;

    GBL_TEST_CALL(GblVariant_constructValueMove(&t,
                                                GBL_OBJECT_TYPE,
                                                GBL_NEW(GblObject,
                                                        "name",     "dynamicChild",
                                                        "userdata", (void*)0xcafebabe,
                                                        "parent",   GBL_NEW(GblObject,
                                                                            "name",    "dynamicParent",
                                                                            "userdata", (void*)0xdeadbeef))));

    GBL_TEST_VERIFY(GblVariant_field(&t, "name", &v));
    GBL_TEST_COMPARE(GblVariant_toString(&v), "dynamicChild");

    GBL_TEST_VERIFY(GblVariant_field(&t, "userdata", &v));
    GBL_TEST_COMPARE(GblVariant_toPointer(&v), (const void*)0xcafebabe);

    GblObject* pParent = GblVariant_objectPeek(GblVariant_field(&t, "parent", &v));
    GBL_TEST_VERIFY(pParent);
    GBL_TEST_COMPARE(GblObject_name(pParent), "dynamicParent");
    //GBL_TEST_COMPARE(GBL_UNREF(pParent), 0);

    GBL_TEST_CALL(GblVariant_destruct(&t));
    GBL_TEST_CALL(GblVariant_destruct(&v));
GBL_TEST_CASE_END

GBL_TEST_CASE(variantITableSetIndex)
    GblVariant t = GBL_VARIANT_INIT;
    GblVariant v = GBL_VARIANT_INIT;
    GblVariant k = GBL_VARIANT_INIT;

    GBL_TEST_CALL(GblVariant_constructValueMove(&t,
                                                GBL_OBJECT_TYPE,
                                                GBL_NEW(GblObject,
                                                        "name",     "dynamicChild",
                                                        "userdata", 0xcafebabe)));

    GblVariant_construct(&k, "name");
    GblVariant_construct(&v, "staticChild");

    GBL_TEST_CALL(GblVariant_setIndex(&t, &k, &v));
    GBL_TEST_COMPARE(GblObject_name(GblVariant_objectPeek(&t)), "staticChild");

    GblVariant_setPointer(&v, GBL_POINTER_TYPE, (void*)0xdeadbeef);
    GBL_TEST_CALL(GblVariant_setField(&t, "userdata", &v));
    GBL_TEST_COMPARE(GblBox_userdata(GblVariant_boxPeek(&t)), (const void*)0xdeadbeef);

    GblVariant_destruct(&t);
    GblVariant_destruct(&k);
    GblVariant_destruct(&v);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantITableNext)
    GblVariant t = GBL_VARIANT_INIT;
    GblVariant v = GBL_VARIANT_INIT;
    GblVariant k = GBL_VARIANT_INIT;

    GBL_TEST_CALL(GblVariant_constructValueMove(&t,
                                                TEST_OBJECT_TYPE,
                                                GBL_NEW(TestObject,
                                                        "name",     "Bulbasaur",
                                                        "userdata", (void*)0xdeadbeef,
                                                        "floater",  -77.7,
                                                        "stringer", "Charmander")));
    size_t i = 0;
    while(GblVariant_next(&t, &k, &v)) {
        switch(i) {
        case 0:
            GBL_TEST_COMPARE(GblVariant_string(&k), "name");
            GBL_TEST_COMPARE(GblVariant_string(&v), "Bulbasaur");
            break;
        case 1:
            GBL_TEST_COMPARE(GblVariant_string(&k), "parent");
            GBL_TEST_COMPARE(GblVariant_objectPeek(&v), NULL);
            break;
        case 2:
            GBL_TEST_COMPARE(GblVariant_string(&k), "children");
            break;
        case 3:
            GBL_TEST_COMPARE(GblVariant_string(&k), "userdata");
            GBL_TEST_COMPARE(GblVariant_pointer(&v), (void*)0xdeadbeef);
            break;
        case 4:
            GBL_TEST_COMPARE(GblVariant_string(&k), "refCount");
            GBL_TEST_COMPARE(GblVariant_uint16(&v), 1);
            break;
        case 5:
            GBL_TEST_COMPARE(GblVariant_string(&k), "floater");
            GBL_TEST_COMPARE(GblVariant_float(&v), -77.7f);
            break;
        case 6:
            GBL_TEST_COMPARE(GblVariant_string(&k), "stringer");
            GBL_TEST_COMPARE(GblVariant_string(&v), "Charmander");
            break;
        case 7:
            GBL_TEST_COMPARE(GblVariant_string(&k), "staticInt32");
            GBL_TEST_COMPARE(GblVariant_int32(&v), 77);
            break;
        default: GBL_TEST_VERIFY(GBL_FALSE);
        }
        ++i;
    }

    GBL_TEST_VERIFY(!GblVariant_isValid(&k));
    GBL_TEST_VERIFY(!GblVariant_isValid(&v));

    GblVariant_destruct(&t);
    GblVariant_destruct(&k);
    GblVariant_destruct(&v);
GBL_TEST_CASE_END

GBL_TEST_CASE(siblingNextByType)
    TestObject* pParent = GBL_NEW(TestObject);
    GblObject*  pChild1 = GBL_NEW(GblObject,  "parent", pParent);
    TestObject* pChild2 = GBL_NEW(TestObject, "parent", pParent);
    GblObject*  pChild3 = GBL_NEW(GblObject,  "parent", pParent);
    TestObject* pChild4 = GBL_NEW(TestObject, "parent", pParent);

    GBL_TEST_COMPARE(GblObject_siblingNextByType(pChild1,             TEST_OBJECT_TYPE), pChild2);
    GBL_TEST_COMPARE(GblObject_siblingNextByType(GBL_OBJECT(pChild2), TEST_OBJECT_TYPE), pChild4);
    GBL_TEST_COMPARE(GblObject_siblingNextByType(pChild3,             TEST_OBJECT_TYPE), pChild4);
    GBL_TEST_COMPARE(GblObject_siblingNextByType(GBL_OBJECT(pChild4), TEST_OBJECT_TYPE), NULL);
    GBL_TEST_COMPARE(GblObject_siblingNextByType(GBL_OBJECT(pParent), TEST_OBJECT_TYPE), NULL);
    GBL_TEST_COMPARE(GblObject_siblingNextByType(pChild1,             GBL_OBJECT_TYPE ), pChild3);
    GBL_TEST_COMPARE(GblObject_siblingNextByType(GBL_OBJECT(pChild2), GBL_OBJECT_TYPE ), pChild3);
    GBL_TEST_COMPARE(GblObject_siblingNextByType(pChild3,             GBL_OBJECT_TYPE ), NULL);
    GBL_TEST_COMPARE(GblObject_siblingNextByType(GBL_OBJECT(pChild4), GBL_OBJECT_TYPE ), NULL);
    GBL_TEST_COMPARE(GblObject_siblingNextByType(GBL_OBJECT(pParent), GBL_OBJECT_TYPE ), NULL);

    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_CASE(siblingNextByName)
    GblObject* pParent = GBL_NEW(GblObject);
    GblObject* pChild1 = GBL_NEW(GblObject, "parent", pParent, "name", "Child1");
    GblObject* pChild2 = GBL_NEW(GblObject, "parent", pParent, "name", "Child2");
    GblObject* pChild3 = GBL_NEW(GblObject, "parent", pParent, "name", "Child3");
    GblObject* pChild4 = GBL_NEW(GblObject, "parent", pParent, "name", "Child4");

    GBL_TEST_COMPARE(GblObject_siblingNextByName(pChild1, "Child3"),         pChild3);
    GBL_TEST_COMPARE(GblObject_siblingNextByName(pChild2, "Child3"),         pChild3);
    GBL_TEST_COMPARE(GblObject_siblingNextByName(pChild3, "Child3"),         NULL);
    GBL_TEST_COMPARE(GblObject_siblingNextByName(pChild4, "Child3"),         NULL);
    GBL_TEST_COMPARE(GblObject_siblingNextByName(pChild1, "Child2"),         pChild2);
    GBL_TEST_COMPARE(GblObject_siblingNextByName(pChild2, "Child2"),         NULL);
    GBL_TEST_COMPARE(GblObject_siblingNextByName(pChild3, "hello!"),         NULL);
    GBL_TEST_COMPARE(GblObject_siblingNextByName(pChild4, "libGimbal"),      NULL);
    GBL_TEST_COMPARE(GblObject_siblingNextByName(pParent, "Child4"),         NULL);
    GBL_TEST_COMPARE(GblObject_siblingNextByName(pParent, "no siblings :("), NULL);

    GBL_UNREF(pParent);
GBL_TEST_CASE_END


GBL_TEST_CASE(siblingPrevious)
    GblObject* pParent = GBL_NEW(GblObject);
    GblObject* pChild1 = GBL_NEW(GblObject, "parent", pParent);
    GblObject* pChild2 = GBL_NEW(GblObject, "parent", pParent);
    GblObject* pChild3 = GBL_NEW(GblObject, "parent", pParent);
    GblObject* pChild4 = GBL_NEW(GblObject, "parent", pParent);

    GBL_TEST_COMPARE(GblObject_siblingPrevious(pChild1), NULL);
    GBL_TEST_COMPARE(GblObject_siblingPrevious(pChild2), pChild1);
    GBL_TEST_COMPARE(GblObject_siblingPrevious(pChild3), pChild2);
    GBL_TEST_COMPARE(GblObject_siblingPrevious(pChild4), pChild3);
    GBL_TEST_COMPARE(GblObject_siblingPrevious(pParent), NULL);

    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_CASE(siblingPreviousByType)
    TestObject* pParent = GBL_NEW(TestObject);
    GblObject*  pChild1 = GBL_NEW(GblObject,  "parent", pParent);
    TestObject* pChild2 = GBL_NEW(TestObject, "parent", pParent);
    GblObject*  pChild3 = GBL_NEW(GblObject,  "parent", pParent);
    TestObject* pChild4 = GBL_NEW(TestObject, "parent", pParent);

    GBL_TEST_COMPARE(GblObject_siblingPreviousByType(pChild1,             TEST_OBJECT_TYPE), NULL);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByType(GBL_OBJECT(pChild2), TEST_OBJECT_TYPE), NULL);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByType(pChild3,             TEST_OBJECT_TYPE), pChild2);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByType(GBL_OBJECT(pChild4), TEST_OBJECT_TYPE), pChild2);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByType(GBL_OBJECT(pParent), TEST_OBJECT_TYPE), NULL);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByType(pChild1,             GBL_OBJECT_TYPE ), NULL);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByType(GBL_OBJECT(pChild2), GBL_OBJECT_TYPE ), pChild1);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByType(pChild3,             GBL_OBJECT_TYPE ), pChild1);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByType(GBL_OBJECT(pChild4), GBL_OBJECT_TYPE ), pChild3);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByType(GBL_OBJECT(pParent), GBL_OBJECT_TYPE ), NULL);

    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_CASE(siblingPreviousByName)
    GblObject* pParent = GBL_NEW(GblObject);
    GblObject* pChild1 = GBL_NEW(GblObject, "parent", pParent, "name", "Child1");
    GblObject* pChild2 = GBL_NEW(GblObject, "parent", pParent, "name", "Child2");
    GblObject* pChild3 = GBL_NEW(GblObject, "parent", pParent, "name", "Child3");
    GblObject* pChild4 = GBL_NEW(GblObject, "parent", pParent, "name", "Child4");

    GBL_TEST_COMPARE(GblObject_siblingPreviousByName(pChild1, "Child3"),    NULL);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByName(pChild2, "Child3"),    NULL);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByName(pChild3, "Child3"),    NULL);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByName(pChild4, "Child3"),    pChild3);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByName(pParent, "Child4"),    NULL);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByName(pChild1, "something"), NULL);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByName(pChild2, "Child1"),    pChild1);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByName(pChild3, "Child1"),    pChild1);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByName(pChild4, "Child1"),    pChild1);
    GBL_TEST_COMPARE(GblObject_siblingPreviousByName(pParent, "something"), NULL);

    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_CASE(childIndex)
    GblObject* pParent = GBL_NEW(GblObject);
    GblObject* pChild1 = GBL_NEW(GblObject, "parent", pParent);
    GblObject* pChild2 = GBL_NEW(GblObject, "parent", pParent);
    GblObject* pChild3 = GBL_NEW(GblObject, "parent", pParent);
    GblObject* pChild4 = GBL_NEW(GblObject, "parent", pParent);

    GBL_TEST_COMPARE(GblObject_childIndex(pChild1), 0);
    GBL_TEST_COMPARE(GblObject_childIndex(pChild2), 1);
    GBL_TEST_COMPARE(GblObject_childIndex(pChild3), 2);
    GBL_TEST_COMPARE(GblObject_childIndex(pChild4), 3);
    GBL_TEST_COMPARE(GblObject_childIndex(pParent), GBL_INDEX_INVALID);

    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_CASE(childrenProperty)
    GblObject* pChild1 = GBL_NEW(GblObject);
    GblObject* pChild2 = GBL_NEW(GblObject);
    GblObject* pChild3 = GBL_NEW(GblObject);
    GblObject* pChild4 = GBL_NEW(GblObject);
    GblObject* pParent = GBL_NEW(GblObject, "children",
                                 GblRingList_create(pChild1, pChild2,
                                                    pChild3, pChild4));

    GblRingList *pChildren = NULL;
    GblObject_property(pParent, "children", &pChildren);

    GBL_TEST_COMPARE(GblRingList_size(pChildren), 4);
    GBL_TEST_COMPARE(GblRingList_at(pChildren, 0), pChild1);
    GBL_TEST_COMPARE(GblRingList_at(pChildren, 1), pChild2);
    GBL_TEST_COMPARE(GblRingList_at(pChildren, 2), pChild3);
    GBL_TEST_COMPARE(GblRingList_at(pChildren, 3), pChild4);

    GblRingList_unref(pChildren);
    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_CASE(childrenPropertyVariant)
    GblVariant variant;
    GblVariant_constructValueMove(&variant, GBL_RING_LIST_TYPE,
                                  GblRingList_create(
                                    GBL_NEW(GblObject, "name", "Child1"),
                                    GBL_NEW(GblObject, "name", "Child2"),
                                    GBL_NEW(GblObject, "name", "Child3"),
                                    GBL_NEW(GblObject, "name", "Child4")
                                  ));

    GblObject* pParent = GblObject_createVariants(GBL_OBJECT_TYPE, 1,
                                                  (const char*[]){"children"}, &variant);

    GblRingList *pChildren = NULL;
    GblObject_property(pParent, "children", &pChildren);

    GBL_TEST_COMPARE(GblRingList_size(pChildren), 4);
    GBL_TEST_COMPARE(GblObject_name(GblRingList_at(pChildren, 0)), "Child1");
    GBL_TEST_COMPARE(GblObject_name(GblRingList_at(pChildren, 1)), "Child2");
    GBL_TEST_COMPARE(GblObject_name(GblRingList_at(pChildren, 2)), "Child3");
    GBL_TEST_COMPARE(GblObject_name(GblRingList_at(pChildren, 3)), "Child4");

    GblRingList_foreach(pChildren, pChild)
        GBL_UNREF(pChild);

    GBL_UNREF(pParent);

    GblVariant_destruct(&variant);
    GblRingList_unref(pChildren);
GBL_TEST_CASE_END

GBL_TEST_CASE(foreachChild)
    GblObject* pParent = GBL_NEW(GblObject);
    GblObject* pChild1 = GBL_NEW(GblObject, "name", "Child1", "parent", pParent);
    GblObject* pChild2 = GBL_NEW(GblObject, "name", "Child2", "parent", pParent);
    GblObject* pChild3 = GBL_NEW(GblObject, "name", "Child3", "parent", pParent);
    GblObject* pChild4 = GBL_NEW(GblObject, "name", "Child4", "parent", pParent);

    int index = 0;

    GblObject_foreachChild(pParent, pChild) {
        GBL_TEST_COMPARE(GblObject_name(pChild),
                         GblObject_name(GblObject_findChildByIndex(pParent, index)));
        ++index;
    }

    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_CASE(foreachChildType)
    GblObject*  pParent = GBL_NEW(GblObject);
    TestObject* pChild1 = GBL_NEW(TestObject, "name", "Child1", "parent", pParent);
    TestObject* pChild2 = GBL_NEW(TestObject, "name", "Child2", "parent", pParent);
    TestObject* pChild3 = GBL_NEW(TestObject, "name", "Child3", "parent", pParent);
    TestObject* pChild4 = GBL_NEW(TestObject, "name", "Child4", "parent", pParent);

    int index = 0;

    GblObject_foreachChild(pParent, pChild, TestObject*) {
        TestObjectClass* pClass = TEST_OBJECT_GET_CLASS(pChild);
        GBL_TEST_COMPARE(GblObject_name(GBL_OBJECT(pChild)),
                         GblObject_name(GblObject_findChildByIndex(pParent, index)));
        GBL_TEST_COMPARE(strcmp(pChild->stringer, "INVALID"), 0);
        GBL_TEST_COMPARE(pClass->staticInt32, 77);
        ++index;
    }

    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_CASE(addChildren)
    GblObject*   pParent    = GBL_NEW(GblObject);
    GblRingList* pChildList = GblRingList_create(
        GBL_NEW(GblObject, "name", "Child1"),
        GBL_NEW(GblObject, "name", "Child2"),
        GBL_NEW(GblObject, "name", "Child3"),
        GBL_NEW(GblObject, "name", "Child4")
    );

    GblObject_addChildren(pParent, pChildList);

    int index = 0;
    GblObject_foreachChild(pParent, pChild) {
        GBL_TEST_COMPARE(GblObject_name(pChild),
                         GblObject_name(GblObject_findChildByIndex(pParent, index)));
        ++index;
    }

    GblRingList_foreach(pChildList, pChild)
        GBL_UNREF(pChild);
    GblRingList_unref(pChildList);
    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_CASE(setChildren)
    GblObject*   pParent    = GBL_NEW(GblObject);
    GblRingList* pChildList = GblRingList_create(
        GBL_NEW(GblObject, "name", "Child1"),
        GBL_NEW(GblObject, "name", "Child2"),
        GBL_NEW(GblObject, "name", "Child3"),
        GBL_NEW(GblObject, "name", "Child4")
    );

    GblObject *pChildDummy = GBL_NEW(GblObject, "name", "Dummy");
    GblObject_setChildren(pParent, pChildList);

    int index = 0;
    GblObject_foreachChild(pParent, pChild) {
        GBL_TEST_COMPARE(GblObject_name(pChild),
                         GblObject_name(GblObject_findChildByIndex(pParent, index)));
        ++index;
    }

    GblRingList_foreach(pChildList, pChild)
        GBL_UNREF(pChild);
    GblRingList_unref(pChildList);

    GBL_UNREF(pChildDummy);
    GBL_UNREF(pParent);
GBL_TEST_CASE_END


static GblBool gblObjectIterator_(GblObject* pChild, void* pClosure) {
    int* pState = (int*)pClosure;
    GblBool retValue = GBL_FALSE;
    switch(*pState) {
    case 0:
        if(strcmp(GblObject_name(pChild), "Child1") == 0)
            ++(*pState);
        break;
    case 1:
        if(strcmp(GblObject_name(pChild), "Child2") == 0)
            ++(*pState);
        break;
    case 2:
        if(strcmp(GblObject_name(pChild), "Child3") == 0)
            ++(*pState);
            retValue = GBL_TRUE;
        break;
    case 3:
        if(strcmp(GblObject_name(pChild), "Child4") == 0) {
            ++(*pState);
        }
        break;
    default: break;
    }
    return retValue;
}

GBL_TEST_CASE(iterateChildren)
    GblObject* pParent = GBL_NEW(GblObject, "children",
                                            GblRingList_create(
                                                GBL_NEW(GblObject, "name", "Child1"),
                                                GBL_NEW(GblObject, "name", "Child2"),
                                                GBL_NEW(GblObject, "name", "Child3"),
                                                GBL_NEW(GblObject, "name", "Child4")
                                            ));

    int state = 0;
    GblObject_iterateChildren(pParent, gblObjectIterator_, &state);
    GBL_TEST_COMPARE(state, 3);

    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_CASE(childLast)
    GblObject* pParent = GBL_NEW(GblObject);
    GblObject* pChild1 = GBL_NEW(GblObject, "parent", pParent);
    GblObject* pChild2 = GBL_NEW(GblObject, "parent", pParent);
    GblObject* pChild3 = GBL_NEW(GblObject, "parent", pParent);
    GblObject* pChild4 = GBL_NEW(GblObject, "parent", pParent);

    GBL_TEST_COMPARE(GblObject_childLast(pParent), pChild4);
    GBL_UNREF(pChild4);
    GBL_TEST_COMPARE(GblObject_childLast(pParent), pChild3);
    GBL_UNREF(pChild2);
    GBL_TEST_COMPARE(GblObject_childLast(pParent), pChild3);
    GBL_UNREF(pChild3);
    GBL_TEST_COMPARE(GblObject_childLast(pParent), pChild1);
    GBL_UNREF(pChild1);
    GBL_TEST_COMPARE(GblObject_childLast(pParent), NULL);
    GBL_UNREF(pParent);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(newDefault,
                  name,
                  ref,
                  unref,
                  extendedData,
                  newVaArgs,
                  newInPlaceVaArgs,
                  newVaArgsWithClass,
                  newInPlaceVaArgsWithClass,
                  newInPlaceVaArgsWithClassInPlace,
                  newVariants,
                  newVariantsWithClass,
                  newInPlaceVariants,
                  newInPlaceVariantsWithClass,
                  propertyGet,
                  propertySet,
                  propertyChange,
                  parenting,
                  classSwizzle,
                  eventNotify,
                  eventNotifyAncestors,
                  eventSendAncestors,
                  eventFilters,
                  variantITableCount,
                  variantITableIndex,
                  variantITableSetIndex,
                  variantITableNext,
                  siblingNextByType,
                  siblingNextByName,
                  siblingPrevious,
                  siblingPreviousByType,
                  siblingPreviousByName,
                  childIndex,
                  childrenProperty,
                  childrenPropertyVariant,
                  foreachChild,
                  foreachChildType,
                  addChildren,
                  setChildren,
                  iterateChildren,
                  childLast
                )

static GBL_RESULT TestObject_IEventReceiver_receiveEvent(GblIEventReceiver* pSelf, GblIEventReceiver* pDest, GblEvent* pEvent) {
    GBL_CTX_BEGIN(pSelf);

    TestObject* pTest = TEST_OBJECT(pSelf);

    if(pSelf == pDest) {
        pTest->eventHandlerCount++;
        pTest->eventHandlerLastType = GBL_TYPEOF(pEvent);

        if(pTest->eventHandlerAccept)
            GblEvent_accept(pEvent);
    } else {
        pTest->eventFilterCount++;
        pTest->eventFilterLastType   = GBL_TYPEOF(pEvent);
        pTest->eventFilterLastTarget = pDest;

        if(pTest->eventFilterAccept)
            GblEvent_accept(pEvent);
    }

    GBL_CTX_END();
}

static GBL_RESULT TestObject_init_(GblInstance* pInstance) {
    GblObject* pSelf = GBL_OBJECT(pInstance);
    GBL_CTX_BEGIN(pSelf);

    TestObject* pTest = TEST_OBJECT(pSelf);
    pTest->floater = -NAN;
    strcpy(pTest->stringer, "INVALID");
    GBL_CTX_END();
}

static GBL_RESULT TestObject_constructed(GblObject* pSelf) {
    GBL_CTX_BEGIN(pSelf);
    GblObjectClass* pParentClass = GBL_OBJECT_CLASS(GblClass_super(GBL_INSTANCE_GET_CLASS(pSelf)));
    if(pParentClass->pFnConstructed) pParentClass->pFnConstructed(pSelf);
    GBL_CTX_END();
}

static GBL_RESULT TestObject_destructor(GblBox* pSelf) {
    GBL_CTX_BEGIN(pSelf);
    GblBoxClass* pParentClass = GBL_BOX_CLASS(GblClass_super(GBL_INSTANCE_GET_CLASS(pSelf)));
    pParentClass->pFnDestructor(GBL_BOX(pSelf));
    GBL_CTX_END();
}

static GBL_RESULT TestObject_property_(const GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(pSelf);
    switch(pProp->id) {
    case TestObject_Property_Id_floater:
        GblVariant_setValueCopy(pValue, pProp->valueType, TEST_OBJECT(pSelf)->floater);
        break;
    case TestObject_Property_Id_stringer:
        GblVariant_setValueCopy(pValue, pProp->valueType, TEST_OBJECT(pSelf)->stringer);
        break;
    case TestObject_Property_Id_staticInt32:
        GblVariant_setValueCopy(pValue, pProp->valueType, TEST_OBJECT_GET_CLASS(pSelf)->staticInt32);
        break;
/*    case TestObject_Property_Id_userdata:
        GblVariant_setValueCopy(pValue, pProp->valueType, GblBox_userdata(GBL_BOX(pSelf)));
        break;*/
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Attempt to get unregistered property: %s",
                                GblProperty_name(pProp));
    }
    GBL_CTX_END();
}

static GBL_RESULT TestObject_setProperty_(GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(pSelf);
    switch(pProp->id) {
    case TestObject_Property_Id_floater: {
        float value = NAN;
        GBL_CTX_CALL(GblVariant_valueCopy(pValue, &value));
        TEST_OBJECT(pSelf)->floater = value;
        break;
    }
    case TestObject_Property_Id_stringer: {
        const char* pStr = NULL;
        GBL_CTX_CALL(GblVariant_valuePeek(pValue, &pStr));
        if(pStr) strcpy(TEST_OBJECT(pSelf)->stringer, pStr);
        break;
    }
/*    case TestObject_Property_Id_userdata: {
        void* pUserdata = NULL;
        GBL_CTX_CALL(GblVariant_valueCopy(pValue, &pUserdata));
        GblBox_setUserdata(GBL_BOX(pSelf), pUserdata);
        break;
    }*/
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Attempt to set unregistered property: %s",
                                GblProperty_name(pProp));
    }
    GBL_CTX_END();
}


static GBL_RESULT TestObjectClass_init_(GblClass* pClass, const void* pUd) {
    GBL_CTX_BEGIN(NULL);

    if(!GblType_classRefCount(TEST_OBJECT_TYPE))
        GBL_PROPERTIES_REGISTER(TestObject);

    TEST_OBJECT_CLASS(pClass)->staticInt32 = 77;
    strcpy(TEST_OBJECT_CLASS(pClass)->string, (const char*)pUd);

    GBL_IEVENT_RECEIVER_CLASS(pClass)->pFnReceiveEvent = TestObject_IEventReceiver_receiveEvent;
    GBL_BOX_CLASS(pClass)            ->pFnDestructor   = TestObject_destructor;
    GBL_OBJECT_CLASS(pClass)         ->pFnConstructed  = TestObject_constructed;
    GBL_OBJECT_CLASS(pClass)         ->pFnProperty     = TestObject_property_;
    GBL_OBJECT_CLASS(pClass)         ->pFnSetProperty  = TestObject_setProperty_;

    GBL_CTX_END();
}

static GblType TestObject_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        const GblTypeInfo info = {
            .pFnClassInit    = TestObjectClass_init_,
            .classSize       = sizeof(TestObjectClass),
            .pClassData      = (void*)"Davey Havoc",
            .pFnInstanceInit = TestObject_init_,
            .instanceSize    = sizeof(TestObject)
        };

        type = GblType_register("TestObject",
                                GBL_OBJECT_TYPE,
                                &info,
                                GBL_TYPE_FLAGS_NONE);
    }

    return type;
}
