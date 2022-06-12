#include <gimbal/meta/gimbal_enum.h>
#include <gimbal/meta/gimbal_variant.h>
#include <gimbal/types/gimbal_string_buffer.h>


typedef struct GblEnumEntry_ {
    GblEnum     value;
    GblQuark    name;    // So that walking table to go from string to value is cheap
    GblQuark    nick;
} GblEnumEntry_;

typedef struct GblEnumClass_ {
    GblEnumEntry_* pEntries;
} GblEnumClass_;

extern GBL_RESULT GblPrimitiveClass_init(GblPrimitiveClass* pClass, GblIVariantIFace* pIFace, GblContext* pCtx);

static GBL_RESULT enumConstruct_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc && pArgs);
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT);
    GblEnumClass* pEnumClass = GBL_ENUM_CLASS(GblClass_peek(GblVariant_type(pVariant)));
    pVariant->enumeration = GblEnumClass_valueFromIndex(pEnumClass, 0);
    GBL_API_END();
}

static GBL_RESULT enumSave_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblEnumClass* pEnumClass = GBL_ENUM_CLASS(GblClass_peek(GblVariant_type(pVariant)));
    GBL_API_CALL(GblStringBuffer_append(pString,
                                        GBL_STRV(GblEnumClass_nameFromValue(pEnumClass,
                                                                            pVariant->enumeration))));
    GBL_API_END();
}

static GBL_RESULT enumLoad_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GblEnumClass* pEnumClass = GBL_ENUM_CLASS(GblClass_peek(GblVariant_type(pVariant)));
    pVariant->enumeration = GblEnumClass_valueFromName(pEnumClass, GblStringBuffer_cString(pString));
    GBL_API_END();
}

static GBL_RESULT enumSet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_ARG(argc == 1);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_UINT32_TYPE);
    GblEnumClass* pEnumClass = GBL_ENUM_CLASS(GblClass_peek(GblVariant_type(pVariant)));
    GBL_API_VERIFY(GblEnumClass_valueCheck(pEnumClass, (GblEnum)pArgs->u32),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    pVariant->enumeration = (GblEnum)pArgs->u32;
    GBL_API_END();
}

static GBL_RESULT enumGet_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY | GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
    *((GblEnum*)pArgs->pVoid) = pVariant->enumeration;
    GBL_API_END();
}


static GBL_RESULT enumCompare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = pVariant->enumeration - pOther->enumeration;
    GBL_API_END();
}

static GBL_RESULT enumConvert_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    GblEnumClass* pEnumClass = GBL_ENUM_CLASS(GblClass_peek(GblVariant_type(pVariant)));
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, GblEnumClass_valueCheck(pEnumClass, pVariant->enumeration));
    else if(type == GBL_UINT8_TYPE) {
        GBL_API_VERIFY(pVariant->enumeration <= UINT8_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setUint8(pOther, pVariant->enumeration);
    } else if(type == GBL_UINT16_TYPE) {
        GBL_API_VERIFY(pVariant->enumeration <= UINT16_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setUint16(pOther, pVariant->enumeration);
    } else if(type == GBL_INT16_TYPE) {
        GBL_API_VERIFY(pVariant->enumeration <= INT16_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setInt16(pOther, pVariant->enumeration);
    } else if(type == GBL_UINT32_TYPE) {
        GBL_API_VERIFY(pVariant->enumeration <= UINT32_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setUint32(pOther, pVariant->enumeration);
    } else if(type == GBL_INT32_TYPE) {
        GBL_API_VERIFY(pVariant->enumeration <= INT32_MAX,
                       GBL_RESULT_ERROR_OVERFLOW);
        GblVariant_setInt32(pOther, pVariant->enumeration);
    } else if(type == GBL_UINT64_TYPE) {
        GblVariant_setUint64(pOther, pVariant->enumeration);
    } else if(type == GBL_INT64_TYPE) {
        GblVariant_setInt64(pOther, pVariant->enumeration);
    } else if(type == GBL_STRING_TYPE) {
        GblVariant_setString(pOther, GblEnumClass_nameFromValue(pEnumClass, pVariant->enumeration));
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

GBL_EXPORT GblQuark GblEnumClass_nameQuarkFromIndex(const GblEnumClass* pSelf, uint16_t index) GBL_NOEXCEPT {
    const GblEnumClass_* pSelf_ = GblClass_private(GBL_CLASS(pSelf), GBL_ENUM_TYPE);
    return index < pSelf->entryCount? pSelf_->pEntries[index].name : GBL_QUARK_INVALID;
}
GBL_EXPORT GblQuark GblEnumClass_nickQuarkFromIndex(const GblEnumClass* pSelf, uint16_t index) GBL_NOEXCEPT {
    const GblEnumClass_* pSelf_ = GblClass_private(GBL_CLASS(pSelf), GBL_ENUM_TYPE);
    return index < pSelf->entryCount? pSelf_->pEntries[index].nick : GBL_QUARK_INVALID;
}
GBL_EXPORT GblEnum  GblEnumClass_valueFromIndex(const GblEnumClass* pSelf, uint16_t index) GBL_NOEXCEPT {
    const GblEnumClass_* pSelf_ = GblClass_private(GBL_CLASS(pSelf), GBL_ENUM_TYPE);
    return index < pSelf->entryCount? pSelf_->pEntries[index].value : 0;
}

GBL_INLINE GblBool GblEnumClass_valueInRange_(const GblEnumClass* pSelf, GblEnum value) {
    return value <= pSelf->valueMax && value >= pSelf->valueMin;
}

GBL_EXPORT GblQuark GblEnumClass_nameQuarkFromValue(const GblEnumClass* pSelf, GblEnum value) {
    GblQuark quark = GBL_QUARK_INVALID;
    if(GblEnumClass_valueInRange_(pSelf, value)) {
        for(uint16_t e = 0; e < pSelf->entryCount; ++e) {
            if(GblEnumClass_valueFromIndex(pSelf, e) == value) {
                quark = GblEnumClass_nameQuarkFromIndex(pSelf, e);
                break;
            }
        }
    }
    return quark;
}

GBL_EXPORT GblQuark GblEnumClass_nickQuarkFromValue(const GblEnumClass* pSelf, GblEnum value) {
    GblQuark quark = GBL_QUARK_INVALID;
    if(GblEnumClass_valueInRange_(pSelf, value)) {
        for(uint16_t e = 0; e < pSelf->entryCount; ++e) {
            if(GblEnumClass_valueFromIndex(pSelf, e) == value) {
                quark = GblEnumClass_nickQuarkFromIndex(pSelf, e);
                break;
            }
        }
    }
    return quark;
}

GBL_EXPORT GblEnum GblEnumClass_valueFromNameQuark(const GblEnumClass* pSelf, GblQuark quark) {
    GblEnum value = 0;
    for(uint16_t e = 0; e < pSelf->entryCount; ++e) {
        if(GblEnumClass_nameQuarkFromIndex(pSelf, e) == quark) {
            value = GblEnumClass_valueFromIndex(pSelf, e);
            break;
        }
    }
    return value;
}

GBL_EXPORT GblEnum GblEnumClass_valueFromNickQuark(const GblEnumClass* pSelf, GblQuark quark) {
    GblEnum value = 0;
    for(uint16_t e = 0; e < pSelf->entryCount; ++e) {
        if(GblEnumClass_nickQuarkFromIndex(pSelf, e) == quark) {
            value = GblEnumClass_valueFromIndex(pSelf, e);
            break;
        }
    }
    return value;
}

GBL_EXPORT GblBool GblEnumClass_valueCheck(const GblEnumClass* pSelf, GblEnum value) {
    GblBool result = GBL_FALSE;

    if(value <= pSelf->valueMax && value >= pSelf->valueMin) {
        for(uint16_t e = 0; e < pSelf->entryCount; ++e) {
            if(GblEnumClass_valueFromIndex(pSelf, e) == value) {
                result = GBL_TRUE;
                break;
            }
        }
    }

    return result;
}

static GBL_RESULT enumClass_init_(GblClass* pClass,
                                  const void* pUd,
                                  GblContext* pCtx)
{
    GBL_API_BEGIN(pCtx);
    GblEnumClass*       pEnumClass  = GBL_ENUM_CLASS(pClass);
    GblEnumClass_*      pEnumClass_ = GblClass_private(pClass, GBL_ENUM_TYPE);
    const GblEnumEntry* pEntries    = pUd;

    // Initialize public/private members
    pEnumClass->entryCount  = 0;
    pEnumClass->valueMin    = UINT32_MAX;
    pEnumClass->valueMax    = 0;
    pEnumClass_->pEntries   = NULL;

    if(!pEntries) {
        GBL_API_WARN("[GblEnum]: Creating class [%s] with no enum entry data!",
                     GblType_name(GBL_CLASS_TYPE(pClass)));
    } else {

        // Iterate over the data, determining entry count
        const GblEnumEntry* pCurEntry = &pEntries[0];
        while(pCurEntry->value || pCurEntry->pName || pCurEntry->pNick) {
            ++pEnumClass->entryCount;
            ++pCurEntry;
        }

        if(pEnumClass->entryCount) {
            // Allocate space for entries, store in private data
            pEnumClass_->pEntries = GBL_API_MALLOC(sizeof(GblEnumEntry_) * pEnumClass->entryCount);

            // Iterate back over entries, copying data
            for(uint16_t e = 0; e < pEnumClass->entryCount; ++e) {
                // Accumulate min
                if(pEntries[e].value < pEnumClass->valueMin)
                    pEnumClass->valueMin = pEntries[e].value;

                // Accumulate max
                if(pEntries[e].value > pEnumClass->valueMax)
                    pEnumClass->valueMax = pEntries[e].value;

                // Copy value + convert strings to quarks for faster lookup
                pEnumClass_->pEntries[e].value  = pEntries[e].value;
                pEnumClass_->pEntries[e].name   = GblQuark_fromString(pEntries[e].pName);
                pEnumClass_->pEntries[e].nick   = GblQuark_fromString(pEntries[e].pNick);
            }
        }
    }

    GBL_API_END();
}

static GBL_RESULT enumClass_final_(GblClass* pClass,
                                   const void* pUd,
                                   GblContext* pCtx)
{
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);
    // Free up local entry copy
    GblEnumClass_* pClass_ = GblClass_private(pClass, GBL_ENUM_TYPE);
    GBL_API_FREE(pClass_->pEntries);
    GBL_API_END();
}

GBL_RESULT  GblEnum_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    const static GblIVariantIFace enumIVariantIFace =  {
            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE        |
                            GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT  |
                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY     |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY     |
                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
            .pSetValueFmt   = { "l"},
            .pGetValueFmt   = { "p" },
            .pFnConstruct   = enumConstruct_,
            .pFnSet         = enumSet_,
            .pFnGet         = enumGet_,
            .pFnCompare     = enumCompare_,
            .pFnSave        = enumSave_,
            .pFnLoad        = enumLoad_
      };
    const GblType enumType =
            GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_ENUM,
                  GBL_INVALID_TYPE,
                  GblQuark_internStringStatic("enum"),
                  &(const GblTypeInfo) {
                      .pFnClassInit     = (GblTypeClassInitializeFn)GblPrimitiveClass_init,
                      .classSize        = sizeof(GblEnumClass),
                      .classPrivateSize = sizeof(GblEnumClass_),
                      .pClassData       = &enumIVariantIFace,
                      .interfaceCount   = 1,
                      .pInterfaceMap    = &(const GblTypeInterfaceMapEntry) {
                           .interfaceType  = GBL_IVARIANT_TYPE,
                           .classOffset    = offsetof(GblPrimitiveClass, iVariantIFace)
                      }
                  },
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED |
                  GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);

    GBL_API_CALL(GblVariant_registerConverter(enumType, GBL_BOOL_TYPE, enumConvert_));
    GBL_API_CALL(GblVariant_registerConverter(enumType, GBL_UINT8_TYPE, enumConvert_));
    GBL_API_CALL(GblVariant_registerConverter(enumType, GBL_UINT16_TYPE, enumConvert_));
    GBL_API_CALL(GblVariant_registerConverter(enumType, GBL_INT16_TYPE, enumConvert_));
    GBL_API_CALL(GblVariant_registerConverter(enumType, GBL_UINT32_TYPE, enumConvert_));
    GBL_API_CALL(GblVariant_registerConverter(enumType, GBL_INT32_TYPE, enumConvert_));
    GBL_API_CALL(GblVariant_registerConverter(enumType, GBL_UINT64_TYPE, enumConvert_));
    GBL_API_CALL(GblVariant_registerConverter(enumType, GBL_INT64_TYPE, enumConvert_));
    GBL_API_CALL(GblVariant_registerConverter(enumType, GBL_STRING_TYPE, enumConvert_));
    GBL_API_END();
}

GBL_EXPORT GblType GblEnum_register(const char* pName,
                                    const GblEnumEntry* pValidEntries)
{
    GblType type = GBL_INVALID_TYPE;
    GBL_API_BEGIN(NULL);
    type = GblType_registerStatic(GBL_ENUM_TYPE,
                                  GblQuark_internString(pName),
                                  &(const GblTypeInfo) {
                                      .pFnClassInit     = enumClass_init_,
                                      .pFnClassFinal    = enumClass_final_,
                                      .classSize        = sizeof(GblEnumClass),
                                      .pClassData       = pValidEntries,
                                  },
                                  GBL_TYPE_FLAGS_NONE);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return type;
}
