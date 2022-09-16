#include <gimbal/meta/ifaces/gimbal_ivariant.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/strings/gimbal_quark.h>
#include "../types/gimbal_type_.h"

extern GBL_RESULT GblIVariant_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    static const GblTypeInfo typeInfo = {
        .classSize      = sizeof(GblIVariantClass)
    };

    GblType_registerBuiltin_(GBL_TYPE_BUILTIN_INDEX_IVARIANT,
                            GBL_INTERFACE_TYPE,
                            GblQuark_internStringStatic("GblIVariant"),
                            &typeInfo,
                            GBL_TYPE_FLAG_ABSTRACT |
                            GBL_TYPE_FLAG_TYPEINFO_STATIC);
    GBL_API_END();
}

// never even being used anywhere!
GBL_EXPORT GBL_RESULT GblIVariantClass_validate(const GblIVariantClass* pSelf) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);

    if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_MASK)
    {
        GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct,
                               "Pure virtual constructor with constructor ops is not allowed!");
    }
    if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_MASK)
    {
        GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet,
                               "Pure virtual set function with set ops is not allowed!");
    }
    if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_GET_MASK)
    {
        GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnGet,
                               "Pure virtual get function with get ops is not allowed!");
    }

    if(!(pSelf->pVTable->supportedOps & (GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY)))
    {
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_TYPE,
                           "Type has no way of having its value set!");
    }

    if(!(pSelf->pVTable->supportedOps & (GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY |
                                GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK)))
    {
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_TYPE,
                           "Type has no way of having its value retrieved!");
    }

    if(!(pSelf->pVTable->supportedOps & (GBL_IVARIANT_OP_FLAG_SET_COPY |
                                GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY |
                                GBL_IVARIANT_OP_FLAG_RELOCATABLE)))
    {
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_TYPE,
                           "Type has no way of being copied!");
    }
    GBL_API_END();
}


GBL_EXPORT GBL_RESULT GblIVariantClass_constructDefault(const GblIVariantClass* pSelf, GblVariant* pVariant) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
    GBL_API_VERIFY_POINTER(pVariant);
    if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
        GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
        GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, 0, NULL, GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT));
    }
    GBL_API_END();
}


/* Order of operations
 * 1) Try copy constructor
 * 2) Try default constructor + copy assignment
 * 3) Shit pants
 */
GBL_EXPORT GBL_RESULT GblIVariantClass_constructCopy(const GblIVariantClass* pSelf, GblVariant* pVariant, const GblVariant* pOther) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pOther);
    if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY) {
        GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
        GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, 1, (GblVariant*)pOther, GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY));
    } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
        if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
            GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, 0, NULL, GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT));
        }
        GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, 1, (GblVariant*)pOther, GBL_IVARIANT_OP_FLAG_SET_COPY));
    } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_RELOCATABLE) {
        memcpy(pVariant, pOther, sizeof(GblVariant));
    } else {
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                           "[GblIVariant] Cannot copy construct type: %s",
                           GblType_name(pVariant->type));
    }
    GBL_API_END();
}

/* Attempt to move construct a variant from another
 *  Order of operations
 * 1) Try move constructor
 * 2) Try default constructor + move assignment
 * 3) Try copy constructor
 * 4) Try default constructor + copy assignment
 * 5) Try memcpy
 * 6) Shit pants
 */
GBL_EXPORT GBL_RESULT GblIVariantClass_constructMove(const GblIVariantClass* pSelf, GblVariant* pVariant, GblVariant* pOther) GBL_NOEXCEPT {
   GBL_API_BEGIN(NULL);
   GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
   GBL_API_VERIFY_POINTER(pVariant);
   GBL_API_VERIFY_POINTER(pOther);
   if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE) {
       GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
       GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, 1, pOther, GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE));
   } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_MOVE) {
       GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
       if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
            GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, 0, NULL, GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT));
       }
       GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, 1, pOther, GBL_IVARIANT_OP_FLAG_SET_MOVE));
   } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY) {
        GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
        GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, 1, pOther, GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY));
   } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_COPY) {
       GBL_API_VERIFY_EXPRESSION(pSelf->pVTable->pFnSet);
       if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
            GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, 0, NULL, GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT));
       }
       GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, 1, pOther, GBL_IVARIANT_OP_FLAG_SET_COPY));
   } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_RELOCATABLE) {
       memcpy(pVariant, pOther, sizeof(GblVariant));
   } else {
       GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                          "[GblIVariant] Cannot move construct type: %s",
                          GblType_name(pVariant->type));
   }
   GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblIVariantClass_setCopy(const GblIVariantClass* pSelf, GblVariant* pVariant, const GblVariant* pOther) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pOther);
    if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
        GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, 1, (GblVariant*)pOther, GBL_IVARIANT_OP_FLAG_SET_COPY));
    } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_RELOCATABLE) {
        memcpy(pVariant, pOther, sizeof(GblVariant));
    } else {

        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                           "[GblIVariant] Cannot copy assign type: %s",
                           GblType_name(GblVariant_typeOf(pVariant)));

    }
    GBL_API_END();
}

/* Order of operations
 * 1) Try move assignment
 * 2) Try copy assignment
 * 3) Try memcpy
 * 4) Shit pants
 */
GBL_EXPORT GBL_RESULT GblIVariantClass_setMove(const GblIVariantClass* pSelf, GblVariant* pVariant, GblVariant* pOther) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pOther);
    if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_MOVE) {
        GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
        GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, 1, pOther, GBL_IVARIANT_OP_FLAG_SET_MOVE));
    } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
        GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, 1, pOther, GBL_IVARIANT_OP_FLAG_SET_COPY));
    } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_RELOCATABLE) {
        memcpy(pVariant, pOther, sizeof(GblVariant));
    } else {
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                           "[GblIVariant] Cannot move assign type: %s",
                           GblType_name(GblVariant_typeOf(pVariant)));
    }
    GBL_API_END();
}


GBL_EXPORT GBL_RESULT GblIVariantClass_destruct(const GblIVariantClass* pSelf, GblVariant* pVariant) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
    GBL_API_VERIFY_POINTER(pVariant);
    if(pSelf->pVTable->pFnDestruct) {
        GBL_API_CALL(pSelf->pVTable->pFnDestruct(pVariant));
    }
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblIVariantClass_compare(const GblIVariantClass* pSelf, const GblVariant* pVariant, const GblVariant* pOther, GblInt* pCmpResult) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_VERIFY_POINTER(pCmpResult);
    if(pVariant->type != pOther->type) {
        *pCmpResult = INT_MAX;
    } else if(pSelf->pVTable->pFnCompare) {
        GBL_API_CALL(pSelf->pVTable->pFnCompare(pVariant, pOther, pCmpResult));
    } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_VALUELESS_TYPE) {
        *pCmpResult = 0;
    }
    GBL_API_END();
}

static GBL_RESULT GblIVariantClass_valuesFromVa_(const GblIVariantClass* pSelf, const char* pFmt, va_list* pArgList, GblSize* pCount, GblVariant pVariant[]) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
    GBL_API_VERIFY_POINTER(pCount);
    GBL_API_VERIFY_POINTER(pVariant);
    *pCount = 0;
    char curChar;
    while((curChar = pFmt[*pCount]) != '\0') {
        GblVariant* pCurVariant = &pVariant[*pCount];
        switch(curChar) {
        case 'z': pCurVariant->type = GBL_UINT64_TYPE;  pCurVariant->u64    = va_arg(*pArgList, uint64_t); break;
        case 'q': pCurVariant->type = GBL_INT64_TYPE;   pCurVariant->i64    = va_arg(*pArgList, int64_t);  break;
        case 'l': pCurVariant->type = GBL_UINT32_TYPE;  pCurVariant->u32    = va_arg(*pArgList, uint32_t); break;
        case 'i': pCurVariant->type = GBL_INT32_TYPE;   pCurVariant->i32    = va_arg(*pArgList, int32_t);  break;
        case 'd': pCurVariant->type = GBL_DOUBLE_TYPE;  pCurVariant->f64    = va_arg(*pArgList, double);   break;
        case 'p': pCurVariant->type = GBL_POINTER_TYPE; pCurVariant->pVoid  = va_arg(*pArgList, void*);    break;
        default: GBL_API_ERROR("[GblIVariant] Unknown va value type: %c", curChar); break;
        }

        if(*pCount + 1 >= GBL_IVARIANT_VALUE_VAR_ARG_MAX) break;
        ++(*pCount);
    }
    GBL_API_END();
}
/* 1) Try construct value copy
 * 2) Try set value copy (and maybe constructor)
 * 3) shit pants
 */
GBL_EXPORT GBL_RESULT GblIVariantClass_constructValueCopy(const GblIVariantClass* pSelf, GblVariant* pVariant, va_list* pArgs) {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
        GblSize count = 0;
        GblVariant* pVariants = GBL_ALLOCA(sizeof(GblVariant)*GBL_IVARIANT_VALUE_VAR_ARG_MAX);
        GBL_API_CALL(GblIVariantClass_valuesFromVa_(pSelf, pSelf->pVTable->pSetValueFmt, pArgs, &count, pVariants));
        if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
            GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY));
        } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
            if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
                GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
                GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, 0, NULL, GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT));
            }
            GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY));
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                               "[GblIVariant] Cannot copy construct values for type: %s",
                               GblType_name(GblVariant_typeOf(pVariant)));
        }
    } GBL_API_END();
}
/* 1) Try construct value move
 * 2) Try default constructor + set value move
 * 3) Try construct value copy
 * 4) Try default constructor + construct value copy
 * 5) Shit pants
 */
GBL_EXPORT GBL_RESULT GblIVariantClass_constructValueMove(const GblIVariantClass* pSelf, GblVariant* pVariant, va_list* pArgs) {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
        GblSize count = 0;
        GblVariant* pVariants = GBL_ALLOCA(sizeof(GblVariant)*GBL_IVARIANT_VALUE_VAR_ARG_MAX);
        GBL_API_CALL(GblIVariantClass_valuesFromVa_(pSelf, pSelf->pVTable->pSetValueFmt, pArgs, &count, pVariants));
        if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
            GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE));
        } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
            if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
                GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
                GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, 0, NULL, GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT));
            }
            GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE));
        } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
            GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY));
        } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
            if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
                GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnConstruct);
                GBL_API_CALL(pSelf->pVTable->pFnConstruct(pVariant, 0, NULL, GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT));
            }
            GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY));
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                               "[GblIVariant] Cannot move construct values for type: %s",
                               GblType_name(GblVariant_typeOf(pVariant)));
        }
    } GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblIVariantClass_setValueCopy(const GblIVariantClass* pSelf, GblVariant* pVariant, va_list* pArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
        GblSize count = 0;
        GblVariant* pVariants = GBL_ALLOCA(sizeof(GblVariant)*GBL_IVARIANT_VALUE_VAR_ARG_MAX);
        GBL_API_CALL(GblIVariantClass_valuesFromVa_(pSelf, pSelf->pVTable->pSetValueFmt, pArgs, &count, pVariants));
        if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
            GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY));
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                               "[GblIVariant] Cannot copy set values for type: %s",
                               GblType_name(GblVariant_typeOf(pVariant)));
        }
    } GBL_API_END();
}
GBL_EXPORT GBL_RESULT GblIVariantClass_setValueMove(const GblIVariantClass* pSelf, GblVariant* pVariant, va_list* pArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
        GblSize count = 0;
        GblVariant* pVariants = GBL_ALLOCA(sizeof(GblVariant)*GBL_IVARIANT_VALUE_VAR_ARG_MAX);
        GBL_API_CALL(GblIVariantClass_valuesFromVa_(pSelf, pSelf->pVTable->pSetValueFmt, pArgs, &count, pVariants));
        if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
            GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE));
        } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSet);
            GBL_API_CALL(pSelf->pVTable->pFnSet(pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY));
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                               "[GblIVariant] Cannot move set values for type: %s",
                               GblType_name(GblVariant_typeOf(pVariant)));
        }
    } GBL_API_END();
}


GBL_EXPORT GBL_RESULT GblIVariantClass_getValueCopy(const GblIVariantClass* pSelf, const GblVariant* pVariant, va_list* pArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
        GblSize count = 0;
        GblVariant* pVariants = GBL_ALLOCA(sizeof(GblVariant)*GBL_IVARIANT_VALUE_VAR_ARG_MAX);
        GBL_API_CALL(GblIVariantClass_valuesFromVa_(pSelf, pSelf->pVTable->pGetValueFmt, pArgs, &count, pVariants));
        if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnGet);
            GBL_API_CALL(pSelf->pVTable->pFnGet((GblVariant*)pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY));
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                               "[GblIVariant] Cannot get values by copy for type: %s",
                               GblType_name(GblVariant_typeOf(pVariant)));
        }
    } GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblIVariantClass_getValuePeek(const GblIVariantClass* pSelf, const GblVariant* pVariant, va_list* pArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
        GblSize count = 0;
        GblVariant* pVariants = GBL_ALLOCA(sizeof(GblVariant)*GBL_IVARIANT_VALUE_VAR_ARG_MAX);
        GBL_API_CALL(GblIVariantClass_valuesFromVa_(pSelf, pSelf->pVTable->pGetValueFmt, pArgs, &count, pVariants));
        if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnGet);
            GBL_API_CALL(pSelf->pVTable->pFnGet((GblVariant*)pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK));
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                               "[GblIVariant] Cannot get values by peeking for type: %s",
                               GblType_name(GblVariant_typeOf(pVariant)));
        }
    } GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblIVariantClass_getValueMove(const GblIVariantClass* pSelf, const GblVariant* pVariant, va_list* pArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
        GblSize count = 0;
        GblVariant* pVariants = GBL_ALLOCA(sizeof(GblVariant)*GBL_IVARIANT_VALUE_VAR_ARG_MAX);
        GBL_API_CALL(GblIVariantClass_valuesFromVa_(pSelf, pSelf->pVTable->pGetValueFmt, pArgs, &count, pVariants));
        if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnGet);
            GBL_API_CALL(pSelf->pVTable->pFnGet((GblVariant*)pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE));
        } else if(pSelf->pVTable->supportedOps & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY) {
            GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnGet);
            GBL_API_CALL(pSelf->pVTable->pFnGet((GblVariant*)pVariant, count, pVariants, GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY));
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION,
                               "[GblIVariant] Cannot get values by peeking for type: %s",
                               GblType_name(GblVariant_typeOf(pVariant)));
        }
    } GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblIVariantClass_save(const GblIVariantClass* pSelf, const GblVariant* pVariant, GblStringBuffer* pString) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
    GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnSave);
    GBL_API_CALL(pSelf->pVTable->pFnSave(pVariant, pString));
    GBL_API_END();
}
GBL_EXPORT GBL_RESULT GblIVariantClass_load(const GblIVariantClass* pSelf, GblVariant* pVariant, const GblStringBuffer* pString) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf && pSelf->pVTable);
    GBL_API_VERIFY_POINTER(pSelf->pVTable->pFnLoad);
    GBL_API_CALL(pSelf->pVTable->pFnLoad(pVariant, pString));
    GBL_API_END();

}



