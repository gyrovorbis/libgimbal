#include "meta/types/gimbal_variant_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <string.h>

static GBL_RESULT GblVariantTestSuite_checkTypeCompatible_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblVariant_checkTypeCompatible(GBL_INVALID_TYPE));
    GBL_TEST_VERIFY(GblVariant_checkTypeCompatible(GBL_OBJECT_TYPE));
    GBL_TEST_VERIFY(!GblVariant_checkTypeCompatible(GBL_INTERFACE_TYPE));
    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_nil_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_VARIANT(variant);
    GBL_VARIANT(variant2);

    const GblRefCount nilRefCountBegin = GblType_classRefCount(GBL_NIL_TYPE);

    // Default initializer
    GBL_TEST_COMPARE(GblVariant_typeOf(&variant), GBL_INVALID_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&variant, GBL_NIL_TYPE));
    GBL_TEST_COMPARE(GblVariant_typeOf(&variant), GBL_NIL_TYPE);

    // Get Helper
    GBL_TEST_VERIFY(GblVariant_isNil(&variant));

    // Set Helper
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(&variant, GBL_BOOL_TYPE, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_setNil(&variant));
    GBL_TEST_VERIFY(GblVariant_isNil(&variant));

    // Copy constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&variant2, &variant));
    GBL_TEST_VERIFY(GblVariant_isNil(&variant2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant2));

    // Move constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&variant2, &variant));
    GBL_TEST_VERIFY(GblVariant_isNil(&variant2));

    // Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setCopy(&variant2, &variant));
    GBL_TEST_VERIFY(GblVariant_isNil(&variant2));

    // Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setMove(&variant2, &variant));
    GBL_TEST_VERIFY(GblVariant_isNil(&variant2));

    // Comparison
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &variant2));
    GBL_CTX_VERIFY_LAST_RECORD();

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant2));

    // Reference leak check
    GBL_TEST_COMPARE(nilRefCountBegin, GblType_classRefCount(GBL_NIL_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_nil_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    const GblRefCount nilRefCountBegin = GblType_classRefCount(GBL_NIL_TYPE);

    GBL_VARIANT(variant);
    GBL_TEST_COMPARE(GblVariant_constructValueCopy(&variant, GBL_NIL_TYPE, 0),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_VERIFY(!GblVariant_isValid(&variant));

    GBL_TEST_COMPARE(GblVariant_constructValueMove(&variant, GBL_NIL_TYPE, 0),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_VERIFY(!GblVariant_isValid(&variant));

    GblVariant_constructNil(&variant);

    GBL_TEST_COMPARE(GblVariant_setValueCopy(&variant, GBL_NIL_TYPE, 0),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_VERIFY(!GblVariant_isValid(&variant));

    GblVariant_constructNil(&variant);

    GBL_TEST_COMPARE(GblVariant_setValueMove(&variant, GBL_NIL_TYPE, 0),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_VERIFY(!GblVariant_isValid(&variant));

    uintptr_t value;

    GblVariant_constructNil(&variant);
    GBL_TEST_COMPARE(GblVariant_copyValue(&variant, &value),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblVariant_moveValue(&variant, &value),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblVariant_peekValue(&variant, &value),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_CTX_CLEAR_LAST_RECORD();

    GblVariant_destruct(&variant);

    // Reference leak check
    GBL_TEST_COMPARE(nilRefCountBegin, GblType_classRefCount(GBL_NIL_TYPE));
    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_nil_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(nilVariant);
    GBL_VARIANT(toVariant);

    // Constructor
    const GblRefCount nilRefCountBegin = GblType_classRefCount(GBL_NIL_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_constructNil(&nilVariant));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&toVariant, GBL_TRUE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&toVariant), GBL_FALSE);
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Char
    GBL_CTX_VERIFY_CALL(GblVariant_setChar(&toVariant, 'a'));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&toVariant), '\0');
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&toVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&toVariant), 0);
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&toVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&toVariant), 0);
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&toVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&toVariant), 0);
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&toVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&toVariant), 0);
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&toVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&toVariant), 0);
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&toVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&toVariant), 0);
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // INT64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&toVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&toVariant), 0);
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Pointer
    GBL_CTX_VERIFY_CALL(GblVariant_setPointer(&toVariant, GBL_POINTER_TYPE, (void*)0xdeadbeef));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_COMPARE(GblVariant_getPointer(&toVariant), NULL);
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&toVariant, "lolol"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&toVariant), GBL_STRV("nil")));
    GBL_TEST_VERIFY(GblVariant_equals(&nilVariant, &toVariant));
    GBL_CTX_VERIFY_LAST_RECORD();

    // Destructors
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&toVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&nilVariant));

    // Reference leak check
    GBL_TEST_COMPARE(nilRefCountBegin, GblType_classRefCount(GBL_NIL_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_bool_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_BOOL_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_BOOL_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_BOOL_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&v, GBL_TRUE));
    GBL_TEST_COMPARE(GblVariant_getBool(&v), GBL_TRUE); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_BOOL_TYPE, GBL_TRUE));
    GBL_TEST_COMPARE(GblVariant_getBool(&v), GBL_TRUE);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setBool(&v, GBL_FALSE));
    GBL_TEST_COMPARE(GblVariant_getBool(&v), GBL_FALSE);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_BOOL_TYPE, GBL_TRUE));
    GBL_TEST_COMPARE(GblVariant_getBool(&v), GBL_TRUE);

    // Value Get Move
    GblBool boolVal = GBL_FALSE;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &boolVal));
    GBL_TEST_COMPARE(boolVal, GBL_TRUE);

    // Value Get Peek
    GblVariant_setBool(&v, GBL_FALSE);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &boolVal));
    GBL_TEST_COMPARE(boolVal, GBL_FALSE);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_BOOL_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_bool_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(bVariant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_BOOL_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&bVariant, GBL_TRUE));

    // Char
    GBL_CTX_VERIFY_CALL(GblVariant_constructChar(&tVariant, '\0'));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&tVariant), 1);
    GBL_TEST_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 1);
    GBL_TEST_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 1);
    GBL_TEST_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 1);
    GBL_TEST_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 1);
    GBL_TEST_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 1);
    GBL_TEST_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 1);
    GBL_TEST_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 1);
    GBL_TEST_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("true")));
    GBL_TEST_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&bVariant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_BOOL_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_char_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_CHAR_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_CHAR_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_CHAR_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructChar(&v, 'c'));
    GBL_TEST_COMPARE(GblVariant_getChar(&v), 'c'); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_CHAR_TYPE, 'w'));
    GBL_TEST_COMPARE(GblVariant_getChar(&v), 'w');

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setChar(&v, 'q'));
    GBL_TEST_COMPARE(GblVariant_getChar(&v), 'q');

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_CHAR_TYPE, 'p'));
    GBL_TEST_COMPARE(GblVariant_getChar(&v), 'p');

    // Value Get Move
    char val = '\0';
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, 'p');

    // Value Get Peek
    GblVariant_setChar(&v, 't');
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, 't');

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_CHAR_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_char_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_CHAR_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructChar(&variant, 'a'));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("a")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_CHAR_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_u8_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT8_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_UINT8_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_UINT8_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructUint8(&v, 17));
    GBL_TEST_COMPARE(GblVariant_getUint8(&v), 17); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_UINT8_TYPE, 18));
    GBL_TEST_COMPARE(GblVariant_getUint8(&v), 18);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&v, 19));
    GBL_TEST_COMPARE(GblVariant_getUint8(&v), 19);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_UINT8_TYPE, 20));
    GBL_TEST_COMPARE(GblVariant_getUint8(&v), 20);

    // Value Get Move
    uint8_t val = 0;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setUint8(&v, 21);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, 21);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT8_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_u8_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT8_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructUint8(&variant, 97));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_CTX_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_CTX_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_CTX_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT8_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_u16_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT16_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_UINT16_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_UINT16_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructUint16(&v, 17));
    GBL_TEST_COMPARE(GblVariant_getUint16(&v), 17); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_UINT16_TYPE, 18));
    GBL_TEST_COMPARE(GblVariant_getUint16(&v), 18);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&v, 19));
    GBL_TEST_COMPARE(GblVariant_getUint16(&v), 19);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_UINT16_TYPE, 20));
    GBL_TEST_COMPARE(GblVariant_getUint16(&v), 20);

    // Value Get Move
    uint16_t val = 0;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setUint16(&v, 21);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, 21);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT16_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_u16_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT16_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructUint16(&variant, 97));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_CTX_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_CTX_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_CTX_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT16_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_i16_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT16_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_INT16_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_INT16_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructInt16(&v, 17));
    GBL_TEST_COMPARE(GblVariant_getInt16(&v), 17); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_INT16_TYPE, 18));
    GBL_TEST_COMPARE(GblVariant_getInt16(&v), 18);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&v, 19));
    GBL_TEST_COMPARE(GblVariant_getInt16(&v), 19);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_INT16_TYPE, 20));
    GBL_TEST_COMPARE(GblVariant_getInt16(&v), 20);

    // Value Get Move
    int16_t val = 0;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setInt16(&v, 21);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, 21);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT16_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_i16_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT16_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructInt16(&variant, 97));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_CTX_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_CTX_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_CTX_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT16_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_u32_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT32_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_UINT32_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_UINT32_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructUint32(&v, 17));
    GBL_TEST_COMPARE(GblVariant_getUint32(&v), 17); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_UINT32_TYPE, 18));
    GBL_TEST_COMPARE(GblVariant_getUint32(&v), 18);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&v, 19));
    GBL_TEST_COMPARE(GblVariant_getUint32(&v), 19);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_UINT32_TYPE, 20));
    GBL_TEST_COMPARE(GblVariant_getUint32(&v), 20);

    // Value Get Move
    uint32_t val = 0;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setUint32(&v, 21);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, 21);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT32_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_u32_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT32_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructUint32(&variant, 97));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_CTX_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_CTX_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_CTX_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT32_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_i32_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT32_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_INT32_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_INT32_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructInt32(&v, 17));
    GBL_TEST_COMPARE(GblVariant_getInt32(&v), 17); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_INT32_TYPE, 18));
    GBL_TEST_COMPARE(GblVariant_getInt32(&v), 18);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&v, 19));
    GBL_TEST_COMPARE(GblVariant_getInt32(&v), 19);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_INT32_TYPE, 20));
    GBL_TEST_COMPARE(GblVariant_getInt32(&v), 20);

    // Value Get Move
    int32_t val = 0;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setInt32(&v, 21);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, 21);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT32_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_i32_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT32_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructInt32(&variant, 97));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_CTX_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_CTX_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_CTX_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT32_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_u64_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT64_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_UINT64_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_UINT64_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructUint64(&v, 17));
    GBL_TEST_COMPARE(GblVariant_getUint64(&v), 17); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    uint64_t value = 18;
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_UINT64_TYPE, (uint64_t)18));
    GBL_TEST_COMPARE(GblVariant_getUint64(&v), 18);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&v, 19));
    GBL_TEST_COMPARE(GblVariant_getUint64(&v), 19);

    // Value Set Move
    value = 20;
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_UINT64_TYPE, value));
    GBL_TEST_COMPARE(GblVariant_getUint64(&v), 20);

    // Value Get Move
    uint64_t val = 0;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setUint64(&v, 21);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, 21);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT64_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_u64_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT64_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructUint64(&variant, 97));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_CTX_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_CTX_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_CTX_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT64_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_i64_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT64_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_INT64_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_INT64_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructInt64(&v, 17));
    GBL_TEST_COMPARE(GblVariant_getInt64(&v), 17); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_INT64_TYPE, (int64_t)18));
    GBL_TEST_COMPARE(GblVariant_getInt64(&v), 18);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&v, 19));
    GBL_TEST_COMPARE(GblVariant_getInt64(&v), 19);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_INT64_TYPE, (int64_t)20));
    GBL_TEST_COMPARE(GblVariant_getInt64(&v), 20);

    // Value Get Move
    int64_t val = 0;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setInt64(&v, 21);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, 21);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT64_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_i64_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT64_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructInt64(&variant, 97));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_CTX_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_CTX_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_CTX_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT64_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_f32_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_FLOAT_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_FLOAT_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_FLOAT_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructFloat(&v, 17.0f));
    GBL_TEST_COMPARE(GblVariant_getFloat(&v), 17.0f); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_FLOAT_TYPE, 18.0f));
    GBL_TEST_COMPARE(GblVariant_getFloat(&v), 18.0f);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setFloat(&v, 19.0f));
    GBL_TEST_COMPARE(GblVariant_getFloat(&v), 19.0f);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_FLOAT_TYPE, 20.0f));
    GBL_TEST_COMPARE(GblVariant_getFloat(&v), 20.0f);

    // Value Get Move
    float val = 0.0f;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, 20.0f);

    // Value Get Peek
    GblVariant_setFloat(&v, 21.0f);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, 21.0f);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_FLOAT_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_f32_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_FLOAT_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructFloat(&variant, 97.0f));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_CTX_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97.000")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_FLOAT_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_f64_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_DOUBLE_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_DOUBLE_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_DOUBLE_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDouble(&v, 17.0));
    GBL_TEST_COMPARE(GblVariant_getDouble(&v), 17.0); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_DOUBLE_TYPE, 18.0));
    GBL_TEST_COMPARE(GblVariant_getDouble(&v), 18.0);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setDouble(&v, 19.0));
    GBL_TEST_COMPARE(GblVariant_getDouble(&v), 19.0);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_DOUBLE_TYPE, 20.0));
    GBL_TEST_COMPARE(GblVariant_getDouble(&v), 20.0);

    // Value Get Move
    double val = 0.0;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, 20.0);

    // Value Get Peek
    GblVariant_setDouble(&v, 21.0);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, 21.0);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_DOUBLE_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_f64_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_DOUBLE_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructDouble(&variant, 97.0));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_CTX_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97.000")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_DOUBLE_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_ptr_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_POINTER_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_POINTER_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_POINTER_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructPointer(&v, GBL_POINTER_TYPE, (void*)0xdeadbeef));
    GBL_TEST_COMPARE(GblVariant_getPointer(&v), (void*)0xdeadbeef); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_POINTER_TYPE, (void*)0xdeadbeaf));
    GBL_TEST_COMPARE(GblVariant_getPointer(&v), (void*)0xdeadbeaf);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setPointer(&v, GBL_POINTER_TYPE, (void*)0xaeadbeaf));
    GBL_TEST_COMPARE(GblVariant_getPointer(&v), (void*)0xaeadbeaf);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_POINTER_TYPE, (void*)0xcafebabe));
    GBL_TEST_COMPARE(GblVariant_getPointer(&v), (void*)0xcafebabe);

    // Value Get Move
    void* pVal = NULL;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &pVal));
    GBL_TEST_COMPARE(pVal, (void*)0xcafebabe);

    // Value Get Peek
    GblVariant_setPointer(&v, GBL_POINTER_TYPE, (void*)0xcafebeef);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &pVal));
    GBL_TEST_COMPARE(pVal, (void*)0xcafebeef);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_POINTER_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_ptr_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_POINTER_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructPointer(&variant, GBL_POINTER_TYPE, (void*)0xdeadbeef));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));

    //GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("0xdeadbeef")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_POINTER_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_string_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_STRING_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_STRING_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_STRING_TYPE);

    // Copy Constructor (arguably this is actually wrong as fuck!!!!)
    // if it weren't empty, it would become invalidated!!
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructString(&v, "lolz"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getString(&v), "lolz"), 0); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_COMPARE(strcmp(GblVariant_getString(&v2), "lolz"), 0);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Value Move Constructor
    GblStringRef* pRef = GblStringRef_create("xbox");
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_STRING_TYPE, pRef));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&v), GBL_STRV("xbox")));

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&v, "dreamcast"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getString(&v), "dreamcast"), 0);

    // Value Set Move
    pRef = GblStringRef_create("playstation");
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_STRING_TYPE, pRef));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&v), GBL_STRV("playstation")));

    // Value Get Move
    const char* pVal = NULL;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &pVal));
    GBL_TEST_VERIFY(GblStringView_equals(GBL_STRV(pVal), GBL_STRV("playstation")));
    GblStringRef_release(pVal);

    // Value Get Peek
    GblVariant_setString(&v, "gamecube");
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &pVal));
    GBL_TEST_COMPARE(strcmp(pVal, "gamecube"), 0);

    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_STRING_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_string_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_STRING_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructString(&variant, "97"));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_CTX_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_CTX_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_CTX_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Type
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&variant, "bool"));
    GBL_CTX_VERIFY_CALL(GblVariant_setTypeValue(&tVariant, GBL_INVALID_TYPE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getTypeValue(&tVariant), GBL_BOOL_TYPE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Pointer
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&variant, "pointer"));
    GBL_CTX_VERIFY_CALL(GblVariant_setPointer(&tVariant, GBL_POINTER_TYPE, NULL));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE((const char*)GblVariant_getPointer(&tVariant), "pointer");
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_STRING_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_string_conversions_char_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_STRING_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructString(&variant, "9"));

    // Length == 1
    GBL_CTX_VERIFY_CALL(GblVariant_constructChar(&tVariant, '\0'));
    GBL_TEST_VERIFY(GblVariant_canConvert(GBL_STRING_TYPE, GBL_CHAR_TYPE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&tVariant), '9');
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Length == 0
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&variant, ""));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getChar(&tVariant), '\0');
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Length > 1 (Invalid)
    GBL_TEST_EXPECT_ERROR();

    GBL_CTX_VERIFY_CALL(GblVariant_setString(&variant, "abc"));
    GBL_TEST_COMPARE(GblVariant_convert(&variant, &tVariant),
                GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_CLEAR_LAST_RECORD();


    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_STRING_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_type_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_TYPE_TYPE);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_TYPE_TYPE));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == GBL_TYPE_TYPE);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructTypeValue(&v, GBL_NIL_TYPE));
    GBL_TEST_COMPARE(GblVariant_getTypeValue(&v), GBL_NIL_TYPE); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_TYPE_TYPE, GBL_STRING_TYPE));
    GBL_TEST_COMPARE(GblVariant_getTypeValue(&v), GBL_STRING_TYPE);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setTypeValue(&v, GBL_BOOL_TYPE));
    GBL_TEST_COMPARE(GblVariant_getTypeValue(&v), GBL_BOOL_TYPE);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_TYPE_TYPE, GBL_CHAR_TYPE));
    GBL_TEST_COMPARE(GblVariant_getTypeValue(&v), GBL_CHAR_TYPE);

    // Value Get Move
    GblType val = GBL_INVALID_TYPE;
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(&v, &val));
    GBL_TEST_COMPARE(val, GBL_CHAR_TYPE);

    // Value Get Peek
    GblVariant_setTypeValue(&v, GBL_TYPE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(&v, &val));
    GBL_TEST_COMPARE(val, GBL_TYPE_TYPE);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_TYPE_TYPE));

    GBL_CTX_END();
}


static GBL_RESULT GblVariantTestSuite_type_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_TYPE_TYPE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructTypeValue(&variant, GBL_UINT8_TYPE));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, ""));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("uint8")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(GBL_TYPE_TYPE));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_construct_generic_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblVariant v = GBL_VARIANT_INIT;

    //nil
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_NIL_TYPE);
    GBL_TEST_VERIFY(GblVariant_isNil(&v));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    //char
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, (char)'a'));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_CHAR_TYPE);
    GBL_TEST_COMPARE(GblVariant_getChar(&v), 'a');
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // uint8
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, (uint8_t)1));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_UINT8_TYPE);
    GBL_TEST_COMPARE(GblVariant_getUint8(&v), 1);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // uint16
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, (uint16_t)2));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_UINT16_TYPE);
    GBL_TEST_COMPARE(GblVariant_getUint16(&v), 2);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // int16
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, (int16_t)-2));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_INT16_TYPE);
    GBL_TEST_COMPARE(GblVariant_getInt16(&v), -2);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // uint32
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, (uint32_t)3));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_UINT32_TYPE);
    GBL_TEST_COMPARE(GblVariant_getUint32(&v), 3);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // int32
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, (int32_t)-3));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_INT32_TYPE);
    GBL_TEST_COMPARE(GblVariant_getInt32(&v), -3);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // uint64
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, (uint64_t)4));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_UINT64_TYPE);
    GBL_TEST_COMPARE(GblVariant_getUint64(&v), 4);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // int64
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, (int64_t)-4));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_INT64_TYPE);
    GBL_TEST_COMPARE(GblVariant_getInt64(&v), -4);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // float
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, 33.33f));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_FLOAT_TYPE);
    GBL_TEST_COMPARE(GblVariant_getFloat(&v), 33.33f);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // double
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, -33.33));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_DOUBLE_TYPE);
    GBL_TEST_COMPARE(GblVariant_getDouble(&v), -33.33);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // string
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, (const char*)"literal"));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_STRING_TYPE);
    GBL_TEST_COMPARE(GblVariant_getString(&v), "literal");
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // string view
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, GBL_STRV("view")));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_STRING_TYPE);
    GBL_TEST_COMPARE(GblVariant_getString(&v), "view");
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
#if 0
    // pointer
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, GBL_POINTER_TYPE, (void*)0xdeadbabe));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_POINTER_TYPE);
    GBL_TEST_COMPARE(GblVariant_getPointer(&v), (void*)0xdeadbabe);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
#endif
    // copy
    GblVariant v2 = GBL_VARIANT_INIT;
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v2, GBL_STRV("copy constructor")));
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, &v2));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_STRING_TYPE);
    GBL_TEST_COMPARE(GblVariant_getString(&v), "copy constructor");
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_set_generic_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblVariant v = GBL_VARIANT_INIT;

    //char
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (char)'a'));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_CHAR_TYPE);
    GBL_TEST_COMPARE(GblVariant_getChar(&v), 'a');

    // uint8
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (uint8_t)1));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_UINT8_TYPE);
    GBL_TEST_COMPARE(GblVariant_getUint8(&v), 1);

    // uint16
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (uint16_t)2));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_UINT16_TYPE);
    GBL_TEST_COMPARE(GblVariant_getUint16(&v), 2);

    // int16
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (int16_t)-2));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_INT16_TYPE);
    GBL_TEST_COMPARE(GblVariant_getInt16(&v), -2);

    // uint32
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (uint32_t)3));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_UINT32_TYPE);
    GBL_TEST_COMPARE(GblVariant_getUint32(&v), 3);

    // int32
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (int32_t)-3));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_INT32_TYPE);
    GBL_TEST_COMPARE(GblVariant_getInt32(&v), -3);

    // uint64
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (uint64_t)4));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_UINT64_TYPE);
    GBL_TEST_COMPARE(GblVariant_getUint64(&v), 4);

    // int64
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (int64_t)-4));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_INT64_TYPE);
    GBL_TEST_COMPARE(GblVariant_getInt64(&v), -4);

    // float
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, 33.33f));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_FLOAT_TYPE);
    GBL_TEST_COMPARE(GblVariant_getFloat(&v), 33.33f);

    // double
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, -33.33));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_DOUBLE_TYPE);
    GBL_TEST_COMPARE(GblVariant_getDouble(&v), -33.33);

    // string
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (const char*)"literal"));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_STRING_TYPE);
    GBL_TEST_COMPARE(GblVariant_getString(&v), "literal");

    // string view
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, GBL_STRV("view")));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_STRING_TYPE);
    GBL_TEST_COMPARE(GblVariant_getString(&v), "view");
#if 0
    // pointer
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (void*)0xdeadbabe));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_POINTER_TYPE);
    GBL_TEST_COMPARE(GblVariant_getPointer(&v), (void*)0xdeadbabe);
#endif
    // copy
    GblVariant v2 = GBL_VARIANT_INIT;
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v2, GBL_STRV("assignment operator")));
    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, &v2));
    GBL_TEST_COMPARE(GblVariant_typeOf(&v), GBL_STRING_TYPE);
    GBL_TEST_COMPARE(GblVariant_getString(&v), "assignment operator");
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_typeName_(GblTestSuite* pSuite, GblContext* pCtx) {
    GBL_UNUSED(pSuite);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;

    //GBL_TEST_COMPARE(GblVariant_typeName(&v), "invalid");

    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, (char)'b'));
    GBL_TEST_COMPARE(GblVariant_typeName(&v), "char");

    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, 12.0f));
    GBL_TEST_COMPARE(GblVariant_typeName(&v), "float");

    GblVariant_destruct(&v);

    GBL_CTX_END();
}

static GBL_RESULT GblVariantTestSuite_to_invalid_(GblTestSuite* pSuite, GblContext* pCtx) {
    GBL_UNUSED(pSuite);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GblVariant v = GBL_VARIANT_INIT;

    const int integer = GblVariant_toInt32(&v);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_TYPE);
    GBL_TEST_COMPARE(integer, 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_VERIFY_CALL(GblVariant_set(&v, 33.0f));
    const GblType type = GblVariant_toTypeValue(&v);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_TEST_COMPARE(type, GBL_INVALID_TYPE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    GBL_CTX_END();
}

// check for class reference leakage?
static GBL_RESULT GblVariantTestSuite_to_(GblTestSuite* pSuite, GblContext* pCtx) {
    GBL_UNUSED(pSuite);
    GBL_CTX_BEGIN(pCtx);

    GblVariant v;
    GBL_CTX_VERIFY_CALL(GblVariant_construct(&v, (uint8_t)7));
    GBL_TEST_COMPARE(GblVariant_toChar(&v),      (char)7);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toUint8(&v),     (uint8_t)7);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toUint16(&v),    (uint16_t)7);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toInt16(&v),     (int16_t)7);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toUint32(&v),    (uint32_t)7);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toInt32(&v),     (int32_t)7);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toUint64(&v),    (uint64_t)7);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toInt64(&v),     (int64_t)7);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toUint64(&v),    (uint64_t)7);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toFloat(&v),     7.0f);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toDouble(&v),    7.0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toBool(&v),      1);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblVariant_toString(&v),    "true");
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GBL_STRING_VIEW_CSTR(GblVariant_toStringView(&v)), "true");
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    GBL_CTX_END();
}


GBL_EXPORT GblType GblVariantTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "checkTypeCompatible",    GblVariantTestSuite_checkTypeCompatible_        },
        { "nil",                    GblVariantTestSuite_nil_                        },
        { "nilInvalid",             GblVariantTestSuite_nil_invalid_                },
        { "nilConversions",         GblVariantTestSuite_nil_conversions_            },
        { "bool",                   GblVariantTestSuite_bool_                       },
        { "boolConversions",        GblVariantTestSuite_bool_conversions_           },
        { "char",                   GblVariantTestSuite_char_                       },
        { "charConversions",        GblVariantTestSuite_char_conversions_           },
        { "uint8",                  GblVariantTestSuite_u8_                         },
        { "uint8Conversions",       GblVariantTestSuite_u8_conversions_             },
        { "uint16",                 GblVariantTestSuite_u16_                        },
        { "uint16Conversions",      GblVariantTestSuite_u16_conversions_            },
        { "int16",                  GblVariantTestSuite_i16_                        },
        { "int16Conversions",       GblVariantTestSuite_i16_conversions_            },
        { "uint32",                 GblVariantTestSuite_u32_                        },
        { "uint32Conversions",      GblVariantTestSuite_u32_conversions_            },
        { "int32",                  GblVariantTestSuite_i32_                        },
        { "int32Conversions",       GblVariantTestSuite_i32_conversions_            },
        { "uint64",                 GblVariantTestSuite_u64_                        },
        { "uint64Conversions",      GblVariantTestSuite_u64_conversions_            },
        { "int64",                  GblVariantTestSuite_i64_                        },
        { "int64Conversions",       GblVariantTestSuite_i64_conversions_            },
        { "float",                  GblVariantTestSuite_f32_                        },
        { "floatConversions",       GblVariantTestSuite_f32_conversions_            },
        { "double",                 GblVariantTestSuite_f64_                        },
        { "doubleConversions",      GblVariantTestSuite_f64_conversions_            },
        { "pointer",                GblVariantTestSuite_ptr_                        },
        { "pointerConversions",     GblVariantTestSuite_ptr_conversions_            },
        { "string",                 GblVariantTestSuite_string_                     },
        { "stringConversions",      GblVariantTestSuite_string_conversions_         },
        { "stringConversionsChar",  GblVariantTestSuite_string_conversions_char_    },
        { "type",                   GblVariantTestSuite_type_                       },
        { "typeConversions",        GblVariantTestSuite_type_conversions_           },
        { "constructGeneric",       GblVariantTestSuite_construct_generic_          },
        { "setGeneric",             GblVariantTestSuite_set_generic_                },
        { "typeName",               GblVariantTestSuite_typeName_                   },
        { "toInvalid",              GblVariantTestSuite_to_invalid_                 },
        { "to",                     GblVariantTestSuite_to_                         },
        { NULL,                     NULL                                            }
    };

    const static GblTestSuiteVTable vTable = {
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("VariantTestSuite"),
                                     &vTable,
                                     sizeof(GblVariantTestSuite),
                                     0,
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}


