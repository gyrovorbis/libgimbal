#ifndef GIMBAL_APP_H
#define GIMBAL_APP_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"
#include "../utils/gimbal_version.h"
#include "../strings/gimbal_string_list.h"

#define GBL_APP_TYPE                (GBL_TYPEOF(GblApp))

#define GBL_APP(instance)           (GBL_INSTANCE_CAST(instance, GblApp))
#define GBL_APP_CLASS(klass)        (GBL_CLASS_CAST(klass, GblApp))
#define GBL_APP_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GblApp))

#define GBL_SELF_TYPE GblApp

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblMainLoop);

GBL_CLASS_DERIVE(GblApp, GblObject)
    GBL_RESULT (*pFnNotify)(GblObject* pReceiver, GblEvent* pEvent);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblApp, GblObject)
    const char** ppArgv;
    size_t       argc;
    GblMainLoop* pLoop;
GBL_INSTANCE_END

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

GBL_EXPORT GblType     GblApp_type           (void)                 GBL_NOEXCEPT;
GBL_EXPORT GblApp*     GblApp_instance       (void)                 GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblApp_startingUp     (void)                 GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblApp_closingDown    (void)                 GBL_NOEXCEPT;
GBL_EXPORT void        GblApp_addLibraryPath (const char* pPath)    GBL_NOEXCEPT;
GBL_EXPORT const GblStringList*
                       GblApp_libraryPaths   (void)                 GBL_NOEXCEPT;

GBL_EXPORT GblApp*     GblApp_create         (int argc,
                                              const char** ppArgv)  GBL_NOEXCEPT;

GBL_EXPORT GblRefCount GblApp_unref          (GBL_SELF)             GBL_NOEXCEPT;

GBL_EXPORT GblVersion  GblApp_version        (GBL_CSELF)            GBL_NOEXCEPT;
GBL_EXPORT const GblStringRef*
                       GblApp_organization   (GBL_CSELF)            GBL_NOEXCEPT;
GBL_EXPORT const GblStringRef*
                       GblApp_domain         (GBL_CSELF)            GBL_NOEXCEPT;
GBL_EXPORT const GblStringList*
                       GblApp_argList        (GBL_CSELF)            GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblApp_exec           (GBL_SELF)             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblApp_exit           (GBL_SELF, int retVal) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblApp_quit           (GBL_SELF, int retVal) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_APP_H
