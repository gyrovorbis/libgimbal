#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/meta/ifaces/gimbal_ivariant.h>
#include <gimbal/meta/ifaces/gimbal_itable_variant.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/meta/types/gimbal_type.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/meta/instances/gimbal_object.h>
#include <gimbal/containers/gimbal_tree_set.h>
#include <gimbal/strings/gimbal_string_buffer.h>

#define GBL_VARIANT_BEGIN_(type, classGetterSuffix)                                         \
    GBL_CTX_BEGIN(NULL); {                                                                  \
        const GblType type_ = type;                                                         \
        GBL_CTX_VERIFY_POINTER(pSelf);                                                      \
        GBL_CTX_VERIFY_TYPE(type_, GBL_IVARIANT_TYPE,                                       \
            "[GblVariant] Attempted to use type that doesn't implement IVariant: %s",       \
                GblType_name(type_));                                                       \
        GblClass* pClass_ = GblClass_##classGetterSuffix##Default(type_);                   \
        GBL_CTX_VERIFY_POINTER(pClass_, "Failed to retrieve class for type: %s",            \
                               GblType_name(type_));                                        \
        const GblIVariantClass* pIFace = GBL_IVARIANT_CLASS(pClass_);                       \
        GBL_CTX_VERIFY_EXPRESSION(pIFace,                                                   \
            "Failed to retrieve GblIVariant Interface for type: %s",                        \
                GblType_name(type_))

#define GBL_VARIANT_END_BLOCK_()                                                            \
    } GBL_CTX_END_BLOCK()

#define GBL_VARIANT_END_()                                                                  \
    } GBL_CTX_END()

typedef struct ConverterEntry_ {
    GblType                 fromType;
    GblType                 toType;
    GblVariantConverterFn   pFnConverter;
} ConverterEntry_;


static GblTreeSet   converterRegistry_;
static uint64_t     registryHint_       = 0;
static GblBool      registryValid_      = GBL_FALSE;

int GblVariant_converterComparator_(const GblTreeSet* pSet,
                                       const void*       pEntry1,
                                       const void*       pEntry2)
{
    GBL_UNUSED(pSet);
    const ConverterEntry_* pConv1  = pEntry1;
    const ConverterEntry_* pConv2  = pEntry2;

    if(pConv1->fromType > pConv2->fromType)         return 1;
    else if(pConv1->fromType < pConv2->fromType)    return -1;
    else                                            return pConv1->toType - pConv2->toType;
}

static GBL_RESULT GblVariant_initDefault_(GblVariant* pSelf, GblType type)  {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);
    if(type != GBL_INVALID_TYPE)
        GBL_CTX_VERIFY_TYPE(type, GBL_IVARIANT_TYPE, "[GblVariant] Incompatible Type: %s", GblType_name(type));
    memset(pSelf, 0, sizeof(GblVariant));
    pSelf->type = type;
    GBL_CTX_END();
}

GBL_RESULT GblVariant_init_(GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERBOSE("[GblVariant] Initializing converter registry");
    GBL_CTX_VERIFY(!registryValid_,
                   GBL_RESULT_PARTIAL,
                   "[GblVariant] Double initialize called?");
    registryHint_ = 0;
    GBL_CTX_VERIFY_CALL(GblTreeSet_construct(&converterRegistry_,
                                             sizeof(ConverterEntry_),
                                             GblVariant_converterComparator_,
                                             NULL,
                                             256,
                                             pCtx));
    registryValid_ = GBL_TRUE;
    GBL_CTX_END();
}

GBL_RESULT GblVariant_final_(GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERBOSE("[GblVariant] Finalizing converter registry");
    GBL_CTX_VERIFY(registryValid_,
                   GBL_RESULT_PARTIAL,
                   "[GblVariant] Double finalize called?");
    GBL_CTX_VERIFY_CALL(GblTreeSet_destruct(&converterRegistry_));
    registryValid_ = GBL_FALSE;
    GBL_CTX_END();
}

GBL_EXPORT size_t  GblVariant_converterCount(void) {
    size_t  count = 0;
    GBL_CTX_BEGIN(GblTreeSet_context(&converterRegistry_));
    count = GblTreeSet_size(&converterRegistry_);
    GBL_CTX_END_BLOCK();
    return count;
}

GBL_EXPORT GBL_RESULT GblVariant_registerConverter(GblType fromType, GblType toType, GblVariantConverterFn pFnConv) {
    GBL_CTX_BEGIN(GblTreeSet_context(&converterRegistry_)); {
        GBL_CTX_PUSH_VERBOSE("[GblVariant] Registering Converter: [%s to %s]",
                             GblType_name(fromType), GblType_name(toType));

        GBL_CTX_VERIFY(fromType != GBL_INVALID_TYPE &&
                       toType   != GBL_INVALID_TYPE,
                       GBL_RESULT_ERROR_INVALID_TYPE);

        const ConverterEntry_ entry = {
            fromType,
            toType,
            pFnConv
        };
        ConverterEntry_* pExisting = GblTreeSet_setHint(&converterRegistry_,
                                                        &entry,
                                                        &registryHint_);
        GBL_CTX_VERIFY_LAST_RECORD();
        if(pExisting) {
            GBL_CTX_WARN("[GblVariant] Overwrote exisitng converter: [%s => %s]",
                         GblType_name(pExisting->fromType), GblType_name(pExisting->toType));
        }
        GBL_CTX_POP(1);
    } GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_unregisterConverter(GblType fromType, GblType toType) {
    GBL_CTX_BEGIN(GblTreeSet_context(&converterRegistry_)); {
        const ConverterEntry_ entry = {
            fromType,
            toType,
            NULL
        };
        GBL_CTX_VERIFY(GblTreeSet_erase(&converterRegistry_,
                                        &entry),
                       GBL_RESULT_NOT_FOUND,
                       "[GblVariant] Removing converter from [%s] to [%s]: NOT FOUND",
                       GblType_name(fromType), GblType_name(toType));
    } GBL_CTX_END();
}

GBL_EXPORT GblBool GblVariant_canConvert(GblType fromType, GblType toType) {
    GblBool         result          = GBL_FALSE;
    GblType         currentFromType = fromType;
    GblType         currentToType   = toType;
    ConverterEntry_ entry           = { .toType = toType };

    GBL_CTX_BEGIN(GblTreeSet_context(&converterRegistry_)); {
        GBL_CTX_VERIFY(fromType != GBL_INVALID_TYPE &&
                       toType   != GBL_INVALID_TYPE,
                       GBL_RESULT_ERROR_INVALID_TYPE);

        if(GblType_check(fromType, toType)) {
            result = GBL_TRUE;
        } else {
            while(!result && currentToType != GBL_INVALID_TYPE) {
                entry.toType = currentToType;
                currentFromType = fromType;

                while(!result && currentFromType != GBL_INVALID_TYPE) {
                    entry.fromType = currentFromType;

                    result = GblTreeSet_contains(&converterRegistry_,
                                                 &entry);

                    if(result) GBL_CTX_DONE();

                    if(!GblType_check(currentFromType,
                                      GblType_parent(currentFromType)))
                        break;

                    currentFromType = GblType_parent(currentFromType);
                }

                if(!GblType_check(currentToType,
                                  GblType_parent(currentToType)))
                    break;

                currentToType = GblType_parent(currentToType);
            }
        }
    } GBL_CTX_END_BLOCK();
    return result;
}

GBL_EXPORT GBL_RESULT GblVariant_convert(const GblVariant* pSelf, GblVariant* pOther) {
    GBL_CTX_BEGIN(GblTreeSet_context(&converterRegistry_)); {
        GBL_CTX_VERIFY(GblVariant_typeOf(pSelf)  != GBL_INVALID_TYPE &&
                       GblVariant_typeOf(pOther) != GBL_INVALID_TYPE,
                       GBL_RESULT_ERROR_INVALID_TYPE);

        // Check whether we actually have to convert shit.
        if(GblType_check(GblVariant_typeOf(pSelf), GblVariant_typeOf(pOther))) {
            GBL_CTX_VERIFY_CALL(GblVariant_setCopy(pOther, pSelf));

        } else { // Okay, fine, we do.

            const ConverterEntry_*  pEntry      = NULL;
            GblType                 fromType    = GblVariant_typeOf(pSelf);
            GblType                 toType      = GblVariant_typeOf(pOther);
            ConverterEntry_         entry       = { .toType = GblVariant_typeOf(pOther) };

            while(!pEntry && toType != GBL_INVALID_TYPE) {
                entry.toType = toType;
                fromType = GblVariant_typeOf(pSelf);

                while(!pEntry && fromType != GBL_INVALID_TYPE) {
                    entry.fromType = fromType;

                    pEntry = GblTreeSet_get(&converterRegistry_,
                                           &entry);

                    if(pEntry) goto done;
                    if(!GblType_check(fromType, GblType_parent(fromType)))
                        break;

                    fromType = GblType_parent(fromType);
                }

                if(!GblType_check(toType, GblType_parent(toType)))
                    break;

                toType = GblType_parent(toType);

            }

            done:
            GBL_CTX_VERIFY(pEntry,
                           GBL_RESULT_ERROR_INVALID_CONVERSION,
                           "[GblVariant] Conversion failed: No converter found! [%s => %s],",
                           GblType_name(GblVariant_typeOf(pSelf)), GblType_name(GblVariant_typeOf(pOther)));
            GBL_CTX_VERIFY_LAST_RECORD();
            GBL_CTX_VERIFY_CALL(pEntry->pFnConverter(pSelf, pOther));
        }

    } GBL_CTX_END();
}

GBL_EXPORT int GblVariant_compare(const GblVariant* pSelf, const GblVariant* pOther) {
    int result = INT_MAX;
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);

    GBL_CTX_VERIFY(GblVariant_typeOf(pSelf)  != GBL_INVALID_TYPE &&
                   GblVariant_typeOf(pOther) != GBL_INVALID_TYPE,
                   GBL_RESULT_ERROR_INVALID_TYPE);

    if(GblType_check(GblVariant_typeOf(pSelf), GblVariant_typeOf(pOther))) {
        GBL_CTX_VERIFY_CALL(GblIVariantClass_compare(pIFace, pSelf, pOther, &result));
    } else {
        if(GblVariant_canConvert(GblVariant_typeOf(pSelf), GblVariant_typeOf(pOther))) {
            GBL_VARIANT(temp);
            const GblClass*         pOtherClass = GblClass_weakRefDefault(GblVariant_typeOf(pOther));
            const GblIVariantClass* pOtherIFace = GBL_IVARIANT_CLASS(pOtherClass);
            GBL_CTX_VERIFY_EXPRESSION(pOtherIFace);
            GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&temp, GblVariant_typeOf(pOther)));
            GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &temp));
            GBL_CTX_VERIFY_CALL(GblIVariantClass_compare(pOtherIFace, &temp, pOther, &result));
            GBL_CTX_VERIFY_CALL(GblVariant_destruct(&temp));
        } else if(GblVariant_canConvert(GblVariant_typeOf(pOther), GblVariant_typeOf(pSelf))) {
            GBL_VARIANT(temp);
            GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&temp, GblVariant_typeOf(pSelf)));
            GBL_CTX_VERIFY_CALL(GblVariant_convert(pOther, &temp));
            GBL_CTX_VERIFY_CALL(GblIVariantClass_compare(pIFace, pSelf, &temp, &result));
            GBL_CTX_VERIFY_CALL(GblVariant_destruct(&temp));
        }
    }
    GBL_VARIANT_END_BLOCK_();
    return result;
}

GBL_EXPORT GBL_RESULT GblVariant_constructDefault(GblVariant* pSelf, GblType type)  {
    GBL_VARIANT_BEGIN_(type, ref);
    GBL_CTX_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_CTX_CALL(GblIVariantClass_constructDefault(pIFace, pSelf));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_constructCopy(GblVariant* pSelf,  const GblVariant* pOther)  {
    GBL_VARIANT_BEGIN_(pOther->type, ref);
    GBL_CTX_CALL(GblVariant_initDefault_(pSelf, pOther->type));
    GBL_CTX_CALL(GblIVariantClass_constructCopy(pIFace, pSelf, pOther));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_constructMove(GblVariant* pSelf, GblVariant* pOther)  {
    GBL_VARIANT_BEGIN_(pOther->type, ref); //still has to copy class reference
    GBL_CTX_CALL(GblVariant_initDefault_(pSelf, pOther->type));
    GBL_CTX_CALL(GblIVariantClass_constructMove(pIFace, pSelf, pOther));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_setCopy(GblVariant* pSelf, const GblVariant* pOther)  {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    if(pSelf->type != pOther->type) {
        GBL_CTX_CALL(GblVariant_destruct(pSelf));
        GBL_CTX_CALL(GblVariant_constructCopy(pSelf, pOther));
    } else {
        GBL_CTX_CALL(GblIVariantClass_setCopy(pIFace, pSelf, pOther));
    }
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_setMove(GblVariant* pSelf, GblVariant* pOther)  {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    /* you still have to destruct, what if it's another ref you're oving over!? */
    //if(pSelf->type != pOther->type) {
        GBL_CTX_CALL(GblVariant_destruct(pSelf));
        GBL_CTX_CALL(GblVariant_constructMove(pSelf, pOther));
    //} else {
    //   GBL_CTX_CALL(GblIVariantClass_setMove(pIFace, pSelf, pOther));
    //}
    GBL_VARIANT_END_();
}
/*
 * Destruction is the only time we should be totally fine with an invalid variant.
 * This could happen because of move semantics "taking" the value away from it
 * and invalidating the variant, or because of a failed construction, assignment,
 * or conversion. This is analogous to free(NULL) being valid.
 */
GBL_EXPORT GBL_RESULT GblVariant_destruct(GblVariant* pSelf)  {
    if(GblVariant_typeOf(pSelf) != GBL_INVALID_TYPE) {
        GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
        GBL_CTX_CALL(GblIVariantClass_destruct(pIFace, pSelf));
        GblClass_unrefDefault(pClass_);
        pSelf->type = GBL_INVALID_TYPE;
        GBL_VARIANT_END_();
    } else return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblVariant_save(const GblVariant* pSelf, GblStringBuffer* pString)  {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_save(pIFace, pSelf, pString));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_load(GblVariant* pSelf, const GblStringBuffer* pString)  {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_load(pIFace, pSelf, pString));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_constructValueCopyVaList(GblVariant* pSelf, GblType type, va_list* pList)  {
    GBL_VARIANT_BEGIN_(type, ref);
    GBL_CTX_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_CTX_CALL(GblIVariantClass_constructValueCopy(pIFace, pSelf, pList));
    if(GBL_RESULT_ERROR(GBL_CTX_LAST_RESULT())) {
        GBL_CTX_CLEAR_LAST_RECORD();
        GblClass_unrefDefault(pClass_);
        GBL_CTX_CALL(GblVariant_initDefault_(pSelf, GBL_INVALID_TYPE));
    }
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_constructValueMoveVaList(GblVariant* pSelf, GblType type, va_list* pList)  {
    GBL_VARIANT_BEGIN_(type, ref);
    GBL_CTX_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_CTX_CALL(GblIVariantClass_constructValueMove(pIFace, pSelf, pList));
    if(GBL_RESULT_ERROR(GBL_CTX_LAST_RESULT())) {
        GBL_CTX_CLEAR_LAST_RECORD();
        GblClass_unrefDefault(pClass_);
        GBL_CTX_CALL(GblVariant_initDefault_(pSelf, GBL_INVALID_TYPE));
    }
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_constructValueCopy(GblVariant* pSelf, GblType type, ...)  {
    va_list list;
    va_start(list, type);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblVariant_constructValueCopyVaList(pSelf, type, &list));
    GBL_CTX_END_BLOCK();
    va_end(list);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_constructValueMove(GblVariant* pSelf, GblType type, ...)  {
    GBL_CTX_BEGIN(NULL);
    va_list varArgs;
    va_start(varArgs, type);
    GBL_CTX_CALL(GblVariant_constructValueMoveVaList(pSelf, type, &varArgs));
    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_setValueCopy(GblVariant* pSelf, GblType type, ...)  {
    va_list varArgs;
    va_start(varArgs, type);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopyVaList(pSelf, type, &varArgs));
    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_setValueMove(GblVariant* pSelf, GblType type, ...)  {
    va_list varArgs;
    va_start(varArgs, type);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMoveVaList(pSelf, type, &varArgs));
    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_setValueCopyVaList(GblVariant* pSelf,
                                                    GblType  type,
                                                    va_list* pVarArgs)
{
    GBL_CTX_BEGIN(NULL);
    if(type != pSelf->type) {
        GBL_CTX_VERIFY_TYPE(type, GBL_IVARIANT_TYPE);
        GblIVariantClass* pIFace = GBL_IVARIANT_CLASS(GblClass_refDefault(type));
        GBL_CTX_CALL(GblVariant_destruct(pSelf));
        GBL_CTX_CALL(GblVariant_initDefault_(pSelf, type));
        GBL_CTX_CALL(GblIVariantClass_constructValueCopy(pIFace, pSelf, pVarArgs));
        if(GBL_RESULT_ERROR(GBL_CTX_LAST_RESULT())) {
            GBL_CTX_CLEAR_LAST_RECORD();
            GblClass_unrefDefault(GblClass_weakRefDefault(type));
            GBL_CTX_CALL(GblVariant_initDefault_(pSelf, GBL_INVALID_TYPE));
        }
    } else {
        GblIVariantClass* pIFace = GBL_IVARIANT_CLASS(GblClass_weakRefDefault(type));
        GBL_CTX_CALL(GblIVariantClass_setValueCopy(pIFace, pSelf, pVarArgs));
        if(GBL_RESULT_ERROR(GBL_CTX_LAST_RESULT())) {
            GBL_CTX_CLEAR_LAST_RECORD();
            GblClass_unrefDefault(GblClass_weakRefDefault(type));
            GBL_CTX_CALL(GblVariant_initDefault_(pSelf, GBL_INVALID_TYPE));
        }
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setValueMoveVaList(GblVariant* pSelf,
                                                    GblType  type,
                                                    va_list* pVarArgs)
{
    GBL_CTX_BEGIN(NULL);
    if(type != pSelf->type) {
        GBL_CTX_VERIFY_TYPE(type, GBL_IVARIANT_TYPE);
        GblIVariantClass* pIFace = GBL_IVARIANT_CLASS(GblClass_refDefault(type));
        GBL_CTX_CALL(GblVariant_destruct(pSelf));
        GBL_CTX_CALL(GblVariant_initDefault_(pSelf, type));
        GBL_CTX_CALL(GblIVariantClass_constructValueMove(pIFace, pSelf, pVarArgs));
        if(GBL_RESULT_ERROR(GBL_CTX_LAST_RESULT())) {
            GBL_CTX_CLEAR_LAST_RECORD();
            GblClass_unrefDefault(GblClass_weakRefDefault(type));
            GBL_CTX_CALL(GblVariant_initDefault_(pSelf, GBL_INVALID_TYPE));
        }
    } else {
        GblIVariantClass* pIFace = GBL_IVARIANT_CLASS(GblClass_weakRefDefault(type));
        GBL_CTX_CALL(GblIVariantClass_setValueMove(pIFace, pSelf, pVarArgs));
        if(GBL_RESULT_ERROR(GBL_CTX_LAST_RESULT())) {
            GBL_CTX_CLEAR_LAST_RECORD();
            GblClass_unrefDefault(GblClass_weakRefDefault(type));
            GBL_CTX_CALL(GblVariant_initDefault_(pSelf, GBL_INVALID_TYPE));
        }
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_copyValue(const GblVariant* pSelf, ...)  {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_getValueCopy(pIFace, pSelf, &varArgs));
    GBL_VARIANT_END_BLOCK_();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_peekValue(const GblVariant* pSelf, ...)  {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_getValuePeek(pIFace, pSelf, &varArgs));
    GBL_VARIANT_END_BLOCK_();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_moveValue(GblVariant* pSelf,  ...)  {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblVariant_moveValueVaList(pSelf, &varArgs));
    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_moveValueVaList(GblVariant* pSelf, va_list* pVarArgs)  {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_getValueMove(pIFace, pSelf, pVarArgs));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_peekValueVaList(GblVariant* pSelf, va_list* pVarArgs)  {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_getValuePeek(pIFace, pSelf, pVarArgs));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_copyValueVaList(const GblVariant* pSelf, va_list* pVarArgs)  {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_getValueCopy(pIFace, pSelf, pVarArgs));
    GBL_VARIANT_END_();
}

GBL_EXPORT GblBox* GblVariant_getBoxCopy(const GblVariant* pSelf)  {
    GblBox* pBox = GBL_NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_BOX_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &pBox));
    GBL_CTX_END_BLOCK();
    return pBox;
}

GBL_EXPORT GblBox* GblVariant_getBoxMove(GblVariant* pSelf)  {
    GblBox* pBox = GBL_NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_BOX_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(pSelf, &pBox));
    GBL_CTX_END_BLOCK();
    return pBox;
}

GBL_EXPORT GblBox* GblVariant_getBoxPeek(const GblVariant* pSelf)  {
    GblBox* pBox = GBL_NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_BOX_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(pSelf, &pBox));
    GBL_CTX_END_BLOCK();
    return pBox;
}

GBL_EXPORT GblObject* GblVariant_getObjectCopy(const GblVariant* pSelf)  {
    GblObject* pObject = GBL_NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OBJECT_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &pObject));
    GBL_CTX_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblVariant_getObjectMove(GblVariant* pSelf)  {
    GblObject* pObject = GBL_NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OBJECT_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(pSelf, &pObject));
    GBL_CTX_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblVariant_getObjectPeek(const GblVariant* pSelf)  {
    GblObject* pObject = GBL_NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OBJECT_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(pSelf, &pObject));
    GBL_CTX_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblBool GblVariant_toBool(GblVariant* pSelf)  {
    GblBool value = GBL_FALSE;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_BOOL_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&v, GBL_FALSE));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getBool(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT char GblVariant_toChar(GblVariant* pSelf)  {
    char value = '\0';
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_CHAR_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructChar(&v, '\0'));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getChar(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT uint8_t GblVariant_toUint8(GblVariant* pSelf)  {
    uint8_t value = 0;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_UINT8_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructUint8(&v, 0));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getUint8(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT uint16_t GblVariant_toUint16(GblVariant* pSelf)  {
    uint16_t value = 0;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_UINT16_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructUint16(&v, 0));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getUint16(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT int16_t GblVariant_toInt16(GblVariant* pSelf)  {
    int16_t value = 0;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_INT16_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructInt16(&v, 0));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getInt16(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT uint32_t GblVariant_toUint32(GblVariant* pSelf)  {
    uint32_t value = 0;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_UINT32_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructUint32(&v, 0));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getUint32(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT int32_t GblVariant_toInt32(GblVariant* pSelf)  {
    int32_t value = 0;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_INT32_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructInt32(&v, 0));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getInt32(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT uint64_t GblVariant_toUint64(GblVariant* pSelf)  {
    uint64_t value = 0;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_UINT64_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructUint64(&v, 0));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getUint64(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT int64_t GblVariant_toInt64(GblVariant* pSelf)  {
    int64_t value = 0;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_INT64_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructInt64(&v, 0));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getInt64(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT size_t  GblVariant_toSize(GblVariant* pSelf)  {
    size_t  value = 0;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_INT64_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructSize(&v, 0));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getSize(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT GblEnum GblVariant_toEnum(GblVariant* pSelf)  {
    GblEnum value = 0;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_ENUM_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructEnum(&v, GBL_ENUM_TYPE, 0));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getEnum(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT GblFlags GblVariant_toFlags(GblVariant* pSelf)  {
    GblFlags value = 0;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_FLAGS_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructFlags(&v, GBL_FLAGS_TYPE, 0));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getFlags(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT float GblVariant_toFloat(GblVariant* pSelf)  {
    float value = 0.0f;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_FLOAT_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructFloat(&v, 0.0f));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getFloat(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT double GblVariant_toDouble(GblVariant* pSelf)  {
    double value = 0.0f;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_DOUBLE_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructDouble(&v, 0.0f));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getDouble(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT void* GblVariant_toPointer(GblVariant* pSelf)  {
    void* pValue = GBL_NULL;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_POINTER_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructPointer(&v, GBL_POINTER_TYPE, NULL));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    pValue = GblVariant_getPointer(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_EXPORT const char* GblVariant_toString(GblVariant* pSelf)  {
    const GblStringRef* pValue = GBL_NULL;

    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_STRING_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructString(&v, NULL));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    pValue = GblVariant_getString(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_EXPORT GblStringView GblVariant_toStringView(GblVariant* pSelf)  {
    return GblStringView_fromString(GblVariant_toString(pSelf));
}

GBL_EXPORT GblType GblVariant_toTypeValue(GblVariant* pSelf)  {
    GblType value = GBL_INVALID_TYPE;
    GBL_CTX_BEGIN(NULL);
    if(GblVariant_typeOf(pSelf) != GBL_TYPE_TYPE) {
        GBL_VARIANT(v);
        GBL_CTX_VERIFY_CALL(GblVariant_constructTypeValue(&v, GBL_INVALID_TYPE));
        GBL_CTX_VERIFY_CALL(GblVariant_convert(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_setMove(pSelf, &v));
        GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    }
    value = GblVariant_getTypeValue(pSelf);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT GblHash GblVariant_hash(const GblVariant* pSelf) {
    return gblHash(pSelf, sizeof(GblVariant));
}

GBL_EXPORT GblVariant* GblVariant_index(const GblVariant* pSelf,
                                        const GblVariant* pKey,
                                        GblVariant*       pValue)
{
    const GblType type = GblVariant_typeOf(pSelf);

    if(!GblType_implements(type, GBL_ITABLE_VARIANT_TYPE)) {
        GblVariant_setNil(pValue);
    } else {
        const GblClass* pClass = GblClass_weakRefDefault(type);
        GBL_ITABLE_VARIANT_CLASS(pClass)->pFnIndex(pSelf, pKey, pValue);
    }

    return pValue;
}

GBL_EXPORT GblVariant* GblVariant_field(const GblVariant* pSelf,
                                        const char*       pName,
                                        GblVariant*       pValue)
{
    GblVariant k;
    GblVariant_construct(&k, pName);
    GblVariant* pRetVal = GblVariant_index(pSelf, &k, pValue);
    GblVariant_destruct(&k);
    return pRetVal;
}

GBL_EXPORT GblVariant* GblVariant_element(const GblVariant* pSelf,
                                          size_t            index,
                                          GblVariant*       pValue)
{
    GblVariant i;
    GblVariant_constructSize(&i, index);
    GblVariant* pRetVal = GblVariant_index(pSelf, &i, pValue);
    GblVariant_destruct(&i);
    return pRetVal;
}

GBL_EXPORT GBL_RESULT GblVariant_setIndex(GblVariant*       pSelf,
                                          const GblVariant* pKey,
                                          GblVariant*       pValue)
{
    GBL_CTX_BEGIN(NULL);

    const GblType type = GblVariant_typeOf(pSelf);

    GBL_CTX_VERIFY(GblType_implements(type, GBL_ITABLE_VARIANT_TYPE),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempt to setIndex() on non-GblITableVariant type: [%s]",
                   GblType_name(type));

    const GblClass* pClass = GblClass_weakRefDefault(type);
    GBL_CTX_VERIFY_CALL(
        GBL_ITABLE_VARIANT_CLASS(pClass)->pFnSetIndex(pSelf, pKey, pValue)
    );

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setField(GblVariant* pSelf,
                                          const char* pName,
                                          GblVariant* pValue)
{
    GblVariant k;
    GblVariant_construct(&k, pName);
    const GBL_RESULT result = GblVariant_setIndex(pSelf, &k, pValue);
    GblVariant_destruct(&k);
    return result;
}

GBL_EXPORT GBL_RESULT GblVariant_setElement(GblVariant* pSelf,
                                            size_t            index,
                                            GblVariant*       pValue)
{
    GblVariant i;
    GblVariant_constructSize(&i, index);
    const GBL_RESULT result = GblVariant_setIndex(pSelf, &i, pValue);
    GblVariant_destruct(&i);
    return result;
}

GBL_EXPORT GblBool GblVariant_next(const GblVariant* pSelf,
                                   GblVariant*       pKey,
                                   GblVariant*       pValue)
{
    const GblType type = GblVariant_typeOf(pSelf);

    if(!GblType_implements(type, GBL_ITABLE_VARIANT_TYPE)) {
        GblVariant_setNil(pKey);
        GblVariant_setNil(pValue);
        return GBL_FALSE;
    } else {
        const GblClass* pClass = GblClass_weakRefDefault(type);
        GBL_ITABLE_VARIANT_CLASS(pClass)->pFnNext(pSelf, pKey, pValue);
        return !GblVariant_isNil(pKey);
    }
}

GBL_EXPORT size_t GblVariant_count(const GblVariant* pSelf) {
    const GblType type = GblVariant_typeOf(pSelf);

    if(!GblType_implements(type, GBL_ITABLE_VARIANT_TYPE))
        return 0;
    else {
        size_t count = 0;
        const GblClass* pClass = GblClass_weakRefDefault(type);
        GBL_ITABLE_VARIANT_CLASS(pClass)->pFnCount(pSelf, &count);
        return count;
    }
}

GBL_EXPORT GblBool GblVariant_checkTypeCompatible(GblType type) {
    return type != GBL_INVALID_TYPE? GblType_check(type, GBL_IVARIANT_TYPE) : GBL_TRUE;
}

GBL_EXPORT GblType GblVariant_typeOf(const GblVariant* pSelf) {
    return pSelf->type;
}

GBL_EXPORT const char* GblVariant_typeName(const GblVariant* pSelf) {
    return GblType_name(GblVariant_typeOf(pSelf));
}

GBL_EXPORT GBL_RESULT GblVariant_constructNil(GblVariant* pSelf) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(pSelf, GBL_NIL_TYPE));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructBool(GblVariant* pSelf, GblBool value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_BOOL_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructChar(GblVariant* pSelf, char value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_CHAR_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructUint8(GblVariant* pSelf, uint8_t value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_UINT8_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructUint16(GblVariant* pSelf, uint16_t value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_UINT16_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructInt16(GblVariant* pSelf, int16_t value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_INT16_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructUint32(GblVariant* pSelf, uint32_t value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_UINT32_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructInt32(GblVariant* pSelf, int32_t value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_INT32_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructUint64(GblVariant* pSelf, uint64_t value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_UINT64_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructInt64(GblVariant* pSelf, int64_t value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_INT64_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructSize(GblVariant* pSelf, size_t  value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_SIZE_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructEnum(GblVariant* pSelf, GblType type, GblEnum value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(type, GBL_ENUM_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, type, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructFlags(GblVariant* pSelf, GblType type, GblFlags value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(type, GBL_FLAGS_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, type, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructFloat(GblVariant* pSelf, float value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_FLOAT_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructDouble(GblVariant* pSelf, double value) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_DOUBLE_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructPointer(GblVariant* pSelf, GblType ptrType, void* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY(GBL_POINTER_TYPE_CHECK(ptrType),
                   GBL_RESULT_ERROR_TYPE_MISMATCH);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, ptrType, pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructString(GblVariant* pSelf, const char* pValue) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_STRING_TYPE, pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructOpaqueCopy(GblVariant* pSelf, GblType opaqueType, void*  pValue) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(opaqueType, GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, opaqueType, pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructOpaqueMove(GblVariant* pSelf, GblType opaqueType, void* pValue) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(opaqueType, GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(pSelf, opaqueType, pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructStringView(GblVariant* pSelf, GblStringView view) {
    return GblVariant_constructString(pSelf, GBL_STRING_VIEW_CSTR(view));
}

GBL_EXPORT GBL_RESULT GblVariant_constructTypeValue(GblVariant* pSelf, GblType type) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_TYPE_TYPE, type));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructInstance(GblVariant* pSelf, GblInstance* pValue) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructBoxCopy(GblVariant* pSelf, GblBox* pValue) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructBoxMove(GblVariant* pSelf, GblBox* pValue) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(pSelf, GBL_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructObjectCopy(GblVariant* pSelf, GblObject* pValue) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_constructObjectMove(GblVariant* pSelf, GblObject* pValue) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(pSelf, GBL_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_EXPORT GblBool GblVariant_isValid(const GblVariant* pSelf) {
    return pSelf && GblVariant_typeOf(pSelf) != GBL_INVALID_TYPE;
}

GBL_EXPORT GblBool GblVariant_isNil(const GblVariant* pSelf) {
    return pSelf && GblVariant_typeOf(pSelf) == GBL_NIL_TYPE;
}

GBL_EXPORT GblBool GblVariant_getBool(const GblVariant* pSelf) {
    GblBool value = GBL_FALSE;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_BOOL_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT char GblVariant_getChar(const GblVariant* pSelf) {
    char value = '\0';;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_CHAR_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT uint8_t GblVariant_getUint8(const GblVariant* pSelf) {
    uint8_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_UINT8_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT uint16_t GblVariant_getUint16(const GblVariant* pSelf) {
    uint16_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_UINT16_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT int16_t GblVariant_getInt16(const GblVariant* pSelf) {
    int16_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_INT16_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT uint32_t GblVariant_getUint32(const GblVariant* pSelf) {
    uint32_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_UINT32_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT int32_t GblVariant_getInt32(const GblVariant* pSelf) {
    int32_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_INT32_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT uint64_t GblVariant_getUint64(const GblVariant* pSelf) {
    uint64_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_UINT64_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT int64_t GblVariant_getInt64(const GblVariant* pSelf) {
    int64_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_INT64_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT size_t  GblVariant_getSize(const GblVariant* pSelf) {
    size_t  value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_SIZE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT GblEnum GblVariant_getEnum(const GblVariant* pSelf) {
    GblEnum value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_ENUM_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT GblEnum GblVariant_getFlags(const GblVariant* pSelf) {
    GblFlags value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_FLAGS_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT float GblVariant_getFloat(const GblVariant* pSelf) {
    float value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_FLOAT_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT double GblVariant_getDouble(const GblVariant* pSelf) {
    double value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_DOUBLE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT void* GblVariant_getPointer(const GblVariant* pSelf) {
    void* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_POINTER_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_EXPORT void* GblVariant_getOpaqueCopy(const GblVariant* pSelf) {
    void* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_copyValue(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_EXPORT void* GblVariant_getOpaqueMove(GblVariant* pSelf) {
    void* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_moveValue(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_EXPORT void* GblVariant_getOpaquePeek(const GblVariant* pSelf) {
    void* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_EXPORT const char* GblVariant_getString(const GblVariant* pSelf) {
    const char* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_STRING_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_EXPORT GblStringRef* GblVariant_getStringRef(const GblVariant* pSelf) {
    GblStringRef* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_STRING_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_EXPORT GblStringView GblVariant_getStringView(const GblVariant* pSelf) {
    return GblStringRef_view(GblVariant_getString(pSelf));
}

GBL_EXPORT GblType GblVariant_getTypeValue(const GblVariant* pSelf) {
    GblType value = GBL_INVALID_TYPE;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_TYPE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT GblInstance* GblVariant_getInstance(const GblVariant* pSelf) {
    GblInstance* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_INSTANCE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_peekValue(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_EXPORT GBL_RESULT GblVariant_invalidate(GblVariant* pSelf) {
    if(GblVariant_typeOf(pSelf) != GBL_INVALID_TYPE)
        GblClass_unrefDefault(GblClass_weakRefDefault(GblVariant_typeOf(pSelf)));
    memset(pSelf, 0, sizeof(GblVariant));
    pSelf->type = GBL_INVALID_TYPE;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblVariant_setNil(GblVariant* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(pSelf));
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(pSelf, GBL_NIL_TYPE));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setBool(GblVariant* pSelf, GblBool value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_BOOL_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setChar(GblVariant* pSelf, char value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_CHAR_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setUint8(GblVariant* pSelf, uint8_t value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_UINT8_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setUint16(GblVariant* pSelf, uint16_t value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_UINT16_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setInt16(GblVariant* pSelf, int16_t value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_INT16_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setUint32(GblVariant* pSelf, uint32_t value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_UINT32_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setInt32(GblVariant* pSelf, int32_t value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_INT32_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setUint64(GblVariant* pSelf, uint64_t value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_UINT64_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setInt64(GblVariant* pSelf, int64_t value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_INT64_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setSize(GblVariant* pSelf, size_t  value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_SIZE_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setEnum(GblVariant* pSelf,
                                         GblType     enumType,
                                         GblEnum     value)
{
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(enumType, GBL_ENUM_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, enumType, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setFlags(GblVariant* pSelf,
                                          GblType     flagsType,
                                          GblFlags    value)
{
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(flagsType, GBL_FLAGS_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, flagsType, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setFloat(GblVariant* pSelf, float value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_FLOAT_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setDouble(GblVariant* pSelf, double value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_DOUBLE_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setPointer(GblVariant* pSelf,
                                            GblType ptrType,
                                            void* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(ptrType, GBL_POINTER_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, ptrType, pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setOpaqueCopy(GblVariant* pSelf, GblType opaqueType, void*  pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(opaqueType, GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, opaqueType, pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setOpaqueMove(GblVariant* pSelf, GblType opaqueType, void*  pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(opaqueType, GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pSelf, opaqueType, pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setString(GblVariant* pSelf, const char* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_STRING_TYPE, pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setStringView(GblVariant* pSelf, GblStringView value) {
    return GblVariant_setString(pSelf, GBL_STRING_VIEW_CSTR(value));
}

GBL_EXPORT GBL_RESULT GblVariant_setStringRef(GblVariant* pSelf, GblStringRef* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pSelf, GBL_STRING_TYPE, pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setTypeValue(GblVariant* pSelf, GblType value) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_TYPE_TYPE, value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setInstance(GblVariant* pSelf, GblInstance* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setBoxCopy(GblVariant* pSelf, GblBox* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setBoxMove(GblVariant* pSelf, GblBox* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pSelf, GBL_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setObjectCopy(GblVariant* pSelf, GblObject* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblVariant_setObjectMove(GblVariant* pSelf, GblObject* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pSelf, GBL_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_EXPORT GblBool GblVariant_equals(const GblVariant* pSelf, const GblVariant* pOther) {
    return GblVariant_compare(pSelf, pOther) == 0;
}
