/*! \file
 *  \brief Dynamic module and plugin system.
 *  \ingroup core
 *
 *  This file contains the type declarations of GblModule
 *  and its associated API, providing the functionality of
 *  a static registry of lazily loaded "services" or C++
 *  singleton types and their associated type registry.
 *
 *  \todo
 *      - advanced unit testing
 *      - plan out type registration/management
 *      - rig up option group parsery
 *      - stop inheriting GblContext
 *
 *  \author     2023, 2025 Falco Girgis
 *  \author           2025 Agustín Bellagamba
 *  \copyright  MIT License
 */
#ifndef GIMBAL_MODULE_H
#define GIMBAL_MODULE_H

#include "../meta/instances/gimbal_context.h"
#include "../meta/ifaces/gimbal_iplugin.h"
#include "../strings/gimbal_string_ref.h"
#include "../utils/gimbal_version.h"
#include "../utils/gimbal_option_group.h"

/*! \name Type System
 *  \brief Type UUID and cast operators.
 *  @{
 */
#define GBL_MODULE_TYPE           GBL_TYPEID(GblModule)            //!< Type UUID for GblModule
#define GBL_MODULE(self)          GBL_CAST(GblModule, self)        //!< Function-style GblInstance cast
#define GBL_MODULE_CLASS(klass)   GBL_CLASS_CAST(GblModule, klass) //!< Function-style GblClass cast
#define GBL_MODULE_CLASSOF(self)  GBL_CLASSOF(GblModule, self)     //!< Gets a GblModuleClass from GblInstance
//! @}

/*! Macro directive for importing a module.
 *
 *  Used to "import" a GblModule whose type has been registered with the type system,
 *  lazily loading it the fist time and returning references to it subsequently.
 *
 *  \note
 *  If the supplied pointer is non-NULL, the module is considered cached and is not going
 *  to be reloaded. This allows you to avoid dynamic lookups for subsequent calls when using
 *  the same pointer.
 *
 *  \code {.c}
 *      static MyModule* pModule = NULL;
 *      if(!GBL_REQUIRE(MyModule, &pModule, "MyModule", "1.1.0"))
 *          GBL_LOG_ERROR("MyModule", "Failed to import module!");
 *  \endcode
 *
 *  \param  instanceStruct Struct name for desired GblModule instance structure.
 *  \param  selfAddr       The address of a pointer to the instance to store the reference in.
 *  \param  name           Name of the module to import.
 *  \param  version        [optional] Major.minor.patch string for specific version.
 *
 *  \returns               A pointer to the imported module, or NULL if the import failed.
 */
#define GBL_REQUIRE(/*instanceStruct, selfAddr, name, versionMin=NULL*/ ...)       GBL_REQUIRE_(__VA_ARGS__)

/*! Macro directive for releasing a module.
 *
 *  Used to optionally "release" a module which has been previously
 *  imported, decrementing is "useCount" property, which causes the
 *  module to be dynamically unloaded upon hitting zero uses.
 *
 *  \note
 *  This does not necessarily need to be called if you do not wish for
 *  your module to be dynamically unloaded at run-time.
 *
 *  \param  selfAddr      The address of a pointer pointing to a module.
 *  \returns              GBL_RESULT status code.
 */
#define GBL_RELEASE(selfAddr)   GBL_RELEASE_(selfAddr)

/*! Scoped macro directive for using a module.
 *
 *  Used to define a local scope in which a module will be imported upon
 *  entry and released upon exit.
 *
 *  \code {.c}
 *      TestModule* pModule = NULL;
 *      GBL_REQUIRE_SCOPE(TestModule, &pModule) {
 *          TestModule_doStuff(pModule);
 *      }
 *  \endcode
 *
 *  \warning
 *  GBL_SCOPE_EXIT must be used to early-break from the scope without
 *  causing unwanted side-effects.
 *
 *  \param instanceStruct  Struct name for desired GblModule instance structure.
 *  \param selfAddr        Address of a pointer to the GblModule struct, which holds the reference.
 *  \param name            Name of the module to import.
 *  \param version         [optional] Major.minor.patch string for specific version.
 */
#define GBL_REQUIRE_SCOPE(/*instanceStruct, selfAddr, name, versionMin=NULL*/ ...) GBL_REQUIRE_SCOPE_(__VA_ARGS__)

#define GBL_SELF_TYPE GblModule

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblOptionGroup);
GBL_FORWARD_DECLARE_STRUCT(GblModule);

//! Function callback used with GblModule_foreach() for iterating over active modules, returning GBL_TRUE to end iteration early.
typedef GblBool (*GblModuleIterFn)(GblModule* pIt, void* pClosure);

/*! \struct     GblModuleClass
 *  \extends    GblContextClass
 *  \implements GblIPluginClass
 *  \brief      GblClass structure for GblModule
 *
 *  Virtual method table for GblModule
 *      - inherits from GblContext
 *      - has to implement GblIPlugin's VTable
 *      - adds pure virtaul methods load/unload
 *
 *  \sa GblModule, GblIPluginClass
 */
GBL_CLASS_DERIVE(GblModule, GblContext, GblIPlugin)
    GBL_RESULT (*pFnLoad)  (GBL_SELF);  //!< Called when a GblModule is first loaded
    GBL_RESULT (*pFnUnload)(GBL_SELF);  //!< Called after the GblModule is done being referenced
GBL_CLASS_END

/*! \struct     GblModule
 *  \ingroup    core
 *  \extends    GblContext
 *  \implements GblIPlugin
 *  \brief      Dynamically loadable service and associated meta types.
 *
 *  A Module is a lazily-loaded object which is registered then later
 *  can be queried for by anywhere else in the application. This
 *  behavior lets you implement "services" as global singletons which
 *  can be accessed from anywhere. Modules also maintain their own inner
 *  registry of associated meta types, which is managed through the
 *  implemented GblIPlugin interface.
 *
 *  \sa GbModuleClass, GblIPlugin
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
    (prefix,      GBL_GENERIC, (READ, WRITE, LOAD, SAVE),     GBL_STRING_TYPE),
    (version,     GBL_GENERIC, (READ, WRITE, LOAD, SAVE),     GBL_UINT32_TYPE),
    (author,      GBL_GENERIC, (READ, WRITE, LOAD, SAVE),     GBL_STRING_TYPE),
    (description, GBL_GENERIC, (READ, WRITE, LOAD, SAVE),     GBL_STRING_TYPE),
    (optionGroup, GBL_GENERIC, (READ, WRITE, LOAD, SAVE, IN), GBL_OPTION_GROUP_TYPE),
    (useCount,    GBL_GENERIC, (READ),                        GBL_INT16_TYPE)
)

GBL_SIGNALS(GblModule,
    (loaded),
    (unloaded)
)
//! \endcond

//! Returns the GblType UUID associated with GblModule.
GBL_EXPORT GblType GblModule_type (void) GBL_NOEXCEPT;

/*! \name  Registry
 *  \brief Static routines for querying the module registry.
 *  @{
 */
//! Returns a pointer to the module with the gibven name, if present, gracefully returning NULL otherwise.
GBL_EXPORT GblModule* GblModule_find      (const char* pName)            GBL_NOEXCEPT;
//! Equivalent to GblModule_find(), except doing a faster search using a GblQuark for the name.
GBL_EXPORT GblModule* GblModule_findQuark (GblQuark name)                GBL_NOEXCEPT;
//! Returns a pointer to the GblModule which has been loaded at the given index, or NULL if there isn't one.
GBL_EXPORT GblModule* GblModule_at        (size_t index)                 GBL_NOEXCEPT;
//! Returns the total number of modules which have been registered.
GBL_EXPORT size_t     GblModule_count     (void)                         GBL_NOEXCEPT;
//! Iterates over every registered module, invoking the provided iterator callback, optionally passing back an arbitrary userdata closure pointer.
GBL_EXPORT GblBool    GblModule_iterate   (GblModuleIterFn pFnIter,
                                           void*           pCl/*=NULL*/) GBL_NOEXCEPT;
//! @}

/*! \name  Importing
 *  \brief Routines for returning a module
 *  @{
 */
//  TODO: update comment
//! Loads or returns a module matching the given name and optional version identifier, raising an error and returning NULL if there was no match.
GBL_EXPORT GblModule* GblModule_require      (const char* pName,
                                              GblModule** ppSelf,
                                              const char* pVersion/*=NULL*/,
                                              const char* pFile/*=__FILE__*/,
                                              const char* pFunc/*=__func__*/,
                                              size_t      line/*=__LINE__*/) GBL_NOEXCEPT;
//! Equivalent to GblModule_require(), except using a faster quark for the name identifier.
GBL_EXPORT GblModule* GblModule_requireQuark (GblQuark    name,
                                              GblModule** ppSelf,
                                              const char* pVersion/*=NULL*/,
                                              const char* pFile/*=__FILE__*/,
                                              const char* pFunc/*=__func__*/,
                                              size_t      line/*=__LINE__*/) GBL_NOEXCEPT;
//! @}

/*! \name  Lifetime
 *  \brief Methods for creating and destroying modules.
 *  @{
 */
//! Creates and returns a reference to a new GblModule-compatible instance type, but does not add it to the registry.
GBL_EXPORT GblModule*  GblModule_create (GblType     derivedType,
                                         const char* pName,
                                         GblVersion  version,
                                         const char* pAuthor,
                                         const char* pDescription,
                                         const char* pPrefix) GBL_NOEXCEPT;
//! Releases a reference to a previously created GblModule instance, destructing and freeing it if there are no references left.
GBL_EXPORT GblRefCount GblModule_unref  (GBL_SELF)            GBL_NOEXCEPT;
//! @}

/*! \name  Registration
 *  \brief Methods for adding and removing from the registry.
 *  @{
 */
//! Attempts to register a previously instantiated GblModule instance as a globally importable module within the internal registry.
GBL_EXPORT GBL_RESULT  GblModule_register   (GBL_SELF) GBL_NOEXCEPT;
//! Attempts to unregister a previously registered GblModule instance, removing it from the internal registry.
GBL_EXPORT GBL_RESULT  GblModule_unregister (GBL_SELF) GBL_NOEXCEPT;
//! @}

/*! \name  Loading/Unloading
 *  \brief Routines for controlling when a module is loaded and unloaded.
 *  @{
 */
//! Manually increments the given module's usage count, loading it on first usage. You can manually control its lifetimeto prevent being dynamically unloaded like this.
GBL_EXPORT GBL_RESULT  GblModule_use      (GBL_SELF)  GBL_NOEXCEPT;
//! Manually decrements the given module's usage count, unloading it when the last user is done. You can manually control unloading it like this.
GBL_EXPORT GBL_RESULT  GblModule_unuse    (GBL_SELF)  GBL_NOEXCEPT;
//! Manually decrements the given module's usage count, and points the passed \param ppModule to NULL.
GBL_EXPORT GBL_RESULT  GblModule_release  (GblModule** ppModule)  GBL_NOEXCEPT;
//! Retrieves the current usage counter for the given module.
GBL_EXPORT GblRefCount GblModule_useCount (GBL_CSELF) GBL_NOEXCEPT;
//! Returns true if the given module is currently loaded and has active uses, return false otherwise.
GBL_EXPORT GblBool     GblModule_isLoaded (GBL_CSELF) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

// ====== IMPLEMENTATION =====
///\cond
#define GBL_REQUIRE_(...) \
    GBL_REQUIRE_DEFAULT_(__VA_ARGS__)
#define GBL_REQUIRE_DEFAULT_(...) \
    GBL_REQUIRE_DEFAULT__(__VA_ARGS__, NULL)
#define GBL_REQUIRE_DEFAULT__(type, pPtr, name, version, ...)   \
        (!GBL_AS(GblModule, *pPtr) ?                            \
        (GblModule_require(name, (GblModule**)pPtr, version,    \
                                __FILE__, __func__, __LINE__))  \
        : GBL_CAST(GblModule, *pPtr))

#define GBL_RELEASE_(pPtr)                  \
    (GBL_AS(GblModule, *pPtr) ?             \
     GblModule_release((GblModule**)pPtr)   \
     : GBL_RESULT_ERROR_INVALID_POINTER)

#define GBL_REQUIRE_SCOPE_(...) \
    GBL_REQUIRE_SCOPE_DEFAULT_(__VA_ARGS__)
#define GBL_REQUIRE_SCOPE_DEFAULT_(...) \
    GBL_REQUIRE_SCOPE_DEFAULT__(__VA_ARGS__, NULL)
#define GBL_REQUIRE_SCOPE_DEFAULT__(type, pPtr, name, version, ...)   \
    GBL_SCOPE(GBL_REQUIRE_DEFAULT__(type, pPtr, name, version), GBL_RELEASE_(pPtr))

#define GblModule_iterate(...) \
    GblModule_iterateDefault_(__VA_ARGS__)
#define GblModule_iterateDefault_(...)  \
    GblModule_iterateDefault__(__VA_ARGS__, NULL)
#define GblModule_iterateDefault__(iter, cl, ...) \
    (GblModule_iterate)(iter, cl)

///\endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_MODULE_H
