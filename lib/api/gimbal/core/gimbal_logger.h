/*! \file
 *  \brief      GblLogger filterable log and logging macros
 *  \ingroup    core
 *
 *  This file provides the public API for creating and
 *  managing custom logger types as well as the utility
 *  macros for actually using the log system.
 *
 *  \todo
 *      - migrate away from GblThd and towards GblThread
 *      - Spam filter: deduplicate messages within a certain interval
 *      - Default constructor values
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */

#ifndef GIMBAL_LOGGER_H
#define GIMBAL_LOGGER_H

#include <time.h>
#include "../meta/instances/gimbal_object.h"
#include "../containers/gimbal_linked_list.h"

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_LOGGER_TYPE             (GBL_TYPEID(GblLogger))            //!< Type UUID for GblLogger
#define GBL_LOGGER(self)            (GBL_CAST(GblLogger, self))        //!< Function-style cast to GblLogger
#define GBL_LOGGER_CLASS(klass)     (GBL_CLASS_CAST(GblLogger, klass)) //!< Function-style cast to GblLoggerClass
#define GBL_LOGGER_GET_CLASS(self)  (GBL_CLASSOF(GblLogger, self))     //!< Get GblLoggerClass from GblLogger
//! @}

/*! \name  Logging Macros
 *  \brief Top-level macros used with logging system
 * @{
 */
#define GBL_LOG_WRITE(flags, domain, ...)   GblLogger_write(__FILE__, __func__, __LINE__, \
                                                            domain, flags, __VA_ARGS__)                //!< Generalized log write operation
#define GBL_LOG_DEBUG(domain, ...)          GBL_LOG_WRITE(GBL_LOG_DEBUG, domain, __VA_ARGS__)          //!< Writes to log with GBL_LOG_DEBUG
#define GBL_LOG_VERBOSE(domain, ...)        GBL_LOG_WRITE(GBL_LOG_VERBOSE, domain, __VA_ARGS__)        //!< Writes to log with GBL_LOG_VERBOSE
#define GBL_LOG_INFO(domain, ...)           GBL_LOG_WRITE(GBL_LOG_INFO, domain, __VA_ARGS__)           //!< Writes to log with GBL_LOG_INFO
#define GBL_LOG_WARN(domain, ...)           GBL_LOG_WRITE(GBL_LOG_WARN, domain, __VA_ARGS__)           //!< Writes to log with GBL_LOG_WARN
#define GBL_LOG_ERROR(domain, ...)          GBL_LOG_WRITE(GBL_LOG_ERROR, domain, __VA_ARGS__)          //!< Writes to log with GBL_LOG_ERROR
#define GBL_LOG_PUSH()                      GblLogger_push()                                           //!< Pushes level to log stack
#define GBL_LOG_POP(n)                      GblLogger_pop(n)                                           //!< Pops N levels from log stack

#define GBL_LOG_DEBUG_PUSH(domain, ...)     GBL_LOG_MAKE_PUSH_(domain, GBL_LOG_DEBUG, __VA_ARGS__)     //!< Pushes level to log stack, then writes to log with GBL_LOG_DEBUG
#define GBL_LOG_VERBOSE_PUSH(domain, ...)   GBL_LOG_MAKE_PUSH_(domain, GBL_LOG_VERBOSE, __VA_ARGS__)   //!< Pushes level to log stack, then writes to log with GBL_LOG_VERBOSE
#define GBL_LOG_INFO_PUSH(domain, ...)      GBL_LOG_MAKE_PUSH_(domain, GBL_LOG_INFO, __VA_ARGS__)      //!< Pushes level to log stack, then writes to log with GBL_LOG_INFO
#define GBL_LOG_WARN_PUSH(domain, ...)      GBL_LOG_MAKE_PUSH_(domain, GBL_LOG_WARN, __VA_ARGS__)      //!< Pushes level to log stack, then writes to log with GBL_LOG_WARN
#define GBL_LOG_ERROR_PUSH(domain, ...)     GBL_LOG_MAKE_PUSH_(domain, GBL_LOG_ERROR, __VA_ARGS__)     //!< Pushes level to log stack, then writes to log with GBL_LOG_ERROR

#define GBL_LOG_DEBUG_SCOPE(domain, ...)    GBL_LOG_MAKE_SCOPE_(domain, GBL_LOG_DEBUG, __VA_ARGS__)    //!< Writes to log with GBL_LOG_DEBUG, then pushes level to log stack. Pops level when scope ends
#define GBL_LOG_VERBOSE_SCOPE(domain, ...)  GBL_LOG_MAKE_SCOPE_(domain, GBL_LOG_VERBOSE, __VA_ARGS__)  //!< Writes to log with GBL_LOG_VERBOSE, then pushes level to log stack. Pops level when scope ends
#define GBL_LOG_INFO_SCOPE(domain, ...)     GBL_LOG_MAKE_SCOPE_(domain, GBL_LOG_INFO, __VA_ARGS__)     //!< Writes to log with GBL_LOG_INFO, then pushes level to log stack. Pops level when scope ends
#define GBL_LOG_WARN_SCOPE(domain, ...)     GBL_LOG_MAKE_SCOPE_(domain, GBL_LOG_WARN, __VA_ARGS__)     //!< Writes to log with GBL_LOG_WARN, then pushes level to log stack. Pops level when scope ends
#define GBL_LOG_ERROR_SCOPE(domain, ...)    GBL_LOG_MAKE_SCOPE_(domain, GBL_LOG_ERROR, __VA_ARGS__)    //!< Writes to log with GBL_LOG_ERROR, then pushes level to log stack. Pops level when scope ends

//! @}

#define GBL_SELF_TYPE GblLogger

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblLogger);

//! Function signature for iterating over all loggers (see \ref GblLogger_foreach())
typedef GblBool (*GblLoggerIterFn)(GblLogger* pLogger, void* pClosure);

//! Flags used to tag a message or message filter
GBL_DECLARE_FLAGS(GBL_LOG_FLAGS) {
    GBL_LOG_REENTRANT = 0x1,    //!< Denotes a GblLogger's virtual methods are reentrant
    GBL_LOG_DEBUG     = 0x2,    //!< Denotes a "debug" level message, which is typically disabled
    GBL_LOG_VERBOSE   = 0x4,    //!< Denotes a "verbose" level message, which is regular-level
    GBL_LOG_INFO      = 0x8,    //!< Denotes an "info" message, which is typically used for noteworthy events
    GBL_LOG_WARN      = 0x10,   //!< Denotes a "warning" message, which means a potential issue has arisen
    GBL_LOG_ERROR     = 0x20,   //!< Denotes an "error" message, which means something has failed
    GBL_LOG_USER      = 0x40    //!< Denotess the first flag which can be used for arbitrary userdata
};

/*! \struct  GblLoggerClass
 *  \extends GblObjectClass
 *  \brief   GblClass vtable for GblLogger
 *
 *  GblLoggerClass provides virtual methods for implementing
 *  a logger back-end. These include push/pop operations for
 *  the stack-based structured logging as well as a generic
 *  write method for doing the actual message logging.
 *
 *  \sa GblLogger
 */
GBL_CLASS_DERIVE(GblLogger, GblObject)
    //! Virtual method used to do the actual log writing for a message
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
    //! Virtual method used to signify that another level has been pushed onto the log stack
    GBL_RESULT (*pFnPush) (GBL_SELF,
                           GblThd* pThread);
    //! Virtual method to denote that that \param{count} levels have been popped from the log stack
    GBL_RESULT (*pFnPop)  (GBL_SELF,
                           GblThd* pThread,
                           size_t  count);
GBL_CLASS_END

/*! \struct  GblLogger
 *  \extends GblObject
 *  \ingroup core
 *  \brief   Object used for implementing a custom logger
 *
 *  GblLogger is used to create a new instance of a custom logger.
 *  You can either subclass it and reimplement its virtual methods or
 *  do so by giving it a floating class (see \ref {Classes::Floating Classes}).
 *
 *  \sa GblLoggerClass
 */
GBL_INSTANCE_DERIVE(GblLogger, GblObject)
    GBL_PRIVATE_BEGIN
        union {
            GblLinkedListNode listNode;    //!< Internal generic list node
            GblLogger*        pNext;       //!< Internal pointer to next logger in list
        };
        GblBool               reentrant;   //!< Whether the logger is already handling a messaage
    GBL_PRIVATE_END
    GblFlags                  flagsFilter; //!< Combination of GBL_LOG_FLAGS to filter messages for
GBL_INSTANCE_END

//! \cond
GBL_PROPERTIES(GblLogger,
    (flagsFilter,   GBL_GENERIC, (READ, WRITE), GBL_FLAGS_TYPE),
    (threadFilters, GBL_GENERIC, (READ, WRITE), GBL_POINTER_TYPE),
    (domainFilters, GBL_GENERIC, (READ, WRITE), GBL_POINTER_TYPE)
)
//! \endcond

// ===== Static Methods =====
GBL_EXPORT GblType      GblLogger_type             (void)                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblLogger_register         (GblLogger* pLogger)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblLogger_unregister       (GblLogger* pLogger)       GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblLogger_foreach          (GblLoggerIterFn pIt,
                                                    void*           pClosure) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblLogger_push             (void)                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblLogger_pop              (size_t  count)            GBL_NOEXCEPT;
GBL_EXPORT size_t       GblLogger_depth            (void)                     GBL_NOEXCEPT;

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

// Helper macro for GBL_LOG_XXX_PUSH
#define GBL_LOG_MAKE_PUSH_(domain, level_macro, ...)    \
    GBL_STMT_START {                                    \
        GBL_LOG_PUSH();                                 \
        level_macro(domain, __VA_ARGS__);               \
    } GBL_STMT_END

// Helper macro for GBL_LOG_XXX_SCOPE
#define GBL_LOG_MAKE_SCOPE_(domain, level_macro, ...)   \
    GBL_STMT_START {                                    \
        level_macro(domain, __VA_ARGS__);               \
    } GBL_STMT_END;                                     \
    GBL_SCOPE(GBL_LOG_PUSH(), GBL_LOG_POP(1))

#endif // GIMBAL_LOGGER_H
