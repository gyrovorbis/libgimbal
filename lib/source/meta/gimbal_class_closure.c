#include <gimbal/meta/gimbal_class_closure.h>
#include <gimbal/core/gimbal_api_frame.h>

static GBL_RESULT GblClassClosureClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);
    ((GblClassClosureClass*)pClass)->base.pFnMetaMarshal = GblMarshal_ClassClosureMeta;
    GBL_API_END();
}

GBL_EXPORT GblType GblClassClosure_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize      = sizeof(GblClassClosureClass),
        .pFnClassInit   = GblClassClosureClass_init_,
        .instanceSize   = sizeof(GblClassClosure)
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);

        type = GblType_registerStatic(GblQuark_internStringStatic("ClassClosure"),
                                      GBL_CLOSURE_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);

        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}
