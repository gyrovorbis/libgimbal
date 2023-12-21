/*! \file
 *  \brief   GblException object-oriented error type
 *  \ingroup core
 *
 *  This file contains the API for GblException, an extensible,
 *  object-oriented error type, similar to exceptions seen
 *  in languages such as C++, C#, and Java.
 *
 *  Unlike GblError, which is minimalistic and lightweight,
 *  GblException provides more flexibility for error data
 *  at the price of being heavier. It's better suited for
 *  errors which need to carry additional data payloads
 *  beyond just an error code and a message.
 *
 *  \todo
 *   - consider making it a GblEvent and using event
 *     propagation to implement error filtering?
 *   - consider supporting child errors which become
 *     the children of a rethrown parent error
 *
 *   \author    2023 Falco Girgis
 *   \copyright MIT License
 */
#ifndef GIMBAL_EXCEPTION_H
#define GIMBAL_EXCEPTION_H

#include "../meta/instances/gimbal_object.h"
#include <gimbal/core/gimbal_thread.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_EXCEPTION_TYPE            (GBL_TYPEID(GblException))            //!< Type UUID for GblException
#define GBL_EXCEPTION(self)           (GBL_CAST(GblException, self))        //!< Casts a GblInstance to GblException
#define GBL_EXCEPTION_CLASS(klass)    (GBL_CLASS_CAST(GblException, klass)) //!< Casts a GblClass to GblExceptionClass
#define GBL_EXCEPTION_GET_CLASS(self) (GBL_CLASSOF(GblException, self))     //!< Retrieves a GblExceptionClass from a GblInstance
//! @}

/*! \name  DSL Macros
 *  \brief Macros for managing exceptions and control flow
 *  \todo  Implement
 *  @{
 */
#define GBL_TRY
#define GBL_THROW(type, ...)
#define GBL_CATCH
#define GBL_CATCH_AS(type, name)
//! @}

#define GBL_SELF_TYPE GblException

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblException);

/*! \struct  GblExceptionClass
 *  \extends GblObjectClass
 *  \brief   GblClass VTable structure for GblException
 *
 *  GblExceptionClass provides virtual methods for implementing
 *  a custom GblException type.
 *
 *  \sa GblException
 */
GBL_CLASS_DERIVE(GblException, GblObject)
    //! Virtual method returning the string representation of an error code
    GBL_RESULT (*pFnResultString)(GBL_CSELF, GblEnum resultCode, const char** ppString);
    //! A GblEnum type representing the exception's set of error codes
    GblType resultType;
GBL_CLASS_END

/*! \struct  GblException
 *  \extends GblObject
 *  \ingroup core
 *  \brief   Object containing an application error and its context
 *
 *  Instance structure encapsulating data for a single exception.
 *  It consists of a result code, an error message, source code
 *  context, and a pointer to the thread that generated it.
 *
 *  \sa GblExceptionClass
 */
GBL_INSTANCE_DERIVE(GblException, GblObject)
    GblType       resultType;   //!< Most derived type of the GblException::result code
    GblEnum       result;       //!< Result status code representing the error
    GblStringRef* pMessage;     //!< Custom message associated with the error
    GblThread*    pThread;      //!< Thread that generated the exception
    const char*   pFile;        //!< Source file from which the exception was generated
    const char*   pFunction;    //!< Function from which the exception was generated
    size_t        line;         //!< Line of code form which the exception was generated
GBL_INSTANCE_END

//! \cond
GBL_PROPERTIES(GblException,
    (result,   GBL_GENERIC, (READ, CONSTRUCT), GBL_ENUM_TYPE),
    (thread,   GBL_GENERIC, (READ, CONSTRUCT), GBL_THREAD_TYPE),
    (message,  GBL_GENERIC, (READ, WRITE    ), GBL_STRING_TYPE),
    (file,     GBL_GENERIC, (READ, WRITE    ), GBL_STRING_TYPE),
    (function, GBL_GENERIC, (READ, WRITE    ), GBL_STRING_TYPE),
    (line,     GBL_GENERIC, (READ, WRITE    ), GBL_SIZE_TYPE)
)
//! \endcond

//! Returns the GblType UUID corresponding to GblException
GBL_EXPORT GblType GblException_type (void) GBL_NOEXCEPT;

/*! \name  Static Methods
 *  \brief Static methods for managing a thread's active exception
 *  @{
 */
//! Returns a pointer to the current thread's pending GblException or NULL if there isn't one
GBL_EXPORT GblException* GblException_current (void)               GBL_NOEXCEPT;
//! Clears the current thread's active GblException, returning GBL_TRUE if it had one
GBL_EXPORT GblBool       GblException_clear   (void)               GBL_NOEXCEPT;
//! Takes ownership of the current thread's active GblException if it matches \p type, otherwise returns NULL
GBL_EXPORT GblException* GblException_catch   (GblType type)       GBL_NOEXCEPT;
//! Sets the given GblException as the current thread's active exception, taking ownership of it
GBL_EXPORT void          GblException_throw   (GblException* pErr) GBL_NOEXCEPT;
//! @}

/*! \name  Instance Methods
 *  \brief Methods for creating and accessing GblExceptions
 *  \relatesalso GblException
 *  @{
 */
//! Creates a GblException of the \p derived type, with the given properties
GBL_EXPORT GblException* GblException_create       (GblType     derived,
                                                    const char* pFile,
                                                    const char* pFunc,
                                                    size_t      line,
                                                    GblType     resultType,
                                                    GblEnum     result,
                                                    const char* pFmt,
                                                    ...)       GBL_NOEXCEPT;
//! Returns a new reference to an existing GblException, incrementing its refcount
GBL_EXPORT GblException* GblException_ref          (GBL_CSELF) GBL_NOEXCEPT;
//! Decrements and returns the refcount of a GblException, potentially destroying it
GBL_EXPORT GblRefCount   GblException_unref        (GBL_SELF)  GBL_NOEXCEPT;
//! Returns the string representation of the GblException::result code
GBL_EXPORT const char*   GblException_resultString (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the enumeration type of the GblException::result code
GBL_EXPORT GblType       GblException_resultType   (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given GblException has a source context associated with it
GBL_EXPORT GblBool       GblException_hasSource    (GBL_CSELF) GBL_NOEXCEPT;
//! Returns a non-NULL string containing the source file from which the GblException originated
GBL_EXPORT const char*   GblException_file         (GBL_CSELF) GBL_NOEXCEPT;
//! Returns a non-NULL string containing the function name from which the GblException originated
GBL_EXPORT const char*   GblException_function     (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the source code line number from which the GblException originated
GBL_EXPORT size_t        GblException_line         (GBL_CSELF) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond
#define GblException_create(derived, result, /*resultType,*/ ...) \
    (GblException_create)(derived, GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN, result, __VA_ARGS__)
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_EXCEPTION_H
