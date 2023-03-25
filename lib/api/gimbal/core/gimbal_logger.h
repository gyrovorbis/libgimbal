/*! \file
 *  \brief GblLogger filterable log and logging macros
 *  \ingroup core
 */

#ifndef GIMBAL_LOGGER_H
#define GIMBAL_LOGGER_H

#include <time.h>
#include "../meta/instances/gimbal_object.h"
#include "../containers/gimbal_linked_list.h"

#define GBL_LOGGER_TYPE                   (GBL_TYPEOF(GblLogger))
#define GBL_LOGGER(self)                  (GBL_INSTANCE_CAST(self, GblLogger))
#define GBL_LOGGER_CLASS(klass)           (GBL_CLASS_CAST(klass, GblLogger))
#define GBL_LOGGER_GET_CLASS(self)        (GBL_INSTANCE_GET_CLASS(self, GblLogger))

#define GBL_LOG_WRITE(flags, domain, ...) GblLogger_write(__FILE__, __func__, __LINE__, domain, flags, __VA_ARGS__)
#define GBL_LOG_DEBUG(domain, ...)        GBL_LOG_WRITE(GBL_LOG_DEBUG, domain, __VA_ARGS__)
#define GBL_LOG_VERBOSE(domain, ...)      GBL_LOG_WRITE(GBL_LOG_VERBOSE, domain, __VA_ARGS__)
#define GBL_LOG_INFO(domain, ...)         GBL_LOG_WRITE(GBL_LOG_INFO, domain, __VA_ARGS__)
#define GBL_LOG_WARN(domain, ...)         GBL_LOG_WRITE(GBL_LOG_WARN, domain, __VA_ARGS__)
#define GBL_LOG_ERROR(domain, ...)        GBL_LOG_WRITE(GBL_LOG_ERROR, domain, __VA_ARGS__)
#define GBL_LOG_PUSH()                    GblLogger_push()
#define GBL_LOG_POP(n)                    GblLogger_pop(n)

#define GBL_SELF_TYPE GblLogger

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblLogger);

typedef GblBool (*GblLoggerIterFn)(GblLogger* pLogger, void* pClosure);

GBL_DECLARE_FLAGS(GBL_LOG_FLAGS) {
    GBL_LOG_REENTRANT = 0x1,
    GBL_LOG_DEBUG     = 0x2,
    GBL_LOG_VERBOSE   = 0x4,
    GBL_LOG_INFO      = 0x8,
    GBL_LOG_WARN      = 0x10,
    GBL_LOG_ERROR     = 0x20,
    GBL_LOG_USER      = 0x40
};

GBL_CLASS_DERIVE(GblLogger, GblObject)
    GBL_RESULT (*pFnWrite)(GBL_SELF,
                           const char*   pFile,
                           const char*   pFunction,
                           size_t        line,
                           GblThd*       pThread,
                           time_t        timeStamp,
                           const char*   pDomain,
                           GBL_LOG_FLAGS flags,
                           const char*   pFmt,
                           va_list       varArgs);

    GBL_RESULT (*pFnPush) (GBL_SELF,
                           GblThd* pThread);

    GBL_RESULT (*pFnPop)  (GBL_SELF,
                           GblThd* pThread,
                           size_t  count);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblLogger, GblObject)
    GBL_PRIVATE()
        union {
            GblLinkedListNode listNode;
            GblLogger*        pNext;
        };
        GblBool               reentrant;
    GBL_PRIVATE_END
    GblFlags                  flagsFilter;
GBL_INSTANCE_END

GBL_PROPERTIES(GblLogger,
    (flagsFilter,   GBL_GENERIC, (READ, WRITE), GBL_FLAGS_TYPE),
    (threadFilters, GBL_GENERIC, (READ, WRITE), GBL_POINTER_TYPE),
    (domainFilters, GBL_GENERIC, (READ, WRITE), GBL_POINTER_TYPE)
)

// ===== Static Methods =====
GBL_EXPORT GblType      GblLogger_type             (void)                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblLogger_register         (GblLogger* pLogger)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblLogger_unregister       (GblLogger* pLogger)       GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblLogger_foreach          (GblLoggerIterFn pIt,
                                                    void* pClosure)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblLogger_push             (void)                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblLogger_pop              (size_t  count)            GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblLogger_write            (const char*   pFile,
                                                    const char*   pFunction,
                                                    size_t        line,
                                                    const char*   pDomain,
                                                    GBL_LOG_FLAGS flags,
                                                    const char*   pFmt,
                                                    ...)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblLogger_writeVa          (const char*   pFile,
                                                    const char*   pFunction,
                                                    size_t        line,
                                                    const char*   pDomain,
                                                    GBL_LOG_FLAGS flags,
                                                    const char*   pFmt,
                                                    va_list       varArgs)    GBL_NOEXCEPT;
// ===== Instance Methods =====
GBL_EXPORT GblLogger*   GblLogger_create           (GblType         derived,
                                                    size_t          allocSize,
                                                    GblLoggerClass* pClass)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblLogger_construct        (GBL_SELF,
                                                    GblType         derived,
                                                    GblLoggerClass* pClass)   GBL_NOEXCEPT;

GBL_EXPORT GblRefCount  GblLogger_unref            (GBL_SELF)                 GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblLogger_hasFilter        (GBL_CSELF,
                                                    const GblThd*    pThread,
                                                    const char*      pDomain,
                                                    GBL_LOG_FLAGS    flags)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblLogger_setDomainFilters (GBL_SELF,
                                                    const char* domains[])    GBL_NOEXCEPT;

GBL_EXPORT const char** GblLogger_domainFilters    (GBL_CSELF)                GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblLogger_hasDomainFilter  (GBL_CSELF,
                                                    const char* pDomain)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblLogger_setThreadFilters (GBL_SELF,
                                                    const GblThd* pThrs[])    GBL_NOEXCEPT;

GBL_EXPORT const GblThd**
                        GblLogger_threadFilters    (GBL_CSELF)                GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblLogger_hasThreadFilter  (GBL_CSELF,
                                                    const GblThd* pThr)       GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_LOGGER_H
