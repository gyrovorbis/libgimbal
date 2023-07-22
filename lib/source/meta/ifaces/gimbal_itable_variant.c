#include <gimbal/meta/ifaces/gimbal_itable_variant.h>
#include <gimbal/meta/types/gimbal_variant.h>

static GBL_RESULT GblITableVariant_index_(const GblVariant* pSelf,
                                          const GblVariant* pKey,
                                          GblVariant*       pValue)
{
    GBL_UNUSED(pSelf, pKey);
    return GblVariant_setNil(pValue);
}

static GBL_RESULT GblITableVariant_setIndex_(const GblVariant* pSelf,
                                             const GblVariant* pKey,
                                             GblVariant*       pValue)
{
    GBL_UNUSED(pSelf, pKey, pValue);
    return GBL_RESULT_ERROR_INVALID_PROPERTY;
}

static GBL_RESULT GblITableVariant_next_(const GblVariant* pSelf,
                                         GblVariant*       pKey,
                                         GblVariant*       pValue)
{
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(pSelf);
    GBL_CTX_CALL(GblVariant_setNil(pKey));
    GBL_CTX_CALL(GblVariant_setNil(pValue));
    GBL_CTX_END();
}

static GBL_RESULT GblITableVariant_count_(const GblVariant* pSelf,
                                          size_t*           pSize)
{
    GBL_UNUSED(pSelf);
    *pSize = 0;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblITableVariantClass_init_(GblClass*   pClass,
                                              const void* pUd,
                                              GblContext* pCtx)
{
    GBL_UNUSED(pUd, pCtx);
    GBL_CTX_BEGIN(NULL);

    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnIndex    = GblITableVariant_index_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnSetIndex = GblITableVariant_setIndex_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnNext     = GblITableVariant_next_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnCount    = GblITableVariant_count_;

    GBL_CTX_END();
}

GBL_EXPORT GblType GblITableVariant_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblType dependency;

    static GblTypeInfo info = {
        .classSize       = sizeof(GblITableVariantClass),
        .pFnClassInit    = GblITableVariantClass_init_,
        .dependencyCount = 1,
        .pDependencies   = &dependency
    };

    if(type == GBL_INVALID_TYPE) {
        dependency = GBL_IVARIANT_TYPE;

        type = GblType_registerStatic(GblQuark_internStringStatic("GblITableVariant"),
                                      GBL_INTERFACE_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC |
                                      GBL_TYPE_FLAG_ABSTRACT);
    }

    return type;
}
