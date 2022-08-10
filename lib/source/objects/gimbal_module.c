#include <gimbal/objects/gimbal_module.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/meta/gimbal_primitives.h>

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
    GBL_API_BEGIN(pPlugin);
    GblModule* pModule = GBL_MODULE(pPlugin);
    GblObject* pObject = GBL_OBJECT(pModule);
    GblRecord_ref(GBL_RECORD(pObject));
    const GblRefCount refCount = GblRecord_refCount(GBL_RECORD(pObject));
    GBL_API_DEBUG("[GblModule: %s] Used: %u",
                  GblObject_name(GBL_OBJECT(pObject)),
                  refCount);
    if(refCount == 1) {
        GBL_INSTANCE_VCALL_PREFIX(GBL_MODULE, pFnLoad, pModule);
    }
    GBL_API_END();
}

GBL_RESULT GblModule_IPlugin_unuse_(GblIPlugin* pPlugin) {
    GBL_API_BEGIN(pPlugin);
    GblModule* pModule = GBL_MODULE(pPlugin);
    GblObject* pObject = GBL_OBJECT(pModule);
    GblRecord_unref(GBL_RECORD(pObject));
    const GblRefCount refCount = GblRecord_refCount(GBL_RECORD(pObject));
    GBL_API_DEBUG("[GblModule: %s] Unused: %u",
                  GblObject_name(GBL_OBJECT(pObject)),
                  refCount);
    if(refCount == 0) {
        GBL_INSTANCE_VCALL_PREFIX(GBL_MODULE, pFnUnload, pModule);
    }
    GBL_API_END();
}

GBL_RESULT GblModule_IPlugin_typeInfo_(const GblIPlugin* pPlugin, GblType type, GblTypeInfo* pInfo) {
    GBL_API_BEGIN(pPlugin);
    GBL_API_END();
}

GBL_RESULT GblModule_propertyGet_(const GblObject* pObject, GblSize id, GblVariant* pValue, const GblProperty* pProperty) {
    GBL_API_BEGIN(pObject);
    GBL_API_END();
}

GBL_RESULT GblModule_propertySet_(GblObject* pObject, GblSize id, const GblVariant* pValue, const GblProperty* pProperty) {
    GBL_API_BEGIN(pObject);
    GBL_API_END();
}

GBL_RESULT GblModule_destructor_(GblRecord* pRecord) {
    GBL_API_BEGIN(pRecord);
    GblModule* pSelf = GBL_MODULE(pRecord);
    GblStringRef_release(&pSelf->author);
    GblStringRef_release(&pSelf->prefix);
    GblStringRef_release(&pSelf->description);
    GBL_INSTANCE_VCALL_SUPER_PREFIX(GBL_CONTEXT, base.base.pFnDestructor, pRecord);
    GBL_API_END();
}

GBL_RESULT GblModule_load_(GblModule* pModule) {
    GBL_API_BEGIN(pModule);
    GBL_API_WARN("[GblModule: %s] Load: UNIMPLEMENTED!");
    GBL_API_END();
}

GBL_RESULT GblModule_unload_(GblModule* pModule) {
    GBL_API_BEGIN(pModule);
    GBL_API_WARN("[GblModule: %s] Unload: UNIMPLEMENTED!");
    GBL_API_END();
}

GBL_RESULT GblModule_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblModule* pSelf = GBL_MODULE(pInstance);


    GBL_API_END();
}

GBL_RESULT GblModuleClass_init_(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_UNUSED(pData);
    GBL_API_BEGIN(pCtx);
    GblModuleClass* pSelf = GBL_MODULE_CLASS(pClass);
    pSelf->pFnLoad                       = GblModule_load_;
    pSelf->pFnUnload                     = GblModule_unload_;
    pSelf->base.base.base.pFnDestructor  = GblModule_destructor_;
    pSelf->base.base.pFnPropertyGet      = GblModule_propertyGet_;
    pSelf->base.base.pFnPropertySet      = GblModule_propertySet_;
    pSelf->iPluginIFace.pFnUse           = GblModule_IPlugin_use_;
    pSelf->iPluginIFace.pFnUnuse         = GblModule_IPlugin_unuse_;
    pSelf->iPluginIFace.pFnTypeInfo      = GblModule_IPlugin_typeInfo_;

    if(!GblType_classRefCount(GBL_MODULE_TYPE)) {

        versionQuark_    = GblQuark_fromStringStatic("version");
        authorQuark_     = GblQuark_fromStringStatic("author");
        descQuark_       = GblQuark_fromStringStatic("description");
        prefixQuark_     = GblQuark_fromStringStatic("prefix");
        typeCountQuark_  = GblQuark_fromStringStatic("typeCount");
#if 0
        pPropertyList_[PROPERTY_IDX_(GBL_MODULE_PROPERTY_ID_VERSION)] =
            gblPropertyTableInsert(GBL_CLASS_TYPEOF(pClass),
                                   versionQuark_,
                                   GBL_MODULE_PROPERTY_ID_VERSION,
                                   GBL_UINT32_TYPE,
                                   GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE |
                                   GBL_PROPERTY_FLAG_SAVE | GBL_PROPERTY_FLAG_CONSTRUCT);

        pPropertyList_[PROPERTY_IDX_(GBL_MODULE_PROPERTY_ID_AUTHOR)] =
            gblPropertyTableInsert(GBL_CLASS_TYPEOF(pClass),
                                   authorQuark_,
                                   GBL_MODULE_PROPERTY_ID_AUTHOR,
                                   GBL_STRING_TYPE,
                                   GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE |
                                   GBL_PROPERTY_FLAG_SAVE | GBL_PROPERTY_FLAG_CONSTRUCT);

        pPropertyList_[PROPERTY_IDX_(GBL_MODULE_PROPERTY_ID_DESCRIPTION)] =
            gblPropertyTableInsert(GBL_CLASS_TYPEOF(pClass),
                                   descQuark_,
                                   GBL_MODULE_PROPERTY_ID_DESCRIPTION,
                                   GBL_STRING_TYPE,
                                   GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE |
                                   GBL_PROPERTY_FLAG_SAVE | GBL_PROPERTY_FLAG_CONSTRUCT);

        pPropertyList_[PROPERTY_IDX_(GBL_MODULE_PROPERTY_ID_PREFIX)] =
            gblPropertyTableInsert(GBL_CLASS_TYPEOF(pClass),
                                   prefixQuark_,
                                   GBL_MODULE_PROPERTY_ID_PREFIX,
                                   GBL_STRING_TYPE,
                                   GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE |
                                   GBL_PROPERTY_FLAG_SAVE | GBL_PROPERTY_FLAG_CONSTRUCT);

        pPropertyList_[PROPERTY_IDX_(GBL_MODULE_PROPERTY_ID_TYPE_COUNT)] =
            gblPropertyTableInsert(GBL_CLASS_TYPEOF(pClass),
                                   typeCountQuark_,
                                   GBL_MODULE_PROPERTY_ID_TYPE_COUNT,
                                   GBL_UINT32_TYPE,
                                   GBL_PROPERTY_FLAG_READ);
#endif
    }

    GBL_API_END();
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

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("Module"),
                                      GBL_CONTEXT_TYPE,
                                      &(const GblTypeInfo) {
                                          .pFnClassInit         = GblModuleClass_init_,
                                          .pFnClassFinal        = GblModuleClass_final_,
                                          .classSize            = sizeof(GblModuleClass),
                                          .pFnInstanceInit      = GblModule_init_,
                                          .instanceSize         = sizeof(GblModule),
                                          .instancePrivateSize  = sizeof(GblModule_),
                                          .interfaceCount       = 1,
                                          .pInterfaceMap        = (const GblTypeInterfaceMapEntry[]) {
                                              {
                                                  .interfaceType = GBL_IPLUGIN_TYPE,
                                                  .classOffset   = offsetof(GblModuleClass, iPluginIFace)
                                              },
                                          }
                                      },
                                      GBL_TYPE_FLAG_ABSTRACT);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }

    return type;;
}

GblType GblModule_registerType(GblModule*           pSelf,
                               GblType              parent,
                               const char*          pName,
                               const GblTypeInfo*   pInfo,
                               GblFlags             flags) GBL_NOEXCEPT {

    GblType newType = GBL_INVALID_TYPE; //GblType_registerDynamic(pName, parent, GBL_IPLUGIN(pSelf), flags);
    return newType;
}

