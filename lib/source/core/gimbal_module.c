#include <gimbal/core/gimbal_module.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/containers/gimbal_array_map.h>
#include <gimbal/meta/classes/gimbal_primitives.h>
#include <gimbal/core/gimbal_logger.h>
#include <gimbal/utils/gimbal_version.h>
#include <gimbal/utils/gimbal_option_group.h>
#include <gimbal/core/gimbal_atomics.h>
#include <tinycthread.h>

#define GBL_MODULE_(self)   ((GblModule_*)GBL_INSTANCE_PRIVATE(self, GBL_MODULE_TYPE))

#define GBL_MODULE_ENSURE_INITIALIZED_()                \
    GBL_STMT_START {                                    \
        if(!initializing_) GBL_LIKELY {                 \
            if(!inittedOnce_) GBL_UNLIKELY              \
                call_once(&initOnce_, GblModule_init_); \
            else if(!initialized_) GBL_UNLIKELY         \
                GblModule_init_();                      \
        }                                               \
    } GBL_STMT_END


typedef struct GblModule_ {
    GblHashSet       typeRegistry;
    GBL_ATOMIC_INT16 useCount;
} GblModule_;

static mtx_t        moduleMtx_;
static GblBool      inittedOnce_  = GBL_FALSE;
static once_flag    initOnce_     = ONCE_FLAG_INIT;
static GblBool      initializing_ = GBL_FALSE;
static GblBool      initialized_  = GBL_FALSE;
static GblArrayMap* pModules_     = NULL;

static void GblModule_init_(void) {
    GBL_CTX_BEGIN(NULL);
    if(initialized_) GBL_CTX_DONE();

    mtx_init(&moduleMtx_, mtx_plain);

    pModules_ = GblArrayMap_create(NULL, GBL_TRUE, NULL);

    //atexit(GblModule_final_);
    initialized_ = GBL_TRUE;
    GBL_CTX_END_BLOCK();
}


GBL_RESULT GblModule_final_(void) {
    GBL_MODULE_ENSURE_INITIALIZED_();
    GBL_CTX_BEGIN(NULL);

    //uninitialized all of them
    mtx_lock(&moduleMtx_);
    GblArrayMap_destroy(&pModules_);
    mtx_unlock(&moduleMtx_);

    mtx_destroy(&moduleMtx_);
    initialized_ = GBL_FALSE;

    GBL_CTX_END();
}

static GBL_RESULT GblModule_arrayMapDtor_(const GblArrayMap* pMap, uintptr_t key, void* pEntry) {
    GBL_UNUSED(pMap, key);
    GblModule* pModule = GBL_MODULE(pEntry);
    const size_t  uses = GblModule_useCount(pModule);

    if(uses) GBL_UNLIKELY {
        GBL_LOG_WARN("gimbal",
                        "[Module] Unregistering %s which still has %u uses!",
                        GblQuark_toString(key),
                        uses);
    }

    GBL_UNREF(pModule);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblModule_register(GblModule* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_MODULE_ENSURE_INITIALIZED_();

    const char* pName = GblObject_name(GBL_OBJECT(pSelf));
    const GblQuark quark = GblQuark_fromString(pName);

    GBL_LOG_INFO("gimbal", "Registering Module: [%s]", pName);
    GBL_LOG_PUSH();

    mtx_lock(&moduleMtx_);

    const size_t prevPos = GblArrayMap_find(&pModules_, quark);

    if(prevPos != GBL_ARRAY_MAP_NPOS) GBL_UNLIKELY {
        GblModule* pOld = GBL_MODULE(GblArrayMap_probeValue(&pModules_, prevPos));
        GBL_LOG_WARN("gimbal",
                     "Orphaning existing module [refCount: %u, useCount: %u].",
                     GblBox_refCount(GBL_BOX(pOld)),
                     GblModule_useCount(pOld));
    }

    GblArrayMap_setUserdata(&pModules_,
                            quark,
                            (uintptr_t)GBL_REF(pSelf),
                            GblModule_arrayMapDtor_);

    mtx_unlock(&moduleMtx_);

    GBL_LOG_POP(1);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblModule_unregister(GblModule* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);

    const char* pName = GblObject_name(GBL_OBJECT(pSelf));

    GBL_LOG_INFO("gimbal", "Unregistering Module: %s", pName);
    GBL_LOG_PUSH();

    const GblQuark quark = GblQuark_tryString(pName);
    GBL_CTX_VERIFY(quark != GBL_QUARK_INVALID,
                   GBL_RESULT_ERROR_INVALID_HANDLE,
                   "Quark wasn't even found, can't be registered!");

    mtx_lock(&moduleMtx_);
    const GblBool erased = GblArrayMap_erase(&pModules_, quark);
    mtx_unlock(&moduleMtx_);

    if(!erased) {
        GBL_LOG_WARN("gimbal", "No registry entry found!");
    }

    GBL_LOG_POP(1);
    GBL_CTX_END();
}

GBL_EXPORT GblModule* GblModule_find(const char* pName) {
    return GblModule_findQuark(GblQuark_tryString(pName));
}

GBL_EXPORT GblModule* GblModule_findQuark(GblQuark quark) {
    GblModule* pModule = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_MODULE_ENSURE_INITIALIZED_();

    if(quark != GBL_QUARK_INVALID) GBL_LIKELY {
        mtx_lock(&moduleMtx_);

        uintptr_t value = 0;
        const size_t  index = GblArrayMap_find(&pModules_, quark);

        if(index != GBL_ARRAY_MAP_NPOS) GBL_LIKELY {
            value = GblArrayMap_probeValue(&pModules_, index);
        }

        mtx_unlock(&moduleMtx_);

        if(value) {
            pModule = GBL_MODULE(value);
        }
    }

    GBL_CTX_END_BLOCK();
    return pModule;
}

GBL_EXPORT GblModule* GblModule_at(size_t  index) {
    GblModule* pModule = NULL;
    GBL_MODULE_ENSURE_INITIALIZED_();

    mtx_lock(&moduleMtx_);
    const uintptr_t value = GblArrayMap_probeValue(&pModules_, index);
    mtx_unlock(&moduleMtx_);

    if(value) pModule = GBL_MODULE(value);

    return pModule;
}

GBL_EXPORT size_t  GblModule_count(void) {
    GBL_MODULE_ENSURE_INITIALIZED_();
    mtx_lock(&moduleMtx_);
    size_t  count = GblArrayMap_size(&pModules_);
    mtx_unlock(&moduleMtx_);
    return count;
}

GBL_EXPORT GblBool GblModule_foreach(GblModuleIterFn pFnIter,
                                     void*           pUserdata) {
    GblBool retVal = GBL_FALSE;

    GBL_MODULE_ENSURE_INITIALIZED_();
    mtx_lock(&moduleMtx_);
    size_t  count = GblArrayMap_size(&pModules_);
    for(size_t  m = 0; m < count; ++m) GBL_LIKELY {
        GblModule* pModule = GBL_MODULE(GblArrayMap_probeValue(&pModules_, m));
        if((retVal = pFnIter(pModule, pUserdata))) GBL_UNLIKELY {
            break;
        }
    }
    mtx_unlock(&moduleMtx_);

    return retVal;
}


GBL_EXPORT GblModule* GblModule_require(const char* pName,
                                        const char* pVersion,
                                        const char* pFile,
                                        const char* pFunc,
                                        size_t      line)
{
    return GblModule_requireQuark(GblQuark_tryString(pName),
                                  pVersion,
                                  pFile,
                                  pFunc,
                                  line);
}

GBL_EXPORT GblModule* GblModule_requireQuark(GblQuark    quark,
                                             const char* pVersion,
                                             const char* pFile,
                                             const char* pFunc,
                                             size_t      line)
{
    GblModule* pModule = GblModule_findQuark(quark);

    if(!pModule) GBL_UNLIKELY {
        GblLogger_write(pFile, pFunc, line, "gimbal", GBL_LOG_ERROR,
                        "Module [%s] could not be imported: Not Found",
                        GblQuark_toString(quark));

    } else if(pVersion && GblVersion_parse(pVersion) != pModule->version) GBL_UNLIKELY {
        char versionBuffer[GBL_VERSION_STRING_SIZE];

        GblLogger_write(pFile, pFunc, line, "gimbal", GBL_LOG_ERROR,
                        "Module [%s] could not be imported: "
                        "Version Mismatch [Actual: %s, Expected %s]",
                        GblQuark_toString(quark),
                        GblVersion_string(pModule->version, versionBuffer),
                        pVersion);

        pModule = NULL;
    } else GBL_LIKELY {
        GblModule_use(pModule);
    }

    return pModule;
}

GBL_EXPORT GblRefCount GblModule_useCount(const GblModule* pSelf) {
    return GBL_ATOMIC_INT16_LOAD(GBL_MODULE_(pSelf)->useCount);
}

GBL_EXPORT GblBool GblModule_isLoaded(const GblModule* pSelf) {
    return GblModule_useCount(pSelf);
}

GBL_EXPORT GBL_RESULT GblModule_use(GblModule* pSelf) {
    return GblIPlugin_use(GBL_IPLUGIN(pSelf));
}

GBL_EXPORT GBL_RESULT GblModule_unuse(GblModule* pSelf) {
    return GblIPlugin_unuse(GBL_IPLUGIN(pSelf));
}

GBL_EXPORT GblModule* GblModule_create(GblType     derivedType,
                                       const char* pName,
                                       GblVersion  version,
                                       const char* pAuthor,
                                       const char* pDescription,
                                       const char* pPrefix)
{
    GblModule* pModule = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(derivedType, GBL_MODULE_TYPE);

    pModule = GBL_MODULE(GblObject_create(GBL_MODULE_TYPE,
                                          "name",        pName,
                                          "version",     version,
                                          "description", pDescription,
                                          "prefix",      pPrefix,
                                          "author",      pAuthor,
                                          NULL));

    GBL_CTX_END_BLOCK();
    return pModule;
}

GBL_EXPORT GblRefCount GblModule_unref(GblModule* pSelf) {
    return GBL_UNREF(pSelf);
}

GBL_EXPORT size_t GblModule_typeCount(const GblModule* pSelf) {
    GBL_UNUSED(pSelf);
    return 0;
}

static GBL_RESULT GblModule_load_(GblModule* pModule) {
    GBL_UNUSED(pModule);
    GBL_CTX_BEGIN(NULL);
    GBL_LOG_WARN("gimbal", "[GblModule: %s] Load: UNIMPLEMENTED!");
    GBL_CTX_RESULT() = GBL_RESULT_UNIMPLEMENTED;
    GBL_CTX_END();
}

static GBL_RESULT GblModule_unload_(GblModule* pModule) {
    GBL_UNUSED(pModule);
    GBL_CTX_BEGIN(NULL);
    GBL_LOG_WARN("gimbal", "[GblModule: %s] Unload: UNIMPLEMENTED!");
    GBL_CTX_RESULT() = GBL_RESULT_UNIMPLEMENTED;
    GBL_CTX_END();
}

static GBL_RESULT GblModule_IPlugin_use_(GblIPlugin* pPlugin) {
    GBL_CTX_BEGIN(NULL);

    GblModule* pSelf = GBL_MODULE(pPlugin);
    GblModule_* pSelf_ = GBL_MODULE_(pSelf);

    if(!GBL_ATOMIC_INT16_INC(pSelf_->useCount)) GBL_UNLIKELY {
        GBL_INSTANCE_VCALL(GblModule, pFnLoad, pSelf);
    }

    GBL_CTX_END();
}

static GBL_RESULT GblModule_IPlugin_unuse_(GblIPlugin* pPlugin) {
    GBL_CTX_BEGIN(NULL);

    GblModule* pSelf = GBL_MODULE(pPlugin);
    GblModule_* pSelf_ = GBL_MODULE_(pSelf);

    if(GBL_ATOMIC_INT16_DEC(pSelf_->useCount) == 1) GBL_UNLIKELY {
        GBL_INSTANCE_VCALL(GblModule, pFnUnload, pSelf);
    }

    GBL_CTX_END();
}

static GBL_RESULT GblModule_IPlugin_typeInfo_(const GblIPlugin* pPlugin, GblType type, GblTypeInfo* pInfo) {
    GBL_UNUSED(pPlugin, type, pInfo);
    return GBL_RESULT_UNIMPLEMENTED;
}

static GBL_RESULT GblModule_GblObject_property_(const GblObject* pObject, const GblProperty* pProperty, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblModule*  pSelf  = GBL_MODULE(pObject);

    switch(pProperty->id) {
    case GblModule_Property_Id_prefix:
        GblVariant_setStringRef(pValue, pSelf->pPrefix); break;
    case GblModule_Property_Id_version:
        GblVariant_setUint32(pValue, pSelf->version); break;
    case GblModule_Property_Id_author:
        GblVariant_setStringRef(pValue, pSelf->pAuthor); break;
    case GblModule_Property_Id_description:
        GblVariant_setStringRef(pValue, pSelf->pDescription); break;
    case GblModule_Property_Id_useCount:
        GblVariant_setUint16(pValue, GblModule_useCount(pSelf)); break;
    case GblModule_Property_Id_typeCount:
        GblVariant_setUint32(pValue, GblModule_typeCount(pSelf)); break;
    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Attemping to read invalid property [%s] on GblModule",
                           GblProperty_nameString(pProperty));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblModule_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProperty, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblModule*  pSelf  = GBL_MODULE(pObject);

    switch(pProperty->id) {
    case GblModule_Property_Id_prefix:
        GblVariant_moveValue(pValue, &pSelf->pPrefix);      break;
    case GblModule_Property_Id_version:
        pSelf->version = GblVariant_getUint32(pValue);         break;
    case GblModule_Property_Id_author:
        GblVariant_moveValue(pValue, &pSelf->pAuthor);      break;
    case GblModule_Property_Id_description:
        GblVariant_moveValue(pValue, &pSelf->pDescription); break;
    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Attemping to write invalid property [%s] on GblModule",
                           GblProperty_nameString(pProperty));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblModule_GblBox_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(pBox);
    GblModule* pSelf = GBL_MODULE(pBox);

    // Unload first!?

    GblStringRef_release(pSelf->pAuthor);
    GblStringRef_release(pSelf->pPrefix);
    GblStringRef_release(pSelf->pDescription);
    GblOptionGroup_unref(pSelf->pOptionGroup);

    GBL_INSTANCE_VCALL_DEFAULT(GblContext, base.base.pFnDestructor, pBox);

    GBL_CTX_END();
}

static GBL_RESULT GblModule_GblInstance_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_UNUSED(pCtx);
    GBL_CTX_BEGIN(NULL);

    GblObject_setName(GBL_OBJECT(pInstance),
                      GblType_name(GBL_INSTANCE_TYPEOF(pInstance)));

    GBL_CTX_END();
}

static GBL_RESULT GblModuleClass_init_(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_UNUSED(pData, pCtx);

    if(!GblType_classRefCount(GBL_MODULE_TYPE)) {
        GBL_PROPERTIES_REGISTER(GblModule);
    }

    GBL_BOX_CLASS(pClass)    ->pFnDestructor  = GblModule_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass) ->pFnProperty    = GblModule_GblObject_property_;
    GBL_OBJECT_CLASS(pClass) ->pFnSetProperty = GblModule_GblObject_setProperty_;
    GBL_IPLUGIN_CLASS(pClass)->pFnUse         = GblModule_IPlugin_use_;
    GBL_IPLUGIN_CLASS(pClass)->pFnUnuse       = GblModule_IPlugin_unuse_;
    GBL_IPLUGIN_CLASS(pClass)->pFnTypeInfo    = GblModule_IPlugin_typeInfo_;
    GBL_MODULE_CLASS(pClass) ->pFnLoad        = GblModule_load_;
    GBL_MODULE_CLASS(pClass) ->pFnUnload      = GblModule_unload_;

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblType GblModule_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInterfaceMapEntry iface = {
        .classOffset = offsetof(GblModuleClass, GblIPluginImpl)
    };

    static GblTypeInfo info = {
        .pFnClassInit         = GblModuleClass_init_,
        .classSize            = sizeof(GblModuleClass),
        .pFnInstanceInit      = GblModule_GblInstance_init_,
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
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}


