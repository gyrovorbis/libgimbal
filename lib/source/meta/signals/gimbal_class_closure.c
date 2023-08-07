#include <gimbal/meta/signals/gimbal_class_closure.h>
#include <gimbal/core/gimbal_ctx.h>

static GBL_RESULT GblClassClosureClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(NULL);
    ((GblClassClosureClass*)pClass)->base.pFnMetaMarshal = GblMarshal_ClassClosureMeta;
    GBL_CTX_END();
}

GBL_EXPORT GblClassClosure* GblClassClosure_create(GblType      classType,
                                                   size_t       offset,
                                                   GblInstance* pInstance,
                                                   void*        pUserdata)
{
    GblClassClosure* pClosure = NULL;
    GBL_CTX_BEGIN(NULL);
    if(pInstance) GBL_CTX_VERIFY_TYPE(GBL_TYPEOF(pInstance), classType);
    pClosure = GBL_CLASS_CLOSURE(GblClosure_create(GBL_CLASS_CLOSURE_TYPE,
                                                  sizeof(GblClassClosure),
                                                  pUserdata,
                                                  NULL));
    GblClassClosure_setMethod(pClosure, classType, offset);
    GblClassClosure_setInstance(pClosure, pInstance);
    GBL_CTX_END_BLOCK();
    return pClosure;
}

GBL_EXPORT void GblClassClosure_setMethod(GblClassClosure* pSelf, GblType classType, size_t  offset) {
    GBL_PRIV_REF(pSelf).classType = classType;
    GBL_PRIV_REF(pSelf).offset = offset;
}

GBL_EXPORT void GblClassClosure_setInstance(GblClassClosure* pSelf, GblInstance* pInstance) {
    GBL_PRIV_REF(pSelf).pInstance = pInstance;
}

GBL_EXPORT GblType GblClassClosure_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize      = sizeof(GblClassClosureClass),
        .pFnClassInit   = GblClassClosureClass_init_,
        .instanceSize   = sizeof(GblClassClosure)
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        type = GblType_register(GblQuark_internStatic("GblClassClosure"),
                                GBL_CLOSURE_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}
