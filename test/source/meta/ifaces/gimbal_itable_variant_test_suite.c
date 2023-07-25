#include "meta/ifaces/gimbal_itable_variant_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_ref.h>

#define GBL_SELF_TYPE               GblITableVariantTestSuite

#define GBL_TEST_BOX_TYPE           (GBL_TYPEOF(GblTestBox))
#define GBL_TEST_BOX(self)          (GBL_INSTANCE_CAST(self, GblTestBox))
#define GBL_TEST_BOX_CLASS(klass)   (GBL_CLASS_CAST(klass, GblTestBox))

#define GBL_TEST_OBJECT_TYPE        (GBL_TYPEOF(GblTestObject))
#define GBL_TEST_OBJECT(self)       (GBL_INSTANCE_CAST(self, GblTestObject))
#define GBL_TEST_OBJECT_CLASS(klass)(GBL_CLASS_CAST(klass, GblTestObject))

GBL_FORWARD_DECLARE_STRUCT(GblTestBox);
GBL_FORWARD_DECLARE_STRUCT(GblTestObject);
static GBL_DECLARE_TYPE(GblTestBox);
static GBL_DECLARE_TYPE(GblTestObject);

GBL_CLASS_DERIVE_EMPTY(GblTestBox, GblBox, GblITableVariant)
GBL_CLASS_DERIVE_EMPTY(GblTestObject, GblObject)

GBL_INSTANCE_DERIVE(GblTestBox, GblBox)
    int32_t       integer;
    GblBool       boolean;
    float         number;
    GblStringRef* pString;
GBL_INSTANCE_END

GBL_INSTANCE_DERIVE(GblTestObject, GblObject)
    void*       pPointer;
    GblTestBox  testBox;
    char        character;
GBL_INSTANCE_END

GBL_PROPERTIES(GblTestObject,
   (pointer,   GBL_GENERIC, (READ, WRITE), GBL_POINTER_TYPE),
   (testBox,   GBL_GENERIC, (READ, WRITE), GBL_TEST_BOX_TYPE),
   (character, GBL_GENERIC, (READ, WRITE), GBL_CHAR_TYPE)
)

GBL_TEST_FIXTURE {
    size_t         refCount;

    GblType        testBoxType;
    GblType        testObjectType;

    GblTestBox*    pTestBox;
    GblObject*     pObject;
    GblTestObject* pTestObject;
};

GBL_TEST_INIT()
    pFixture->refCount    = GblRef_activeCount();
    pFixture->testBoxType = GBL_TEST_BOX_TYPE;
    pFixture->pTestBox    = GBL_TEST_BOX(GblBox_create(GBL_TEST_BOX_TYPE));
    pFixture->pObject     = GBL_NEW(GblObject,
                                    "name",     "objy",
                                    "userdata", 0xdeadbeef);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_UNREF(pFixture->pObject);
    GBL_UNREF(pFixture->pTestBox);
    GblType_unregister(pFixture->testBoxType);
    GBL_TEST_COMPARE(pFixture->refCount, GblRef_activeCount());
GBL_TEST_CASE_END

GBL_TEST_CASE(incompatibleCount)
    GblVariant v;
    GblVariant_construct(&v, 12);

    GBL_TEST_COMPARE(GblVariant_count(&v), 0);

    GblVariant_destruct(&v);
GBL_TEST_CASE_END

GBL_TEST_CASE(incompatibleIndex)
    GblVariant t, k, v;
    GblVariant_construct(&t, 12);
    GblVariant_construct(&k, "key");
    GblVariant_construct(&v, 'a');

    GBL_TEST_COMPARE(GblVariant_index(&t, &k, &v), &v);
    GBL_TEST_VERIFY(GblVariant_isNil(&v));

    GblVariant_destruct(&v);
    GblVariant_destruct(&k);
    GblVariant_destruct(&t);
GBL_TEST_CASE_END

GBL_TEST_CASE(incompatibleSetIndexInvalid)
    GBL_TEST_EXPECT_ERROR();

    GblVariant t, k, v;
    GblVariant_construct(&t, 12);
    GblVariant_construct(&k, "key");
    GblVariant_construct(&v, 'a');

    GBL_TEST_COMPARE(GblVariant_setIndex(&t, &k, &v),
                     GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_CTX_CLEAR_LAST_RECORD();

    GblVariant_destruct(&v);
    GblVariant_destruct(&k);
    GblVariant_destruct(&t);
GBL_TEST_CASE_END

GBL_TEST_CASE(incompatibleNext)
    GblVariant t, k, v;
    GblVariant_construct(&t, 12);
    GblVariant_construct(&k);
    GblVariant_construct(&v, 'a');

    GBL_TEST_COMPARE(GblVariant_next(&t, &k, &v), GBL_FALSE);
    GBL_TEST_VERIFY(GblVariant_isNil(&v));
    GBL_TEST_VERIFY(GblVariant_isNil(&k));

    GblVariant_set(&v, 'a');
    GblVariant_set(&k, "key");

    GBL_TEST_COMPARE(GblVariant_next(&t, &k, &v), GBL_FALSE);
    GBL_TEST_VERIFY(GblVariant_isNil(&v));
    GBL_TEST_VERIFY(GblVariant_isNil(&k));

    GblVariant_destruct(&v);
    GblVariant_destruct(&k);
    GblVariant_destruct(&t);
GBL_TEST_CASE_END

GBL_TEST_CASE(boxCount)
    GblVariant t;

    GblVariant_constructBoxCopy(&t, GBL_BOX(pFixture->pTestBox));

    GBL_TEST_COMPARE(GblVariant_count(&t), 4);

    GblVariant_destruct(&t);
GBL_TEST_CASE_END

GBL_TEST_CASE(boxIndex)
    GblVariant t, k, v;

    GblVariant_constructBoxCopy(&t, GBL_BOX(pFixture->pTestBox));
    GblVariant_construct(&k);
    GblVariant_construct(&v);

    GblVariant_set(&k, "integer");
    GBL_TEST_COMPARE(GblVariant_index(&t, &k, &v), &v);
    GBL_TEST_COMPARE(GblVariant_toInt32(&v), -32);

    GblVariant_set(&k, "boolean");
    GBL_TEST_COMPARE(GblVariant_index(&t, &k, &v), &v);
    GBL_TEST_COMPARE(GblVariant_toInt32(&v), GBL_TRUE);

    GblVariant_set(&k, "number");
    GBL_TEST_COMPARE(GblVariant_index(&t, &k, &v), &v);
    GBL_TEST_COMPARE(GblVariant_toFloat(&v), 77.123f);

    GblVariant_set(&k, "string");
    GBL_TEST_COMPARE(GblVariant_index(&t, &k, &v), &v);
    GBL_TEST_COMPARE(GblVariant_toString(&v), "stringy");

    GblVariant_destruct(&v);
    GblVariant_destruct(&k);
    GblVariant_destruct(&t);
GBL_TEST_CASE_END

GBL_TEST_CASE(boxSetIndex)
    GblVariant t, k, v;

    // Construct table variant from our GblBox instance
    GblVariant_constructBoxCopy(&t, GBL_BOX(pFixture->pTestBox));
    // Default construct key variant
    GblVariant_construct(&k);
    // Default construct value variant
    GblVariant_construct(&v);

    // t["boolean"] = false
    // Direct assignment
    GblVariant_set(&k, "boolean");
    GblVariant_setBool(&v, GBL_FALSE);
    GBL_TEST_CALL(GblVariant_setIndex(&t, &k, &v));
    GBL_TEST_COMPARE(pFixture->pTestBox->boolean, GBL_FALSE);

    // t["integer"] = "12"
    // NOTE: "integer" is an int32, so it's automatically converting string => int32.
    GblVariant_set(&k, "integer");
    GblVariant_set(&v, "12");
    GBL_TEST_CALL(GblVariant_setIndex(&t, &k, &v));
    GBL_TEST_COMPARE(pFixture->pTestBox->integer, 12);

    // t["number"] = "7"
    // NOTE: "number" is a float, so it's automatically converting string => float.
    GblVariant_set(&k, "number");
    GblVariant_set(&v, "7");
    GBL_TEST_CALL(GblVariant_setIndex(&t, &k, &v));
    GBL_TEST_COMPARE(pFixture->pTestBox->number, 7.0f);

    // t["string"] = -1.123f
    // NOTE: Field is a string, so it's automatically converting float => string.
    GblVariant_set(&k, "string");
    GblVariant_setFloat(&v, -1.123f);
    GBL_TEST_CALL(GblVariant_setIndex(&t, &k, &v));
    GBL_TEST_COMPARE(pFixture->pTestBox->pString, "-1.123");

    // Clean up variants
    GblVariant_destruct(&v);
    GblVariant_destruct(&k);
    GblVariant_destruct(&t);
GBL_TEST_CASE_END

GBL_TEST_CASE(boxNext)
    GblVariant t, k, v;

    GblVariant_constructBoxCopy(&t, GBL_BOX(pFixture->pTestBox));
    GblVariant_construct(&k);
    GblVariant_construct(&v);

    GBL_TEST_VERIFY(GblVariant_next(&t, &k, &v));
    GBL_TEST_COMPARE(GblVariant_toString(&k), "integer");
    GBL_TEST_COMPARE(GblVariant_toInt32(&v), 12);

    GBL_TEST_VERIFY(GblVariant_next(&t, &k, &v));
    GBL_TEST_COMPARE(GblVariant_toString(&k), "boolean");
    GBL_TEST_COMPARE(GblVariant_toBool(&v), GBL_FALSE);

    GBL_TEST_VERIFY(GblVariant_next(&t, &k, &v));
    GBL_TEST_COMPARE(GblVariant_toString(&k), "number");
    GBL_TEST_COMPARE(GblVariant_toFloat(&v), 7.0f);

    GBL_TEST_VERIFY(GblVariant_next(&t, &k, &v));
    GBL_TEST_COMPARE(GblVariant_toString(&k), "string");
    GBL_TEST_COMPARE(GblVariant_toString(&v), "-1.123");

    GBL_TEST_VERIFY(!GblVariant_next(&t, &k, &v));
    GBL_TEST_VERIFY(GblVariant_isNil(&k));
    GBL_TEST_VERIFY(GblVariant_isNil(&v));

    GblVariant_destruct(&v);
    GblVariant_destruct(&k);
    GblVariant_destruct(&t);
GBL_TEST_CASE_END

GBL_TEST_CASE(objectCount)
    GblVariant t;

    GBL_TEST_SKIP("FIXME");

    GblVariant_constructObjectCopy(&t, pFixture->pObject);
    GBL_TEST_COMPARE(GblVariant_count(&t), 4);

    GblVariant_destruct(&t);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(incompatibleCount,
                  incompatibleIndex,
                  incompatibleSetIndexInvalid,
                  incompatibleNext,
                  boxCount,
                  boxIndex,
                  boxSetIndex,
                  boxNext,
                  objectCount)

static GBL_RESULT GblTestBox_ITableVariant_index_(const GblVariant* pVariant, const GblVariant* pKey, GblVariant* pValue) {
    GblVariant k = GBL_VARIANT_INIT;

    GBL_CTX_BEGIN(NULL);

    GblTestBox* pSelf  = GBL_TEST_BOX(GblVariant_getBoxPeek(pVariant));

    GblVariant_constructCopy(&k, pKey);
    const char* pField = GblVariant_toString(&k);

    if(!strcmp(pField, "integer"))
        GblVariant_setInt32(pValue, pSelf->integer);
    else if(!strcmp(pField, "boolean"))
        GblVariant_setBool(pValue, pSelf->boolean);
    else if(!strcmp(pField, "number"))
        GblVariant_setFloat(pValue, pSelf->number);
    else if(!strcmp(pField, "string"))
        GblVariant_setStringRef(pValue, GblStringRef_ref(pSelf->pString));
    else GBL_CTX_VERIFY(GBL_FALSE,
                        GBL_RESULT_ERROR_INVALID_KEY,
                        "Failed to index into field: %s[%s]",
                        GblVariant_typeName(pVariant),
                        pField);

    GBL_CTX_END_BLOCK();

    GblVariant_destruct(&k);
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblTestBox_ITableVariant_setIndex_(const GblVariant* pVariant, const GblVariant* pKey, GblVariant* pValue) {
    GblVariant k = GBL_VARIANT_INIT;

    GBL_CTX_BEGIN(NULL);

    GblTestBox* pSelf  = GBL_TEST_BOX(GblVariant_getBoxPeek(pVariant));

    GblVariant_constructCopy(&k, pKey);
    const char* pField = GblVariant_toString(&k);

    if(!strcmp(pField, "integer"))
        pSelf->integer = GblVariant_toInt32(pValue);
    else if(!strcmp(pField, "boolean"))
        pSelf->boolean = GblVariant_toBool(pValue);
    else if(!strcmp(pField, "number"))
        pSelf->number = GblVariant_toFloat(pValue);
    else if(!strcmp(pField, "string")) {
        GblVariant_toString(pValue);
        GblStringRef_unref(pSelf->pString);
        GblVariant_moveValue(pValue, &pSelf->pString);
    } else GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_INVALID_KEY,
                       "Failed to set unknown field: %s[%s] = %s",
                       GblVariant_typeName(pVariant),
                       GblVariant_toString(pValue),
                       pField);

    GBL_CTX_END_BLOCK();
    GblVariant_destruct(&k);
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblTestBox_ITableVariant_next_(const GblVariant* pVariant, GblVariant* pKey, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblTestBox* pSelf = GBL_TEST_BOX(GblVariant_getBoxPeek(pVariant));

    if(GblVariant_isNil(pKey)) {
        GblVariant_set(pKey, "integer");
        GblVariant_setInt32(pValue, pSelf->integer);
    } else {
        const char* pField = GblVariant_toString(pKey);

        if(!strcmp(pField, "integer")) {
            GblVariant_set(pKey, "boolean");
            GblVariant_setBool(pValue, pSelf->boolean);

        } else if(!strcmp(pField, "boolean")) {
            GblVariant_set(pKey, "number");
            GblVariant_setFloat(pValue, pSelf->number);

        } else if(!strcmp(pField, "number")) {
            GblVariant_set(pKey, "string");
            GblVariant_setStringRef(pValue, GblStringRef_ref(pSelf->pString));

        } else if(!strcmp(pField, "string")) {
            GblVariant_setNil(pKey);
            GblVariant_setNil(pValue);

        } else GBL_CTX_VERIFY(GBL_FALSE,
                              GBL_RESULT_ERROR_INVALID_KEY,
                              "Cannot get next from unknown field: %s[%s]",
                              GblVariant_typeName(pVariant),
                              pField);
    }

    GBL_CTX_END();
}

static GBL_RESULT GblTestBox_ITableVariant_count_(const GblVariant* pVariant, size_t* pSize) {
    GBL_UNUSED(pVariant);

    *pSize = 4;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblTestBox_Box_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);

    GblStringRef_unref(GBL_TEST_BOX(pBox)->pString);
    GBL_INSTANCE_VCALL_DEFAULT(GblBox, pFnDestructor, pBox);

    GBL_CTX_END();
}

static GBL_RESULT GblTestBox_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_UNUSED(pCtx);
    GBL_CTX_BEGIN(NULL);

    GblTestBox* pSelf = GBL_TEST_BOX(pInstance);

    pSelf->integer = -32;
    pSelf->boolean = GBL_TRUE;
    pSelf->number  = 77.123f;
    pSelf->pString = GblStringRef_create("stringy");

    GBL_CTX_END();
}

static GBL_RESULT GblTestBoxClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd, pCtx);

    GBL_BOX_CLASS(pClass)           ->pFnDestructor = GblTestBox_Box_destructor_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnIndex      = GblTestBox_ITableVariant_index_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnSetIndex   = GblTestBox_ITableVariant_setIndex_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnNext       = GblTestBox_ITableVariant_next_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnCount      = GblTestBox_ITableVariant_count_;

    return GBL_RESULT_SUCCESS;
}

static GblType GblTestBox_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInterfaceMapEntry ifaceEntry = {
        .classOffset = offsetof(GblTestBoxClass, GblITableVariantImpl)
    };

    static const GblTypeInfo info = {
        .classSize       = sizeof(GblTestBoxClass),
        .pFnClassInit    = GblTestBoxClass_init_,
        .instanceSize    = sizeof(GblTestBox),
        .pFnInstanceInit = GblTestBox_init_,
        .pInterfaceMap   = &ifaceEntry,
        .interfaceCount  = 1
    };

    if(type == GBL_INVALID_TYPE) {
        ifaceEntry.interfaceType = GBL_ITABLE_VARIANT_TYPE;

        type = GblType_registerStatic(GblQuark_internStringStatic("GblTestBox"),
                                      GBL_BOX_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);

    }

    return type;
}

static GblType GblTestObject_type(void) {
    return GBL_INVALID_TYPE;
}

