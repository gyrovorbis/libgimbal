/*! \file
 *  \brief GblScanner, generic text tokenizer and lexer
 *  \ingroup utils
 */
#ifndef GIMBAL_SCANNER_H
#define GIMBAL_SCANNER_H

#include "../meta/instances/gimbal_object.h"
#include "../strings/gimbal_pattern.h"
#include "../containers/gimbal_linked_list.h"
#include "../meta/signals/gimbal_signal.h"

#define GBL_SCANNER_TYPE                (GBL_TYPEOF(GblScanner))
#define GBL_SCANNER(self)               (GBL_INSTANCE_CAST(self, GblScanner))
#define GBL_SCANNER_CLASS(klass)        (GBL_CLASS_CAST(klass, GblScanner))
#define GBL_SCANNER_GET_CLASS(self)     (GBL_INSTANCE_GET_CLASS(self, GblScanner))

#define GBL_SCANNER_DELIMETERS_DEFAULT  " \t\n\r"

#define GBL_SELF_TYPE GblScanner

GBL_FORWARD_DECLARE_STRUCT(GblScanner);
GBL_FORWARD_DECLARE_STRUCT(GblArrayList);

GBL_DECLARE_ENUM(GBL_SCANNER_FLAGS) {
    GBL_SCANNER_SCAN_ERROR = 0x1,
    GBL_SCANNER_PEEK_ERROR = 0x2,
    GBL_SCANNER_EOF        = 0x4
};

GBL_DECLARE_STRUCT(GblScannerCursor) {
    size_t position;
    size_t line;
    size_t column;
    size_t length;
};

GBL_CLASS_DERIVE(GblScanner, GblObject)
    GBL_PRIVATE()
        GblStringRef* pDefaultDelimeters;
    GBL_PRIVATE_END

    GBL_RESULT (*pFnScanToken)(GBL_SELF);
    GBL_RESULT (*pFnPeekToken)(GBL_SELF);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblScanner, GblObject)
    GBL_PRIVATE()
        GblStringView streamBuffer;
        GblStringRef* pInputString;
        GblStringRef* pDelimeters;
        GblArrayList* pCursorStack;
    GBL_PRIVATE_END
    const
    GblScannerCursor* pCursor;
    GblStringView     token;
    GblStringView     next;

    GblStringRef*     pError;
    GBL_SCANNER_FLAGS status;
GBL_INSTANCE_END

GBL_PROPERTIES(GblScanner,
    (input,      GBL_GENERIC, (READ, WRITE, CONSTRUCT), GBL_STRING_TYPE),
    (delimeters, GBL_GENERIC, (READ, WRITE, CONSTRUCT), GBL_STRING_TYPE),
    (token,      GBL_GENERIC, (READ),                   GBL_STRING_TYPE),
    (next,       GBL_GENERIC, (READ),                   GBL_STRING_TYPE),
    (status,     GBL_GENERIC, (READ),                   GBL_FLAGS_TYPE),
    (error,      GBL_GENERIC, (READ, WRITE),            GBL_STRING_TYPE)
)

GBL_SIGNALS(GblScanner,
    (reset,   (GBL_INSTANCE_TYPE, pReceiver)),
    (eof,     (GBL_INSTANCE_TYPE, pReceiver)),
    (peeked,  (GBL_INSTANCE_TYPE, pReceiver)),
    (scanned, (GBL_INSTANCE_TYPE, pReceiver)),
    (raised,  (GBL_INSTANCE_TYPE, pReceiver))
)

GBL_EXPORT GblType     GblScanner_type          (void)                       GBL_NOEXCEPT;

GBL_EXPORT GblScanner* GblScanner_create        (const char* pStr,
                                                 size_t      count)          GBL_NOEXCEPT;

GBL_EXPORT GblRefCount GblScanner_unref         (GBL_SELF)                   GBL_NOEXCEPT;

GBL_EXPORT size_t      GblScanner_tell          (GBL_CSELF)                  GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_seek          (GBL_SELF, ssize_t whence)   GBL_NOEXCEPT;
GBL_EXPORT void        GblScanner_reset         (GBL_SELF)                   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblScanner_pushCursor    (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblScanner_popCursor     (GBL_SELF)                   GBL_NOEXCEPT;

GBL_EXPORT void        GblScanner_clearError    (GBL_SELF)                   GBL_NOEXCEPT;

GBL_EXPORT void        GblScanner_raiseError    (GBL_SELF,
                                                 const char* pFmt,
                                                 ...)                        GBL_NOEXCEPT;

GBL_EXPORT const char* GblScanner_input         (GBL_CSELF)                  GBL_NOEXCEPT;

GBL_EXPORT void        GblScanner_setInput      (GBL_SELF,
                                                 const char* pStr,
                                                 size_t      count)          GBL_NOEXCEPT;

GBL_EXPORT void        GblScanner_setDelimeters (GBL_SELF,
                                                 const char* pPattern)       GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblScanner_scanToken     (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_scanMatch     (GBL_SELF, const char* pPat) GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_scanLine      (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_scanBytes     (GBL_SELF, size_t bytes)     GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_scanBool      (GBL_SELF, GblBool* pBool)   GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_scanChar      (GBL_SELF, char* pChar)      GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_scanInt32     (GBL_SELF, int32_t* pInt)    GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_scanUint32    (GBL_SELF, uint32_t* pUint)  GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_scanFloat     (GBL_SELF, float* pFloat)    GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_scanType      (GBL_SELF, GblType t, ...)   GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblScanner_peekToken     (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_peekMatch     (GBL_SELF, const char* pPat) GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_peekLine      (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_peekBytes     (GBL_SELF, size_t bytes)     GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_peekBool      (GBL_SELF, GblBool* pBool)   GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_peekChar      (GBL_SELF, char* pChar)      GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_peekInt32     (GBL_SELF, int32_t* pInt)    GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_peekUint32    (GBL_SELF, uint32_t* pUint)  GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_peekFloat     (GBL_SELF, float* pFloat)    GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_peekType      (GBL_SELF, GblType t, ...)   GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblScanner_tryToken      (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_tryMatch      (GBL_SELF, const char* pPat) GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_tryLine       (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_tryBytes      (GBL_SELF, size_t bytes)     GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_nextBool      (GBL_SELF, GblBool* pBool)   GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_tryChar       (GBL_SELF, char* pChar)      GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_tryInt32      (GBL_SELF, int32_t* pInt)    GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_tryUint32     (GBL_SELF, uint32_t* pUint)  GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_tryFloat      (GBL_SELF, float* pFloat)    GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_tryType       (GBL_SELF, GblType t, ...)   GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblScanner_skipTokens    (GBL_SELF, size_t count)     GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_skipMatch     (GBL_SELF, const char* pStr) GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_skipToMatch   (GBL_SELF, const char* pStr) GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_skipLines     (GBL_SELF, size_t count)     GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblScanner_skipBytes     (GBL_SELF, size_t count)     GBL_NOEXCEPT;

GBL_EXPORT int         GblScanner_scanf         (GBL_SELF,
                                                 const char* pFmt,
                                                 ...)                        GBL_NOEXCEPT;

GBL_EXPORT int         GblScanner_vscanf        (GBL_SELF,
                                                 const char* pFmt,
                                                 va_list*    pList)          GBL_NOEXCEPT;

#undef GBL_SELF_TYPE

#endif // GIMBAL_SCANNER_H
