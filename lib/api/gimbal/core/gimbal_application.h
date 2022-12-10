#ifndef GIMBAL_APPLICATION_H
#define GIMBAL_APPLICATION_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"
#include "../utils/gimbal_version.h"
#include "../strings/gimbal_string_list.h"

#define GBL_APPLICATION_TYPE                (GBL_TYPEOF(GblApplication))

#define GBL_APPLICATION(instance)           (GBL_INSTANCE_CAST(instance, GblApplication))
#define GBL_APPLICATION_CLASS(klass)        (GBL_CLASS_CAST(klass, GblApplication))
#define GBL_APPLICATION_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GblApplication))

#define GBL_SELF_TYPE GblApplication

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblApplication, GblObject)

GBL_INSTANCE_DERIVE_EMPTY(GblApplication, GblObject)

GBL_PROPERTIES(GblApplication,
    (argc,         GBL_GENERIC, (READ, CONSTRUCT), GBL_UINT32_TYPE),
    (argv,         GBL_GENERIC, (READ, CONSTRUCT), GBL_POINTER_TYPE),
    (libraryPaths, GBL_GENERIC, (READ, WRITE),     GBL_STRING_TYPE),
    (name,         GBL_GENERIC, (READ, WRITE),     GBL_STRING_TYPE),
    (version,      GBL_GENERIC, (READ, WRITE),     GBL_UINT32_TYPE),
    (organization, GBL_GENERIC, (READ, WRITE),     GBL_STRING_TYPE),
    (domain,       GBL_GENERIC, (READ, WRITE),     GBL_STRING_TYPE)
)

//GBL_SIGNALS(GblApplication,
//    (aboutToQuit, GBL_INSTANCE_TYPE)
//)

GBL_EXPORT GblType              GblApplication_type           (void)                    GBL_NOEXCEPT;
GBL_EXPORT GblApplication*      GblApplication_instance       (void)                    GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblApplication_startingUp     (void)                    GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblApplication_closingDown    (void)                    GBL_NOEXCEPT;
GBL_EXPORT const GblStringList* GblApplication_libraryPaths   (void)                    GBL_NOEXCEPT;
GBL_EXPORT void                 GblApplication_addLibraryPath (const char* pPath)       GBL_NOEXCEPT;

GBL_EXPORT GblApplication*      GblApplication_create         (int argc, char** ppArgv) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           GblApplication_destroy        (GBL_SELF)                GBL_NOEXCEPT;

GBL_EXPORT const char*          GblApplication_name           (GBL_CSELF)               GBL_NOEXCEPT;
GBL_EXPORT GblVersion           GblApplication_version        (GBL_CSELF)               GBL_NOEXCEPT;
GBL_EXPORT const char*          GblApplication_organization   (GBL_CSELF)               GBL_NOEXCEPT;
GBL_EXPORT const char*          GblApplication_domain         (GBL_CSELF)               GBL_NOEXCEPT;
GBL_EXPORT const GblStringList* GblApplication_argList        (GBL_CSELF)               GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblApplication_exec           (GBL_SELF)                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           GblApplication_exit           (GBL_SELF, int retVal)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           GblApplication_quit           (GBL_SELF, int retVal)    GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_APPLICATION_H
