#include <gimbal/meta/gimbal_variant.h>
#include <gimbal/ifaces/gimbal_ivariant.h>
#include <gimbal/types/gimbal_variant.h>
#include <gimbal/meta/gimbal_type.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/objects/gimbal_object.h>

#define GBL_VARIANT_BEGIN_(type, classGetterSuffix)                                         \
    GBL_API_BEGIN(NULL); {                                                                  \
        const GblType type_ = type;                                                         \
        GBL_API_VERIFY_POINTER(pSelf);                                                      \
        GBL_API_VERIFY_TYPE(type_, GBL_IVARIANT_TYPE,                                       \
            "Attempted to use GblVariant with a type that doesn't implement IVariant: %s",  \
                GblType_name(type_));                                                       \
        GblClass* pClass_ = GblClass_##classGetterSuffix(type_);                            \
        GBL_API_VERIFY_POINTER(pClass_, "Failed to retrieve class for type: %s",            \
                               GblType_name(type_));                                        \
        const GblIVariantIFace* pIFace = GBL_IVARIANT_IFACE(pClass_);                       \
        GBL_API_VERIFY_EXPRESSION(pIFace,                                                   \
            "Failed to retrieve GblIVariant Interface for type: %s",                        \
                GblType_name(type_))

#define GBL_VARIANT_END_()                                                                  \
    } GBL_API_END()

GBL_API GblVariant_initDefault_(GblVariant* pSelf, GblType type) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_TYPE(type, GBL_IVARIANT_TYPE, "Invalid Type: %s", GblType_name(type));
    memset(pSelf, 0, sizeof(GblVariant));
    pSelf->type = type;
    GBL_API_END();
}

GBL_EXPORT GblType GblVariant_type(const GblVariant* pSelf) GBL_NOEXCEPT {
    return pSelf->type;
}
#if 0
GBL_INLINE GBL_RESULT GblVariant_typeSet_(GblVariant* pSelf, GblType type) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    // If we're changing from a valid to invalid type, call destructor
    if(pSelf->type != type) {
       if(pSelf->type != GBL_INVALID_TYPE) {
            GBL_API_CALL(GblVariant_destruct(pSelf));
       }
        GBL_API_CALL(GblVariant_initDefault_(pSelf, type));
    }
    GBL_API_END();
}
#endif


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

GBL_EXPORT GblInt GblVariant_compare(const GblVariant* pSelf, const GblVariant* pOther) GBL_NOEXCEPT {
    GblInt result = INT_MAX;
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
        if(pSelf->type == pOther->type) {
            GBL_API_CALL(GblIVariantIFace_compare(pIFace, pSelf, pOther, &result));
        }
    } GBL_API_END_BLOCK();
    return result;
}

GBL_API GblVariant_convert(const GblVariant* pSelf, GblType toType, GblVariant* pToVariant) GBL_NOEXCEPT {
    GBL_VARIANT_BEGIN_(pSelf->type, peek);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_UNUSED(toType);
    GBL_API_VERIFY_POINTER(pToVariant);
    GBL_API_ERROR("[GblVariant_convert]: UNIMPLEMENTED!");
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

