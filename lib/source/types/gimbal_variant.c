#include <gimbal/meta/gimbal_variant.h>
#include <gimbal/ifaces/gimbal_ivariant.h>
#include <gimbal/types/gimbal_variant.h>
#include <gimbal/meta/gimbal_type.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/objects/gimbal_object.h>
#include <gimbal/containers/gimbal_tree_set.h>

#define GBL_VARIANT_BEGIN_(type, classGetterSuffix)                                         \
    GBL_API_BEGIN(NULL); {                                                                  \
        const GblType type_ = type;                                                         \
        GBL_API_VERIFY_POINTER(pSelf);                                                      \
        GBL_API_VERIFY_TYPE(type_, GBL_IVARIANT_TYPE,                                       \
            "[GblVariant] Attempted to use type that doesn't implement IVariant: %s",       \
                GblType_name(type_));                                                       \
        GblClass* pClass_ = GblClass_##classGetterSuffix(type_);                            \
        GBL_API_VERIFY_POINTER(pClass_, "Failed to retrieve class for type: %s",            \
                               GblType_name(type_));                                        \
        const GblIVariantIFace* pIFace = GBL_IVARIANT_IFACE(pClass_);                       \
        GBL_API_VERIFY_EXPRESSION(pIFace,                                                   \
            "Failed to retrieve GblIVariant Interface for type: %s",                        \
                GblType_name(type_))

#define GBL_VARIANT_END_BLOCK_()                                                            \
    } GBL_API_END_BLOCK()

#define GBL_VARIANT_END_()                                                                  \
    } GBL_API_END()

typedef struct ConverterEntry_ {
    GblType                 fromType;
    GblType                 toType;
    GblVariantConverterFn   pFnConverter;
} ConverterEntry_;


static GblTreeSet   converterRegistry_;
static uint64_t     registryHint_       = 0;
static GblBool      registryValid_      = GBL_FALSE;

GblInt GblVariant_converterComparator_(const GblTreeSet* pSet,
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
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_TYPE(type, GBL_IVARIANT_TYPE, "Invalid Type: %s", GblType_name(type));
    memset(pSelf, 0, sizeof(GblVariant));
    pSelf->type = type;
    GBL_API_END();
}

GBL_RESULT GblVariant_init_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERBOSE("[GblVariant] Initializing converter registry");
    GBL_API_VERIFY(!registryValid_,
                   GBL_RESULT_PARTIAL,
                   "[GblVariant] Double initialize called?");
    registryHint_ = 0;
    GBL_API_VERIFY_CALL(GblTreeSet_construct(&converterRegistry_,
                                             sizeof(ConverterEntry_),
                                             GblVariant_converterComparator_,
                                             NULL,
                                             256,
                                             pCtx));
    registryValid_ = GBL_TRUE;
    GBL_API_END();
}

GBL_RESULT GblVariant_final_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERBOSE("[GblVariant] Finalizing converter registry");
    GBL_API_VERIFY(registryValid_,
                   GBL_RESULT_PARTIAL,
                   "[GblVariant] Double finalize called?");
    GBL_API_VERIFY_CALL(GblTreeSet_destruct(&converterRegistry_));
    registryValid_ = GBL_FALSE;
    GBL_API_END();
}

GBL_EXPORT GblSize GblVariant_converterCount(void) {
    GblSize count = 0;
    GBL_API_BEGIN(GblTreeSet_context(&converterRegistry_));
    count = GblTreeSet_size(&converterRegistry_);
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT GBL_RESULT GblVariant_registerConverter(GblType fromType, GblType toType, GblVariantConverterFn pFnConv) {
    GBL_API_BEGIN(GblTreeSet_context(&converterRegistry_)); {
        GBL_API_PUSH_VERBOSE("[GblVariant] Registering Converter: [%s to %s]",
                             GblType_name(fromType), GblType_name(toType));
        const ConverterEntry_ entry = {
            fromType,
            toType,
            pFnConv
        };
        ConverterEntry_* pExisting = GblTreeSet_setHint(&converterRegistry_,
                                                        &entry,
                                                        &registryHint_);
        GBL_API_VERIFY_LAST_RECORD();
        if(pExisting) {
            GBL_API_WARN("[GblVariant] Overwrote exisitng converter for [%s] to [%s].",
                         GblType_name(pExisting->fromType), GblType_name(pExisting->toType));
        }
        GBL_API_POP(1);
    } GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblVariant_unregisterConverter(GblType fromType, GblType toType) {
    GBL_API_BEGIN(GblTreeSet_context(&converterRegistry_)); {
        const ConverterEntry_ entry = {
            fromType,
            toType,
            NULL
        };
        GBL_API_VERIFY(GblTreeSet_erase(&converterRegistry_,
                                        &entry),
                       GBL_RESULT_NOT_FOUND,
                       "[GblVariant] Removing converter from [%s] to [%s]: NOT FOUND",
                       GblType_name(fromType), GblType_name(toType));
    } GBL_API_END();
}

GBL_EXPORT GblBool GblVariant_canConvert(GblType fromType, GblType toType) {
    GblBool         result          = GBL_FALSE;
    GblType         currentFromType = fromType;
    ConverterEntry_ entry           = { .toType = toType };

    GBL_API_BEGIN(GblTreeSet_context(&converterRegistry_)); {
        while(!result && currentFromType != GBL_INVALID_TYPE) {
            entry.fromType = currentFromType;

            result = GblTreeSet_contains(&converterRegistry_,
                                         &entry);

            if(!GblType_check(currentFromType,
                              GblType_parent(currentFromType)))
                break;

            currentFromType = GblType_parent(currentFromType);
        }
    } GBL_API_END_BLOCK();
    return result;
}

GBL_EXPORT GBL_RESULT GblVariant_convert(const GblVariant* pSelf, GblVariant* pOther) {
    GBL_API_BEGIN(GblTreeSet_context(&converterRegistry_)); {
        const ConverterEntry_*  pEntry      = NULL;
        GblType                 fromType    = GblVariant_type(pSelf);
        ConverterEntry_         entry       = { .toType = GblVariant_type(pOther) };

        while(!pEntry && fromType != GBL_INVALID_TYPE) {
            entry.toType = fromType;

            pEntry = GblTreeSet_at(&converterRegistry_,
                                   &entry);

            if(!GblType_check(fromType, GblType_parent(fromType)))
                break;

            fromType = GblType_parent(fromType);
        } while(!pEntry);

        GBL_API_VERIFY(pEntry,
                       GBL_RESULT_ERROR_INVALID_CONVERSION,
                       "[GblVariant] Conversion failed: No converter found! [%s to %s],",
                       GblType_name(GblVariant_type(pSelf)), GblType_name(GblVariant_type(pOther)));
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_VERIFY_CALL(pEntry->pFnConverter(pSelf, pOther));

    } GBL_API_END();
}

GBL_EXPORT GblInt GblVariant_compare(const GblVariant* pSelf, const GblVariant* pOther) {
    GblInt result = INT_MAX;
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    if(GblType_check(GblVariant_type(pSelf), GblVariant_type(pOther))) {
        GBL_API_VERIFY_CALL(GblIVariantIFace_compare(pIFace, pSelf, pOther, &result));
    } else {
        if(GblVariant_canConvert(GblVariant_type(pSelf), GblVariant_type(pOther))) {
            GblVariant              temp        = { .type = GblVariant_type(pOther) };
            const GblClass*         pOtherClass = GblClass_peek(GblVariant_type(pOther));
            const GblIVariantIFace* pOtherIFace = GBL_IVARIANT_IFACE(pOtherClass);
            GBL_API_VERIFY_EXPRESSION(pOtherIFace);
            GBL_API_VERIFY_CALL(GblVariant_convert(pSelf, &temp));
            GBL_API_VERIFY_CALL(GblIVariantIFace_compare(pOtherIFace, &temp, pOther, &result));

        } else if(GblVariant_canConvert(GblVariant_type(pOther), GblVariant_type(pSelf))) {
            GblVariant temp = { .type = GblVariant_type(pSelf) };
            GBL_API_VERIFY_CALL(GblVariant_convert(pSelf, &temp));
            GBL_API_VERIFY_CALL(GblIVariantIFace_compare(pIFace, pSelf, &temp, &result));
        }
    }
    GBL_VARIANT_END_BLOCK_();
    return result;
}

GBL_API GblVariant_constructDefault(GblVariant* pSelf,  GblType type) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, ref);
    GBL_API_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_API_CALL(GblIVariantIFace_constructDefault(pIFace, pSelf));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_constructCopy(GblVariant* pSelf,  const GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pOther->type, ref);
    GBL_API_CALL(GblVariant_initDefault_(pSelf, pOther->type));
    GBL_API_CALL(GblIVariantIFace_constructCopy(pIFace, pSelf, pOther));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_constructMove(GblVariant* pSelf, GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pOther->type, ref); //still has to copy class reference
    GBL_API_CALL(GblVariant_initDefault_(pSelf, pOther->type));
    GBL_API_CALL(GblIVariantIFace_constructMove(pIFace, pSelf, pOther));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_setCopy(GblVariant* pSelf, const GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    if(pSelf->type != pOther->type) {
        GBL_API_CALL(GblVariant_destruct(pSelf));
        GBL_API_CALL(GblVariant_constructCopy(pSelf, pOther));
    } else {
        GBL_API_CALL(GblIVariantIFace_setCopy(pIFace, pSelf, pOther));
    }
    GBL_VARIANT_END_();
}

GBL_API GblVariant_setMove(GblVariant* pSelf, GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    if(pSelf->type != pOther->type) {
        GBL_API_CALL(GblVariant_destruct(pSelf));
        GBL_API_CALL(GblVariant_constructMove(pSelf, pOther));
    } else {
        GBL_API_CALL(GblIVariantIFace_setMove(pIFace, pSelf, pOther));
    }
    GBL_VARIANT_END_();
}

GBL_API GblVariant_destruct(GblVariant* pSelf) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    GBL_API_CALL(GblIVariantIFace_destruct(pIFace, pSelf));
    GblClass_unref(pClass_);
    pSelf->type = GBL_INVALID_TYPE;
    GBL_VARIANT_END_();
}

GBL_API GblVariant_save(const GblVariant* pSelf, GblStringBuffer* pString) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    GBL_API_CALL(GblIVariantIFace_save(pIFace, pSelf, pString));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_load(GblVariant* pSelf, const GblStringBuffer* pString) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    GBL_API_CALL(GblIVariantIFace_load(pIFace, pSelf, pString));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_constructValueCopyVaList(GblVariant* pSelf, GblType type, va_list* pList) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, ref);
    GBL_API_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_API_CALL(GblIVariantIFace_constructValueCopy(pIFace, pSelf, pList));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_constructValueMoveVaList(GblVariant* pSelf, GblType type, va_list* pList) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, ref);
    GBL_API_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_API_CALL(GblIVariantIFace_constructValueMove(pIFace, pSelf, pList));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_constructValueCopy(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    va_list list;
    va_start(list, type);
    GBL_API_CALL(GblVariant_constructValueCopyVaList(pSelf, type, &list));
    GBL_API_END_BLOCK();
    va_end(list);
    return GBL_API_RESULT();
}

GBL_API GblVariant_constructValueMove(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, ref);
    va_list varArgs;
    va_start(varArgs, type);
    GBL_API_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_API_CALL(GblIVariantIFace_constructValueMove(pIFace, pSelf, &varArgs));
    va_end(varArgs);
    GBL_VARIANT_END_();
}

GBL_API GblVariant_setValueCopy(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, ref);
    va_list varArgs;
    va_start(varArgs, type);
    GBL_API_VERIFY_CALL(GblVariant_setValueCopyVaList(pSelf, type, &varArgs));
    va_end(varArgs);
    GBL_VARIANT_END_();
}

GBL_API GblVariant_setValueMove(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, ref);
    va_list varArgs;
    va_start(varArgs, type);
    GBL_API_VERIFY_CALL(GblVariant_setValueMoveVaList(pSelf, type, &varArgs));
    va_end(varArgs);
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_setValueCopyVaList(GblVariant* pSelf,
                                                    GblType  type,
                                                    va_list* pVarArgs) GBL_NOEXCEPT
{
    GBL_VARIANT_BEGIN_(type, ref);
    if(type != pSelf->type) {
        GBL_API_CALL(GblVariant_destruct(pSelf));
        GBL_API_CALL(GblVariant_initDefault_(pSelf, type));
        GBL_API_CALL(GblIVariantIFace_constructValueCopy(pIFace, pSelf, pVarArgs));
    } else {
        GBL_API_CALL(GblIVariantIFace_setValueCopy(pIFace, pSelf, pVarArgs));
    }
    GBL_VARIANT_END_();
}

GBL_EXPORT GBL_RESULT GblVariant_setValueMoveVaList(GblVariant* pSelf,
                                                    GblType  type,
                                                    va_list* pVarArgs) GBL_NOEXCEPT
{
    GBL_VARIANT_BEGIN_(type, ref);
    if(type != pSelf->type) {
        GBL_API_CALL(GblVariant_destruct(pSelf));
        GBL_API_CALL(GblVariant_initDefault_(pSelf, type));
        GBL_API_CALL(GblIVariantIFace_constructValueMove(pIFace, pSelf, pVarArgs));
    } else {
        GBL_API_CALL(GblIVariantIFace_setValueMove(pIFace, pSelf, pVarArgs));
    }
    GBL_VARIANT_END_();
}

GBL_API GblVariant_getValueCopy(const GblVariant* pSelf, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_CALL(GblIVariantIFace_getValueCopy(pIFace, pSelf, &varArgs));
    va_end(varArgs);
    GBL_VARIANT_END_();
}

GBL_API GblVariant_getValuePeek(const GblVariant* pSelf, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_CALL(GblIVariantIFace_getValuePeek(pIFace, pSelf, &varArgs));
    va_end(varArgs);
    GBL_VARIANT_END_();
}

GBL_API GblVariant_getValueMove(GblVariant* pSelf,  ...) GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblVariant_getValueMoveVaList(pSelf, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

GBL_API GblVariant_getValueMoveVaList(GblVariant* pSelf, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    GBL_API_CALL(GblIVariantIFace_getValueMove(pIFace, pSelf, pVarArgs));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_getValuepeekVaList(GblVariant* pSelf, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    GBL_API_CALL(GblIVariantIFace_getValuePeek(pIFace, pSelf, pVarArgs));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_getValueCopyVaList(const GblVariant* pSelf, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    GBL_API_CALL(GblIVariantIFace_getValueCopy(pIFace, pSelf, pVarArgs));
    GBL_VARIANT_END_();
}

GBL_EXPORT GblObject* GblVariant_getObjectCopy(const GblVariant* pSelf) GBL_NOEXCEPT {
    GblObject* pObject = GBL_NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(GblVariant_type(pSelf), GBL_OBJECT_TYPE);
    GBL_API_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &pObject));
    GBL_API_END_BLOCK();
    return pObject;
}
GBL_EXPORT GblObject* GblVariant_getObjectMove(GblVariant* pSelf) GBL_NOEXCEPT {
    GblObject* pObject = GBL_NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(GblVariant_type(pSelf), GBL_OBJECT_TYPE);
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(pSelf, &pObject));
    GBL_API_END_BLOCK();
    return pObject;
}
GBL_EXPORT GblObject* GblVariant_getObjectPeek(const GblVariant* pSelf) GBL_NOEXCEPT {
    GblObject* pObject = GBL_NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(GblVariant_type(pSelf), GBL_OBJECT_TYPE);
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(pSelf, &pObject));
    GBL_API_END_BLOCK();
    return pObject;
}

