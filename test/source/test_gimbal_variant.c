#include "test_gimbal_variant.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/meta/gimbal_variant.h>
#include <string.h>

GBL_API GblVariant_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}
GBL_API GblVariant_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}
GBL_API GblVariant_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblVariant_test_nil(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VARIANT(variant);
    GBL_VARIANT(variant2);

    const GblRefCount nilRefCountBegin = GblType_classRefCount(GBL_NIL_TYPE);

    // Default initializer
    GBL_COMPARE(GblVariant_type(&variant), GBL_INVALID_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&variant, GBL_NIL_TYPE));
    GBL_COMPARE(GblVariant_type(&variant), GBL_NIL_TYPE);

    // Get Helper
    GBL_VERIFY(GblVariant_isNil(&variant));

    // Set Helper
    GBL_API_VERIFY_CALL(GblVariant_setValueCopy(&variant, GBL_BOOL_TYPE, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_setNil(&variant));
    GBL_VERIFY(GblVariant_isNil(&variant));

    // Copy constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&variant2, &variant));
    GBL_VERIFY(GblVariant_isNil(&variant2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant2));

    // Move constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&variant2, &variant));
    GBL_VERIFY(GblVariant_isNil(&variant2));

    // Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setCopy(&variant2, &variant));
    GBL_VERIFY(GblVariant_isNil(&variant2));

    // Set Move
    GBL_API_VERIFY_CALL(GblVariant_setMove(&variant2, &variant));
    GBL_VERIFY(GblVariant_isNil(&variant2));

    // Comparison
    GBL_VERIFY(GblVariant_equals(&variant, &variant2));
    GBL_API_VERIFY_LAST_RECORD();

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant2));

    // Reference leak check
    GBL_COMPARE(nilRefCountBegin, GblType_classRefCount(GBL_NIL_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_nil_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    const GblRefCount nilRefCountBegin = GblType_classRefCount(GBL_NIL_TYPE);

    GBL_VARIANT(variant);
    GBL_COMPARE(GblVariant_constructValueCopy(&variant, GBL_NIL_TYPE, 0),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_VERIFY(!GblVariant_isValid(&variant));

    GBL_COMPARE(GblVariant_constructValueMove(&variant, GBL_NIL_TYPE, 0),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_VERIFY(!GblVariant_isValid(&variant));

    GblVariant_constructNil(&variant);

    GBL_COMPARE(GblVariant_setValueCopy(&variant, GBL_NIL_TYPE, 0),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_VERIFY(!GblVariant_isValid(&variant));

    GblVariant_constructNil(&variant);

    GBL_COMPARE(GblVariant_setValueMove(&variant, GBL_NIL_TYPE, 0),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_VERIFY(!GblVariant_isValid(&variant));

    uintptr_t value;

    GblVariant_constructNil(&variant);
    GBL_COMPARE(GblVariant_getValueCopy(&variant, &value),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_COMPARE(GblVariant_getValueMove(&variant, &value),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_COMPARE(GblVariant_getValuePeek(&variant, &value),
                GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_API_CLEAR_LAST_RECORD();

    GblVariant_destruct(&variant);

    // Reference leak check
    GBL_COMPARE(nilRefCountBegin, GblType_classRefCount(GBL_NIL_TYPE));
    GBL_API_END();
}

GBL_API GblVariant_test_nil_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(nilVariant);
    GBL_VARIANT(toVariant);

    // Constructor
    const GblRefCount nilRefCountBegin = GblType_classRefCount(GBL_NIL_TYPE);
    GBL_API_VERIFY_CALL(GblVariant_constructNil(&nilVariant));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&toVariant, GBL_TRUE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_COMPARE(GblVariant_getBool(&toVariant), GBL_FALSE);
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Char
    GBL_API_VERIFY_CALL(GblVariant_setChar(&toVariant, 'a'));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_COMPARE(GblVariant_getChar(&toVariant), '\0');
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&toVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_COMPARE(GblVariant_getUint8(&toVariant), 0);
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&toVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_COMPARE(GblVariant_getUint16(&toVariant), 0);
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&toVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_COMPARE(GblVariant_getInt16(&toVariant), 0);
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&toVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_COMPARE(GblVariant_getUint32(&toVariant), 0);
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&toVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_COMPARE(GblVariant_getInt32(&toVariant), 0);
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&toVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_COMPARE(GblVariant_getUint64(&toVariant), 0);
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // INT64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&toVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_COMPARE(GblVariant_getInt64(&toVariant), 0);
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // Pointer
    GBL_API_VERIFY_CALL(GblVariant_setPointer(&toVariant, (void*)0xdeadbeef));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_COMPARE(GblVariant_getPointer(&toVariant), NULL);
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&toVariant, "lolol"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&nilVariant, &toVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&toVariant), GBL_STRV("nil")));
    GBL_VERIFY(GblVariant_equals(&nilVariant, &toVariant));
    GBL_API_VERIFY_LAST_RECORD();

    // Destructors
    GBL_API_VERIFY_CALL(GblVariant_destruct(&toVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&nilVariant));

    // Reference leak check
    GBL_COMPARE(nilRefCountBegin, GblType_classRefCount(GBL_NIL_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_bool(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_BOOL_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_BOOL_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_BOOL_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&v, GBL_TRUE));
    GBL_COMPARE(GblVariant_getBool(&v), GBL_TRUE); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_BOOL_TYPE, GBL_TRUE));
    GBL_COMPARE(GblVariant_getBool(&v), GBL_TRUE);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setBool(&v, GBL_FALSE));
    GBL_COMPARE(GblVariant_getBool(&v), GBL_FALSE);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_BOOL_TYPE, GBL_TRUE));
    GBL_COMPARE(GblVariant_getBool(&v), GBL_TRUE);

    // Value Get Move
    GblBool boolVal = GBL_FALSE;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &boolVal));
    GBL_COMPARE(boolVal, GBL_TRUE);

    // Value Get Peek
    GblVariant_setBool(&v, GBL_FALSE);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &boolVal));
    GBL_COMPARE(boolVal, GBL_FALSE);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_BOOL_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_bool_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(bVariant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_BOOL_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructBool(&bVariant, GBL_TRUE));

    // Char
    GBL_API_VERIFY_CALL(GblVariant_constructChar(&tVariant, '\0'));
    GBL_API_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_COMPARE(GblVariant_getChar(&tVariant), 1);
    GBL_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 1);
    GBL_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 1);
    GBL_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 1);
    GBL_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 1);
    GBL_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 1);
    GBL_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 1);
    GBL_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 1);
    GBL_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&bVariant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("true")));
    GBL_VERIFY(GblVariant_equals(&bVariant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&bVariant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_BOOL_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_char(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_CHAR_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_CHAR_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_CHAR_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructChar(&v, 'c'));
    GBL_COMPARE(GblVariant_getChar(&v), 'c'); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_CHAR_TYPE, 'w'));
    GBL_COMPARE(GblVariant_getChar(&v), 'w');

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setChar(&v, 'q'));
    GBL_COMPARE(GblVariant_getChar(&v), 'q');

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_CHAR_TYPE, 'p'));
    GBL_COMPARE(GblVariant_getChar(&v), 'p');

    // Value Get Move
    char val = '\0';
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, 'p');

    // Value Get Peek
    GblVariant_setChar(&v, 't');
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, 't');

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_CHAR_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_char_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_CHAR_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructChar(&variant, 'a'));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("a")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_CHAR_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_u8(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT8_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_UINT8_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_UINT8_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructUint8(&v, 17));
    GBL_COMPARE(GblVariant_getUint8(&v), 17); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_UINT8_TYPE, 18));
    GBL_COMPARE(GblVariant_getUint8(&v), 18);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&v, 19));
    GBL_COMPARE(GblVariant_getUint8(&v), 19);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_UINT8_TYPE, 20));
    GBL_COMPARE(GblVariant_getUint8(&v), 20);

    // Value Get Move
    uint8_t val = 0;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setUint8(&v, 21);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, 21);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT8_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_u8_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT8_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructUint8(&variant, 97));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_API_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_API_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_API_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT8_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_u16(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT16_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_UINT16_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_UINT16_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructUint16(&v, 17));
    GBL_COMPARE(GblVariant_getUint16(&v), 17); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_UINT16_TYPE, 18));
    GBL_COMPARE(GblVariant_getUint16(&v), 18);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&v, 19));
    GBL_COMPARE(GblVariant_getUint16(&v), 19);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_UINT16_TYPE, 20));
    GBL_COMPARE(GblVariant_getUint16(&v), 20);

    // Value Get Move
    uint16_t val = 0;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setUint16(&v, 21);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, 21);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT16_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_u16_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT16_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructUint16(&variant, 97));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_API_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_API_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_API_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT16_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_i16(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT16_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_INT16_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_INT16_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructInt16(&v, 17));
    GBL_COMPARE(GblVariant_getInt16(&v), 17); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_INT16_TYPE, 18));
    GBL_COMPARE(GblVariant_getInt16(&v), 18);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&v, 19));
    GBL_COMPARE(GblVariant_getInt16(&v), 19);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_INT16_TYPE, 20));
    GBL_COMPARE(GblVariant_getInt16(&v), 20);

    // Value Get Move
    int16_t val = 0;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setInt16(&v, 21);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, 21);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT16_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_i16_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT16_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructInt16(&variant, 97));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_API_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_API_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_API_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT16_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_u32(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT32_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_UINT32_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_UINT32_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructUint32(&v, 17));
    GBL_COMPARE(GblVariant_getUint32(&v), 17); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_UINT32_TYPE, 18));
    GBL_COMPARE(GblVariant_getUint32(&v), 18);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&v, 19));
    GBL_COMPARE(GblVariant_getUint32(&v), 19);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_UINT32_TYPE, 20));
    GBL_COMPARE(GblVariant_getUint32(&v), 20);

    // Value Get Move
    uint32_t val = 0;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setUint32(&v, 21);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, 21);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT32_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_u32_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT32_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructUint32(&variant, 97));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_API_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_API_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_API_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT32_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_i32(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT32_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_INT32_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_INT32_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructInt32(&v, 17));
    GBL_COMPARE(GblVariant_getInt32(&v), 17); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_INT32_TYPE, 18));
    GBL_COMPARE(GblVariant_getInt32(&v), 18);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&v, 19));
    GBL_COMPARE(GblVariant_getInt32(&v), 19);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_INT32_TYPE, 20));
    GBL_COMPARE(GblVariant_getInt32(&v), 20);

    // Value Get Move
    int32_t val = 0;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setInt32(&v, 21);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, 21);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT32_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_i32_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT32_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructInt32(&variant, 97));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_API_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_API_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_API_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT32_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_u64(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT64_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_UINT64_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_UINT64_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructUint64(&v, 17));
    GBL_COMPARE(GblVariant_getUint64(&v), 17); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_UINT64_TYPE, 18));
    GBL_COMPARE(GblVariant_getUint64(&v), 18);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&v, 19));
    GBL_COMPARE(GblVariant_getUint64(&v), 19);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_UINT64_TYPE, 20));
    GBL_COMPARE(GblVariant_getUint64(&v), 20);

    // Value Get Move
    uint64_t val = 0;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setUint64(&v, 21);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, 21);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT64_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_u64_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_UINT64_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructUint64(&variant, 97));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_API_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_API_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_API_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_UINT64_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_i64(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT64_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_INT64_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_INT64_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructInt64(&v, 17));
    GBL_COMPARE(GblVariant_getInt64(&v), 17); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_INT64_TYPE, 18));
    GBL_COMPARE(GblVariant_getInt64(&v), 18);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&v, 19));
    GBL_COMPARE(GblVariant_getInt64(&v), 19);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_INT64_TYPE, 20));
    GBL_COMPARE(GblVariant_getInt64(&v), 20);

    // Value Get Move
    int64_t val = 0;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, 20);

    // Value Get Peek
    GblVariant_setInt64(&v, 21);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, 21);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT64_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_i64_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_INT64_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructInt64(&variant, 97));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Char
    GBL_API_VERIFY_CALL(GblVariant_setChar(&tVariant, '\0'));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getChar(&tVariant), 'a');
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_API_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_API_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_INT64_TYPE));

    GBL_API_END();
}

GBL_API GblVariant_test_f32(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_FLOAT_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_FLOAT_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_FLOAT_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructFloat(&v, 17.0f));
    GBL_COMPARE(GblVariant_getFloat(&v), 17.0f); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_FLOAT_TYPE, 18.0f));
    GBL_COMPARE(GblVariant_getFloat(&v), 18.0f);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setFloat(&v, 19.0f));
    GBL_COMPARE(GblVariant_getFloat(&v), 19.0f);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_FLOAT_TYPE, 20.0f));
    GBL_COMPARE(GblVariant_getFloat(&v), 20.0f);

    // Value Get Move
    float val = 0.0f;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, 20.0f);

    // Value Get Peek
    GblVariant_setFloat(&v, 21.0f);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, 21.0f);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_FLOAT_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_f32_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_FLOAT_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructFloat(&variant, 97.0f));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_API_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97.000")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_FLOAT_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_f64(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_DOUBLE_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_DOUBLE_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_DOUBLE_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDouble(&v, 17.0));
    GBL_COMPARE(GblVariant_getDouble(&v), 17.0); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_DOUBLE_TYPE, 18.0));
    GBL_COMPARE(GblVariant_getDouble(&v), 18.0);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setDouble(&v, 19.0));
    GBL_COMPARE(GblVariant_getDouble(&v), 19.0);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_DOUBLE_TYPE, 20.0));
    GBL_COMPARE(GblVariant_getDouble(&v), 20.0);

    // Value Get Move
    double val = 0.0;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, 20.0);

    // Value Get Peek
    GblVariant_setDouble(&v, 21.0);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, 21.0);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_DOUBLE_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_f64_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_DOUBLE_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructDouble(&variant, 97.0));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_API_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("97.000")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_DOUBLE_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_ptr(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_POINTER_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_POINTER_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_POINTER_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructPointer(&v, (void*)0xdeadbeef));
    GBL_COMPARE(GblVariant_getPointer(&v), (void*)0xdeadbeef); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_POINTER_TYPE, (void*)0xdeadbeaf));
    GBL_COMPARE(GblVariant_getPointer(&v), (void*)0xdeadbeaf);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setPointer(&v, (void*)0xaeadbeaf));
    GBL_COMPARE(GblVariant_getPointer(&v), (void*)0xaeadbeaf);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_POINTER_TYPE, (void*)0xcafebabe));
    GBL_COMPARE(GblVariant_getPointer(&v), (void*)0xcafebabe);

    // Value Get Move
    void* pVal = NULL;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &pVal));
    GBL_COMPARE(pVal, (void*)0xcafebabe);

    // Value Get Peek
    GblVariant_setPointer(&v, (void*)0xcafebeef);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &pVal));
    GBL_COMPARE(pVal, (void*)0xcafebeef);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_POINTER_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_ptr_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_POINTER_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructPointer(&variant, (void*)0xdeadbeef));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));

    //GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("0xdeadbeef")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_POINTER_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_string(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_STRING_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_STRING_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_STRING_TYPE);

    // Copy Constructor (arguably this is actually wrong as fuck!!!!)
    // if it weren't empty, it would become invalidated!!
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructString(&v, "lolz"));
    GBL_COMPARE(strcmp(GblVariant_getString(&v), "lolz"), 0); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_COMPARE(strcmp(GblVariant_getString(&v2), "lolz"), 0);
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Value Move Constructor
    GblStringRef* pRef = GblStringRef_create("xbox");
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_STRING_TYPE, pRef));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&v), GBL_STRV("xbox")));

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setString(&v, "dreamcast"));
    GBL_COMPARE(strcmp(GblVariant_getString(&v), "dreamcast"), 0);

    // Value Set Move
    pRef = GblStringRef_create("playstation");
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_STRING_TYPE, pRef));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&v), GBL_STRV("playstation")));

    // Value Get Move
    const char* pVal = NULL;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &pVal));
    GBL_VERIFY(GblStringView_equals(GBL_STRV(pVal), GBL_STRV("playstation")));
    GblStringRef_release(pVal);

    // Value Get Peek
    GblVariant_setString(&v, "gamecube");
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &pVal));
    GBL_COMPARE(strcmp(pVal, "gamecube"), 0);

    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_STRING_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_string_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_STRING_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructString(&variant, "97"));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int64
    GBL_API_VERIFY_CALL(GblVariant_setInt64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), 97);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Float
    GBL_API_VERIFY_CALL(GblVariant_setFloat(&tVariant, 0.0f));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getFloat(&tVariant), 97.0f);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Double
    GBL_API_VERIFY_CALL(GblVariant_setDouble(&tVariant, 0.0));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getDouble(&tVariant), 97.0);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Type
    GBL_API_VERIFY_CALL(GblVariant_setString(&variant, "nil"));
    GBL_API_VERIFY_CALL(GblVariant_setTypeValue(&tVariant, GBL_INVALID_TYPE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getTypeValue(&tVariant), GBL_NIL_TYPE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_STRING_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_type(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_TYPE_TYPE);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, GBL_TYPE_TYPE));
    GBL_VERIFY(GblVariant_type(&v) == GBL_TYPE_TYPE);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructTypeValue(&v, GBL_NIL_TYPE));
    GBL_COMPARE(GblVariant_getTypeValue(&v), GBL_NIL_TYPE); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, GBL_TYPE_TYPE, GBL_STRING_TYPE));
    GBL_COMPARE(GblVariant_getTypeValue(&v), GBL_STRING_TYPE);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setTypeValue(&v, GBL_BOOL_TYPE));
    GBL_COMPARE(GblVariant_getTypeValue(&v), GBL_BOOL_TYPE);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, GBL_TYPE_TYPE, GBL_CHAR_TYPE));
    GBL_COMPARE(GblVariant_getTypeValue(&v), GBL_CHAR_TYPE);

    // Value Get Move
    GblType val = GBL_INVALID_TYPE;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &val));
    GBL_COMPARE(val, GBL_CHAR_TYPE);

    // Value Get Peek
    GblVariant_setTypeValue(&v, GBL_TYPE_TYPE);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &val));
    GBL_COMPARE(val, GBL_TYPE_TYPE);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_TYPE_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_type_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(GBL_TYPE_TYPE);

    GBL_API_VERIFY_CALL(GblVariant_constructTypeValue(&variant, GBL_UINT8_TYPE));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, ""));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("uint8")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(GBL_TYPE_TYPE));

    GBL_API_END();
}


GBL_API GblVariant_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}
