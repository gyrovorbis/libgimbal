#include <gimbal/meta/instances/gimbal_module.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/meta/classes/gimbal_primitives.h>

#define PROPERTY_IDX_(id)   (id - GBL_CONTEXT_PROPERTY_ID_COUNT)

typedef struct GblModule_ {
    GblHashSet  typeRegistry;
} GblModule_;

static GblQuark versionQuark_    = GBL_QUARK_INVALID;
static GblQuark authorQuark_     = GBL_QUARK_INVALID;
static GblQuark descQuark_       = GBL_QUARK_INVALID;
static GblQuark prefixQuark_     = GBL_QUARK_INVALID;
static GblQuark typeCountQuark_  = GBL_QUARK_INVALID;

GBL_RESULT GblModule_IPlugin_use_(GblIPlugin* pPlugin) {
    GBL_CTX_BEGIN(pPlugin);
    GblModule* pModule = GBL_MODULE(pPlugin);
    GblObject* pObject = GBL_OBJECT(pModule);
    GblBox_ref(GBL_BOX(pObject));
    const GblRefCount refCount = GblBox_refCount(GBL_BOX(pObject));
    GBL_CTX_DEBUG("[GblModule: %s] Used: %u",
                  GblObject_name(GBL_OBJECT(pObject)),
                  refCount);
    if(refCount == 1) {
        GBL_INSTANCE_VCALL(GblModule, pFnLoad, pModule);
    }
    GBL_CTX_END();
}

GBL_RESULT GblModule_IPlugin_unuse_(GblIPlugin* pPlugin) {
    GBL_CTX_BEGIN(pPlugin);
    GblModule* pModule = GBL_MODULE(pPlugin);
    GblObject* pObject = GBL_OBJECT(pModule);
    GblBox_unref(GBL_BOX(pObject));
    const GblRefCount refCount = GblBox_refCount(GBL_BOX(pObject));
    GBL_CTX_DEBUG("[GblModule: %s] Unused: %u",
                  GblObject_name(GBL_OBJECT(pObject)),
                  refCount);
    if(refCount == 0) {
        GBL_INSTANCE_VCALL(GblModule, pFnUnload, pModule);
    }
    GBL_CTX_END();
}

GBL_RESULT GblModule_IPlugin_typeInfo_(const GblIPlugin* pPlugin, GblType type, GblTypeInfo* pInfo) {
    GBL_CTX_BEGIN(pPlugin);
    GBL_CTX_END();
}

GBL_RESULT GblModule_property_(const GblObject* pObject, const GblProperty* pProperty, GblVariant* pValue) {
    GBL_CTX_BEGIN(pObject);
    GBL_CTX_END();
}

GBL_RESULT GblModule_setProperty_(GblObject* pObject, const GblProperty* pProperty, GblVariant* pValue) {
    GBL_CTX_BEGIN(pObject);
    GBL_CTX_END();
}

GBL_RESULT GblModule_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(pBox);
    GblModule* pSelf = GBL_MODULE(pBox);
    GblStringRef_release(&pSelf->author);
    GblStringRef_release(&pSelf->prefix);
    GblStringRef_release(&pSelf->description);

    GblContextClass* pClass = GBL_CONTEXT_CLASS(GblClass_weakRefDefault(GBL_CONTEXT_TYPE));
    GBL_CTX_VERIFY_CALL(pClass->base.base.pFnDestructor(pBox));
    GBL_CTX_END();
}

GBL_RESULT GblModule_load_(GblModule* pModule) {
    GBL_CTX_BEGIN(pModule);
    GBL_CTX_WARN("[GblModule: %s] Load: UNIMPLEMENTED!");
    GBL_CTX_END();
}

GBL_RESULT GblModule_unload_(GblModule* pModule) {
    GBL_CTX_BEGIN(pModule);
    GBL_CTX_WARN("[GblModule: %s] Unload: UNIMPLEMENTED!");
    GBL_CTX_END();
}

GBL_RESULT GblModule_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblModule* pSelf = GBL_MODULE(pInstance);


    GBL_CTX_END();
}

GBL_RESULT GblModuleClass_init_(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_UNUSED(pData);
    GBL_CTX_BEGIN(pCtx);
    GblModuleClass* pSelf = GBL_MODULE_CLASS(pClass);
    pSelf->pFnLoad                         = GblModule_load_;
    pSelf->pFnUnload                       = GblModule_unload_;
    pSelf->base.base.base.pFnDestructor    = GblModule_destructor_;
    pSelf->base.base.pFnProperty           = GblModule_property_;
    pSelf->base.base.pFnSetProperty        = GblModule_setProperty_;
    pSelf->GblIPluginImpl.pFnUse           = GblModule_IPlugin_use_;
    pSelf->GblIPluginImpl.pFnUnuse         = GblModule_IPlugin_unuse_;
    pSelf->GblIPluginImpl.pFnTypeInfo      = GblModule_IPlugin_typeInfo_;

    if(!GblType_classRefCount(GBL_MODULE_TYPE)) {

        versionQuark_    = GblQuark_fromStringStatic("version");
        authorQuark_     = GblQuark_fromStringStatic("author");
        descQuark_       = GblQuark_fromStringStatic("description");
        prefixQuark_     = GblQuark_fromStringStatic("prefix");
        typeCountQuark_  = GblQuark_fromStringStatic("typeCount");

        GBL_PROPERTIES_REGISTER(GblModule);
    }

    GBL_CTX_END();
}

GBL_RESULT GblModuleClass_final_(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_UNUSED(pData);

    if(!GblType_classRefCount(GBL_MODULE_TYPE)) {
  //      memset(pPropertyList_, 0, sizeof(pPropertyList_));
    }
    return GBL_RESULT_UNIMPLEMENTED;
}

GBL_EXPORT GblType GblModule_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInterfaceMapEntry iface = {
        .classOffset = offsetof(GblModuleClass, GblIPluginImpl)
    };

    static GblTypeInfo info = {
        .pFnClassInit         = GblModuleClass_init_,
        .pFnClassFinal        = GblModuleClass_final_,
        .classSize            = sizeof(GblModuleClass),
        .pFnInstanceInit      = GblModule_init_,
        .instanceSize         = sizeof(GblModule),
        .instancePrivateSize  = sizeof(GblModule_),
        .interfaceCount       = 1,
        .pInterfaceMap        = &iface
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        GBL_CTX_BEGIN(NULL);

        iface.interfaceType = GBL_IPLUGIN_TYPE;

        type = GblType_registerStatic(GblQuark_internStringStatic("GblModule"),
                                      GBL_CONTEXT_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_ABSTRACT |
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}

GblType GblModule_registerType(GblModule*           pSelf,
                               GblType              parent,
                               const char*          pName,
                               const GblTypeInfo*   pInfo,
                               GblFlags             flags) GBL_NOEXCEPT {

    GblType newType = GBL_INVALID_TYPE; //GblType_registerDynamic(pName, parent, GBL_IPLUGIN(pSelf), flags);
    return newType;
}

