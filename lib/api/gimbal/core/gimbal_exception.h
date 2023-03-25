#ifndef GIMBAL_EXCEPTION_H
#define GIMBAL_EXCEPTION_H

#include "gimbal_thread.h"
#include "../meta/instances/gimbal_object.h"

#define GBL_EXCEPTION_TYPE              (GBL_TYPEOF(GblException))
#define GBL_EXCEPTION(self)             (GBL_INSTANCE_CAST(self, GblException))
#define GBL_EXCEPTION_CLASS(klass)      (GBL_CLASS_CAST(klass, GblException))
#define GBL_EXCEPTION_GET_CLASS(self)   (GBL_INSTANCE_GET_CLASS(self, GblException))

#define GBL_TRY
#define GBL_THROW(type, ...)
#define GBL_CATCH
#define GBL_CATCH_AS(type, name)

#define GBL_SELF_TYPE GblException

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblException);

GBL_CLASS_DERIVE(GblException, GblObject)
    GBL_RESULT (*pFnResultString)(GBL_CSELF,
                                  GblEnum      result,
                                  const char** ppString);
    GblType     resultType;
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblException, GblObject)
    GblEnum       result;
    GblStringRef* pMessage;
    GblThread*    pThread;
    const char*   pFile;
    const char*   pFunction;
    size_t        line;
GBL_INSTANCE_END

GBL_PROPERTIES(GblException,
    (result,   GBL_GENERIC, (READ, CONSTRUCT), GBL_ENUM_TYPE),
    (thread,   GBL_GENERIC, (READ, CONSTRUCT), GBL_THREAD_TYPE),
    (message,  GBL_GENERIC, (READ, WRITE    ), GBL_STRING_TYPE),
    (file,     GBL_GENERIC, (READ, WRITE    ), GBL_STRING_TYPE),
    (function, GBL_GENERIC, (READ, WRITE    ), GBL_STRING_TYPE),
    (line,     GBL_GENERIC, (READ, WRITE    ), GBL_SIZE_TYPE)
)

GBL_EXPORT GblType       GblException_type         (void)         GBL_NOEXCEPT;

GBL_EXPORT GblException* GblException_current      (void)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblException_clear        (void)         GBL_NOEXCEPT;
GBL_EXPORT GblException* GblException_catch        (GblType type) GBL_NOEXCEPT;

GBL_EXPORT GblException* GblException_create       (GblType     derived,
                                                    const char* pFile,
                                                    const char* pFunc,
                                                    size_t      line,
                                                    GblThread*  pThread,
                                                    GblEnum     result,
                                                    const char* pMsg,
                                                    ...)          GBL_NOEXCEPT;

GBL_EXPORT GblRefCount   GblException_unref        (GBL_SELF)     GBL_NOEXCEPT;

GBL_EXPORT const char*   GblException_resultString (GBL_CSELF)    GBL_NOEXCEPT;
GBL_EXPORT GblType       GblException_resultType   (GBL_CSELF)    GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblException_hasSource    (GBL_CSELF)    GBL_NOEXCEPT;
GBL_EXPORT const char*   GblException_file         (GBL_CSELF)    GBL_NOEXCEPT;
GBL_EXPORT const char*   GblException_function     (GBL_CSELF)    GBL_NOEXCEPT;
GBL_EXPORT size_t        GblException_line         (GBL_CSELF)    GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_EXCEPTION_H
