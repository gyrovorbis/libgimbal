#include "test_gimbal_enum.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/meta/gimbal_enum.h>
#include <gimbal/meta/gimbal_variant.h>

typedef enum COLOR {
    RED     = 1,
    GREEN   = 5,
    BLUE    = 10
} COLOR;

GblType         enumType_    = GBL_INVALID_TYPE;
GblEnumClass*   pEnumClass_  = NULL;

GBL_API GblEnum_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblEnum_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblEnum_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblEnum_test_register(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    const static GblEnumEntry enumEntries[] = {
        GBL_ENUM_ENTRY(RED,    "Red"),
        GBL_ENUM_ENTRY(GREEN,  "Green"),
        GBL_ENUM_ENTRY(BLUE,   "Blue"),
        GBL_ENUM_ENTRY_LAST()
    };

    enumType_ = GblEnum_register("Color",
                                enumEntries);
    GBL_API_VERIFY_LAST_RECORD();

    GBL_VERIFY(enumType_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_ENUM_TYPE_CHECK(enumType_));

    pEnumClass_ = (GblEnumClass*)GblClass_ref(enumType_);
    GBL_VERIFY(pEnumClass_);
    GBL_API_END();
}

GBL_API GblEnum_test_class_name(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(strcmp(GblEnumClass_nameFromIndex(pEnumClass_,
                                                  1),
                       "GREEN"),
                0);
    GBL_COMPARE(strcmp(GblQuark_toString(GblEnumClass_nameQuarkFromIndex(pEnumClass_,
                                                                         0)),
                       "RED"),
                0);
    GBL_COMPARE(strcmp(GblEnumClass_nameFromValue(pEnumClass_,
                                                  BLUE),
                       "BLUE"),
                0);
    GBL_COMPARE(strcmp(GblQuark_toString(GblEnumClass_nameQuarkFromValue(pEnumClass_,
                                                                         GREEN)),
                       "GREEN"),
                0);
    GBL_API_END();
}

GBL_API GblEnum_test_class_nick(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(strcmp(GblEnumClass_nickFromIndex(pEnumClass_,
                                                  1),
                       "Green"),
                0);
    GBL_COMPARE(strcmp(GblQuark_toString(GblEnumClass_nickQuarkFromIndex(pEnumClass_,
                                                                         0)),
                       "Red"),
                0);
    GBL_COMPARE(strcmp(GblEnumClass_nickFromValue(pEnumClass_,
                                                  BLUE),
                       "Blue"),
                0);
    GBL_COMPARE(strcmp(GblQuark_toString(GblEnumClass_nickQuarkFromValue(pEnumClass_,
                                                                         GREEN)),
                       "Green"),
                0);
    GBL_API_END();
}

GBL_API GblEnum_test_class_value(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblEnumClass_valueFromIndex(pEnumClass_, 1), GREEN);
    GBL_COMPARE(GblEnumClass_valueFromName(pEnumClass_, "BLUE"), BLUE);
    GBL_COMPARE(GblEnumClass_valueFromNameQuark(pEnumClass_, GblQuark_fromString("RED")), RED);
    GBL_COMPARE(GblEnumClass_valueFromNick(pEnumClass_, "Green"), GREEN);
    GBL_COMPARE(GblEnumClass_valueFromNickQuark(pEnumClass_, GblQuark_fromString("Red")), RED);
    GBL_VERIFY(GblEnumClass_valueCheck(pEnumClass_, BLUE));
    GBL_VERIFY(!GblEnumClass_valueCheck(pEnumClass_, 9999));
    GBL_API_END();
}

GBL_API GblEnum_test_value_from(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblEnum_fromName("RED", enumType_), RED);
    GBL_COMPARE(GblEnum_fromNameQuark(GblQuark_fromString("GREEN"), enumType_), GREEN);
    GBL_COMPARE(GblEnum_fromNick("Blue", enumType_), BLUE);
    GBL_COMPARE(GblEnum_fromNickQuark(GblQuark_fromString("Green"), enumType_), GREEN);
    GBL_API_END();
}

GBL_API GblEnum_test_value_to(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(strcmp(GblEnum_name(RED, enumType_), "RED"), 0);
    GBL_COMPARE(GblEnum_nameQuark(GREEN, enumType_), GblQuark_fromString("GREEN"));
    GBL_COMPARE(strcmp(GblEnum_nick(BLUE, enumType_), "Blue"), 0);
    GBL_COMPARE(GblEnum_nickQuark(BLUE, enumType_), GblQuark_fromString("Blue"));
    GBL_API_END();
}

GBL_API GblEnum_test_value_check(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(!GblEnum_check(99999, enumType_));
    GBL_VERIFY(GblEnum_check(BLUE, enumType_));
    GBL_API_END();
}

GBL_API GblEnum_test_variant(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(enumType_);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, enumType_));
    GBL_VERIFY(GblVariant_type(&v) == enumType_);
    GBL_COMPARE(GblVariant_getEnum(&v), RED);

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
    GBL_API_VERIFY_CALL(GblVariant_constructEnum(&v, enumType_, GREEN));
    GBL_COMPARE(GblVariant_getEnum(&v), GREEN); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, enumType_, BLUE));
    GBL_COMPARE(GblVariant_getEnum(&v), BLUE);

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setEnum(&v, enumType_, RED));
    GBL_COMPARE(GblVariant_getEnum(&v), RED);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, enumType_, GREEN));
    GBL_COMPARE(GblVariant_getEnum(&v), GREEN);

    // Value Get Move
    COLOR color = 0;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &color));
    GBL_COMPARE(color, GREEN);

    // Value Get Peek
    GblVariant_setEnum(&v, enumType_, BLUE);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &color));
    GBL_COMPARE(color, BLUE);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(enumType_));
    GBL_API_END();
}

GBL_API GblEnum_test_variant_fail(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblVariant v;
    GBL_COMPARE(GblVariant_constructValueCopy(&v, enumType_, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_COMPARE(GblVariant_constructValueMove(&v, enumType_, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_COMPARE(GblVariant_setValueMove(&v, enumType_, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_COMPARE(GblVariant_setValueCopy(&v, enumType_, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_END();
}

GBL_API GblEnum_test_variant_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(enumType_);

    GBL_API_VERIFY_CALL(GblVariant_constructValueCopy(&variant, enumType_, BLUE));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), BLUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), BLUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), BLUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), BLUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), BLUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), BLUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("BLUE")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // FROM String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "BLUE"));
    GBL_VERIFY(GblVariant_canConvert(GblVariant_type(&tVariant), GblVariant_type(&variant)));
    GBL_API_VERIFY_CALL(GblVariant_convert(&tVariant, &variant));
    GBL_API_VERIFY_CALL(GblVariant_equals(&tVariant, &variant));
    GBL_COMPARE(GblVariant_getEnum(&variant), BLUE);

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(enumType_));

    GBL_API_END();
}

#if 0
GBL_API GblEnum_test_variant_construct(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
 //   GblVariant v;
  //  GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, enumType_));
   // GBL_COMPARE(GblVariant_getEnum(&v), RED);) {

    GBL_API_END();
}

GBL_API GblEnum_test_variant_value_to(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblEnum_test_variant_value_from(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblEnum_test_variant_operators(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}
#endif
GBL_API GblEnum_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblClass_unref(GBL_CLASS(pEnumClass_)), 0);
    GBL_API_END();
}
