#ifndef GIMBAL_MODULE_H
#define GIMBAL_MODULE_H

#include "gimbal_context.h"
#include "../ifaces/gimbal_iplugin.h"
#include "../types/gimbal_string_buffer.h"

#define GBL_MODULE_TYPE                 (GBL_BUILTIN_TYPE(MODULE))
#define GBL_MODULE_PARENT_PREFIX        GBL_CONTEXT
#define GBL_MODULE_STRUCT               GblModule
#define GBL_MODULE_CLASS_STRUCT         GblModuleClass

#define GBL_MODULE(instance)            GBL_INSTANCE_CAST_PREFIX(instance,  GBL_MODULE)
#define GBL_MODULE_CHECK(instance)      GBL_INSTANCE_CHECK_PREFIX(instance, GBL_MODULE)
#define GBL_MODULE_TRY(instance)        GBL_INSTANCE_TRY_PREFIX(instance, GBL_MODULE)

#define GBL_MODULE_CLASS(klass)         GBL_CLASS_CAST_PREFIX(klass, GBL_MODULE)
#define GBL_MODULE_CLASS_CHECK(klass)   GBL_CLASS_CHECK_PREFIX(klass, GBL_MODULE)
#define GBL_MODULE_CLASS_TRY(klass)     GBL_CLASS_TRY_PREFIX(klass, GBL_MODULE)

#define GBL_MODULE_GET_CLASS(instance)  GBL_INSTANCE_CAST_CLASS_PREFIX(instance, GBL_MODULE)
#define GBL_MODULE_TRY_CLASS(instance)  GBL_INSTANCE_TRY_CLASS_PREFIX(instance, GBL_MODULE)

#define SELF    GblModule* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblModule);
GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblModulePrivate);

typedef struct GblModuleClass {
    GblContextClass      base;
    GblIPluginIFace     iPluginIFace;
    GBL_RESULT          (*pFnLoad)  (SELF);
    GBL_RESULT          (*pFnUnload)(SELF);
} GblModuleClass;

typedef struct GblModule {
    union {
        GblModuleClass*     pClass;
        GblContext          base;
    };
    GblVersion              version;
    GblStringBuffer         author;
    GblStringBuffer         description;
    GblStringBuffer         prefixName;
    GblModulePrivate*       pPrivate_;
} GblModule;

typedef enum GBL_MODULE_PROPERTY_ID {
    GBL_MODULE_PROPERTY_ID_PREFIX = GBL_CONTEXT_PROPERTY_ID_COUNT,
    GBL_MODULE_PROPERTY_ID_VERSION,
    GBL_MODULE_PROPERTY_ID_AUTHOR,
    GBL_MODULE_PROPERTY_ID_DESCRIPTION,
    GBL_MODULE_PROPERTY_ID_TYPE_COUNT
} GBL_MODULE_PROPERTY_ID;

GBL_PROPERTY_TABLE_BEGIN(GBL_MODULE)
    GBL_PROPERTY(GBL_MODULE_PROPERTY_ID_PREFIX,
                 "prefix",
                 GBL_STRING_TYPE,
                 GBL_PROPERTY_FLAGS_MASK(READ, WRITE, LOAD, SAVE, CONSTRUCT))
    GBL_PROPERTY(GBL_MODULE_PROPERTY_ID_VERSION,
                 "version",
                 GBL_UINT32_TYPE,
                 GBL_PROPERTY_FLAGS_MASK(READ, WRITE, LOAD, SAVE, CONSTRUCT))
    GBL_PROPERTY(GBL_MODULE_PROPERTY_ID_AUTHOR,
                 "author",
                 GBL_STRING_TYPE,
                 GBL_PROPERTY_FLAGS_MASK(READ, WRITE, LOAD, SAVE, CONSTRUCT))
    GBL_PROPERTY(GBL_MODULE_PROPERTY_ID_DESCRIPTION,
                 "description",
                 GBL_STRING_TYPE,
                 GBL_PROPERTY_FLAGS_MASK(READ, WRITE, LOAD, SAVE, CONSTRUCT))
    GBL_PROPERTY(GBL_MODULE_PROPERTY_ID_TYPE_COUNT,
                 "typeCount",
                 GBL_UINT32_TYPE,
                 GBL_PROPERTY_FLAGS_MASK(READ))
GBL_PROPERTY_TABLE_END()

GblModule*  GblModule_create(const char* pName,
                             GblVersion version,
                             const char* pDescription,
                             const char* pPrefix)               GBL_NOEXCEPT;


GblType     GblModule_registerType(SELF,
                                   GblType              parent,
                                   const char*          pName,
                                   const GblTypeInfo*   pInfo,
                                   GblFlags             flags)  GBL_NOEXCEPT;

GblType     GblModule_typeFromName  (CSELF, const char* pName)  GBL_NOEXCEPT;
GblType     GblModule_typeFromIndex (CSELF, GblSize     index)  GBL_NOEXCEPT;
GblSize     GblModule_typeCount     (CSELF)                     GBL_NOEXCEPT;

GblBool     GblModule_isLoaded      (CSELF)                     GBL_NOEXCEPT;

GBL_RESULT  GblModule_use           (SELF)                      GBL_NOEXCEPT;
GBL_RESULT  GblModule_unuse         (SELF)                      GBL_NOEXCEPT;
GblSize     GblModule_useCount      (CSELF)                     GBL_NOEXCEPT;


GBL_DECLS_END

#undef CSELF
#undef SELF



#endif // GIMBAL_MODULE_H
