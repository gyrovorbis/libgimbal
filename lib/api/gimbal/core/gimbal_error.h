/*! \file
 *  \brief GblError thread-local generic error code
 *  \ingroup core
 *
 *  This file contains the public API for GblError, which is a structure
 *  used to represent a single pending error status for a single thread.
 *
 *   \author    2023 Falco Girgis
 *   \copyright MIT License
 */
#ifndef GIMBAL_ERROR_H
#define GIMBAL_ERROR_H

#include "gimbal_thd.h"
#include "../strings/gimbal_quark.h"

#define GBL_ERROR_DOMAIN        gblErrorDomain()    //!< Builtin standard error domain
#define GBL_ERROR_MESSAGE_SIZE  128                 //!< Size of the GblError::message string

#define GBL_SELF_TYPE GblError

GBL_DECLS_BEGIN

/*! Defines a GblError category
 *
 *  GblErrorDomain is a user-definable structure which represents a
 *  grouping of error codes. It provides both a domain name and a method
 *  for converting an error code to its string representation.
 *
 *  \sa GblError
 */
typedef struct GblErrorDomain {
    GblQuark name;                              //!< Name of the error domain or category
    const char* (*pFnCodeString)(GblEnum code); //!< Returns a string describing an error code
} GblErrorDomain;

/*! Low-level error structure
 *
 *  GblError is a low-level error structure which represents a particular
 *  error code, a message, a source location, and a "domain" for the error,
 *  which is what is used to distinguish different categories or enum codes.
 *
 *  GblError uses thread-local storage so that every thread has its own unique
 *  pending error, which can be assigned and manipulated independently.
 *
 * \sa GblErrorDomain
 */
typedef struct GblError {
    char                  message[GBL_ERROR_MESSAGE_SIZE];  //!< Custom detailed error string
    GblSourceLocation     srcLocation;                      //!< C/C++ source code location
    const GblErrorDomain* pDomain;                          //!< Pointer to the code's domain
    GblEnum               code;                             //!< Actual raw error code returned
} GblError;

/*! \name Accessors
 *  \brief Methods for accessing errors and their state
 *  @{
 */
//! Returns a pointer to the current thread's pending error or NULL
GBL_EXPORT const GblError* GblError_pending (void) GBL_NOEXCEPT;
//! Returns GblQuark of the name of the current thread's pending error domain
GBL_EXPORT GblQuark        GblError_domain  (void) GBL_NOEXCEPT;
//! Returns the stringified version of the current thread's pending error code
GBL_EXPORT const char*     GblError_string  (void) GBL_NOEXCEPT;

/*! \name Management
 *  \brief Methods for managing pending errors
 *  @{
 */
//! Clears the current thread's pending error, returning GBL_TRUE if there was a pending error
GBL_EXPORT GblBool         GblError_clear   (void)                          GBL_NOEXCEPT;
//! Equivalent to GblError_clear(), except the pending error is copied into the destination buffer
GBL_EXPORT GblBool         GblError_lower   (GBL_SELF)                      GBL_NOEXCEPT;
//! Raises an error on the current thread with the given domain, code, and message
GBL_EXPORT const GblError* GblError_raise   (const char*           pFile/*=NULL*/,
                                             const char*           pFunction/*=NULL*/,
                                             size_t                line/*=0*/,
                                             const GblErrorDomain* pDomain,
                                             GblEnum               code,
                                             const char*           pFmt/*=NULL*/,
                                             ...)                           GBL_NOEXCEPT;
//! Equivalent to GblError_raise(), except the additional args are passed via a va_list
GBL_EXPORT const GblError* GblError_raisev  (const char*           pFile/*=NULL*/,
                                             const char*           pFunction/*=NULL*/,
                                             size_t                line/*=0*/,
                                             const GblErrorDomain* pDomain,
                                             GblEnum               code,
                                             const char*           pFmt,
                                             va_list               varArgs) GBL_NOEXCEPT;
//! Reraises the current thread's pending error, updating its source context
GBL_EXPORT const GblError* GblError_reraise (const char* pFile/*=NULL*/,
                                             const char* pFunction/*=NULL*/,
                                             size_t      line/*=0*/)        GBL_NOEXCEPT;
//! @}

//! Returns the builtin error domain used by GBL_RESULT
GBL_EXPORT const GblErrorDomain* gblErrorDomain(void) GBL_NOEXCEPT;

GBL_DECLS_END

//! \cond
#define GblError_raise(...) \
    GblError_raiseDefault_(__VA_ARGS__)
#define GblError_raiseDefault_(...) \
    GblError_raiseDefault__(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__, GBL_NULL)
#define GblError_raiseDefault__(file, func, line, domain, code, ...) \
    ((GblError_raise)(file, func, line, domain, code, __VA_ARGS__))

#define GblError_reraise() \
    ((GblError_reraise)(__FILE__, __FUNCTION__, __LINE__))
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_ERROR_H
