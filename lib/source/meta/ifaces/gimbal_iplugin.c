#include <gimbal/meta/ifaces/gimbal_iplugin.h>
#include <gimbal/strings/gimbal_quark.h>

GBL_RESULT GblIPlugin_typeInfo_(const GblIPlugin* pSelf, GblType type, GblTypeInfo* pInfo) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(NULL);;
    GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_TYPE,
                       "[GblPlugin] Querying for unknown type: [%s]",
                       GblType_name(type));
    memset(pInfo, 0, sizeof(GblTypeInfo));
    GBL_CTX_END();
}

static GBL_RESULT GblIPluginClass_init_(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_UNUSED(pData);
    GBL_CTX_BEGIN(pCtx);
    GblIPluginClass* pSelf = (GblIPluginClass*)pClass;
    pSelf->pFnTypeInfo = GblIPlugin_typeInfo_;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblIPlugin_use(GblIPlugin* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_INSTANCE_VCALL(GblIPlugin, pFnUse, pSelf);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblIPlugin_unuse(GblIPlugin* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_INSTANCE_VCALL(GblIPlugin, pFnUnuse, pSelf);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblIPlugin_typeInfo(const GblIPlugin* pSelf, GblType requestedType, GblTypeInfo* pInfo) {
    GBL_CTX_BEGIN(NULL);
    GBL_INSTANCE_VCALL(GblIPlugin, pFnTypeInfo, pSelf, requestedType, pInfo);
    GBL_CTX_END();
}

GBL_EXPORT GblType GblIPlugin_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("GblIPlugin"),
                                      GBL_INTERFACE_TYPE,
                                      &(const GblTypeInfo) {
                                          .pFnClassInit     = GblIPluginClass_init_,
                                          .classSize        = sizeof(GblIPluginClass)
                                      },
                                      GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}
