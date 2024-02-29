/*! \file
 *  \brief GblModule loadable plugin instance and management API
 *  \ingroup core
 *  \todo
 *      - advanced unit testing
 *      - plan out type registration/management
 *      - rig up option group parsery
 *      - stop inheriting GblContext
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_MODULE_H
#define GIMBAL_MODULE_H

#include "../meta/instances/gimbal_context.h"
#include "../meta/ifaces/gimbal_iplugin.h"
#include "../strings/gimbal_string_ref.h"
#include "../utils/gimbal_version.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_MODULE_TYPE             (GBL_TYPEID(GblModule))            //!< Type UUID for GblModule
#define GBL_MODULE(self)            (GBL_CAST(GblModule, self))        //!< Function-style GblInstance cast
#define GBL_MODULE_CLASS(klass)     (GBL_CLASS_CAST(GblModule, klass)) //!< Function-style GblClass cast
#define GBL_MODULE_GET_CLASS(self)  (GBL_CLASSOF(GblModule, self))     //!< Gets a GblModuleClass from GblInstance
//! @}

#define GBL_REQUIRE(...)            GBL_REQUIRE_(__VA_ARGS__)   //!< Returns the module for the given type

#define GBL_SELF_TYPE   GblModule

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblOptionGroup);
GBL_FORWARD_DECLARE_STRUCT(GblModule);

//! Function callback used with GblModule_foreach() for iterating over active modules
typedef GblBool (*GblModuleIterFn)(GblModule* pIt, void* pClosure);

/*! \struct     GblModuleClass
 *  \extends    GblContextClass
 *  \implements GblIPluginClass
 *  \brief      GblClass structure for GblModule
 *
 *  Virtual method table for GblModule
 *  - inherits from GblContext
 *  - has to implement GblIPlugin's VTable
 *  - adds pure virtaul methods load/unload
 *
 *  \sa GblModule
 */
GBL_CLASS_DERIVE(GblModule, GblContext, GblIPlugin)
    GBL_RESULT (*pFnLoad)  (GBL_SELF); //!< Called when a GblModule is first loaded
    GBL_RESULT (*pFnUnload)(GBL_SELF); //!< Called after the GblModule is done being referenced
GBL_CLASS_END

/*! \struct     GblModule
 *  \ingroup    core
 *  \extends    GblContext
 *  \implements GblIPlugin
 *  \brief      Dynamically loadable service and associated meta types
 *
 *  A Module is a lazily-loaded object which is registered then later
 *  can be queried for by anywhere else in the application. This
 *  behavior lets you implement "services" as global singletons which
 *  can be accessed from anywhere. Modules also maintain their own inner
 *  registry of associated meta types, which is managed through the
 *  implemented GblIPlugin interface.
 *
 *  \sa GbModuleClass
 */
GBL_INSTANCE_DERIVE(GblModule, GblContext)
    GblVersion      version;        //!< Version of a module
    GblStringRef*   pPrefix;        //!< Namespace prefix of a module
    GblStringRef*   pAuthor;        //!< Author(s) of a module
    GblStringRef*   pDescription;   //!< Description of a module
    GblOptionGroup* pOptionGroup;   //!< Command-line option handler of a module
GBL_INSTANCE_END

//! \cond
GBL_PROPERTIES(GblModule,
    (prefix,      GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_STRING_TYPE),
    (version,     GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_UINT32_TYPE),
    (author,      GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_STRING_TYPE),
    (description, GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_STRING_TYPE),
    (useCount,    GBL_GENERIC, (READ),                    GBL_INT16_TYPE),
    (typeCount,   GBL_GENERIC, (READ),                    GBL_SIZE_TYPE)
)
//! \cond

// ===== Static/Service API =====
GBL_EXPORT GblType     GblModule_type         (void)                     GBL_NOEXCEPT;
GBL_EXPORT GblModule*  GblModule_find         (const char* pName)        GBL_NOEXCEPT;
GBL_EXPORT GblModule*  GblModule_findQuark    (GblQuark name)            GBL_NOEXCEPT;
GBL_EXPORT GblModule*  GblModule_at           (size_t  index)            GBL_NOEXCEPT;
GBL_EXPORT size_t      GblModule_count        (void)                     GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblModule_foreach      (GblModuleIterFn pFnIter,
                                               void*           pClosure) GBL_NOEXCEPT;

GBL_EXPORT GblModule*  GblModule_require      (const char* pName,
                                               const char* pVersion,
                                               const char* pFile,
                                               const char* pFunc,
                                               size_t      line)         GBL_NOEXCEPT;

GBL_EXPORT GblModule*  GblModule_requireQuark (GblQuark    quark,
                                               const char* pVersion,
                                               const char* pFile,
                                               const char* pFunc,
                                               size_t      line)         GBL_NOEXCEPT;

// ===== Instance API =====

GBL_EXPORT GblModule*  GblModule_create       (GblType     derivedType,
                                               const char* pName,
                                               GblVersion  version,
                                               const char* pAuthor,
                                               const char* pDescription,
                                               const char* pPrefix)      GBL_NOEXCEPT;

GBL_EXPORT GblRefCount GblModule_unref        (GBL_SELF)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblModule_register     (GBL_SELF)                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblModule_unregister   (GBL_SELF)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblModule_use          (GBL_SELF)                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblModule_unuse        (GBL_SELF)                 GBL_NOEXCEPT;
GBL_EXPORT GblRefCount GblModule_useCount     (GBL_CSELF)                GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblModule_isLoaded     (GBL_CSELF)                GBL_NOEXCEPT;


// ===== SubType API ===== (TODO)

GblType     GblModule_registerType  (GBL_SELF,
                                     GblType            parent,
                                     const char*        pName,
                                     const GblTypeInfo* pInfo,
                                     GblFlags           flags)      GBL_NOEXCEPT;

GblType     GblModule_typeFromName  (GBL_CSELF, const char* pName)  GBL_NOEXCEPT;
GblType     GblModule_typeFromIndex (GBL_CSELF, size_t      index)  GBL_NOEXCEPT;

// Fix for Windows: "Redefinition with different linkage"
// Added GBL_EXPORT here and removed from implementation in gimbal_module.c
GBL_EXPORT size_t GblModule_typeCount     (GBL_CSELF)               GBL_NOEXCEPT;

GBL_DECLS_END

// ====== IMPLEMENTATION =====
///\cond
#define GBL_REQUIRE_(...) \
    GBL_VA_OVERLOAD_CALL(GBL_REQUIRE_, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

#define GBL_REQUIRE__1(type) \
    GBL_CAST(type, GblModule_require(GblType_name(GBL_TYPEID(type)), \
                                                     GBL_NULL, __FILE__, __func__, \
                                                     __LINE__))
#define GBL_REQUIRE__2(type, name) \
    GBL_CAST(type, GblModule_require(name, GBL_NULL, __FILE__, __func__, \
                                        __LINE__))

#define GBL_REQUIRE__3(type, name, version) \
    GBL_CAST(type, GblModule_require(name, version, __FILE__, __func__, \
                                        __LINE__))
///\endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_MODULE_H
