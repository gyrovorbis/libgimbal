#include "meta/classes/gimbal_enum_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/meta/classes/gimbal_enum.h>
#include <gimbal/meta/types/gimbal_variant.h>

#define GBL_ENUM_TEST_SUITE_(inst)  (GBL_PRIVATE(GblEnumTestSuite, inst))

typedef struct GblEnumTestSuite_ {
    GblType         enumType;
    GblEnumClass*   pEnumClass;
} GblEnumTestSuite_;

typedef enum COLOR {
    RED     = 1,
    GREEN   = 5,
    BLUE    = 10
} COLOR;

static GBL_RESULT GblEnumTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblEnumTestSuite_));
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblClass_unrefDefault(GBL_CLASS(pSelf_->pEnumClass)), 0);
    GblType_unregister(pSelf_->enumType);
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_register_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);

    const static GblEnumEntry enumEntries[] = {
        GBL_ENUM_ENTRY(RED,    "Red"),
        GBL_ENUM_ENTRY(GREEN,  "Green"),
        GBL_ENUM_ENTRY(BLUE,   "Blue"),
        GBL_ENUM_ENTRY_LAST()
    };

    pSelf_->enumType = GblEnum_register("Color",
                                enumEntries);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_TEST_VERIFY(pSelf_->enumType != GBL_INVALID_TYPE);
    GBL_CTX_VERIFY_TYPE(pSelf_->enumType, GBL_ENUM_TYPE);

    pSelf_->pEnumClass = (GblEnumClass*)GblClass_refDefault(pSelf_->enumType);
    GBL_TEST_VERIFY(pSelf_->pEnumClass);
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_class_name_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblEnumClass_nameFromIndex(pSelf_->pEnumClass, 1),"GREEN");
    GBL_TEST_COMPARE(GblQuark_toString(GblEnumClass_nameQuarkFromIndex(pSelf_->pEnumClass, 0)), "RED");
    GBL_TEST_COMPARE(GblEnumClass_nameFromValue(pSelf_->pEnumClass, BLUE), "BLUE");
    GBL_TEST_COMPARE(GblQuark_toString(GblEnumClass_nameQuarkFromValue(pSelf_->pEnumClass, GREEN)), "GREEN");
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_class_nick_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblEnumClass_nickFromIndex(pSelf_->pEnumClass, 1), "Green");
    GBL_TEST_COMPARE(GblQuark_toString(GblEnumClass_nickQuarkFromIndex(pSelf_->pEnumClass, 0)), "Red");
    GBL_TEST_COMPARE(GblEnumClass_nickFromValue(pSelf_->pEnumClass, BLUE), "Blue");
    GBL_TEST_COMPARE(GblQuark_toString(GblEnumClass_nickQuarkFromValue(pSelf_->pEnumClass, GREEN)), "Green");
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_class_value_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblEnumClass_valueFromIndex(pSelf_->pEnumClass, 1), GREEN);
    GBL_TEST_COMPARE(GblEnumClass_valueFromName(pSelf_->pEnumClass, "BLUE"), BLUE);
    GBL_TEST_COMPARE(GblEnumClass_valueFromNameQuark(pSelf_->pEnumClass, GblQuark_fromString("RED")), RED);
    GBL_TEST_COMPARE(GblEnumClass_valueFromNick(pSelf_->pEnumClass, "Green"), GREEN);
    GBL_TEST_COMPARE(GblEnumClass_valueFromNickQuark(pSelf_->pEnumClass, GblQuark_fromString("Red")), RED);
    GBL_TEST_VERIFY(GblEnumClass_valueCheck(pSelf_->pEnumClass, BLUE));
    GBL_TEST_VERIFY(!GblEnumClass_valueCheck(pSelf_->pEnumClass, 9999));
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_value_from_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblEnum_fromName("RED", pSelf_->enumType), RED);
    GBL_TEST_COMPARE(GblEnum_fromNameQuark(GblQuark_fromString("GREEN"), pSelf_->enumType), GREEN);
    GBL_TEST_COMPARE(GblEnum_fromNick("Blue", pSelf_->enumType), BLUE);
    GBL_TEST_COMPARE(GblEnum_fromNickQuark(GblQuark_fromString("Green"), pSelf_->enumType), GREEN);
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_value_to_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblEnum_name(RED, pSelf_->enumType), "RED");
    GBL_TEST_COMPARE(GblEnum_nameQuark(GREEN, pSelf_->enumType), GblQuark_fromString("GREEN"));
    GBL_TEST_COMPARE(GblEnum_nick(BLUE, pSelf_->enumType), "Blue");
    GBL_TEST_COMPARE(GblEnum_nickQuark(BLUE, pSelf_->enumType), GblQuark_fromString("Blue"));
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_value_check_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    GBL_TEST_VERIFY(!GblEnum_check(99999, pSelf_->enumType));
    GBL_TEST_VERIFY(GblEnum_check(BLUE, pSelf_->enumType));
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_variant_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(pSelf_->enumType);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, pSelf_->enumType));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == pSelf_->enumType);
    GBL_TEST_COMPARE(GblVariant_enum(&v), RED);

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
    GBL_CTX_VERIFY_CALL(GblVariant_constructEnum(&v, pSelf_->enumType, GREEN));
    GBL_TEST_COMPARE(GblVariant_enum(&v), GREEN); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, pSelf_->enumType, BLUE));
    GBL_TEST_COMPARE(GblVariant_enum(&v), BLUE);

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setEnum(&v, pSelf_->enumType, RED));
    GBL_TEST_COMPARE(GblVariant_enum(&v), RED);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, pSelf_->enumType, GREEN));
    GBL_TEST_COMPARE(GblVariant_enum(&v), GREEN);

    // Value Get Move
    GblEnum color = 0;
    GBL_CTX_VERIFY_CALL(GblVariant_valueMove(&v, &color));
    GBL_TEST_COMPARE(color, GREEN);

    // Value Get Peek
    GblVariant_setEnum(&v, pSelf_->enumType, BLUE);
    GBL_CTX_VERIFY_CALL(GblVariant_valuePeek(&v, &color));
    GBL_TEST_COMPARE(color, BLUE);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(pSelf_->enumType));
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_variant_fail_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    GblVariant v;
    GBL_TEST_COMPARE(GblVariant_constructValueCopy(&v, pSelf_->enumType, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(GblVariant_constructValueMove(&v, pSelf_->enumType, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(GblVariant_setValueMove(&v, pSelf_->enumType, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(GblVariant_setValueCopy(&v, pSelf_->enumType, 9999),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_END();
}

static GBL_RESULT GblEnumTestSuite_variant_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblEnumTestSuite_* pSelf_ = GBL_ENUM_TEST_SUITE_(pSelf);
    GBL_VARIANT(variant);
    GBL_VARIANT(tVariant);
    const GblRefCount refCountBegin = GblType_classRefCount(pSelf_->enumType);

    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(&variant, pSelf_->enumType, BLUE));

    // Bool
    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&tVariant, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_bool(&tVariant), GBL_TRUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint8
    GBL_CTX_VERIFY_CALL(GblVariant_setUint8(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_uint8(&tVariant), BLUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint16
    GBL_CTX_VERIFY_CALL(GblVariant_setUint16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_uint16(&tVariant), BLUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int16
    GBL_CTX_VERIFY_CALL(GblVariant_setInt16(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_int16(&tVariant), BLUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint32
    GBL_CTX_VERIFY_CALL(GblVariant_setUint32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_uint32(&tVariant), BLUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Int32
    GBL_CTX_VERIFY_CALL(GblVariant_setInt32(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_int32(&tVariant), BLUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // Uint64
    GBL_CTX_VERIFY_CALL(GblVariant_setUint64(&tVariant, 255));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_COMPARE(GblVariant_uint64(&tVariant), BLUE);
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "trololo"));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&variant, &tVariant));
    GBL_TEST_VERIFY(GblStringView_equals(GblVariant_stringView(&tVariant), GBL_STRV("BLUE")));
    GBL_TEST_VERIFY(GblVariant_equals(&variant, &tVariant));

    // FROM String
    GBL_CTX_VERIFY_CALL(GblVariant_setString(&tVariant, "BLUE"));
    GBL_TEST_VERIFY(GblVariant_canConvert(GblVariant_typeOf(&tVariant), GblVariant_typeOf(&variant)));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&tVariant, &variant));
    GBL_CTX_VERIFY_CALL(GblVariant_equals(&tVariant, &variant));
    GBL_TEST_COMPARE(GblVariant_enum(&variant), BLUE);

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&tVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&variant));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(pSelf_->enumType));

    GBL_CTX_END();
}

GBL_EXPORT GblType GblEnumTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "register",           GblEnumTestSuite_register_              },
        { "className",          GblEnumTestSuite_class_name_            },
        { "classNick",          GblEnumTestSuite_class_nick_            },
        { "classvalue",         GblEnumTestSuite_class_value_           },
        { "valueFrom",          GblEnumTestSuite_value_from_            },
        { "valueTo",            GblEnumTestSuite_value_to_              },
        { "valueCheck",         GblEnumTestSuite_value_check_           },
        { "variant",            GblEnumTestSuite_variant_               },
        { "variantFail",        GblEnumTestSuite_variant_fail_          },
        { "variantConversions", GblEnumTestSuite_variant_conversions_   },
        { NULL,                 NULL                                    }
    };

    const static GblTestSuiteVTable vTable = {
        .pFnSuiteInit   = GblEnumTestSuite_init_,
        .pFnSuiteFinal  = GblEnumTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("EnumTestSuite"),
                                     &vTable,
                                     sizeof(GblEnumTestSuite),
                                     sizeof(GblEnumTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}


