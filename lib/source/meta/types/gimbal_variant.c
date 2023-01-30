#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/meta/ifaces/gimbal_ivariant.h>
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

static GBL_RESULT GblVariant_initDefault_(GblVariant* pSelf, GblType type) GBL_NOEXCEPT {
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

GBL_EXPORT GblSize GblVariant_converterCount(void) {
    GblSize count = 0;
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

GBL_EXPORT GBL_RESULT GblVariant_constructDefault(GblVariant* pSelf, GblType type) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, ref);
    GBL_CTX_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_CTX_CALL(GblIVariantClass_constructDefault(pIFace, pSelf));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_constructCopy(GblVariant* pSelf,  const GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pOther->type, ref);
    GBL_CTX_CALL(GblVariant_initDefault_(pSelf, pOther->type));
    GBL_CTX_CALL(GblIVariantClass_constructCopy(pIFace, pSelf, pOther));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_constructMove(GblVariant* pSelf, GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pOther->type, ref); //still has to copy class reference
    GBL_CTX_CALL(GblVariant_initDefault_(pSelf, pOther->type));
    GBL_CTX_CALL(GblIVariantClass_constructMove(pIFace, pSelf, pOther));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_setCopy(GblVariant* pSelf, const GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    if(pSelf->type != pOther->type) {
        GBL_CTX_CALL(GblVariant_destruct(pSelf));
        GBL_CTX_CALL(GblVariant_constructCopy(pSelf, pOther));
    } else {
        GBL_CTX_CALL(GblIVariantClass_setCopy(pIFace, pSelf, pOther));
    }
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_setMove(GblVariant* pSelf, GblVariant* pOther) GBL_NOEXCEPT {
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
GBL_EXPORT GBL_RESULT GblVariant_destruct(GblVariant* pSelf) GBL_NOEXCEPT {
    if(GblVariant_typeOf(pSelf) != GBL_INVALID_TYPE) {
        GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
        GBL_CTX_CALL(GblIVariantClass_destruct(pIFace, pSelf));
        GblClass_unrefDefault(pClass_);
        pSelf->type = GBL_INVALID_TYPE;
        GBL_VARIANT_END_();
    } else return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblVariant_save(const GblVariant* pSelf, GblStringBuffer* pString) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_save(pIFace, pSelf, pString));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_load(GblVariant* pSelf, const GblStringBuffer* pString) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_load(pIFace, pSelf, pString));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_constructValueCopyVaList(GblVariant* pSelf, GblType type, va_list* pList) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblVariant_constructValueMoveVaList(GblVariant* pSelf, GblType type, va_list* pList) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblVariant_constructValueCopy(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    va_list list;
    va_start(list, type);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblVariant_constructValueCopyVaList(pSelf, type, &list));
    GBL_CTX_END_BLOCK();
    va_end(list);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_constructValueMove(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    va_list varArgs;
    va_start(varArgs, type);
    GBL_CTX_CALL(GblVariant_constructValueMoveVaList(pSelf, type, &varArgs));
    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_setValueCopy(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, type);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopyVaList(pSelf, type, &varArgs));
    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_setValueMove(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
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
                                                    va_list* pVarArgs) GBL_NOEXCEPT
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
                                                    va_list* pVarArgs) GBL_NOEXCEPT
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

GBL_EXPORT GBL_RESULT GblVariant_getValueCopy(const GblVariant* pSelf, ...) GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_getValueCopy(pIFace, pSelf, &varArgs));
    GBL_VARIANT_END_BLOCK_();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_getValuePeek(const GblVariant* pSelf, ...) GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_getValuePeek(pIFace, pSelf, &varArgs));
    GBL_VARIANT_END_BLOCK_();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_getValueMove(GblVariant* pSelf,  ...) GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblVariant_getValueMoveVaList(pSelf, &varArgs));
    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblVariant_getValueMoveVaList(GblVariant* pSelf, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_getValueMove(pIFace, pSelf, pVarArgs));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_getValuePeekVaList(GblVariant* pSelf, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_getValuePeek(pIFace, pSelf, pVarArgs));
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_getValueCopyVaList(const GblVariant* pSelf, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, weakRef);
    GBL_CTX_CALL(GblIVariantClass_getValueCopy(pIFace, pSelf, pVarArgs));
    GBL_VARIANT_END_();
}

GBL_EXPORT GblObject* GblVariant_getObjectCopy(const GblVariant* pSelf) GBL_NOEXCEPT {
    GblObject* pObject = GBL_NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OBJECT_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &pObject));
    GBL_CTX_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblVariant_getObjectMove(GblVariant* pSelf) GBL_NOEXCEPT {
    GblObject* pObject = GBL_NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OBJECT_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueMove(pSelf, &pObject));
    GBL_CTX_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblVariant_getObjectweakRef(const GblVariant* pSelf) GBL_NOEXCEPT {
    GblObject* pObject = GBL_NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OBJECT_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValuePeek(pSelf, &pObject));
    GBL_CTX_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblBool GblVariant_toBool(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT char GblVariant_toChar(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT uint8_t GblVariant_toUint8(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT uint16_t GblVariant_toUint16(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT int16_t GblVariant_toInt16(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT uint32_t GblVariant_toUint32(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT int32_t GblVariant_toInt32(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT uint64_t GblVariant_toUint64(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT int64_t GblVariant_toInt64(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT GblEnum GblVariant_toEnum(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT GblFlags GblVariant_toFlags(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT float GblVariant_toFloat(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT double GblVariant_toDouble(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT void* GblVariant_toPointer(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT const char* GblVariant_toString(GblVariant* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT GblStringView GblVariant_toStringView(GblVariant* pSelf) GBL_NOEXCEPT {
    return GblStringView_fromString(GblVariant_toString(pSelf));
}

GBL_EXPORT GblType GblVariant_toTypeValue(GblVariant* pSelf) GBL_NOEXCEPT {
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

