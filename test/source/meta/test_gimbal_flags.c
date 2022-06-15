#include "meta/test_gimbal_flags.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/meta/gimbal_flags.h>
#include <gimbal/meta/gimbal_variant.h>
#include <gimbal/objects/gimbal_property.h>
#include <gimbal/strings/gimbal_string_buffer.h>

GBL_DECLARE_FLAGS(PROPERTY_FLAGS) {
    READ    = 0x1,
    WRITE   = 0x2,
    LOAD    = 0x4,
    SAVE    = 0x8
};

GblType         flagsType_    = GBL_INVALID_TYPE;
GblFlagsClass*  pFlagsClass_  = NULL;

GBL_API GblFlags_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblFlags_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblFlags_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblFlags_test_register(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    flagsType_ = GblFlags_register("PropertyFlags",
                                   (const GblFlagEntry[]){
                                       GBL_FLAGS_ENTRY(READ,    "Read"),
                                       GBL_FLAGS_ENTRY(WRITE,   "Write"),
                                       GBL_FLAGS_ENTRY(LOAD,    "Load"),
                                       GBL_FLAGS_ENTRY(SAVE,    "Save"),
                                       GBL_FLAGS_ENTRY_LAST()
                                   });
    GBL_API_VERIFY_LAST_RECORD();

    GBL_VERIFY(flagsType_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_FLAGS_TYPE_CHECK(flagsType_));

    pFlagsClass_ = (GblFlagsClass*)GblClass_ref(flagsType_);
    GBL_VERIFY(pFlagsClass_);
    GBL_API_END();
}

GBL_API GblFlags_test_class_name(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(strcmp(GblFlagsClass_nameFromIndex(pFlagsClass_,
                                                  1),
                       "WRITE"),
                0);
    GBL_COMPARE(strcmp(GblQuark_toString(GblFlagsClass_nameQuarkFromIndex(pFlagsClass_,
                                                                         0)),
                       "READ"),
                0);
    GBL_COMPARE(strcmp(GblFlagsClass_nameFromValue(pFlagsClass_,
                                                   LOAD),
                       "LOAD"),
                0);
    GBL_COMPARE(strcmp(GblQuark_toString(GblFlagsClass_nameQuarkFromValue(pFlagsClass_,
                                                                          SAVE)),
                       "SAVE"),
                0);
    GBL_API_END();
}

GBL_API GblFlags_test_class_nick(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(strcmp(GblFlagsClass_nickFromIndex(pFlagsClass_,
                                                  1),
                       "Write"),
                0);
    GBL_COMPARE(strcmp(GblQuark_toString(GblFlagsClass_nickQuarkFromIndex(pFlagsClass_,
                                                                         3)),
                       "Save"),
                0);
    GBL_COMPARE(strcmp(GblFlagsClass_nickFromValue(pFlagsClass_,
                                                   READ),
                       "Read"),
                0);
    GBL_COMPARE(strcmp(GblQuark_toString(GblFlagsClass_nickQuarkFromValue(pFlagsClass_,
                                                                          LOAD)),
                       "Load"),
                0);
    GBL_API_END();
}

GBL_API GblFlags_test_class_value(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblFlagsClass_valueFromIndex(pFlagsClass_, 1), WRITE);
    GBL_COMPARE(GblFlagsClass_valueFromName(pFlagsClass_, "READ"), READ);
    GBL_COMPARE(GblFlagsClass_valueFromNameQuark(pFlagsClass_, GblQuark_fromString("SAVE")), SAVE);
    GBL_COMPARE(GblFlagsClass_valueFromNick(pFlagsClass_, "Load"), LOAD);
    GBL_COMPARE(GblFlagsClass_valueFromNickQuark(pFlagsClass_, GblQuark_fromString("Write")), WRITE);
    GBL_VERIFY(GblFlagsClass_valueCheck(pFlagsClass_, READ|WRITE|LOAD));
    GBL_VERIFY(!GblFlagsClass_valueCheck(pFlagsClass_, SAVE|9999));
    GBL_API_END();
}

GBL_API GblFlags_test_class_string(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblFlags flags = GblFlagsClass_valueFromString(pFlagsClass_, "WRITE|SAVE");
    GBL_API_VERIFY_LAST_RECORD();

    GBL_COMPARE(flags, (WRITE|SAVE));

    GblStringBuffer buffer;
    GBL_API_VERIFY_CALL(GblStringBuffer_construct(&buffer));
    GBL_API_VERIFY_CALL(GblFlagsClass_valueAppendString(pFlagsClass_,
                                                        flags,
                                                        &buffer));
    GBL_COMPARE(strcmp(GblStringBuffer_cString(&buffer),
                       "WRITE|SAVE"), 0);

    GBL_API_VERIFY_CALL(GblStringBuffer_destruct(&buffer));

    GBL_API_END();
}

GBL_API GblFlags_test_value_from(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblFlags_fromName("READ", flagsType_), READ);
    GBL_COMPARE(GblFlags_fromNameQuark(GblQuark_fromString("WRITE"), flagsType_), WRITE);
    GBL_COMPARE(GblFlags_fromNick("Load", flagsType_), LOAD);
    GBL_COMPARE(GblFlags_fromNickQuark(GblQuark_fromString("Save"), flagsType_), SAVE);
    GBL_API_END();
}

GBL_API GblFlags_test_value_to(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(strcmp(GblFlags_name(READ, flagsType_), "READ"), 0);
    GBL_COMPARE(GblFlags_nameQuark(WRITE, flagsType_), GblQuark_fromString("WRITE"));
    GBL_COMPARE(strcmp(GblFlags_nick(LOAD, flagsType_), "Load"), 0);
    GBL_COMPARE(GblFlags_nickQuark(SAVE, flagsType_), GblQuark_fromString("Save"));
    GBL_API_END();
}

GBL_API GblFlags_test_value_check(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(!GblFlags_check(WRITE|99999, flagsType_));
    GBL_VERIFY(GblFlags_check(SAVE, flagsType_));
    GBL_API_END();
}

GBL_API GblFlags_test_value_string(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblFlags flags = GblFlags_fromString("READ|WRITE", flagsType_);
    GBL_API_VERIFY_LAST_RECORD();

    GBL_COMPARE(flags, (READ|WRITE));

    GblStringBuffer buffer;
    GBL_API_VERIFY_CALL(GblStringBuffer_construct(&buffer));
    GBL_API_VERIFY_CALL(GblFlags_appendString(flags, flagsType_, &buffer));
    GBL_COMPARE(strcmp(GblStringBuffer_cString(&buffer),
                       "READ|WRITE"), 0);

    GBL_API_VERIFY_CALL(GblStringBuffer_destruct(&buffer));

    GBL_API_END();
}

GBL_API GblFlags_test_variant(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(flagsType_);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, flagsType_));
    GBL_VERIFY(GblVariant_type(&v) == flagsType_);
    GBL_COMPARE(GblVariant_getFlags(&v), 0);

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
    GBL_API_VERIFY_CALL(GblVariant_constructFlags(&v, flagsType_, READ|WRITE));
    GBL_COMPARE(GblVariant_getFlags(&v), (READ|WRITE)); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, flagsType_, SAVE|LOAD));
    GBL_COMPARE(GblVariant_getFlags(&v), (SAVE|LOAD));

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setFlags(&v, flagsType_, WRITE|LOAD));
    GBL_COMPARE(GblVariant_getFlags(&v), (WRITE|LOAD));

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, flagsType_, LOAD|SAVE));
    GBL_COMPARE(GblVariant_getFlags(&v), (LOAD|SAVE));

    // Value Get Move
    PROPERTY_FLAGS flags = 0;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &flags));
    GBL_COMPARE(flags, (LOAD|SAVE));

    // Value Get Peek
    GblVariant_setFlags(&v, flagsType_, WRITE);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &flags));
    GBL_COMPARE(flags, WRITE);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(flagsType_));

    GBL_API_END();
}

GBL_API GblFlags_test_variant_fail(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GblVariant v;
    GBL_COMPARE(GblVariant_constructValueCopy(&v, flagsType_, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_COMPARE(GblVariant_constructValueMove(&v, flagsType_, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_COMPARE(GblVariant_setValueMove(&v, flagsType_, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_COMPARE(GblVariant_setValueCopy(&v, flagsType_, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_END();
}

GBL_API GblFlags_test_variant_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(flagsType_);

    GBL_API_VERIFY_CALL(GblVariant_constructValueCopy(&variant, flagsType_, (READ|WRITE)));

    // Bool
    GBL_API_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getBool(&tVariant), GBL_TRUE);
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_API_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint8(&tVariant), (READ|WRITE));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_API_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint16(&tVariant), (READ|WRITE));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_API_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt16(&tVariant), (READ|WRITE));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_API_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint32(&tVariant), (READ|WRITE));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_API_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getInt32(&tVariant), (READ|WRITE));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_API_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_COMPARE(GblVariant_getUint64(&tVariant), (READ|WRITE));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_API_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_VERIFY(GblStringView_equals(GblVariant_getStringView(&tVariant), GBL_STRV("READ|WRITE")));
    GBL_VERIFY(GblVariant_equals(&variant, &tVariant));

    // FROM String
    GBL_API_VERIFY_CALL(GblVariant_setString(&tVariant, "READ|WRITE|LOAD|SAVE"));
    GBL_VERIFY(GblVariant_canConvert(GblVariant_type(&tVariant), GblVariant_type(&variant)));
    GBL_API_VERIFY_CALL(GblVariant_convert(&tVariant, &variant));
    GBL_API_VERIFY_CALL(GblVariant_equals(&tVariant, &variant));
    GBL_COMPARE(GblVariant_getFlags(&variant), (READ|WRITE|LOAD|SAVE));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(flagsType_));

    GBL_API_END();
}


GBL_API GblFlags_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblClass_unref(GBL_CLASS(pFlagsClass_)), 0);
    GBL_API_END();
}
