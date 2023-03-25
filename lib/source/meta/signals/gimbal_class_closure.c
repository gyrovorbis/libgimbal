#include <gimbal/meta/signals/gimbal_class_closure.h>
#include <gimbal/core/gimbal_ctx.h>

static GBL_RESULT GblClassClosureClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(pCtx);
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
    if(pInstance) GBL_CTX_VERIFY_TYPE(GBL_INSTANCE_TYPEOF(pInstance), classType);
    pClosure = GBL_CLASS_CLOSURE(GblClosure_create(GBL_CLASS_CLOSURE_TYPE,
                                                  sizeof(GblClassClosure),
                                                  pUserdata,
                                                  NULL));
    GblClassClosure_setMethod(pClosure, classType, offset);
    GblClassClosure_setInstance(pClosure, pInstance);
    GBL_CTX_END_BLOCK();
    return pClosure;
}

GBL_EXPORT GblType GblClassClosure_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize      = sizeof(GblClassClosureClass),
        .pFnClassInit   = GblClassClosureClass_init_,
        .instanceSize   = sizeof(GblClassClosure)
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);

        type = GblType_registerStatic(GblQuark_internStringStatic("GblClassClosure"),
                                      GBL_CLOSURE_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);

        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}
