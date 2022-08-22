/*! \file
 *  \brief GblModule loadable plugin instance and management API
 *  \ingroup metaBuiltinTypes
 */
#ifndef GIMBAL_MODULE_H
#define GIMBAL_MODULE_H

#include "gimbal_context.h"
#include "../ifaces/gimbal_iplugin.h"
#include "../../strings/gimbal_string_buffer.h"

/// \ingroup metaBuiltinTypes
#define GBL_MODULE_TYPE                 (GblModule_type())
#define GBL_MODULE_PARENT_PREFIX        GBL_CONTEXT
#define GBL_MODULE_STRUCT               GblModule
#define GBL_MODULE_CLASS_STRUCT         GblModuleClass

#define GBL_MODULE(instance)            GBL_INSTANCE_CAST_PREFIX(instance,  GBL_MODULE)
#define GBL_MODULE_CHECK(instance)      GBL_INSTANCE_CHECK_PREFIX(instance, GBL_MODULE)
#define GBL_MODULE_TRY(instance)        GBL_INSTANCE_TRY_PREFIX(instance, GBL_MODULE)

#define GBL_MODULE_CLASS(klass)         GBL_CLASS_CAST_PREFIX(klass, GBL_MODULE)
#define GBL_MODULE_CLASS_CHECK(klass)   GBL_CLASS_CHECK_PREFIX(klass, GBL_MODULE)
#define GBL_MODULE_CLASS_TRY(klass)     GBL_CLASS_TRY_PREFIX(klass, GBL_MODULE)

#define GBL_MODULE_GET_CLASS(instance)  GBL_INSTANCE_GET_CLASS_PREFIX(instance, GBL_MODULE)
#define GBL_MODULE_TRY_CLASS(instance)  GBL_INSTANCE_TRY_CLASS_PREFIX(instance, GBL_MODULE)

#define GBL_SELF    GblModule* pSelf
#define GBL_CSELF   const GBL_SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblModule);

GBL_CLASS_DERIVE(GblModuleClass, GblContextClass,
                 GblIPluginIFace)
    GBL_RESULT  (*pFnLoad)  (GBL_SELF);
    GBL_RESULT  (*pFnUnload)(GBL_SELF);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblModule, GblContext, GblModuleClass)
    GblVersion      version;
    GblStringRef    prefix;
    GblStringRef    author;
    GblStringRef    description;
GBL_INSTANCE_END

GBL_PROPERTIES(GblModule,
    (prefix,      GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_STRING_TYPE),
    (version,     GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_UINT32_TYPE),
    (author,      GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_STRING_TYPE),
    (description, GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_STRING_TYPE),
    (typeCount,   GBL_GENERIC, (READ),                    GBL_UINT32_TYPE)
)

GblType     GblModule_type          (void)                          GBL_NOEXCEPT;

GblModule*  GblModule_create(const char* pName,
                             GblVersion version,
                             const char* pDescription,
                             const char* pPrefix)                   GBL_NOEXCEPT;


GblType     GblModule_registerType(GBL_SELF,
                                   GblType              parent,
                                   const char*          pName,
                                   const GblTypeInfo*   pInfo,
                                   GblFlags             flags)      GBL_NOEXCEPT;

GblType     GblModule_typeFromName  (GBL_CSELF, const char* pName)  GBL_NOEXCEPT;
GblType     GblModule_typeFromIndex (GBL_CSELF, GblSize     index)  GBL_NOEXCEPT;
GblSize     GblModule_typeCount     (GBL_CSELF)                     GBL_NOEXCEPT;

GblBool     GblModule_isLoaded      (GBL_CSELF)                     GBL_NOEXCEPT;

GBL_RESULT  GblModule_use           (GBL_SELF)                      GBL_NOEXCEPT;
GBL_RESULT  GblModule_unuse         (GBL_SELF)                      GBL_NOEXCEPT;
GblSize     GblModule_useCount      (GBL_CSELF)                     GBL_NOEXCEPT;


GBL_DECLS_END

#undef GBL_CSELF
#undef GBL_SELF



#endif // GIMBAL_MODULE_H
