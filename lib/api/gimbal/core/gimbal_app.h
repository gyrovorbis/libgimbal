/*! \file
 *  \brief      GblApp top-level application object
 *  \ingroup    core
 *
 *  This file contains the class and instance definitions
 *  for GblApp. Currently a WIP.
 *
 *  \todo
 *      - Finish me
 *      - add GblSettings
 *  \test
 *      - Test me
 *
 *   \author    2023 Falco Girgis
 *   \copyright MIT License
 */
#ifndef GIMBAL_APP_H
#define GIMBAL_APP_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"
#include "../utils/gimbal_version.h"
#include "../strings/gimbal_string_list.h"

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_APP_TYPE            (GBL_TYPEID(GblApp))            //!< Type UUID for GblApp
#define GBL_APP(self)           (GBL_CAST(GblApp, self))        //!< Cast GblInstance to GblApp
#define GBL_APP_CLASS(klass)    (GBL_CLASS_CAST(GblApp, klass)) //!< Cast GblClass to GblAppClass
#define GBL_APP_GET_CLASS(self) (GBL_CLASSOF(GblApp, self))     //!< Get GblAppClass from GblInstance
//! @}

#define GBL_SELF_TYPE GblApp

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblMainLoop);

/*! \struct  GblAppClass
 *  \extends GblObjectClass
 *  \brief GblClass VTable structure for GblApp
 *
 *  \todo Implement me.
 *
 *  \sa GblApp
 */
GBL_CLASS_DERIVE(GblApp, GblObject)
    //! Called for every event sent to any object in the application
    GBL_RESULT (*pFnNotify)(GblObject* pReceiver, GblEvent* pEvent);
GBL_CLASS_END

/*! \struct  GblApp
 *  \extends GblObject
 *  \ingroup core
 *  \brief   Top-level per-program application instance
 *
 *  \todo Implement me.
 *
 *  \sa GblAppClass
 */
GBL_INSTANCE_DERIVE(GblApp, GblObject)
    const char** ppArgv;
    size_t       argc;
    GblMainLoop* pLoop;
GBL_INSTANCE_END

//! \cond
GBL_PROPERTIES(GblApp,
    (argc,         GBL_GENERIC, (READ, CONSTRUCT), GBL_UINT32_TYPE),
    (argv,         GBL_GENERIC, (READ, CONSTRUCT), GBL_POINTER_TYPE),
    (libraryPaths, GBL_GENERIC, (READ, WRITE),     GBL_STRING_TYPE),
    (version,      GBL_GENERIC, (READ, WRITE),     GBL_UINT32_TYPE),
    (organization, GBL_GENERIC, (READ, WRITE),     GBL_STRING_TYPE),
    (domain,       GBL_GENERIC, (READ, WRITE),     GBL_STRING_TYPE)
)

GBL_SIGNALS(GblApp,
    (quitting, (GBL_INSTANCE_TYPE, pReceiver))
)
//! \endcond


/*! \name  Static Methods
 *  \brief Methods that operate on static global state
 *  @{
 */
//! Returns the GblType UUID associated with the GblApp type
GBL_EXPORT GblType GblApp_type           (void)              GBL_NOEXCEPT;
//! Returns a pointer to the current application instance (or NULL if not instantiated)
GBL_EXPORT GblApp* GblApp_instance       (void)              GBL_NOEXCEPT;
//! Returns GBL_TRUE if the GblApp instance is being initialized or constructed, GBL_FALSE otherwise
GBL_EXPORT GblBool GblApp_startingUp     (void)              GBL_NOEXCEPT;
//! Returns GBL_TRUE if the GblApp instance is being deinitialized or destructed, GBL_FALSE otherwise
GBL_EXPORT GblBool GblApp_closingDown    (void)              GBL_NOEXCEPT;
//! Adds \p pPath to the list of search paths for dynamic libraries and modules
GBL_EXPORT void    GblApp_addLibraryPath (const char* pPath) GBL_NOEXCEPT;
//! Returns a GblStringList containing the list of search paths for dynamic libraries and modules
GBL_EXPORT const GblStringList*
                   GblApp_libraryPaths   (void)              GBL_NOEXCEPT;
//! @}

/*! \name  Instance Methods
 *  \brief Methods which operate on a GblApp instance
 *  \relatesalso GblApp
 *  @{
 */
//! Creates a GblApp instance with the given command-line arguments, returning a pointer to it
GBL_EXPORT GblApp*     GblApp_create       (int argc,
                                            const char** ppArgv)  GBL_NOEXCEPT;
//! Decrements the refcount of a GblApp instance, destroying it if it is the last one
GBL_EXPORT GblRefCount GblApp_unref        (GBL_SELF)             GBL_NOEXCEPT;
//! Returns the version associated with the GblApp instance
GBL_EXPORT GblVersion  GblApp_version      (GBL_CSELF)            GBL_NOEXCEPT;
//! Returns the organization string associated with the GblApp instance
GBL_EXPORT const GblStringRef*
                       GblApp_organization (GBL_CSELF)            GBL_NOEXCEPT;
//! Returns the domain string associated with the GblApp instance
GBL_EXPORT const GblStringRef*
                       GblApp_domain       (GBL_CSELF)            GBL_NOEXCEPT;
//! Returns a GblStringList containing all command-line arguments as individual strings
GBL_EXPORT const GblStringList*
                       GblApp_argList      (GBL_CSELF)            GBL_NOEXCEPT;
//! Top-level execution function for a GblApp--blocks until execution is complete, returning a result code
GBL_EXPORT GBL_RESULT  GblApp_exec         (GBL_SELF)             GBL_NOEXCEPT;
//! Terminates an application abruptly, without signalling "quitting"
GBL_EXPORT GBL_RESULT  GblApp_exit         (GBL_SELF, int retVal) GBL_NOEXCEPT;
//! Terminates an application gracefully, emitting the "quitting" signal and doing any cleanup
GBL_EXPORT GBL_RESULT  GblApp_quit         (GBL_SELF, int retVal) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_APP_H
