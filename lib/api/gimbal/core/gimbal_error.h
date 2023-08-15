/*! \file
 *  \brief GblError extensible OO error type
 *  \ingroup core
 *  \todo
 *   - finish me
 *   - consider making it a GblEvent and using event
 *     propagation to implement error filtering?
 *   - consider supporting child errors which become
 *     the children of a rethrown parent error
 *
 *   \author 2023 Falco Girgis
 *   \copyright MIT License
 */
#ifndef GIMBAL_ERROR_H
#define GIMBAL_ERROR_H

#include "gimbal_thread.h"
#include "../meta/instances/gimbal_object.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_ERROR_TYPE              (GBL_TYPEID(GblError))
#define GBL_ERROR(self)             (GBL_CAST(GblError, self))
#define GBL_ERROR_CLASS(klass)      (GBL_CLASS_CAST(GblError, klass))
#define GBL_ERROR_GET_CLASS(self)   (GBL_CLASSOF(GblError, self))
//! @}

#define GBL_TRY
#define GBL_THROW(type, ...)
#define GBL_CATCH
#define GBL_CATCH_AS(type, name)

#define GBL_SELF_TYPE GblError

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblError);

/*! \struct GblErrorClass
 *  \extends GblObjectClass
 *  \brief GblClass VTable structure for GblError
 *
 *  \sa GblError
 */
GBL_CLASS_DERIVE(GblError, GblObject)
    GBL_RESULT (*pFnResultString)(GBL_CSELF,
                                  GblEnum      result,
                                  const char** ppString);
//! onHandle/onUnHandle/onRaised
    GblType       resultType;
GBL_CLASS_END

/*! \struct GblError
 *  \extends GblObject
 *  \ingroup core
 *  \brief Object containing an application error and its context
 *  \sa GblErrorClass
 */
GBL_INSTANCE_DERIVE(GblError, GblObject)
    GblType       resultType;
    GblEnum       result;
    GblStringRef* pMessage;
    GblThread*    pThread;
    const char*   pFile;
    const char*   pFunction;
    size_t        line;
GBL_INSTANCE_END

GBL_PROPERTIES(GblError,
    (result,   GBL_GENERIC, (READ, CONSTRUCT), GBL_ENUM_TYPE),
    (thread,   GBL_GENERIC, (READ, CONSTRUCT), GBL_THREAD_TYPE),
    (message,  GBL_GENERIC, (READ, WRITE    ), GBL_STRING_TYPE),
    (file,     GBL_GENERIC, (READ, WRITE    ), GBL_STRING_TYPE),
    (function, GBL_GENERIC, (READ, WRITE    ), GBL_STRING_TYPE),
    (line,     GBL_GENERIC, (READ, WRITE    ), GBL_SIZE_TYPE)
)

GBL_EXPORT GblType    GblError_type         (void)           GBL_NOEXCEPT;

GBL_EXPORT GblError*  GblError_current      (void)           GBL_NOEXCEPT;
GBL_EXPORT GblBool    GblError_clear        (void)           GBL_NOEXCEPT;
GBL_EXPORT GblError*  GblError_catch        (GblType type)   GBL_NOEXCEPT;
GBL_EXPORT void       GblError_throw        (GblError* pErr) GBL_NOEXCEPT;

GBL_EXPORT GblError*  GblError_create       (GblType     derived,
                                             const char* pFile,
                                             const char* pFunc,
                                             size_t      line,
                                             GblType     resultType,
                                             GblEnum     result,
                                             const char* pFmt,
                                             ...)        GBL_NOEXCEPT;

GBL_EXPORT GblRefCount GblError_unref        (GBL_SELF)  GBL_NOEXCEPT;

GBL_EXPORT const char* GblError_resultString (GBL_CSELF) GBL_NOEXCEPT;
GBL_EXPORT GblType     GblError_resultType   (GBL_CSELF) GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblError_hasSource    (GBL_CSELF) GBL_NOEXCEPT;
GBL_EXPORT const char* GblError_file         (GBL_CSELF) GBL_NOEXCEPT;
GBL_EXPORT const char* GblError_function     (GBL_CSELF) GBL_NOEXCEPT;
GBL_EXPORT size_t      GblError_line         (GBL_CSELF) GBL_NOEXCEPT;

GBL_DECLS_END

#define GblError_create(derived, result, /*resultType,*/ ...) \
    (GblError_create)(derived, GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN, result, __VA_ARGS__)

#undef GBL_SELF_TYPE

#endif // GIMBAL_ERROR_H
