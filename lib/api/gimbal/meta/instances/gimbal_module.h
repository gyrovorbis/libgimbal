/*! \file
 *  \brief GblModule loadable plugin instance and management API
 *  \ingroup metaBuiltinTypes
 */
#ifndef GIMBAL_MODULE_H
#define GIMBAL_MODULE_H

#include "gimbal_context.h"
#include "../ifaces/gimbal_iplugin.h"
#include "../../strings/gimbal_string_ref.h"
#include "../../utils/gimbal_version.h"

/// \ingroup metaBuiltinTypes
#define GBL_MODULE_TYPE                 (GBL_TYPEOF(GblModule))
#define GBL_MODULE(instance)            (GBL_INSTANCE_CAST(instance,  GblModule))
#define GBL_MODULE_CLASS(klass)         (GBL_CLASS_CAST(klass, GblModule))
#define GBL_MODULE_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GblModule))
#define GBL_SELF_TYPE                   GblModule

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblOptionGroup);
GBL_FORWARD_DECLARE_STRUCT(GblModule);

typedef GblBool (*GblModuleIterFn)(GblModule* pIt, void* pUd);

GBL_CLASS_DERIVE(GblModule, GblContext, GblIPlugin)
    GBL_RESULT  (*pFnLoad)  (GBL_SELF);
    GBL_RESULT  (*pFnUnload)(GBL_SELF);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblModule, GblContext)
    GblVersion      version;
    GblStringRef    prefix;
    GblStringRef    author;
    GblStringRef    description;
    GblOptionGroup* pOptionGroup;
    GblType         resultType;
GBL_INSTANCE_END

GBL_PROPERTIES(GblModule,
    (prefix,      GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_STRING_TYPE),
    (version,     GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_UINT32_TYPE),
    (author,      GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_STRING_TYPE),
    (description, GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_STRING_TYPE),
    (resultType,  GBL_GENERIC, (READ),                    GBL_TYPE_TYPE),
    (typeCount,   GBL_GENERIC, (READ),                    GBL_UINT32_TYPE)
)

// ===== Static/Service API =====
GblType     GblModule_type          (void)                          GBL_NOEXCEPT;
GblModule*  GblModule_find          (const char* pName)             GBL_NOEXCEPT;
GblSize     GblModule_count         (void)                          GBL_NOEXCEPT;

GBL_RESULT  GblModule_register      (GBL_SELF)                      GBL_NOEXCEPT;
GBL_RESULT  GblModule_unregister    (GBL_SELF)                      GBL_NOEXCEPT;

GblBool     GblModule_foreach       (GblModuleIterFn pFnIter,
                                     void*           pUserdata)     GBL_NOEXCEPT;
// ===== Instance API =====

GblModule*  GblModule_create        (GblType     derivedType,
                                     const char* pName,
                                     GblVersion  version,
                                     const char* pDescription,
                                     const char* pPrefix)           GBL_NOEXCEPT;

GBL_RESULT  GblModule_unref         (GBL_SELF)                      GBL_NOEXCEPT;

// ===== SubType API =====

GblType     GblModule_registerType  (GBL_SELF,
                                     GblType            parent,
                                     const char*        pName,
                                     const GblTypeInfo* pInfo,
                                     GblFlags           flags)      GBL_NOEXCEPT;

GblType     GblModule_typeFromName  (GBL_CSELF, const char* pName)  GBL_NOEXCEPT;
GblType     GblModule_typeFromIndex (GBL_CSELF, GblSize     index)  GBL_NOEXCEPT;
GblSize     GblModule_typeCount     (GBL_CSELF)                     GBL_NOEXCEPT;

GblBool     GblModule_isLoaded      (GBL_CSELF)                     GBL_NOEXCEPT;

GBL_RESULT  GblModule_use           (GBL_SELF)                      GBL_NOEXCEPT;
GBL_RESULT  GblModule_unuse         (GBL_SELF)                      GBL_NOEXCEPT;
GblSize     GblModule_useCount      (GBL_CSELF)                     GBL_NOEXCEPT;


GBL_DECLS_END

#undef GBL_SELF_TYPE



#endif // GIMBAL_MODULE_H
