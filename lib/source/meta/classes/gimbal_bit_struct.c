#include <gimbal/meta/classes/gimbal_bit_struct.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/strings/gimbal_string_buffer.h>

#define GBL_BIT_STRUCT_CLASS_(klass)    (GBL_CLASS_PRIVATE(GblBitStruct, klass))

typedef struct GblBitStructField_ {
    GblQuark   name;
    GblBitmask mask;
    GblType    valueType;
} GblBitStructField_;

typedef struct GblBitStructClass_ {
    GblBitStructField_* pFields;
} GblBitStructClass_;

GBL_EXPORT GblQuark GblBitStructClass_nameQuarkFromIndex(const GblBitStructClass* pClass, size_t index) {
    return GBL_BIT_STRUCT_CLASS_(pClass)->pFields[index].name;
}

GBL_EXPORT GblQuark GblBitStructClass_nameQuarkFromBitmask(const GblBitStructClass* pClass, GblBitmask mask) {
    if(!(pClass->valueMask & mask))
        return GBL_QUARK_INVALID;

    GblBitStructClass_* pClass_ = GBL_BIT_STRUCT_CLASS_(pClass);

    for(size_t f = 0; f < pClass->fieldCount; ++f)
        if(pClass_->pFields[f].mask == mask)
            return pClass_->pFields[f].name;

    return GBL_QUARK_INVALID;
}

GBL_EXPORT const char* GblBitStructClass_nameFromIndex(const GblBitStructClass* pClass, size_t index) {
    return GblQuark_toString(GblBitStructClass_nameQuarkFromIndex(pClass, index));
}

GBL_EXPORT const char* GblBitStructClass_nameFromBitmask(const GblBitStructClass* pClass, GblBitmask mask) {
    return GblQuark_toString(GblBitStructClass_nameQuarkFromBitmask(pClass, mask));
}

GBL_EXPORT GblBitmask GblBitStructClass_maskFromIndex(const GblBitStructClass* pClass, size_t index) {
    return GBL_BIT_STRUCT_CLASS_(pClass)->pFields[index].mask;
}

GBL_EXPORT GblBitmask GblBitStructClass_maskFromName(const GblBitStructClass* pClass, const char* pName) {
    return GblBitStructClass_maskFromNameQuark(pClass, GblQuark_fromString(pName));
}

GBL_EXPORT GblBitmask GblBitStructClass_maskFromNameQuark(const GblBitStructClass* pClass, GblQuark quark) {
    GblBitStructClass_* pClass_ = GBL_BIT_STRUCT_CLASS_(pClass);

    for(size_t f = 0; f < pClass->fieldCount; ++f)
        if(pClass_->pFields[f].name == quark)
            return pClass_->pFields[f].mask;

    return 0;
}

GBL_EXPORT GblType GblBitStructClass_typeFromIndex(const GblBitStructClass* pClass, size_t index) {
    return GBL_BIT_STRUCT_CLASS_(pClass)->pFields[index].valueType;
}

GBL_EXPORT GblType GblBitStructClass_typeFromName(const GblBitStructClass* pClass, const char* pName) {
    return GblBitStructClass_typeFromNameQuark(pClass, GblQuark_fromString(pName));
}

GBL_EXPORT GblType GblBitStructClass_typeFromNameQuark(const GblBitStructClass* pClass, GblQuark quark) {
    GblBitStructClass_* pClass_ = GBL_BIT_STRUCT_CLASS_(pClass);

    for(size_t f = 0; f < pClass->fieldCount; ++f)
        if(pClass_->pFields[f].name == quark)
            return pClass_->pFields[f].valueType;

    return GBL_INVALID_TYPE;
}

GBL_EXPORT GblType GblBitStructClass_typeFromBitmask(const GblBitStructClass* pClass, GblBitmask msk) {
    if(!(pClass->valueMask & msk))
        return GBL_INVALID_TYPE;

    GblBitStructClass_* pClass_ = GBL_BIT_STRUCT_CLASS_(pClass);

    for(size_t f = 0; f < pClass->fieldCount; ++f)
        if(pClass_->pFields[f].mask == msk)
            return pClass_->pFields[f].valueType;

    return GBL_INVALID_TYPE;
}

static GBL_RESULT GblBitStruct_IVariant_construct_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc && pArgs);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT);
    GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_typeOf(pVariant)));
    pVariant->flags = GblFlagsClass_valueFromIndex(pFlagsClass, 0);
    GBL_CTX_END();
}

static GBL_RESULT GblBitStruct_IVariant_save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_typeOf(pVariant)));
    GBL_CTX_CALL(GblStringBuffer_append(pString,
                                        GBL_STRV(GblFlagsClass_nameFromValue(pFlagsClass,
                                                                            pVariant->flags))));
    GBL_CTX_END();
}

static GBL_RESULT GblBitStruct_IVariant_load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_typeOf(pVariant)));
    pVariant->flags = GblFlagsClass_valueFromName(pFlagsClass, GblStringBuffer_cString(pString));
    GBL_CTX_END();
}

static GBL_RESULT GblBitStruct_IVariant_set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_CTX_VERIFY_ARG(argc == 1);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_UINT32_TYPE);
    GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_typeOf(pVariant)));
    GBL_CTX_VERIFY(GblFlagsClass_valueCheck(pFlagsClass, (GblFlags)pArgs->u32),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    pVariant->flags = (GblFlags)pArgs->u32;
    GBL_CTX_END();
}

static GBL_RESULT GblBitStruct_IVariant_get_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((GblFlags*)pArgs->pVoid) = pVariant->flags;
    GBL_CTX_END();
}

static GBL_RESULT GblBitStruct_IVariant_compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    *pResult = pVariant->flags - pOther->flags;
    GBL_CTX_END();
}

static GBL_RESULT GblBitStruct_IVariant_convertTo_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
    GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_typeOf(pVariant)));
    const GblType type = GblVariant_typeOf(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, GblFlagsClass_valueCheck(pFlagsClass, pVariant->flags));
    else if(type == GBL_UINT8_TYPE) {
        GBL_CTX_VERIFY(pVariant->flags <= UINT8_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setUint8(pOther, pVariant->flags);
    } else if(type == GBL_UINT16_TYPE) {
        GBL_CTX_VERIFY(pVariant->flags <= UINT16_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setUint16(pOther, pVariant->flags);
    } else if(type == GBL_INT16_TYPE) {
        GBL_CTX_VERIFY(pVariant->flags <= INT16_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setInt16(pOther, pVariant->flags);
    } else if(type == GBL_UINT32_TYPE) {
        GBL_CTX_VERIFY(pVariant->flags <= UINT32_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setUint32(pOther, pVariant->flags);
    } else if(type == GBL_INT32_TYPE) {
        GBL_CTX_VERIFY(pVariant->flags <= INT32_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setInt32(pOther, pVariant->flags);
    } else if(type == GBL_UINT64_TYPE) {
        GblVariant_setUint64(pOther, pVariant->flags);
    } else if(type == GBL_INT64_TYPE) {
        GblVariant_setInt64(pOther, pVariant->flags);
    } else if(type == GBL_STRING_TYPE) {
        struct {
            GblStringBuffer str;
            char extraSpace[512];
        } buffer;

        GBL_CTX_CALL(GblStringBuffer_construct(&buffer.str, GBL_STRV(""), sizeof(buffer)));
        GBL_CTX_CALL(GblFlagsClass_valueAppendString(pFlagsClass, pVariant->flags, &buffer.str));

        GBL_CTX_CALL(GblVariant_setString(pOther, GblStringBuffer_cString(&buffer.str)));

        GBL_CTX_CALL(GblStringBuffer_destruct(&buffer.str));
    }
    else
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}


static GBL_RESULT GblBitStruct_IVariant_convertFrom_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_UNUSED(pVariant);
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pVariant);
     GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_typeOf(pOther)));
    if(type == GBL_STRING_TYPE)
        GBL_CTX_CALL(GblVariant_setValueCopy(pOther,
                                             GblVariant_typeOf(pOther),
                                             GblFlagsClass_valueFromString(pFlagsClass,
                                                                           GblVariant_string(pVariant))));
    else
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT GblBitStructClass_init_(GblClass* pClass, const void* pUd) {
    GBL_CTX_BEGIN(NULL);

    GblBitStructClass*       pSelfClass  = GBL_BIT_STRUCT_CLASS(pClass);
    GblBitStructClass_*      pSelfClass_ = GBL_BIT_STRUCT_CLASS_(pClass);
    const GblBitStructField* pFields     = pUd;

    if(!pUd) {
        GBL_CTX_WARN("[GblBitStruct]: Creating class [%s] with no fields!",
                     GblType_name(GBL_CLASS_TYPEOF(pClass)));
    } else {

        const GblBitStructField* pField = pUd;
        while(pField->pName || pField->mask || pField->valueType != GBL_INVALID_TYPE) {
            ++pSelfClass->fieldCount;
            ++pField;
        }

        if(pSelfClass->fieldCount) {
            pSelfClass_->pFields = GBL_CTX_MALLOC(sizeof(GblBitStructField_) *
                                                  pSelfClass->fieldCount);

            for(size_t f = 0; f < pSelfClass->fieldCount; ++f) {
                pSelfClass->valueMask |= pFields[f].mask;

                pSelfClass_->pFields[f].name      = GblQuark_fromString(pFields[f].pName);
                pSelfClass_->pFields[f].mask      = pFields[f].mask;
                pSelfClass_->pFields[f].valueType = pFields[f].valueType;
            }
        }
    }

    GBL_CTX_END();
}

static GBL_RESULT GblBitStructClass_final_(GblClass* pClass, const void* pUd) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_FREE(GBL_BIT_STRUCT_CLASS_(pClass)->pFields);
    GBL_CTX_END();
}

GBL_EXPORT GblType GblBitStruct_register(const char*              pName,
                                         const GblBitStructField* pFields)
{
    return GblType_register(GblQuark_internString(pName),
                            GBL_BIT_STRUCT_TYPE,
                            &(const GblTypeInfo) {
                                .pFnClassInit  = GblBitStructClass_init_,
                                .pFnClassFinal = GblBitStructClass_final_,
                                .classSize     = sizeof(GblBitStructClass),
                                .pClassData    = pFields
                            },
                            GBL_TYPE_FLAGS_NONE);
}

GBL_EXPORT GblType GblBitStruct_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblIVariantVTable iVariantIFace =  {
            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
            .pSetValueFmt   = { "z"},
            .pGetValueFmt   = { "p" },
            .pFnConstruct   = GblBitStruct_IVariant_construct_,
            .pFnSet         = GblBitStruct_IVariant_set_,
            .pFnGet         = GblBitStruct_IVariant_get_,
            .pFnCompare     = GblBitStruct_IVariant_compare_,
            .pFnSave        = GblBitStruct_IVariant_save_,
            .pFnLoad        = GblBitStruct_IVariant_load_
      };

    if(type == GBL_INVALID_TYPE) {
        type = GblPrimitive_register(GblQuark_internStatic("GblBitStruct"),
                                     sizeof(GblBitStructClass),
                                     sizeof(GblBitStructClass_),
                                     &iVariantIFace,
                                     GBL_TYPE_FLAGS_NONE);

        GblVariant_registerConverter(type, GBL_BOOL_TYPE,   GblBitStruct_IVariant_convertTo_);
        GblVariant_registerConverter(type, GBL_UINT8_TYPE,  GblBitStruct_IVariant_convertTo_);
        GblVariant_registerConverter(type, GBL_UINT16_TYPE, GblBitStruct_IVariant_convertTo_);
        GblVariant_registerConverter(type, GBL_INT16_TYPE,  GblBitStruct_IVariant_convertTo_);
        GblVariant_registerConverter(type, GBL_UINT32_TYPE, GblBitStruct_IVariant_convertTo_);
        GblVariant_registerConverter(type, GBL_INT32_TYPE,  GblBitStruct_IVariant_convertTo_);
        GblVariant_registerConverter(type, GBL_UINT64_TYPE, GblBitStruct_IVariant_convertTo_);
        GblVariant_registerConverter(type, GBL_INT64_TYPE,  GblBitStruct_IVariant_convertTo_);
        GblVariant_registerConverter(type, GBL_STRING_TYPE, GblBitStruct_IVariant_convertTo_);

        GblVariant_registerConverter(GBL_STRING_TYPE, type, GblBitStruct_IVariant_convertFrom_);

    }

    return type;
}
