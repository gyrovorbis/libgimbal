#include <gimbal/meta/gimbal_variant.h>
#include <gimbal/meta/gimbal_ivariant.h>
#include <gimbal/meta/gimbal_type.h>
#include <gimbal/core/gimbal_api_frame.h>

#define GBL_VARIANT_BEGIN_(type, classGetterSuffix)                     \
    GBL_API_BEGIN(NULL); {                                              \
        const GblType type_ = type;                                     \
        GBL_API_VERIFY_POINTER(pSelf);                                  \
        GBL_API_VERIFY_TYPE(type_, GBL_TYPE_IVARIANT);                  \
        GblClass* pClass_ = gblTypeClass##classGetterSuffix(type_);     \
        GBL_API_VERIFY_EXPRESSION(pClass_);                             \
        const GblIVariantIFace* pIFace = GBL_IVARIANT_IFACE(pClass_);   \
        GBL_API_VERIFY_EXPRESSION(pIFace,                               \
            "Failed to retrieve GblIVariant Interface for type: %s",    \
                gblTypeName(type_))

#define GBL_VARIANT_END_()                                              \
    } GBL_API_END()

GBL_EXPORT GblType GblVariant_type(const GblVariant* pSelf) GBL_NOEXCEPT {
    return pSelf->type;
}

GBL_API GblVariant_initDefault_(GblVariant* pSelf, GblType type) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_TYPE(type, GBL_TYPE_IVARIANT, "Invalid Type: %s", gblTypeName(type));
    memset(pSelf, 0, sizeof(GblVariant));
    pSelf->type = type;
    GBL_API_END();
}

GBL_API GblVariant_constructDefault(GblVariant* pSelf,  GblType type) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, Ref);
    GBL_API_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_API_CALL(GblIVariantIFace_constructDefault(pIFace, pSelf));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_constructCopy(GblVariant* pSelf,  const GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pOther->type, Ref);
    GBL_API_CALL(GblVariant_initDefault_(pSelf, pOther->type));
    GBL_API_CALL(GblIVariantIFace_constructCopy(pIFace, pSelf, pOther));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_constructMove(GblVariant* pSelf, GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pOther->type, Ref); //still has to copy class reference
    GBL_API_CALL(GblVariant_initDefault_(pSelf, pOther->type));
    GBL_API_CALL(GblIVariantIFace_constructMove(pIFace, pSelf, pOther));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_setCopy(GblVariant* pSelf, const GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, Peek);
    GBL_API_CALL(GblIVariantIFace_setCopy(pIFace, pSelf, pOther));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_setMove(GblVariant* pSelf, GblVariant* pOther) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, Peek);
    GBL_API_CALL(GblIVariantIFace_setMove(pIFace, pSelf, pOther));
    GBL_VARIANT_END_();
}

GBL_API GblVariant_destruct(GblVariant* pSelf) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, Peek);
    gblTypeClassUnref(pClass_);
    GBL_VARIANT_END_();
}

GBL_API GblVariant_save(const GblVariant* pSelf, GblString* pString) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, Peek);
    GBL_API_CALL(GblIVariantIFace_save(pIFace, pSelf, pString));
    GBL_VARIANT_END_();
}


GBL_API GblVariant_load(GblVariant* pSelf, const GblString* pString) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, Peek);
    GBL_API_CALL(GblIVariantIFace_load(pIFace, pSelf, pString));
    GBL_VARIANT_END_();
}


GBL_EXPORT GblInt GblVariant_compare(const GblVariant* pSelf, const GblVariant* pOther) GBL_NOEXCEPT {
    GblInt result = INT_MAX;
    GBL_VARIANT_BEGIN_(pSelf->type, Peek);
        if(pSelf->type == pOther->type) {
            GBL_API_CALL(GblIVariantIFace_compare(pIFace, pSelf, pOther, &result));
        }
    } GBL_API_END_BLOCK();
    return result;
}

GBL_API GblVariant_convert(const GblVariant* pSelf, GblType toType, GblVariant* pToVariant) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, Peek);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_UNUSED(toType);
    GBL_API_VERIFY_POINTER(pToVariant);
    GBL_API_ERROR("[GblVariant_convert]: UNIMPLEMENTED!");
    GBL_VARIANT_END_();
}


GBL_API GblVariant_constructValueCopy(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, Ref);
    va_list varArgs;
    va_start(varArgs, type);
    GBL_API_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_API_CALL(GblIVariantIFace_constructValueCopy(pIFace, pSelf, varArgs));
    va_end(varArgs);
    GBL_VARIANT_END_();
}
GBL_API GblVariant_constructValueMove(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, Ref);
    va_list varArgs;
    va_start(varArgs, type);
    GBL_API_CALL(GblVariant_initDefault_(pSelf, type));
    GBL_API_CALL(GblIVariantIFace_constructValueMove(pIFace, pSelf, varArgs));
    va_end(varArgs);
    GBL_VARIANT_END_();
}

GBL_API GblVariant_setValueCopy(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, Ref);
    if(type != pSelf->type) {
        GBL_API_CALL(GblVariant_destruct(pSelf));
    }

    va_list varArgs;
    va_start(varArgs, type);
    GBL_API_CALL(GblIVariantIFace_setValueCopy(pIFace, pSelf, varArgs));
    va_end(varArgs);

    GBL_VARIANT_END_();
}

GBL_API GblVariant_setValueMove(GblVariant* pSelf, GblType type, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(type, Ref);
    if(type != pSelf->type) {
        GBL_API_CALL(GblVariant_destruct(pSelf));
    }

    va_list varArgs;
    va_start(varArgs, type);
    GBL_API_CALL(GblIVariantIFace_setValueMove(pIFace, pSelf, varArgs));
    va_end(varArgs);

    GBL_VARIANT_END_();
}

GBL_API GblVariant_getValueCopy(const GblVariant* pSelf, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, Peek);
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_CALL(GblIVariantIFace_getValueCopy(pIFace, pSelf, varArgs));
    va_end(varArgs);
    GBL_VARIANT_END_();
}

GBL_API GblVariant_getValuePeek(const GblVariant* pSelf, ...) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, Peek);
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_CALL(GblIVariantIFace_getValuePeek(pIFace, pSelf, varArgs));
    va_end(varArgs);
    GBL_VARIANT_END_();
}

