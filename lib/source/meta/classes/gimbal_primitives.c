#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/meta/ifaces/gimbal_ivariant.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/strings/gimbal_quark.h>
#include <gimbal/meta/types/gimbal_pointer.h>
#include <inttypes.h>
#include "../types/gimbal_type_.h"

GBL_RESULT GblPrimitiveClass_init_(GblPrimitiveClass* pClass, GblIVariantClassVTable* pVTable, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    pClass->GblIVariantImpl.pVTable = pVTable;
    GBL_CTX_END();
}

static GBL_RESULT nilSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_EXPRESSION(GblVariant_typeOf(pVariant) == GBL_NIL_TYPE);
    GBL_CTX_CALL(GblStringBuffer_appendNil(pString));
    GBL_CTX_END();
}

static GBL_RESULT nilLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GblBool isNil = GBL_FALSE;
    GBL_CTX_BEGIN(GblStringBuffer_context(pString));
    isNil = GblStringView_toNil(GblStringBuffer_view(pString));
    GBL_CTX_VERIFY_EXPRESSION(isNil);
    GBL_CTX_VERIFY_TYPE(pVariant->type, GBL_NIL_TYPE);
    GBL_CTX_END();
}

static GBL_RESULT nilConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_UNUSED(pVariant);
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
    else if(type == GBL_STRING_TYPE)
        GblVariant_setString(pOther, "nil");
    else
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT boolSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_EXPRESSION(GblVariant_typeOf(pVariant) == GBL_NIL_TYPE);
    GBL_CTX_CALL(GblStringBuffer_appendBool(pString, pVariant->boolean));
    GBL_CTX_END();
}

static GBL_RESULT boolLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(GblStringBuffer_context(pString));
    pVariant->boolean = GblStringView_toBool(GblStringBuffer_view(pString));
    GBL_CTX_END();
}

static GBL_RESULT boolSet_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    pVariant->boolean = pArgs->i32? GBL_TRUE : GBL_FALSE;
    GBL_CTX_END();
}

static GBL_RESULT boolGet_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((GblBool*)pArgs->pVoid) = pVariant->boolean;
    GBL_CTX_END();
}

static GBL_RESULT boolCompare_(const GblVariant* pSelf, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    *pResult = pSelf->boolean - pOther->boolean;
    GBL_CTX_END();
}

static GBL_RESULT boolConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT charSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "'%c'", pVariant->character);
    GBL_CTX_END();
}

static GBL_RESULT charLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    const char* pStr = NULL;
    size_t  length = GblStringBuffer_length(pString);
    if(length == 3) {
        pStr = GblStringBuffer_cString(pString);
        GBL_CTX_VERIFY_EXPRESSION(pStr[0] == '\'');
        GBL_CTX_VERIFY_EXPRESSION(pStr[2] == '\'');
        pVariant->character = pStr[1];
    }
    GBL_CTX_END();
}

static GBL_RESULT charSet_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_INT32_TYPE);
    pVariant->character = pArgs->i32;
    GBL_CTX_END();
}

static GBL_RESULT charGet_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((char*)pArgs->pVoid) = pVariant->character;
    GBL_CTX_END();
}

static GBL_RESULT charCompare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    *pResult = pVariant->character - pOther->character;
    GBL_CTX_END();
}

static GBL_RESULT charConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}


static GBL_RESULT u8Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%u", pVariant->u8);
    GBL_CTX_END();
}

static GBL_RESULT u8Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    int integer = GblStringView_toUint8(GblStringBuffer_view(pString));
    pVariant->u8 = (uint8_t)integer;
    GBL_CTX_END();
}

static GBL_RESULT u8Set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_INT32_TYPE);
    pVariant->u8 = (uint8_t)pArgs->i32;
    GBL_CTX_END();
}

static GBL_RESULT u8Get_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((uint8_t*)pArgs->pVoid) = pVariant->u8;
    GBL_CTX_END();
}

static GBL_RESULT u8Compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    *pResult = pVariant->u8 - pOther->u8;
    GBL_CTX_END();
}

static GBL_RESULT u8Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}


static GBL_RESULT i16Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%d", pVariant->i16);
    GBL_CTX_END();
}

static GBL_RESULT i16Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    int integer = GblStringView_toInt16(GblStringBuffer_view(pString));
    pVariant->i16 = (int16_t)integer;
    GBL_CTX_END();
}

static GBL_RESULT i16Set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_INT32_TYPE);
    pVariant->i16 = (int16_t)pArgs->i32;
    GBL_CTX_END();
}

static GBL_RESULT i16Get_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((int16_t*)pArgs->pVoid) = pVariant->i16;
    GBL_CTX_END();
}

static GBL_RESULT i16Compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    *pResult = pVariant->i16 - pOther->i16;
    GBL_CTX_END();
}

static GBL_RESULT i16Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT u16Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%u", pVariant->u16);
    GBL_CTX_END();
}

static GBL_RESULT u16Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    int integer = GblStringView_toUint16(GblStringBuffer_view(pString));
    pVariant->u16 = (uint16_t)integer;
    GBL_CTX_END();
}

static GBL_RESULT u16Set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_INT32_TYPE);
    pVariant->u16 = (uint16_t)pArgs->i32;
    GBL_CTX_END();
}

static GBL_RESULT u16Get_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((uint16_t*)pArgs->pVoid) = pVariant->u16;
    GBL_CTX_END();
}

static GBL_RESULT u16Compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    *pResult = pVariant->u16 - pOther->u16;
    GBL_CTX_END();
}

static GBL_RESULT u16Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT i32Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%d", pVariant->i32);
    GBL_CTX_END();
}

static GBL_RESULT i32Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    int integer = GblStringView_toInt32(GblStringBuffer_view(pString));
    pVariant->i32 = (int32_t)integer;
    GBL_CTX_END();
}

static GBL_RESULT i32Set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_INT32_TYPE);
    pVariant->i32 = (int32_t)pArgs->i32;
    GBL_CTX_END();
}

static GBL_RESULT i32Get_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((int32_t*)pArgs->pVoid) = pVariant->i32;
    GBL_CTX_END();
}

static GBL_RESULT i32Compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    *pResult = pVariant->i32 - pOther->i32;
    GBL_CTX_END();
}

static GBL_RESULT i32Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}


static GBL_RESULT u32Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%u", pVariant->u32);
    GBL_CTX_END();
}

static GBL_RESULT u32Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    int integer = GblStringView_toUint32(GblStringBuffer_view(pString));
    pVariant->u32 = (uint32_t)integer;
    GBL_CTX_END();
}

static GBL_RESULT u32Set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_UINT32_TYPE);
    pVariant->u32 = (uint32_t)pArgs->u32;
    GBL_CTX_END();
}

static GBL_RESULT u32Get_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY|GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((uint32_t*)pArgs->pVoid) = pVariant->u32;
    GBL_CTX_END();
}

static GBL_RESULT u32Compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    *pResult = pVariant->u32 - pOther->u32;
    GBL_CTX_END();
}

static GBL_RESULT u32Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT i64Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%d", pVariant->i64);
    GBL_CTX_END();
}

static GBL_RESULT i64Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    int integer = GblStringView_toInt64(GblStringBuffer_view(pString));
    pVariant->i64 = (int64_t)integer;
    GBL_CTX_END();
}

static GBL_RESULT i64Set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_INT64_TYPE);
    pVariant->i64 = (int64_t)pArgs->i64;
    GBL_CTX_END();
}

static GBL_RESULT i64Get_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((int64_t*)pArgs->pVoid) = pVariant->i64;
    GBL_CTX_END();
}

static GBL_RESULT i64Compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    if(pVariant->i64 > pOther->i64)         *pResult = 1;
    else if (pVariant->i64 < pOther->i64)   *pResult = -1;
    else                                    *pResult = 0;
    GBL_CTX_END();
}

static GBL_RESULT i64Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT u64Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblStringBuffer_appendPrintf(pString, "%u", pVariant->u64);
    GBL_CTX_END();
}

static GBL_RESULT u64Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    int integer = GblStringView_toUint64(GblStringBuffer_view(pString));
    pVariant->u64 = (uint64_t)integer;
    GBL_CTX_END();
}

static GBL_RESULT u64Set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_UINT64_TYPE);
    pVariant->u64 = pArgs->u64;
    GBL_CTX_END();
}

static GBL_RESULT u64Get_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((uint64_t*)pArgs->pVoid) = pVariant->u64;
    GBL_CTX_END();
}

static GBL_RESULT u64Compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    if(pVariant->u64 > pOther->u64)         *pResult = 1;
    else if (pVariant->u64 < pOther->u64)   *pResult = -1;
    else                                    *pResult = 0;
    GBL_CTX_END();
}

static GBL_RESULT u64Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT f32Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblStringBuffer_appendFloat(pString, pVariant->f32));
    GBL_CTX_END();
}

static GBL_RESULT f32Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    pVariant->f32 = GblStringView_toFloat(GblStringBuffer_view(pString));
    GBL_CTX_END();
}

static GBL_RESULT f32Set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_DOUBLE_TYPE);
    pVariant->f32 = (float)pArgs->f64;
    GBL_CTX_END();
}

static GBL_RESULT f32Get_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((float*)pArgs->pVoid) = pVariant->f32;
    GBL_CTX_END();
}

static GBL_RESULT f32Compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    if(pVariant->f32 > pOther->f32)         *pResult = 1;
    else if (pVariant->f32 < pOther->f32)   *pResult = -1;
    else                                    *pResult = 0;
    GBL_CTX_END();
}

static GBL_RESULT f32Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT f64Save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblStringBuffer_appendDouble(pString, pVariant->f64));
    GBL_CTX_END();
}

static GBL_RESULT f64Load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    pVariant->f64 = GblStringView_toDouble(GblStringBuffer_view(pString));
    GBL_CTX_END();
}

static GBL_RESULT f64Set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_DOUBLE_TYPE);
    pVariant->f64 = pArgs->f64;
    GBL_CTX_END();
}

static GBL_RESULT f64Get_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((double*)pArgs->pVoid) = pVariant->f64;
    GBL_CTX_END();
}

static GBL_RESULT f64Compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    if(pVariant->f64 > pOther->f64)         *pResult = 1;
    else if (pVariant->f64 < pOther->f64)   *pResult = -1;
    else                                    *pResult = 0;
    GBL_CTX_END();
}

static GBL_RESULT f64Convert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT stringConstruct_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pVariant);

    // Default constructor
    if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
        pVariant->pString = NULL;

    // Copy constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY) {
        GBL_CTX_VERIFY_ARG(argc == 1);
        GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(&pArgs[0]), GBL_STRING_TYPE);
        if(pArgs[0].pString)
            pVariant->pString = GblStringRef_acquire(pArgs[0].pString);
    // Move constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE) {
        GBL_CTX_VERIFY_ARG(argc == 1);
        GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(&pArgs[0]), GBL_STRING_TYPE);
        if(pArgs[0].pString) {
            pVariant->pString   = pArgs[0].pString;
            GblVariant_invalidate(&pArgs[0]);
        }
    // Value copying constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY) {
        GBL_CTX_VERIFY_ARG(argc == 1);
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        if(pArgs[0].pVoid) {
            pVariant->pString = GblStringRef_create(pArgs[0].pVoid);
        }
    // Value moving constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE) {
        GBL_CTX_VERIFY_ARG(argc == 1);
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_CTX_VERIFY_POINTER(pArgs[0].pVoid);
        GblStringRef_release(pVariant->pString);
        pVariant->pString = pArgs[0].pVoid;
    }
    GBL_CTX_END();
}

static GBL_RESULT stringDestruct_(GblVariant* pVariant) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(pVariant->type, GBL_STRING_TYPE);
    GblStringRef_release(pVariant->pString);
    GBL_CTX_END();
}

static GBL_RESULT stringSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblStringBuffer_set(pString, GblStringRef_view(pVariant->pString)));
    GBL_CTX_END();
}

static GBL_RESULT stringLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblStringRef_release(pVariant->pString);
    pVariant->pString = GblStringRef_createFromViewWithContext(GblStringBuffer_view(pString),
                                                               GblStringBuffer_context(pString));
    GBL_CTX_END();
}

static GBL_RESULT stringSet_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    if(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY) {
        GBL_CTX_VERIFY_ARG(argc == 1);
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GblStringRef_release(pVariant->pString);
        pVariant->pString = pArgs[0].pVoid? GblStringRef_create(pArgs[0].pVoid) :
                                            NULL;
    } else if(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
        GBL_CTX_VERIFY_ARG(argc == 1);
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GblStringRef_release(pVariant->pString);
        pVariant->pString = pArgs[0].pVoid? pArgs[0].pVoid :
                                            NULL;

    } else if(op & GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GBL_CTX_VERIFY_ARG(argc == 1);
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_STRING_TYPE);
        GblStringRef_release(pVariant->pString);
        pVariant->pString = pArgs[0].pString? GblStringRef_acquire(pArgs[0].pString) :
                                              NULL;

    } else if(op & GBL_IVARIANT_OP_FLAG_SET_MOVE) {
        GBL_CTX_VERIFY_ARG(argc == 1);
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_STRING_TYPE);
        GblStringRef_release(pVariant->pString);
        if(pArgs[0].pString) {
            pVariant->pString = pArgs[0].pString;
            GBL_CTX_CALL(GblVariant_invalidate(&pArgs[0]));
        }
    }
    GBL_CTX_END();
}

static GBL_RESULT stringGet_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    GBL_CTX_VERIFY_POINTER(pArgs[0].pVoid);
    if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY) {
        *((const char**)pArgs->pVoid) = GblStringView_strdup(GblStringRef_view(pVariant->pString));
    } else if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK) {
        *((const char**)pArgs->pVoid) = pVariant->pString? pVariant->pString : "";
    } else if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE) {
        *((GblStringRef**)pArgs->pVoid) = pVariant->pString;
        GBL_CTX_CALL(GblVariant_invalidate(pVariant));

    }
    GBL_CTX_END();
}

static GBL_RESULT stringCompare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pVariant);
    GBL_CTX_VERIFY_POINTER(pOther);
    GBL_CTX_VERIFY_POINTER(pResult);
    GBL_CTX_VERIFY_TYPE(pVariant->type, GBL_STRING_TYPE);
    GBL_CTX_VERIFY_TYPE(pOther->type, GBL_STRING_TYPE);
    *pResult = GblStringView_compare(GBL_STRV(pVariant->pString), GBL_STRV(pOther->pString));
    GBL_CTX_END();
}

static GBL_RESULT stringConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, GblStringView_toBool(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_CHAR_TYPE) {
        const size_t  len = GblStringRef_length(pVariant->pString);
        GBL_CTX_VERIFY(len <= 1,
                       GBL_RESULT_ERROR_INVALID_CONVERSION);
        if(GblStringRef_length(pVariant->pString) == 1)
            GblVariant_setChar(pOther, pVariant->pString[0]);
        else GblVariant_setChar(pOther, '\0');
    }
    else if(type == GBL_UINT8_TYPE)
        GblVariant_setUint8(pOther, GblStringView_toUint8(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_UINT16_TYPE)
        GblVariant_setUint16(pOther, GblStringView_toUint16(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_INT16_TYPE)
        GblVariant_setInt16(pOther, GblStringView_toInt16(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_UINT32_TYPE)
        GblVariant_setUint32(pOther, GblStringView_toUint32(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_INT32_TYPE)
        GblVariant_setInt32(pOther, GblStringView_toInt32(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_UINT64_TYPE)
        GblVariant_setUint64(pOther, GblStringView_toUint64(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_INT64_TYPE)
        GblVariant_setInt64(pOther, GblStringView_toInt64(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_FLOAT_TYPE)
        GblVariant_setFloat(pOther, GblStringView_toFloat(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_DOUBLE_TYPE)
        GblVariant_setDouble(pOther, GblStringView_toDouble(GblStringRef_view(pVariant->pString)));
    else if(type == GBL_TYPE_TYPE)
        GblVariant_setTypeValue(pOther, GblType_fromName(GblVariant_getString(pVariant)));
    else
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT typeSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblStringBuffer_append(pString,
                                        GBL_STRV(GblType_name(pVariant->typeValue))));
    GBL_CTX_END();
}

static GBL_RESULT typeLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    pVariant->typeValue = GblType_fromName(GblStringBuffer_cString(pString));
    GBL_CTX_END();
}

static GBL_RESULT typeSet_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_POINTER_TYPE);
    pVariant->typeValue = (GblType)pArgs->pVoid;
    GBL_CTX_END();
}

static GBL_RESULT typeGet_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((GblType*)pArgs->pVoid) = pVariant->typeValue;
    GBL_CTX_END();
}


static GBL_RESULT typeCompare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    if(pVariant->typeValue > pOther->typeValue)         *pResult = 1;
    else if (pVariant->typeValue < pOther->typeValue)   *pResult = -1;
    else                                                *pResult = 0;
    GBL_CTX_END();
}

static GBL_RESULT typeConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->typeValue != GBL_INVALID_TYPE);
    else if(type == GBL_STRING_TYPE) {
        GblVariant_setString(pOther, GblType_name(pVariant->typeValue));
    }
    else
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}


GblType GblPrimitive_register(const char*                     pName,
                              size_t                          classSize,
                              size_t                          classPrivateSize,
                              const GblIVariantClassVTable*   pVTable,
                              GblFlags                        typeFlags)
{
    GblType type = GBL_INVALID_TYPE;
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_ARG(classSize >= sizeof(GblPrimitiveClass));

    type = GblType_registerStatic(GblQuark_internString(pName),
                                   GBL_STATIC_CLASS_TYPE,
                                   &(const GblTypeInfo) {
                                       .pFnClassInit        = (GblTypeClassInitializeFn)GblPrimitiveClass_init_,
                                       .classSize           = classSize,
                                       .classPrivateSize    = classPrivateSize,
                                       .pClassData          = pVTable,
                                       .interfaceCount      = 1,
                                       .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                                           .interfaceType  = GBL_IVARIANT_TYPE,
                                           .classOffset    = offsetof(GblPrimitiveClass, GblIVariantImpl)
                                       }
                                   },
                                   typeFlags);

    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return type;
}


GblType GblPrimitive_registerBuiltin(size_t                          index,
                                     const char*                     pName,
                                     size_t                          classSize,
                                     size_t                          classPrivateSize,
                                     const GblIVariantClassVTable*   pVTable,
                                     GblFlags                        typeFlags)
{
    GblType type = GBL_INVALID_TYPE;
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_ARG(classSize >= sizeof(GblPrimitiveClass));

    type = GblType_registerBuiltin_(index,
                                   GBL_STATIC_CLASS_TYPE,
                                   GblQuark_internString(pName),
                                   &(const GblTypeInfo) {
                                       .pFnClassInit        = (GblTypeClassInitializeFn)GblPrimitiveClass_init_,
                                       .classSize           = classSize,
                                       .classPrivateSize    = classPrivateSize,
                                       .pClassData          = pVTable,
                                       .interfaceCount      = 1,
                                       .pInterfaceMap = &(const GblTypeInterfaceMapEntry) {
                                           .interfaceType  = GBL_IVARIANT_TYPE,
                                           .classOffset    = offsetof(GblPrimitiveClass, GblIVariantImpl)
                                       }
                                   },
                                   typeFlags);

    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return type;
}

static GBL_RESULT GblPrimitive_valueTypesRegisterConverters_(GblContext* pCtx);

#define GBL_PRIMITIVE_TYPEINFO_DECL(name, vtable)                                   \
    static GblTypeInfo name = {                                                     \
        .pFnClassInit        = (GblTypeClassInitializeFn)GblPrimitiveClass_init_,   \
        .classSize           = sizeof(GblPrimitiveClass),                           \
        .classPrivateSize    = 0,                                                   \
        .pClassData          = &vtable,                                             \
        .interfaceCount      = 1,                                                   \
        .pInterfaceMap      = &iVariantMapEntry                                     \
    }

#define GBL_PRIMITIVE_REGISTER(name, vtable)                    \
    GBL_PRIMITIVE_TYPEINFO_DECL(name##_typeInfo, vtable)        \
    GblType_registerStatic(GBL_INVALID_TYPE,                    \
                           GblQuark_internStringStatic(#name),  \
                           &name##_typeInfo,                    \
                           GBL_TYPE_FLAG_TYPEINFO_STATIC    |   \
                           GBL_TYPE_FLAG_CLASS_PINNED)          \
    GBL_CTX_VERIFY_LAST_RECORD()

extern GBL_RESULT GblPrimitive_valueTypesRegister_(GblContext* pCtx) {

    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_PUSH_VERBOSE("[GblType] Registering Builtin Types");

    // =============== NIL ===============
    static const GblIVariantClassVTable nilIVariantIFace = {
        .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE |
                        GBL_IVARIANT_OP_FLAG_VALUELESS_TYPE,
        .pFnDestruct = NULL,
        .pFnSave = nilSave_,
        .pFnLoad = nilLoad_
    };

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_NIL,
                                 GblQuark_internStringStatic("nil"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &nilIVariantIFace,
                                 GBL_TYPE_FLAG_CLASS_PINNED |
                                 GBL_TYPE_FLAG_CLASS_PREINIT);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== BOOL ===============
    static const GblIVariantClassVTable boolIVariantIFace = {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_BOOL,
                                 GblQuark_internStringStatic("bool"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &boolIVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== CHAR ===============
    static const GblIVariantClassVTable charIVariantIFace = {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_CHAR,
                                 GblQuark_internStringStatic("char"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &charIVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

     // =============== UINT8 ===============
    static const GblIVariantClassVTable uint8IVariantIFace = {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_UINT8,
                                 GblQuark_internStringStatic("uint8"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &uint8IVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== INT16 ===============
    static const GblIVariantClassVTable int16IVariantIFace = {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_INT16,
                                 GblQuark_internStringStatic("int16"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &int16IVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== UNT16 ===============
    static const GblIVariantClassVTable uint16IVariantIFace = {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_UINT16,
                                 GblQuark_internStringStatic("uint16"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &uint16IVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== INT32 ===============
    const static GblIVariantClassVTable int32IVariantIFace = {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_INT32,
                                 GblQuark_internStringStatic("int32"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &int32IVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== UINT32 ===============
    const static GblIVariantClassVTable uint32IVariantIFace = {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_UINT32,
                                 GblQuark_internStringStatic("uint32"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &uint32IVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== INT64 ===============
    const static GblIVariantClassVTable int64IVariantIFace =  {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_INT64,
                                 GblQuark_internStringStatic("int64"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &int64IVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== UINT64 ===============
    const static GblIVariantClassVTable uint64IVariantIFace =  {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_UINT64,
                                 GblQuark_internStringStatic("uint64"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &uint64IVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== FLOAT ===============
    static const GblIVariantClassVTable floatIVariantIFace = {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_FLOAT,
                                 GblQuark_internStringStatic("float"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &floatIVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== DOUBLE ===============
    static const GblIVariantClassVTable doubleIVariantIFace =  {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_DOUBLE,
                                 GblQuark_internStringStatic("double"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &doubleIVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    // =============== STRING ===============
    static const GblIVariantClassVTable stringIVariantIFace = {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_STRING,
                                 GblQuark_internStringStatic("string"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &stringIVariantIFace,
                                 GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();


    GBL_CTX_CALL(GblPrimitive_valueTypesRegisterConverters_(pCtx));

    GBL_CTX_POP(1);
    GBL_CTX_END();
}

static GBL_RESULT GblPrimitive_valueTypesRegisterConverters_(GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_PUSH_VERBOSE("[GblType] Registering Primitive Type Converters");

    // =============== NIL ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_BOOL_TYPE, nilConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_CHAR_TYPE, nilConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_UINT8_TYPE, nilConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_UINT16_TYPE, nilConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_INT16_TYPE, nilConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_UINT32_TYPE, nilConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_INT32_TYPE, nilConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_UINT64_TYPE, nilConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_INT64_TYPE, nilConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_STRING_TYPE, nilConvert_));

    // =============== BOOL ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_CHAR_TYPE, boolConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_UINT8_TYPE, boolConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_UINT16_TYPE, boolConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_INT16_TYPE, boolConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_UINT32_TYPE, boolConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_INT32_TYPE, boolConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_UINT64_TYPE, boolConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_INT64_TYPE, boolConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_BOOL_TYPE, GBL_STRING_TYPE, boolConvert_));

    // =============== CHAR ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_BOOL_TYPE, charConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_UINT8_TYPE, charConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_UINT16_TYPE, charConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_INT16_TYPE, charConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_UINT32_TYPE, charConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_INT32_TYPE, charConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_UINT64_TYPE, charConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_INT64_TYPE, charConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_CHAR_TYPE, GBL_STRING_TYPE, charConvert_));

    // =============== UINT8 ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_BOOL_TYPE, u8Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_CHAR_TYPE, u8Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_UINT16_TYPE, u8Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_INT16_TYPE, u8Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_UINT32_TYPE, u8Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_INT32_TYPE, u8Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_UINT64_TYPE, u8Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_INT64_TYPE, u8Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_FLOAT_TYPE, u8Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_DOUBLE_TYPE, u8Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT8_TYPE, GBL_STRING_TYPE, u8Convert_));

    // =============== INT16 ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_BOOL_TYPE, i16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_CHAR_TYPE, i16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_UINT8_TYPE, i16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_UINT16_TYPE, i16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_UINT32_TYPE, i16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_INT32_TYPE, i16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_UINT64_TYPE, i16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_INT64_TYPE, i16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_FLOAT_TYPE, i16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_DOUBLE_TYPE, i16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT16_TYPE, GBL_STRING_TYPE, i16Convert_));

    // =============== UNT16 ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_BOOL_TYPE, u16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_CHAR_TYPE, u16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_UINT8_TYPE, u16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_INT16_TYPE, u16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_UINT32_TYPE, u16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_INT32_TYPE, u16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_UINT64_TYPE, u16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_INT64_TYPE, u16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_FLOAT_TYPE, u16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_DOUBLE_TYPE, u16Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT16_TYPE, GBL_STRING_TYPE, u16Convert_));

    // =============== INT32 ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_BOOL_TYPE, i32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_CHAR_TYPE, i32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_UINT8_TYPE, i32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_UINT16_TYPE, i32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_INT16_TYPE, i32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_UINT32_TYPE, i32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_UINT64_TYPE, i32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_INT64_TYPE, i32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_FLOAT_TYPE, i32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_DOUBLE_TYPE, i32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT32_TYPE, GBL_STRING_TYPE, i32Convert_));

    // =============== UINT32 ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_BOOL_TYPE, u32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_CHAR_TYPE, u32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_UINT8_TYPE, u32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_UINT16_TYPE, u32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_INT16_TYPE, u32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_INT32_TYPE, u32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_UINT64_TYPE, u32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_INT64_TYPE, u32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_FLOAT_TYPE, u32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_DOUBLE_TYPE, u32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT32_TYPE, GBL_STRING_TYPE, u32Convert_));

    // =============== INT64 ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_BOOL_TYPE, i64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_CHAR_TYPE, i64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_UINT8_TYPE, i64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_UINT16_TYPE, i64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_INT16_TYPE, i64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_UINT32_TYPE, i64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_INT32_TYPE, i64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_UINT64_TYPE, i64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_FLOAT_TYPE, i64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_DOUBLE_TYPE, i64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_INT64_TYPE, GBL_STRING_TYPE, i64Convert_));

    // =============== UINT64 ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_BOOL_TYPE, u64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_CHAR_TYPE, u64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_UINT8_TYPE, u64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_UINT16_TYPE, u64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_INT16_TYPE, u64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_UINT32_TYPE, u64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_INT32_TYPE, u64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_INT64_TYPE, u64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_FLOAT_TYPE, u64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_DOUBLE_TYPE, u64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_UINT64_TYPE, GBL_STRING_TYPE, u64Convert_));

    // =============== FLOAT ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_BOOL_TYPE, f32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_UINT8_TYPE, f32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_UINT16_TYPE, f32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_INT16_TYPE, f32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_UINT32_TYPE, f32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_INT32_TYPE, f32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_UINT64_TYPE, f32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_INT64_TYPE, f32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_DOUBLE_TYPE, f32Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_FLOAT_TYPE, GBL_STRING_TYPE, f32Convert_));

    // =============== DOUBLE ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_BOOL_TYPE, f64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_UINT8_TYPE, f64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_UINT16_TYPE, f64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_INT16_TYPE, f64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_UINT32_TYPE, f64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_INT32_TYPE, f64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_UINT64_TYPE, f64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_INT64_TYPE, f64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_FLOAT_TYPE, f64Convert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_DOUBLE_TYPE, GBL_STRING_TYPE, f64Convert_));

    // =============== STRING ===============
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_BOOL_TYPE, stringConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_CHAR_TYPE, stringConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_UINT8_TYPE, stringConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_UINT16_TYPE, stringConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_INT16_TYPE, stringConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_UINT32_TYPE, stringConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_INT32_TYPE, stringConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_UINT64_TYPE, stringConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_INT64_TYPE, stringConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_FLOAT_TYPE, stringConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, GBL_DOUBLE_TYPE, stringConvert_));

    GBL_CTX_POP(1);
    GBL_CTX_END();
}

GBL_EXPORT GblType GblType_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    // =============== TYPE ===============
    const static GblIVariantClassVTable typeIVariantIFace =  {
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

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblPrimitive_register(GblQuark_internStringStatic("type"),
                                     sizeof(GblPrimitiveClass),
                                     0,
                                     &typeIVariantIFace,
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();

        // =============== TYPE ===============
        GBL_CTX_CALL(GblVariant_registerConverter(type, GBL_BOOL_TYPE, typeConvert_));
        GBL_CTX_CALL(GblVariant_registerConverter(type, GBL_STRING_TYPE, typeConvert_));


        GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, type, stringConvert_));


        GBL_CTX_END_BLOCK();
    }

    return type;
}
