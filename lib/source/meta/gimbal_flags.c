#include <gimbal/meta/gimbal_flags.h>
#include <gimbal/types/gimbal_variant.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include "../meta/gimbal_type_.h"

typedef struct GblFlagsEntry_ {
    GblFlags    value;
    GblQuark    name;    // So that walking table to go from string to value is cheap
    GblQuark    nick;
} GblFlagsEntry_;

typedef struct GblFlagsClass_ {
    GblFlagsEntry_* pEntries;
} GblFlagsClass_;

static GBL_RESULT flagsConstruct_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc && pArgs);
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT);
    GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_type(pVariant)));
    pVariant->flags = GblFlagsClass_valueFromIndex(pFlagsClass, 0);
    GBL_API_END();
}

static GBL_RESULT flagsSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_type(pVariant)));
    GBL_API_CALL(GblStringBuffer_append(pString,
                                        GBL_STRV(GblFlagsClass_nameFromValue(pFlagsClass,
                                                                            pVariant->flags))));
    GBL_API_END();
}

static GBL_RESULT flagsLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_type(pVariant)));
    pVariant->flags = GblFlagsClass_valueFromName(pFlagsClass, GblStringBuffer_cString(pString));
    GBL_API_END();
}

static GBL_RESULT flagsSet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_ARG(argc == 1);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_UINT32_TYPE);
    GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_type(pVariant)));
    GBL_API_VERIFY(GblFlagsClass_valueCheck(pFlagsClass, (GblFlags)pArgs->u32),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    pVariant->flags = (GblFlags)pArgs->u32;
    GBL_API_END();
}

static GBL_RESULT flagsGet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((GblFlags*)pArgs->pVoid) = pVariant->flags;
    GBL_API_END();
}


static GBL_RESULT flagsCompare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = pVariant->flags - pOther->flags;
    GBL_API_END();
}

static GBL_RESULT flagsConvertTo_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_type(pVariant)));
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, GblFlagsClass_valueCheck(pFlagsClass, pVariant->flags));
    else if(type == GBL_UINT8_TYPE) {
        GBL_API_VERIFY(pVariant->flags <= UINT8_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setUint8(pOther, pVariant->flags);
    } else if(type == GBL_UINT16_TYPE) {
        GBL_API_VERIFY(pVariant->flags <= UINT16_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setUint16(pOther, pVariant->flags);
    } else if(type == GBL_INT16_TYPE) {
        GBL_API_VERIFY(pVariant->flags <= INT16_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setInt16(pOther, pVariant->flags);
    } else if(type == GBL_UINT32_TYPE) {
        GBL_API_VERIFY(pVariant->flags <= UINT32_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setUint32(pOther, pVariant->flags);
    } else if(type == GBL_INT32_TYPE) {
        GBL_API_VERIFY(pVariant->flags <= INT32_MAX,
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

        GBL_API_CALL(GblStringBuffer_construct(&buffer.str, GBL_STRV(""), sizeof(buffer)));
        GBL_API_CALL(GblFlagsClass_valueAppendString(pFlagsClass, pVariant->flags, &buffer.str));

        GBL_API_CALL(GblVariant_setString(pOther, GblStringBuffer_cString(&buffer.str)));

        GBL_API_CALL(GblStringBuffer_destruct(&buffer.str));
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}


static GBL_RESULT flagsConvertFrom_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_UNUSED(pVariant);
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pVariant);
     GblFlagsClass* pFlagsClass = GBL_FLAGS_CLASS(GblClass_weakRefDefault(GblVariant_type(pOther)));
    if(type == GBL_STRING_TYPE)
        GBL_API_CALL(GblVariant_setValueCopy(pOther,
                                             GblVariant_type(pOther),
                                             GblFlagsClass_valueFromString(pFlagsClass,
                                                                           GblVariant_getString(pVariant))));
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

GBL_EXPORT GblQuark GblFlagsClass_nameQuarkFromIndex(const GblFlagsClass* pSelf, uint16_t index) GBL_NOEXCEPT {
    const GblFlagsClass_* pSelf_ = GblClass_private(GBL_CLASS(pSelf), GBL_FLAGS_TYPE);
    return index < pSelf->entryCount? pSelf_->pEntries[index].name : GBL_QUARK_INVALID;
}

GBL_EXPORT GblQuark GblFlagsClass_nickQuarkFromIndex(const GblFlagsClass* pSelf, uint16_t index) GBL_NOEXCEPT {
    const GblFlagsClass_* pSelf_ = GblClass_private(GBL_CLASS(pSelf), GBL_FLAGS_TYPE);
    return index < pSelf->entryCount? pSelf_->pEntries[index].nick : GBL_QUARK_INVALID;
}

GBL_EXPORT GblFlags  GblFlagsClass_valueFromIndex(const GblFlagsClass* pSelf, uint16_t index) GBL_NOEXCEPT {
    const GblFlagsClass_* pSelf_ = GblClass_private(GBL_CLASS(pSelf), GBL_FLAGS_TYPE);
    return index < pSelf->entryCount? pSelf_->pEntries[index].value : 0;
}

GBL_EXPORT GblQuark GblFlagsClass_nameQuarkFromValue(const GblFlagsClass* pSelf, GblFlags value) {
    GblQuark quark = GBL_QUARK_INVALID;
    if(GblFlagsClass_valueCheck(pSelf, value)) {
        for(uint16_t e = 0; e < pSelf->entryCount; ++e) {
            if(GblFlagsClass_valueFromIndex(pSelf, e) == value) {
                quark = GblFlagsClass_nameQuarkFromIndex(pSelf, e);
                break;
            }
        }
    }
    return quark;
}

GBL_EXPORT GblQuark GblFlagsClass_nickQuarkFromValue(const GblFlagsClass* pSelf, GblFlags value) {
    GblQuark quark = GBL_QUARK_INVALID;
    if(GblFlagsClass_valueCheck(pSelf, value)) {
        for(uint16_t e = 0; e < pSelf->entryCount; ++e) {
            if(GblFlagsClass_valueFromIndex(pSelf, e) == value) {
                quark = GblFlagsClass_nickQuarkFromIndex(pSelf, e);
                break;
            }
        }
    }
    return quark;
}

GBL_EXPORT GblFlags GblFlagsClass_valueFromNameQuark(const GblFlagsClass* pSelf, GblQuark quark) {
    GblFlags value = 0;
    for(uint16_t e = 0; e < pSelf->entryCount; ++e) {
        if(GblFlagsClass_nameQuarkFromIndex(pSelf, e) == quark) {
            value = GblFlagsClass_valueFromIndex(pSelf, e);
            break;
        }
    }
    return value;
}

GBL_EXPORT GblFlags GblFlagsClass_valueFromNickQuark(const GblFlagsClass* pSelf, GblQuark quark) {
    GblFlags value = 0;
    for(uint16_t e = 0; e < pSelf->entryCount; ++e) {
        if(GblFlagsClass_nickQuarkFromIndex(pSelf, e) == quark) {
            value = GblFlagsClass_valueFromIndex(pSelf, e);
            break;
        }
    }
    return value;
}

GBL_EXPORT GblFlags GblFlagsClass_valueFromString(const GblFlagsClass* pClass, const char* pString) {
    GBL_API_BEGIN(NULL);
    GblFlags value = 0;

    GblStringView view = GBL_STRV(pString);
    GblFlagsClass* pSelf = GBL_FLAGS_CLASS(pClass);
    GblFlagsClass_* pSelf_ = GblClass_private(GBL_CLASS(pClass), GBL_FLAGS_TYPE);

    for(uint16_t f = 0; f < pSelf->entryCount; ++f) {
        if(GblStringView_contains(view, GBL_STRV(GblQuark_toString(pSelf_->pEntries[f].name)))) {
            value |= pSelf_->pEntries[f].value;
        }
    }

    GBL_API_END_BLOCK();
    return value;
}


GBL_EXPORT GBL_RESULT GblFlagsClass_valueAppendString(const GblFlagsClass* pClass,
                                                      GblFlags flags,
                                                      GblStringBuffer* pStr)
{
    GBL_API_BEGIN(GblStringBuffer_context(pStr));
    GblFlagsClass* pSelf = GBL_FLAGS_CLASS(pClass);
    GblFlagsClass_* pSelf_ = GblClass_private(GBL_CLASS(pClass), GBL_FLAGS_TYPE);

    GblSize count = 0;
    for(uint16_t f = 0; f < pSelf->entryCount; ++f) {
        if(flags & pSelf_->pEntries[f].value) {
            if(count++) GBL_API_CALL(GblStringBuffer_append(pStr, GBL_STRV("|")));
            GBL_API_CALL(GblStringBuffer_append(pStr, GBL_STRV(GblQuark_toString(pSelf_->pEntries[f].name))));
        }
    }

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblFlags_appendString(GblFlags value,
                                            GblType type,
                                            GblStringBuffer* pBuffer) GBL_NOEXCEPT
{
    GBL_API_BEGIN(NULL);
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    GBL_API_CALL(GblFlagsClass_valueAppendString(pClass, value, pBuffer));
    GblClass_unrefDefault(GBL_CLASS(pClass));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

static GBL_RESULT flagsClass_init_(GblClass* pClass,
                                  const void* pUd,
                                  GblContext* pCtx)
{
    GBL_API_BEGIN(pCtx);
    GblFlagsClass*       pFlagsClass  = GBL_FLAGS_CLASS(pClass);
    GblFlagsClass_*      pFlagsClass_ = GblClass_private(pClass, GBL_FLAGS_TYPE);
    const GblFlagEntry* pEntries    = pUd;

    // Initialize public/private members
    pFlagsClass->entryCount  = 0;
    pFlagsClass->entryCount = 0;
    pFlagsClass_->pEntries   = NULL;

    if(!pEntries) {
        GBL_API_WARN("[GblFlags]: Creating class [%s] with no flags entry data!",
                     GblType_name(GBL_CLASS_TYPEOF(pClass)));
    } else {

        // Iterate over the data, determining entry count
        const GblFlagEntry* pCurEntry = &pEntries[0];
        while(pCurEntry->value || pCurEntry->pName || pCurEntry->pNick) {
            ++pFlagsClass->entryCount;
            ++pCurEntry;
        }

        if(pFlagsClass->entryCount) {
            // Allocate space for entries, store in private data
            pFlagsClass_->pEntries = GBL_API_MALLOC(sizeof(GblFlagsEntry_) * pFlagsClass->entryCount);

            // Iterate back over entries, copying data
            for(uint16_t f = 0; f < pFlagsClass->entryCount; ++f) {
                pFlagsClass->valueMask |= pEntries[f].value;

                // Copy value + convert strings to quarks for faster lookup
                pFlagsClass_->pEntries[f].value  = pEntries[f].value;
                pFlagsClass_->pEntries[f].name   = GblQuark_fromString(pEntries[f].pName);
                pFlagsClass_->pEntries[f].nick   = GblQuark_fromString(pEntries[f].pNick);
            }
        }
    }

    GBL_API_END();
}

static GBL_RESULT flagsClass_final_(GblClass* pClass,
                                   const void* pUd,
                                   GblContext* pCtx)
{
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);
    // Free up local entry copy
    GblFlagsClass_* pClass_ = GblClass_private(pClass, GBL_FLAGS_TYPE);
    GBL_API_FREE(pClass_->pEntries);
    GBL_API_END();
}

GBL_RESULT GblFlags_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    const static GblIVariantIFaceVTable flagsIVariantIFace =  {
            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE        |
                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY     |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY     |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
            .pSetValueFmt   = { "l"},
            .pGetValueFmt   = { "p" },
            .pFnConstruct   = flagsConstruct_,
            .pFnSet         = flagsSet_,
            .pFnGet         = flagsGet_,
            .pFnCompare     = flagsCompare_,
            .pFnSave        = flagsSave_,
            .pFnLoad        = flagsLoad_
      };

    const GblType flagsType =
        GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_FLAGS,
                  GblQuark_internStringStatic("flags"),
                  sizeof(GblFlagsClass),
                  sizeof(GblFlagsClass_),
                  &flagsIVariantIFace,
                  GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE);

    GBL_API_CALL(GblVariant_registerConverter(flagsType, GBL_BOOL_TYPE, flagsConvertTo_));
    GBL_API_CALL(GblVariant_registerConverter(flagsType, GBL_UINT8_TYPE, flagsConvertTo_));
    GBL_API_CALL(GblVariant_registerConverter(flagsType, GBL_UINT16_TYPE, flagsConvertTo_));
    GBL_API_CALL(GblVariant_registerConverter(flagsType, GBL_INT16_TYPE, flagsConvertTo_));
    GBL_API_CALL(GblVariant_registerConverter(flagsType, GBL_UINT32_TYPE, flagsConvertTo_));
    GBL_API_CALL(GblVariant_registerConverter(flagsType, GBL_INT32_TYPE, flagsConvertTo_));
    GBL_API_CALL(GblVariant_registerConverter(flagsType, GBL_UINT64_TYPE, flagsConvertTo_));
    GBL_API_CALL(GblVariant_registerConverter(flagsType, GBL_INT64_TYPE, flagsConvertTo_));
    GBL_API_CALL(GblVariant_registerConverter(flagsType, GBL_STRING_TYPE, flagsConvertTo_));

    GBL_API_CALL(GblVariant_registerConverter(GBL_STRING_TYPE, flagsType, flagsConvertFrom_));

    GBL_API_END();
}

GBL_EXPORT GblType GblFlags_register(const char* pName,
                                     const GblFlagEntry* pValidEntries)
{
    GblType type = GBL_INVALID_TYPE;
    GBL_API_BEGIN(NULL);
    type = GblType_registerStatic(GblQuark_internString(pName),
                                  GBL_FLAGS_TYPE,
                                  &(const GblTypeInfo) {
                                      .pFnClassInit     = flagsClass_init_,
                                      .pFnClassFinal    = flagsClass_final_,
                                      .classSize        = sizeof(GblFlagsClass),
                                      .pClassData       = pValidEntries,
                                  },
                                  GBL_TYPE_FLAGS_NONE);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return type;
}

