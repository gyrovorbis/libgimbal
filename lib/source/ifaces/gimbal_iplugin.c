#include <gimbal/ifaces/gimbal_iplugin.h>
#include <gimbal/strings/gimbal_quark.h>

GBL_RESULT GblIPlugin_typeInfo_(const GblIPlugin* pSelf, GblType type, GblTypeInfo* pInfo) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(NULL);;
    GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_TYPE,
                       "[GblPlugin] Querying for unknown type: [%s]",
                       GblType_name(type));
    memset(pInfo, 0, sizeof(GblTypeInfo));
    GBL_API_END();
}

static GBL_RESULT GblIPluginIFace_init_(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_UNUSED(pData);
    GBL_API_BEGIN(pCtx);
    GblIPluginIFace* pSelf = (GblIPluginIFace*)pClass;
    pSelf->pFnTypeInfo = GblIPlugin_typeInfo_;
    GBL_API_END();
}

extern GBL_RESULT GblIPlugin_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_IPLUGIN,
        GBL_INTERFACE_TYPE,
        GblQuark_internStringStatic("IPlugin"),
        &(const GblTypeInfo) {
            .pFnClassInit     = GblIPluginIFace_init_,
            .classSize        = sizeof(GblIPluginIFace)
        },
        GBL_TYPE_FLAGS_NONE);
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblIPlugin_use(GblIPlugin* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_INSTANCE_VCALL_PREFIX(GBL_IPLUGIN, pFnUse, pSelf);
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblIPlugin_unuse(GblIPlugin* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_INSTANCE_VCALL_PREFIX(GBL_IPLUGIN, pFnUnuse, pSelf);
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblIPlugin_typeInfo(const GblIPlugin* pSelf, GblType requestedType, GblTypeInfo* pInfo) {
    GBL_API_BEGIN(NULL);
    GBL_INSTANCE_VCALL_PREFIX(GBL_IPLUGIN, pFnTypeInfo, pSelf, requestedType, pInfo);
    GBL_API_END();
}
