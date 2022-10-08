#include <gimbal/meta/types/gimbal_pointer.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include "gimbal_type_.h"

static GBL_RESULT pSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblStringBuffer_appendPointer(pString, pVariant->pVoid));
    GBL_CTX_END();
}

static GBL_RESULT pLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    pVariant->pVoid = (void*)GblStringView_toPointer(GblStringBuffer_view(pString));
    GBL_CTX_END();
}

static GBL_RESULT pSet_(GblVariant* pVariant, GblSize argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_TYPE(pArgs->type, GBL_POINTER_TYPE);
    pVariant->pVoid = pArgs->pVoid;
    GBL_CTX_END();
}

static GBL_RESULT pGet_(GblVariant* pVariant, GblSize argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((void**)pArgs->pVoid) = pVariant->pVoid;
    GBL_CTX_END();
}

static GBL_RESULT pCompare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    if(pVariant->pVoid > pOther->pVoid)         *pResult = 1;
    else if(pVariant->pVoid < pOther->pVoid)    *pResult = -1;
    else                                        *pResult = 0;
    GBL_CTX_END();
}

static GBL_RESULT pConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->pVoid? GBL_TRUE : GBL_FALSE);
    else if(type == GBL_STRING_TYPE) {
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%p", pVariant->pVoid);
        GblVariant_setString(pOther, buffer);
    }
    else
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}


static GBL_RESULT pConvertFrom_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_UNUSED(pVariant);
    GBL_CTX_BEGIN(NULL);
    const GblType dstType = GblVariant_typeOf(pOther);
    const GblType srcType = GblVariant_typeOf(pVariant);
    if(dstType == GBL_POINTER_TYPE) {
        if(srcType == GBL_NIL_TYPE)
            GblVariant_setPointer(pOther, GBL_POINTER_TYPE, NULL);
        else if(srcType == GBL_STRING_TYPE)
            GblVariant_setPointer(pOther, GBL_POINTER_TYPE, pVariant->pString);
    } else
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

extern GBL_RESULT GblPointer_typeRegister_(GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    static const GblIVariantClassVTable pointerIVariantIFace =  {
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

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_POINTER,
                                 GblQuark_internStringStatic("pointer"),
                                 sizeof(GblPrimitiveClass),
                                 0,
                                 &pointerIVariantIFace,
                                 GBL_TYPE_FLAG_CLASS_PINNED);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_CALL(GblVariant_registerConverter(GBL_POINTER_TYPE, GBL_BOOL_TYPE, pConvert_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_POINTER_TYPE, GBL_STRING_TYPE, pConvert_));

    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE,     GBL_POINTER_TYPE, pConvertFrom_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_STRING_TYPE,  GBL_POINTER_TYPE, pConvertFrom_));

    GBL_CTX_END();
}

GblType GblPointer_register(const char* pName) {
    GblType type = GBL_INVALID_TYPE;
    GBL_CTX_BEGIN(NULL);
    type = GblType_registerStatic(GblQuark_internString(pName),
                                  GBL_POINTER_TYPE,
                                  &(const GblTypeInfo) {
                                      .classSize = sizeof(GblPrimitiveClass),
                                  },
                                  GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return type;
}

