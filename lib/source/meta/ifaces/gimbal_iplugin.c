#include <gimbal/meta/ifaces/gimbal_iplugin.h>
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

GBL_EXPORT GblType GblIPlugin_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("IPlugin"),
                                      GBL_INTERFACE_TYPE,
                                      &(const GblTypeInfo) {
                                          .pFnClassInit     = GblIPluginIFace_init_,
                                          .classSize        = sizeof(GblIPluginIFace)
                                      },
                                      GBL_TYPE_FLAG_ABSTRACT);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }

    return type;
}
