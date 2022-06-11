#include <gimbal/meta/gimbal_type.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/ifaces/gimbal_ivariant.h>
#include <gimbal/meta/gimbal_variant.h>
#include <gimbal/types/gimbal_string_buffer.h>
#include <gimbal/types/gimbal_variant.h>
#include <gimbal/types/gimbal_quark.h>
#include <inttypes.h>

GBL_RESULT GblPrimitiveClass_init(GblPrimitiveClass* pClass, GblIVariantIFace* pIFace, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    memcpy(&pClass->iVariantIFace.supportedOps,
           &pIFace->supportedOps,
           sizeof(GblIVariantIFace)-offsetof(GblIVariantIFace, supportedOps));
    GBL_API_END();
}

static GBL_RESULT nilSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_EXPRESSION(GblVariant_type(pVariant) == GBL_NIL_TYPE);
    GBL_API_CALL(GblStringBuffer_appendNil(pString));
    GBL_API_END();
}

static GBL_RESULT nilLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GblBool isNil = GBL_FALSE;
    GBL_API_BEGIN(pString->data.pCtx);
    isNil = GblStringView_toNil(GblStringBuffer_view(pString));
    GBL_API_VERIFY_EXPRESSION(isNil);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_NIL_TYPE);
    GBL_API_END();
}

static GBL_RESULT nilConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_UNUSED(pVariant);
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, GBL_FALSE);
    else if(type == GBL_CHAR_TYPE)
        GblVariant_setChar(pOther, '\0');
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, 0);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, 0);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, 0);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, 0);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, 0);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, 0);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, 0);
    else if(type == GBL_POINTER_TYPE)
        GblVariant_setPointer(pOther, NULL);
    else if(type == GBL_STRING_TYPE)
        GblVariant_setString(pOther, "nil");
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT boolSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_EXPRESSION(GblVariant_type(pVariant) == GBL_NIL_TYPE);
    GBL_API_CALL(GblStringBuffer_appendBool(pString, pVariant->boolean));
    GBL_API_END();
}

static GBL_RESULT boolLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(pString->data.pCtx);
    pVariant->boolean = GblStringView_toBool(GblStringBuffer_view(pString));
    GBL_API_END();
}

static GBL_RESULT boolSet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    pVariant->boolean = pArgs->i32? GBL_TRUE : GBL_FALSE;
    GBL_API_END();
}

static GBL_RESULT boolGet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((GblBool*)pArgs->pVoid) = pVariant->boolean;
    GBL_API_END();
}

static GBL_RESULT boolCompare_(const GblVariant* pSelf, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = pSelf->boolean - pOther->boolean;
    GBL_API_END();
}

static GBL_RESULT boolConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_CHAR_TYPE)
        GblVariant_setChar(pOther, pVariant->boolean? 1 : 0);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, pVariant->boolean? 1 : 0);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, pVariant->boolean? 1 : 0);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, pVariant->boolean? 1 : 0);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, pVariant->boolean? 1 : 0);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, pVariant->boolean? 1 : 0);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->boolean? 1 : 0);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, pVariant->boolean? 1 : 0);
    else if(type == GBL_STRING_TYPE)
        GblVariant_setString(pOther, pVariant->boolean? "true" : "false");
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT charSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "'%c'", pVariant->character);
    GBL_API_END();
}

static GBL_RESULT charLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    const char* pStr = NULL;
    GblSize length = GblStringBuffer_length(pString);
    if(length == 3) {
        pStr = GblStringBuffer_cString(pString);
        GBL_API_VERIFY_EXPRESSION(pStr[0] == '\'');
        GBL_API_VERIFY_EXPRESSION(pStr[2] == '\'');
        pVariant->character = pStr[1];
    }
    GBL_API_END();
}

static GBL_RESULT charSet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_INT32_TYPE);
    pVariant->character = pArgs->i32;
    GBL_API_END();
}

static GBL_RESULT charGet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((char*)pArgs->pVoid) = pVariant->character;
    GBL_API_END();
}

static GBL_RESULT charCompare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = pVariant->character - pOther->character;
    GBL_API_END();
}

static GBL_RESULT charConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->character? GBL_TRUE : GBL_FALSE);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, pVariant->character);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, pVariant->character);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, pVariant->character);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, pVariant->character);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, pVariant->character);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->character);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, pVariant->character);
    else if(type == GBL_STRING_TYPE) {
        const char string[2] = { pVariant->character, '\0' };
        GblVariant_setString(pOther, string);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}


static GBL_RESULT u8Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%u", pVariant->u8);
    GBL_API_END();
}

static GBL_RESULT u8Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = GblStringView_toInt(GblStringBuffer_view(pString));
    pVariant->u8 = (uint8_t)integer;
    GBL_API_END();
}

static GBL_RESULT u8Set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_INT32_TYPE);
    pVariant->u8 = (uint8_t)pArgs->i32;
    GBL_API_END();
}

static GBL_RESULT u8Get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((uint8_t*)pArgs->pVoid) = pVariant->u8;
    GBL_API_END();
}

static GBL_RESULT u8Compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = pVariant->u8 - pOther->u8;
    GBL_API_END();
}

static GBL_RESULT u8Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->u8);
    else if(type == GBL_CHAR_TYPE)
        GblVariant_setChar(pOther, pVariant->u8);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, pVariant->u8);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, pVariant->u8);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, pVariant->u8);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, pVariant->u8);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->u8);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, pVariant->u8);
    else if(type == GBL_FLOAT_TYPE)
        GblVariant_setFloat(pOther, pVariant->u8);
    else if(type == GBL_DOUBLE_TYPE)
        GblVariant_setDouble(pOther, pVariant->u8);
    else if(type == GBL_STRING_TYPE) {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%u", pVariant->u8);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}


static GBL_RESULT i16Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%d", pVariant->i16);
    GBL_API_END();
}

static GBL_RESULT i16Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = GblStringView_toInt(GblStringBuffer_view(pString));
    pVariant->i16 = (int16_t)integer;
    GBL_API_END();
}

static GBL_RESULT i16Set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_INT32_TYPE);
    pVariant->i16 = (int16_t)pArgs->i32;
    GBL_API_END();
}

static GBL_RESULT i16Get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((int16_t*)pArgs->pVoid) = pVariant->i16;
    GBL_API_END();
}

static GBL_RESULT i16Compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = pVariant->i16 - pOther->i16;
    GBL_API_END();
}

static GBL_RESULT i16Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->i16);
    else if(type == GBL_CHAR_TYPE)
        GblVariant_setChar(pOther, pVariant->i16);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, pVariant->i16);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, pVariant->i16);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, pVariant->i16);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, pVariant->i16);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->i16);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, pVariant->i16);
    else if(type == GBL_FLOAT_TYPE)
        GblVariant_setFloat(pOther, pVariant->i16);
    else if(type == GBL_DOUBLE_TYPE)
        GblVariant_setDouble(pOther, pVariant->i16);
    else if(type == GBL_STRING_TYPE) {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%d", pVariant->i16);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT u16Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%u", pVariant->u16);
    GBL_API_END();
}

static GBL_RESULT u16Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = GblStringView_toInt(GblStringBuffer_view(pString));
    pVariant->u16 = (uint16_t)integer;
    GBL_API_END();
}

static GBL_RESULT u16Set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_INT32_TYPE);
    pVariant->u16 = (uint16_t)pArgs->i32;
    GBL_API_END();
}

static GBL_RESULT u16Get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((uint16_t*)pArgs->pVoid) = pVariant->u16;
    GBL_API_END();
}

static GBL_RESULT u16Compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = pVariant->u16 - pOther->u16;
    GBL_API_END();
}

static GBL_RESULT u16Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->u16);
    else if(type == GBL_CHAR_TYPE)
        GblVariant_setChar(pOther, pVariant->u16);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, pVariant->u16);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, pVariant->u16);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, pVariant->u16);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, pVariant->u16);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->u16);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, pVariant->u16);
    else if(type == GBL_FLOAT_TYPE)
        GblVariant_setFloat(pOther, pVariant->u16);
    else if(type == GBL_DOUBLE_TYPE)
        GblVariant_setDouble(pOther, pVariant->u16);
    else if(type == GBL_STRING_TYPE) {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%u", pVariant->u16);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT i32Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%d", pVariant->i32);
    GBL_API_END();
}

static GBL_RESULT i32Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = GblStringView_toInt(GblStringBuffer_view(pString));
    pVariant->i32 = (int32_t)integer;
    GBL_API_END();
}

static GBL_RESULT i32Set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_INT32_TYPE);
    pVariant->i32 = (int32_t)pArgs->i32;
    GBL_API_END();
}

static GBL_RESULT i32Get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((int32_t*)pArgs->pVoid) = pVariant->i32;
    GBL_API_END();
}

static GBL_RESULT i32Compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = pVariant->i32 - pOther->i32;
    GBL_API_END();
}

static GBL_RESULT i32Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->i32);
    else if(type == GBL_CHAR_TYPE)
        GblVariant_setChar(pOther, pVariant->i32);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, pVariant->i32);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, pVariant->i32);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, pVariant->i32);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, pVariant->i32);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->i32);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, pVariant->i32);
    else if(type == GBL_FLOAT_TYPE)
        GblVariant_setFloat(pOther, pVariant->i32);
    else if(type == GBL_DOUBLE_TYPE)
        GblVariant_setDouble(pOther, pVariant->i32);
    else if(type == GBL_STRING_TYPE) {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%d", pVariant->i32);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}


static GBL_RESULT u32Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%u", pVariant->u32);
    GBL_API_END();
}

static GBL_RESULT u32Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = GblStringView_toInt(GblStringBuffer_view(pString));
    pVariant->u32 = (uint32_t)integer;
    GBL_API_END();
}

static GBL_RESULT u32Set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_UINT32_TYPE);
    pVariant->u32 = (uint32_t)pArgs->u32;
    GBL_API_END();
}

static GBL_RESULT u32Get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY|GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((uint32_t*)pArgs->pVoid) = pVariant->u32;
    GBL_API_END();
}

static GBL_RESULT u32Compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = pVariant->u32 - pOther->u32;
    GBL_API_END();
}

static GBL_RESULT u32Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->u32);
    else if(type == GBL_CHAR_TYPE)
        GblVariant_setChar(pOther, pVariant->u32);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, pVariant->u32);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, pVariant->u32);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, pVariant->u32);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, pVariant->u32);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->u32);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, pVariant->u32);
    else if(type == GBL_FLOAT_TYPE)
        GblVariant_setFloat(pOther, pVariant->u32);
    else if(type == GBL_DOUBLE_TYPE)
        GblVariant_setDouble(pOther, pVariant->u32);
    else if(type == GBL_STRING_TYPE) {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%u", pVariant->u32);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT i64Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%d", pVariant->i64);
    GBL_API_END();
}

static GBL_RESULT i64Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = GblStringView_toInt(GblStringBuffer_view(pString));
    pVariant->i64 = (int64_t)integer;
    GBL_API_END();
}

static GBL_RESULT i64Set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_INT64_TYPE);
    pVariant->i64 = (int64_t)pArgs->i64;
    GBL_API_END();
}

static GBL_RESULT i64Get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((int64_t*)pArgs->pVoid) = pVariant->i64;
    GBL_API_END();
}

static GBL_RESULT i64Compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    if(pVariant->i64 > pOther->i64)         *pResult = 1;
    else if (pVariant->i64 < pOther->i64)   *pResult = -1;
    else                                    *pResult = 0;
    GBL_API_END();
}

static GBL_RESULT i64Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->i64);
    else if(type == GBL_CHAR_TYPE)
        GblVariant_setChar(pOther, pVariant->i64);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, pVariant->u64);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, pVariant->i64);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, pVariant->i64);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, pVariant->i64);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, pVariant->i64);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->i64);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->i64);
    else if(type == GBL_FLOAT_TYPE)
        GblVariant_setFloat(pOther, pVariant->i64);
    else if(type == GBL_DOUBLE_TYPE)
        GblVariant_setDouble(pOther, pVariant->i64);
    else if(type == GBL_STRING_TYPE) {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%" SCNd64, pVariant->i64);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT u64Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%u", pVariant->u64);
    GBL_API_END();
}

static GBL_RESULT u64Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = GblStringView_toInt(GblStringBuffer_view(pString));
    pVariant->u64 = (uint64_t)integer;
    GBL_API_END();
}

static GBL_RESULT u64Set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_UINT64_TYPE);
    pVariant->u64 = pArgs->u64;
    GBL_API_END();
}

static GBL_RESULT u64Get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((uint64_t*)pArgs->pVoid) = pVariant->u64;
    GBL_API_END();
}

static GBL_RESULT u64Compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    if(pVariant->u64 > pOther->u64)         *pResult = 1;
    else if (pVariant->u64 < pOther->u64)   *pResult = -1;
    else                                    *pResult = 0;
    GBL_API_END();
}

static GBL_RESULT u64Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->u64);
    else if(type == GBL_CHAR_TYPE)
        GblVariant_setChar(pOther, pVariant->u64);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, pVariant->u64);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, pVariant->u64);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, pVariant->u64);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, pVariant->u64);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, pVariant->u64);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, pVariant->u64);
    else if(type == GBL_FLOAT_TYPE)
        GblVariant_setFloat(pOther, pVariant->u64);
    else if(type == GBL_DOUBLE_TYPE)
        GblVariant_setDouble(pOther, pVariant->u64);
    else if(type == GBL_STRING_TYPE) {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%" SCNu64, pVariant->i64);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT f32Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblStringBuffer_appendFloat(pString, pVariant->f32));
    GBL_API_END();
}

static GBL_RESULT f32Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    pVariant->f32 = GblStringView_toFloat(GblStringBuffer_view(pString));
    GBL_API_END();
}

static GBL_RESULT f32Set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_DOUBLE_TYPE);
    pVariant->f32 = (float)pArgs->f64;
    GBL_API_END();
}

static GBL_RESULT f32Get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((float*)pArgs->pVoid) = pVariant->f32;
    GBL_API_END();
}

static GBL_RESULT f32Compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    if(pVariant->f32 > pOther->f32)         *pResult = 1;
    else if (pVariant->f32 < pOther->f32)   *pResult = -1;
    else                                    *pResult = 0;
    GBL_API_END();
}

static GBL_RESULT f32Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->f32 != 0.0f && pVariant->f32 != FP_NAN);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, pVariant->f32);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, pVariant->f32);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, pVariant->f32);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, pVariant->f32);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, pVariant->f32);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->f32);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, pVariant->f32);
    else if(type == GBL_DOUBLE_TYPE)
        GblVariant_setDouble(pOther, pVariant->f32);
    else if(type == GBL_STRING_TYPE) {
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%.3f", pVariant->f32);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT f64Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblStringBuffer_appendDouble(pString, pVariant->f64));
    GBL_API_END();
}

static GBL_RESULT f64Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    pVariant->f64 = GblStringView_toDouble(GblStringBuffer_view(pString));
    GBL_API_END();
}

static GBL_RESULT f64Set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_DOUBLE_TYPE);
    pVariant->f64 = pArgs->f64;
    GBL_API_END();
}

static GBL_RESULT f64Get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((double*)pArgs->pVoid) = pVariant->f64;
    GBL_API_END();
}

static GBL_RESULT f64Compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    if(pVariant->f64 > pOther->f64)         *pResult = 1;
    else if (pVariant->f64 < pOther->f64)   *pResult = -1;
    else                                    *pResult = 0;
    GBL_API_END();
}

static GBL_RESULT f64Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->f64 != 0.0 && pVariant->f64 != FP_NAN);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, pVariant->f64);
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, pVariant->f64);
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, pVariant->f64);
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, pVariant->f64);
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, pVariant->f64);
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, pVariant->f64);
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, pVariant->f64);
    else if(type == GBL_FLOAT_TYPE)
        GblVariant_setFloat(pOther, pVariant->f64);
    else if(type == GBL_STRING_TYPE) {
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%.3f", pVariant->f64);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT pSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblStringBuffer_appendPointer(pString, pVariant->pVoid));
    GBL_API_END();
}

static GBL_RESULT pLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    pVariant->pVoid = (void*)(uintptr_t)GblStringView_toInt(GblStringBuffer_view(pString));
    GBL_API_END();
}

static GBL_RESULT pSet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_POINTER_TYPE);
    pVariant->pVoid = pArgs->pVoid;
    GBL_API_END();
}

static GBL_RESULT pCompare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    if(pVariant->pVoid > pOther->pVoid)         *pResult = 1;
    else if(pVariant->pVoid < pOther->pVoid)    *pResult = -1;
    else                                        *pResult = 0;
    GBL_API_END();
}

static GBL_RESULT pConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->pVoid? GBL_TRUE : GBL_FALSE);
    else if(type == GBL_STRING_TYPE) {
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%p", pVariant->pVoid);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT pGet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((void**)pArgs->pVoid) = pVariant->pVoid;
    GBL_API_END();
}

static GBL_RESULT stringConstruct_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pVariant);

    // Default constructor
    if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
        pVariant->pString = NULL;

    // Copy constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY) {
        GBL_API_VERIFY_ARG(argc == 1);
        GBL_API_VERIFY_TYPE(GblVariant_type(&pArgs[0]), GBL_STRING_TYPE);
        if(pArgs[0].pString)
            pVariant->pString = GblStringRef_acquire(pArgs[0].pString);
    // Move constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE) {
        GBL_API_VERIFY_ARG(argc == 1);
        GBL_API_VERIFY_TYPE(GblVariant_type(&pArgs[0]), GBL_STRING_TYPE);
        if(pArgs[0].pString) {
            pVariant->pString   = pArgs[0].pString;
            GblVariant_invalidate(&pArgs[0]);
        }
    // Value copying constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY) {
        GBL_API_VERIFY_ARG(argc == 1);
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        if(pArgs[0].pVoid) {
            pVariant->pString = GblStringRef_create(pArgs[0].pVoid);
        }
    // Value moving constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE) {
        GBL_API_VERIFY_ARG(argc == 1);
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_API_VERIFY_POINTER(pArgs[0].pVoid);
        GblStringRef_release(pVariant->pString);
        pVariant->pString = pArgs[0].pVoid;
    }
    GBL_API_END();
}

static GBL_RESULT stringDestruct_(GblVariant* pVariant) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_STRING_TYPE);
    GblStringRef_release(pVariant->pString);
    GBL_API_END();
}

static GBL_RESULT stringSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblStringBuffer_assign(pString, GblStringRef_view(pVariant->pString)));
    GBL_API_END();
}

static GBL_RESULT stringLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblStringRef_release(pVariant->pString);
    pVariant->pString = GblStringRef_createFromViewWithContext(GblStringBuffer_view(pString),
                                                               GblStringBuffer_context(pString));
    GBL_API_END();
}

static GBL_RESULT stringSet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_POINTER_TYPE);
    if(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY) {
        GBL_API_VERIFY_ARG(argc == 1);
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GblStringRef_release(pVariant->pString);
        pVariant->pString = pArgs[0].pVoid? GblStringRef_create(pArgs[0].pVoid) :
                                            NULL;
    } else if(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
        GBL_API_VERIFY_ARG(argc == 1);
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GblStringRef_release(pVariant->pString);
        pVariant->pString = pArgs[0].pVoid? pArgs[0].pVoid :
                                            NULL;

    } else if(op & GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GBL_API_VERIFY_ARG(argc == 1);
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_STRING_TYPE);
        GblStringRef_release(pVariant->pString);
        pVariant->pString = pArgs[0].pString? GblStringRef_acquire(pArgs[0].pString) :
                                              NULL;

    } else if(op & GBL_IVARIANT_OP_FLAG_SET_MOVE) {
        GBL_API_VERIFY_ARG(argc == 1);
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_STRING_TYPE);
        GblStringRef_release(pVariant->pString);
        if(pArgs[0].pString) {
            pVariant->pString = pArgs[0].pString;
            GBL_API_CALL(GblVariant_invalidate(&pArgs[0]));
        }
    }
    GBL_API_END();
}

static GBL_RESULT stringGet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    GBL_API_VERIFY_POINTER(pArgs[0].pVoid);
    if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY) {
        *((const char**)pArgs->pVoid) = GblStringView_strdup(GblStringRef_view(pVariant->pString));
    } else if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK) {
        *((const char**)pArgs->pVoid) = pVariant->pString? pVariant->pString : "";
    } else if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE) {
        *((GblStringRef**)pArgs->pVoid) = pVariant->pString;
        GBL_API_CALL(GblVariant_invalidate(pVariant));

    }
    GBL_API_END();
}

static GBL_RESULT stringCompare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_VERIFY_POINTER(pResult);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_STRING_TYPE);
    GBL_API_VERIFY_TYPE(pOther->type, GBL_STRING_TYPE);
    *pResult = GblStringView_compare(GBL_STRV(pVariant->pString), GBL_STRV(pOther->pString));
    GBL_API_END();
}

static GBL_RESULT stringConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, GblStringView_toBool(GblStringRef_view(pVariant->pString)));
    //else if(type == GBL_CHAR_TYPE)
    //    GblVariant_setChar(pOther, GblStringBuffer_cString(&pVariant->string)[0]);
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, GblStringView_toUint(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, GblStringView_toUint(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, GblStringView_toInt(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, GblStringView_toUint(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, GblStringView_toInt(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, GblStringView_toUint(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, GblStringView_toInt(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_FLOAT_TYPE)
        GblVariant_setFloat(pOther, GblStringView_toFloat(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_DOUBLE_TYPE)
        GblVariant_setDouble(pOther, GblStringView_toDouble(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_TYPE_TYPE)
        GblVariant_setTypeValue(pOther, GblType_fromName(GblVariant_getString(pVariant)));
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT typeSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblStringBuffer_append(pString,
                                        GBL_STRV(GblType_name(pVariant->typeValue))));
    GBL_API_END();
}

static GBL_RESULT typeLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    pVariant->typeValue = GblType_fromName(GblStringBuffer_cString(pString));
    GBL_API_END();
}

static GBL_RESULT typeSet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_POINTER_TYPE);
    pVariant->typeValue = (GblType)pArgs->pVoid;
    GBL_API_END();
}

static GBL_RESULT typeGet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((GblType*)pArgs->pVoid) = pVariant->typeValue;
    GBL_API_END();
}


static GBL_RESULT typeCompare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    if(pVariant->typeValue > pOther->typeValue)         *pResult = 1;
    else if (pVariant->typeValue < pOther->typeValue)   *pResult = -1;
    else                                                *pResult = 0;
    GBL_API_END();
}

static GBL_RESULT typeConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->typeValue != GBL_INVALID_TYPE);
    else if(type == GBL_STRING_TYPE) {
        GblVariant_setString(pOther, GblType_name(pVariant->typeValue));
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}


static GBL_RESULT gblValueTypesRegisterConverters_(GblContext* pCtx);

extern GBL_RESULT gblValueTypesRegister_(GblContext* pCtx) {

    GBL_API_BEGIN(pCtx);
    GBL_API_PUSH_VERBOSE("[GblType] Registering Builtin Types");

    const GblType iVariantType = GBL_IVARIANT_TYPE;

    // =============== NIL ===============
    static GblIVariantIFace nilIVariantIFace = {
        .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE |
                        GBL_IVARIANT_OP_FLAG_VALUELESS_TYPE,
        .pFnDestruct = NULL,
        .pFnSave = nilSave_,
        .pFnLoad = nilLoad_
    };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_NIL,
        GBL_INVALID_TYPE,
        GblQuark_internStringStatic("nil"),
        &(const GblTypeInfo) {
            .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
            .classSize    = sizeof(GblPrimitiveClass),
            .pClassData   = &nilIVariantIFace,
            .interfaceCount = 1,
            .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                .interfaceType  = iVariantType,
                .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
            }
        },
        GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== BOOL ===============
    static GblIVariantIFace boolIVariantIFace = {
            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
            .pSetValueFmt   = { "i"},
            .pGetValueFmt   = { "p" },
            .pFnSet         = boolSet_,
            .pFnGet         = boolGet_,
            .pFnCompare     = boolCompare_,
            .pFnSave        = boolSave_,
            .pFnLoad        = boolLoad_
      };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_BOOL,
      GBL_INVALID_TYPE,
      GblQuark_internStringStatic("bool"),
      &(const GblTypeInfo) {
          .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
          .classSize    = sizeof(GblPrimitiveClass),
          .pClassData   = &boolIVariantIFace,
          .interfaceCount = 1,
          .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
             .interfaceType   = iVariantType,
             .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
        }
      },
      GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== CHAR ===============
    static const GblIVariantIFace charIVariantIFace = {
            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
            .pSetValueFmt   = { "i"},
            .pGetValueFmt   = { "p" },
            .pFnSet         = charSet_,
            .pFnGet         = charGet_,
            .pFnCompare     = charCompare_,
            .pFnSave        = charSave_,
            .pFnLoad        = charLoad_
      };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_CHAR,
          GBL_INVALID_TYPE,
          GblQuark_internStringStatic("char"),
          &(const GblTypeInfo) {
              .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
              .classSize    = sizeof(GblPrimitiveClass),
              .pClassData   = &charIVariantIFace,
              .interfaceCount = 1,
              .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                   .interfaceType   = iVariantType,
                   .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
              }
          },
          GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

     // =============== UINT8 ===============
    static const GblIVariantIFace uint8IVariantIFace = {
            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
            .pSetValueFmt   = { "i"},
            .pGetValueFmt   = { "p" },
            .pFnSet         = u8Set_,
            .pFnGet         = u8Get_,
            .pFnCompare     = u8Compare_,
            .pFnSave        = u8Save_,
            .pFnLoad        = u8Load_
    };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_UINT8,
      GBL_INVALID_TYPE,
      GblQuark_internStringStatic("uint8"),
      &(const GblTypeInfo) {
          .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
          .classSize    = sizeof(GblPrimitiveClass),
          .pClassData   = &uint8IVariantIFace,
          .interfaceCount = 1,
          .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
               .interfaceType   = iVariantType,
               .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
          }
      },
      GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== INT16 ===============
    static const GblIVariantIFace int16IVariantIFace = {
        .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
        .pSetValueFmt   = { "i"},
        .pGetValueFmt   = { "p" },
        .pFnSet         = i16Set_,
        .pFnGet         = i16Get_,
        .pFnCompare     = i16Compare_,
        .pFnSave        = i16Save_,
        .pFnLoad        = i16Load_
    };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_INT16,
          GBL_INVALID_TYPE,
          GblQuark_internStringStatic("int16"),
          &(const GblTypeInfo) {
              .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
              .classSize    = sizeof(GblPrimitiveClass),
              .pClassData   = &int16IVariantIFace,
              .interfaceCount = 1,
              .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                   .interfaceType   = iVariantType,
                   .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
              }
          },
          GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== UNT16 ===============
    static const GblIVariantIFace uint16IVariantIFace = {
        .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
        .pSetValueFmt   = { "i"},
        .pGetValueFmt   = { "p" },
        .pFnSet         = u16Set_,
        .pFnGet         = u16Get_,
        .pFnCompare     = u16Compare_,
        .pFnSave        = u16Save_,
        .pFnLoad        = u16Load_
    };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_UINT16,
          GBL_INVALID_TYPE,
          GblQuark_internStringStatic("uint16"),
          &(const GblTypeInfo) {
              .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
              .classSize    = sizeof(GblPrimitiveClass),
              .pClassData   = &uint16IVariantIFace,
              .interfaceCount = 1,
              .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                   .interfaceType  = iVariantType,
                   .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
              }
          },
          GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== INT32 ===============
    const static GblIVariantIFace int32IVariantIFace = {
            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
            .pSetValueFmt   = { "i"},
            .pGetValueFmt   = { "p" },
            .pFnSet         = i32Set_,
            .pFnGet         = i32Get_,
            .pFnCompare     = i32Compare_,
            .pFnSave        = i32Save_,
            .pFnLoad        = i32Load_
      };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_INT32,
          GBL_INVALID_TYPE,
          GblQuark_internStringStatic("int32"),
          &(const GblTypeInfo) {
              .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
              .classSize    = sizeof(GblPrimitiveClass),
              .pClassData   = &int32IVariantIFace,
              .interfaceCount = 1,
              .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                   .interfaceType  = iVariantType,
                   .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
              }
          },
          GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== UINT32 ===============
    const static GblIVariantIFace uint32IVariantIFace = {
        .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
        .pSetValueFmt   = { "l"},
        .pGetValueFmt   = { "p" },
        .pFnSet         = u32Set_,
        .pFnGet         = u32Get_,
        .pFnCompare     = u32Compare_,
        .pFnSave        = u32Save_,
        .pFnLoad        = u32Load_
    };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_UINT32,
          GBL_INVALID_TYPE,
          GblQuark_internStringStatic("uint32"),
          &(const GblTypeInfo) {
              .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
              .classSize    = sizeof(GblPrimitiveClass),
              .pClassData   = &uint32IVariantIFace,
              .interfaceCount = 1,
              .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                   .interfaceType  = iVariantType,
                   .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
              }
          },
          GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== INT64 ===============
    const static GblIVariantIFace int64IVariantIFace =  {
        .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
        .pSetValueFmt   = { "q"},
        .pGetValueFmt   = { "p" },
        .pFnSet         = i64Set_,
        .pFnGet         = i64Get_,
        .pFnCompare     = i64Compare_,
        .pFnSave        = i64Save_,
        .pFnLoad        = i64Load_
    };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_INT64,
          GBL_INVALID_TYPE,
          GblQuark_internStringStatic("int64"),
          &(const GblTypeInfo) {
              .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
              .classSize    = sizeof(GblPrimitiveClass),
              .pClassData   = &int64IVariantIFace,
              .interfaceCount = 1,
              .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                   .interfaceType  = iVariantType,
                   .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
              }
          },
          GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== UINT64 ===============
    const static GblIVariantIFace uint64IVariantIFace =  {
            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
            .pSetValueFmt   = { "z"},
            .pGetValueFmt   = { "p" },
            .pFnSet         = u64Set_,
            .pFnGet         = u64Get_,
            .pFnCompare     = u64Compare_,
            .pFnSave        = u64Save_,
            .pFnLoad        = u64Load_
      };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_UINT64,
          GBL_INVALID_TYPE,
          GblQuark_internStringStatic("uint64"),
          &(const GblTypeInfo) {
              .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
              .classSize    = sizeof(GblPrimitiveClass),
              .pClassData   = &uint64IVariantIFace,
              .interfaceCount = 1,
              .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                   .interfaceType  = iVariantType,
                   .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
              }
          },
          GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== FLOAT ===============
    static const GblIVariantIFace floatIVariantIFace = {
        .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
        .pSetValueFmt   = { "d"},
        .pGetValueFmt   = { "p" },
        .pFnSet         = f32Set_,
        .pFnGet         = f32Get_,
        .pFnCompare     = f32Compare_,
        .pFnSave        = f32Save_,
        .pFnLoad        = f32Load_
    };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_FLOAT,
                  GBL_INVALID_TYPE,
                  GblQuark_internStringStatic("float"),
                  &(const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .pClassData   = &floatIVariantIFace,
                      .interfaceCount = 1,
                      .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                           .interfaceType   = iVariantType,
                           .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
                      }
                   },
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== DOUBLE ===============
    static const GblIVariantIFace doubleIVariantIFace =  {
            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
            .pSetValueFmt   = { "d"},
            .pGetValueFmt   = { "p" },
            .pFnSet         = f64Set_,
            .pFnGet         = f64Get_,
            .pFnCompare     = f64Compare_,
            .pFnSave        = f64Save_,
            .pFnLoad        = f64Load_
      };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_DOUBLE,
                  GBL_INVALID_TYPE,
                  GblQuark_internStringStatic("double"),
                  &(const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .pClassData   = &doubleIVariantIFace,
                      .interfaceCount = 1,
                      .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                           .interfaceType  = iVariantType,
                           .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
                      }
                  },
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== POINTER ===============
    static const GblIVariantIFace pointerIVariantIFace =  {
        .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
        .pSetValueFmt   = { "p"},
        .pGetValueFmt   = { "p" },
        .pFnSet         = pSet_,
        .pFnGet         = pGet_,
        .pFnCompare     = pCompare_,
        .pFnSave        = pSave_,
        .pFnLoad        = pLoad_
    };

    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_POINTER,
                  GBL_INVALID_TYPE,
                  GblQuark_internStringStatic("pointer"),
                  &(const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .pClassData   = &pointerIVariantIFace,
                      .interfaceCount = 1,
                      .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                           .interfaceType  = iVariantType,
                           .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
                      }
                  },
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== STRING ===============
    static const GblIVariantIFace stringIface = {
        .supportedOps = GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT      |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY         |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE         |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY   |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE   |
                        GBL_IVARIANT_OP_FLAG_SET_COPY               |
                        GBL_IVARIANT_OP_FLAG_SET_MOVE               |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY         |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE         |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY         |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK         |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE,
        .pSetValueFmt   = { "p"},
        .pGetValueFmt   = { "p" },
        .pFnConstruct   = stringConstruct_,
        .pFnDestruct    = stringDestruct_,
        .pFnSet         = stringSet_,
        .pFnGet         = stringGet_,
        .pFnCompare     = stringCompare_,
        .pFnSave        = stringSave_,
        .pFnLoad        = stringLoad_
    };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_STRING,
                  GBL_INVALID_TYPE,
                  GblQuark_internStringStatic("string"),
                  &(const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .pClassData   = &stringIface,
                      .interfaceCount = 1,
                      .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                             .interfaceType   = iVariantType,
                             .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
                      }
                  },
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== TYPE ===============
    const static GblIVariantIFace typeIVariantIFace =  {
            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
            .pSetValueFmt   = { "p"},
            .pGetValueFmt   = { "p" },
            .pFnSet         = typeSet_,
            .pFnGet         = typeGet_,
            .pFnCompare     = typeCompare_,
            .pFnSave        = typeSave_,
            .pFnLoad        = typeLoad_
      };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_TYPE,
          GBL_INVALID_TYPE,
          GblQuark_internStringStatic("type"),
          &(const GblTypeInfo) {
              .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
              .classSize    = sizeof(GblPrimitiveClass),
              .pClassData   = &typeIVariantIFace,
              .interfaceCount = 1,
              .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                   .interfaceType  = iVariantType,
                   .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
              }
          },
          GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== FLAGS  ===============
    static const GblIVariantIFace flagsIVariantIFace =  {
        .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
        .pSetValueFmt = { "q"},
        .pGetValueFmt = { "p" },
        .pFnSet  = u64Set_,
        .pFnGet  = u64Get_,
        .pFnSave = u64Save_,
        .pFnLoad = u64Load_
    };
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_FLAGS,
                  GBL_INVALID_TYPE,
                  GblQuark_internStringStatic("flags"),
                  &(const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .pClassData   = &flagsIVariantIFace,
                      .interfaceCount = 1,
                      .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                           .interfaceType  = iVariantType,
                           .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
                      }
                  },
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);

    // =============== BOXED ===============
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_BOXED,
                               GBL_INVALID_TYPE,
                               GblQuark_internStringStatic("boxed"),
                               &(const GblTypeInfo) {
                                     .classSize = 0
                               },
                               GBL_TYPE_FLAG_ABSTRACT);

    GBL_API_CALL(gblValueTypesRegisterConverters_(pCtx));

    GBL_API_POP(1);
    GBL_API_END();
}

static GBL_RESULT gblValueTypesRegisterConverters_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_PUSH_VERBOSE("[GblType] Registering Primitive Type Converters");

    // =============== NIL ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_BOOL_TYPE, nilConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_CHAR_TYPE, nilConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_UINT8_TYPE, nilConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_UINT16_TYPE, nilConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_INT16_TYPE, nilConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_UINT32_TYPE, nilConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_INT32_TYPE, nilConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_UINT64_TYPE, nilConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_INT64_TYPE, nilConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_POINTER_TYPE, nilConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_STRING_TYPE, nilConvert_));

    // =============== BOOL ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_CHAR_TYPE, boolConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_UINT8_TYPE, boolConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_UINT16_TYPE, boolConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_INT16_TYPE, boolConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_UINT32_TYPE, boolConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_INT32_TYPE, boolConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_UINT64_TYPE, boolConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_INT64_TYPE, boolConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_STRING_TYPE, boolConvert_));

    // =============== CHAR ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_BOOL_TYPE, charConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_UINT8_TYPE, charConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_UINT16_TYPE, charConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_INT16_TYPE, charConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_UINT32_TYPE, charConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_INT32_TYPE, charConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_UINT64_TYPE, charConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_INT64_TYPE, charConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_STRING_TYPE, charConvert_));

    // =============== UINT8 ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_BOOL_TYPE, u8Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_CHAR_TYPE, u8Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_UINT16_TYPE, u8Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_INT16_TYPE, u8Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_UINT32_TYPE, u8Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_INT32_TYPE, u8Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_UINT64_TYPE, u8Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_INT64_TYPE, u8Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_FLOAT_TYPE, u8Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_DOUBLE_TYPE, u8Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_STRING_TYPE, u8Convert_));

    // =============== INT16 ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_BOOL_TYPE, i16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_CHAR_TYPE, i16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_UINT8_TYPE, i16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_UINT16_TYPE, i16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_UINT32_TYPE, i16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_INT32_TYPE, i16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_UINT64_TYPE, i16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_INT64_TYPE, i16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_FLOAT_TYPE, i16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_DOUBLE_TYPE, i16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_STRING_TYPE, i16Convert_));

    // =============== UNT16 ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_BOOL_TYPE, u16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_CHAR_TYPE, u16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_UINT8_TYPE, u16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_INT16_TYPE, u16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_UINT32_TYPE, u16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_INT32_TYPE, u16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_UINT64_TYPE, u16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_INT64_TYPE, u16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_FLOAT_TYPE, u16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_DOUBLE_TYPE, u16Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_STRING_TYPE, u16Convert_));

    // =============== INT32 ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_BOOL_TYPE, i32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_CHAR_TYPE, i32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_UINT8_TYPE, i32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_UINT16_TYPE, i32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_INT16_TYPE, i32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_UINT32_TYPE, i32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_UINT64_TYPE, i32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_INT64_TYPE, i32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_FLOAT_TYPE, i32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_DOUBLE_TYPE, i32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_STRING_TYPE, i32Convert_));

    // =============== UINT32 ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_BOOL_TYPE, u32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_CHAR_TYPE, u32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_UINT8_TYPE, u32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_UINT16_TYPE, u32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_INT16_TYPE, u32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_INT32_TYPE, u32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_UINT64_TYPE, u32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_INT64_TYPE, u32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_FLOAT_TYPE, u32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_DOUBLE_TYPE, u32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_STRING_TYPE, u32Convert_));

    // =============== INT64 ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_BOOL_TYPE, i64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_CHAR_TYPE, i64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_UINT8_TYPE, i64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_UINT16_TYPE, i64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_INT16_TYPE, i64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_UINT32_TYPE, i64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_INT32_TYPE, i64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_UINT64_TYPE, i64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_FLOAT_TYPE, i64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_DOUBLE_TYPE, i64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_STRING_TYPE, i64Convert_));

    // =============== UINT64 ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_BOOL_TYPE, u64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_CHAR_TYPE, u64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_UINT8_TYPE, u64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_UINT16_TYPE, u64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_INT16_TYPE, u64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_UINT32_TYPE, u64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_INT32_TYPE, u64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_INT64_TYPE, u64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_FLOAT_TYPE, u64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_DOUBLE_TYPE, u64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_STRING_TYPE, u64Convert_));

    // =============== FLOAT ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_BOOL_TYPE, f32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_UINT8_TYPE, f32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_UINT16_TYPE, f32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_INT16_TYPE, f32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_UINT32_TYPE, f32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_INT32_TYPE, f32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_UINT64_TYPE, f32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_INT64_TYPE, f32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_DOUBLE_TYPE, f32Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_STRING_TYPE, f32Convert_));

    // =============== DOUBLE ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_BOOL_TYPE, f64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_UINT8_TYPE, f64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_UINT16_TYPE, f64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_INT16_TYPE, f64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_UINT32_TYPE, f64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_INT32_TYPE, f64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_UINT64_TYPE, f64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_INT64_TYPE, f64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_FLOAT_TYPE, f64Convert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_STRING_TYPE, f64Convert_));

    // =============== POINTER ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_POINTER_TYPE, GBL_BOOL_TYPE, pConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_POINTER_TYPE, GBL_STRING_TYPE, pConvert_));

    // =============== STRING ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_BOOL_TYPE, stringConvert_));
    //GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_CHAR_TYPE, stringConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_UINT8_TYPE, stringConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_UINT16_TYPE, stringConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_INT16_TYPE, stringConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_UINT32_TYPE, stringConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_INT32_TYPE, stringConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_UINT64_TYPE, stringConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_INT64_TYPE, stringConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_FLOAT_TYPE, stringConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_DOUBLE_TYPE, stringConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_TYPE_TYPE, stringConvert_));

    // =============== TYPE ===============
    GBL_API_CALL(GblVariant_registerConverter(GBL_TYPE_TYPE, GBL_BOOL_TYPE, typeConvert_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_TYPE_TYPE, GBL_STRING_TYPE, typeConvert_));

    GBL_API_POP(1);
    GBL_API_END();
}
