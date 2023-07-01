/*! \file
 *  \brief GblScanner, generic text tokenizer and lexer
 *  \ingroup utils
 *  \todo
 *      - _Generic()/type-deduced peek/scanPrimitive()s
 *      - write own scanf() implementation that knows how
 *        many characters have been read
 *
 *  \author Falco Girgis
 */
#ifndef GIMBAL_SCANNER_H
#define GIMBAL_SCANNER_H

#include "../meta/instances/gimbal_object.h"
#include "../strings/gimbal_pattern.h"
#include "../meta/signals/gimbal_signal.h"

#define GBL_SCANNER_TYPE                (GBL_TYPEOF(GblScanner))
#define GBL_SCANNER(self)               (GBL_INSTANCE_CAST(self, GblScanner))
#define GBL_SCANNER_CLASS(klass)        (GBL_CLASS_CAST(klass, GblScanner))
#define GBL_SCANNER_GET_CLASS(self)     (GBL_INSTANCE_GET_CLASS(self, GblScanner))

#define GBL_SCANNER_DELIMETERS_DEFAULT  "[ \t\n\r]"

#define GBL_SELF_TYPE GblScanner

GBL_FORWARD_DECLARE_STRUCT(GblScanner);

GBL_DECLARE_ENUM(GBL_SCANNER_FLAGS) {
    // Builtin Non-error state flags (bit 0)
    GBL_SCANNER_OK         = 0x00000000,
    GBL_SCANNER_EOF        = 0x00000001,
    // Builtin Error flags (bits 29-31)
    GBL_SCANNER_SCAN_ERROR = 0x20000000,
    GBL_SCANNER_PEEK_ERROR = 0x40000000,
    GBL_SCANNER_SKIP_ERROR = 0x80000000,
    // User masks (bits 1-28)
    GBL_SCANNER_USER_STATE = 0x00fffff2,
    GBL_SCANNER_USER_ERROR = 0x1f000000,
    // Masks for combined state and error flags
    GBL_SCANNER_STATE      = 0x00ffffff,
    GBL_SCANNER_ERROR      = 0xff000000,
};

GBL_DECLARE_STRUCT(GblScannerCursor) {
    size_t position;
    size_t line;
    size_t column;
    size_t length;
};

GBL_CLASS_DERIVE(GblScanner, GblObject)
    GBL_RESULT (*pFnNextToken)(GBL_SELF, GblStringView* pToken);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblScanner, GblObject)
    GblStringView     token;
    GblStringView     next;
    GblStringRef*     pError;
    GBL_SCANNER_FLAGS status;
GBL_INSTANCE_END

//! \cond
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
//! \endcond

GBL_EXPORT void          GblScannerClass_setDefaultDelimeters
                                                  (GBL_KLASS,
                                                   const char* pStr)           GBL_NOEXCEPT;

GBL_EXPORT void          GblScannerClass_setDefaultDelimetersRef
                                                  (GBL_KLASS,
                                                   GblStringRef* pStr)         GBL_NOEXCEPT;

GBL_EXPORT GblStringRef* GblScannerClass_defaultDelimeters
                                                  (GBL_CKLASS)                 GBL_NOEXCEPT;

GBL_EXPORT GblType       GblScanner_type          (void)                       GBL_NOEXCEPT;

GBL_EXPORT GblScanner*   GblScanner_create        (const char* pStr,
                                                   size_t      length)         GBL_NOEXCEPT;

GBL_EXPORT GblRefCount   GblScanner_unref         (GBL_SELF)                   GBL_NOEXCEPT;

GBL_EXPORT GblStringRef* GblScanner_input         (GBL_CSELF)                  GBL_NOEXCEPT;

GBL_EXPORT void          GblScanner_setInput      (GBL_SELF,
                                                   const char* pStr,
                                                   size_t      count)          GBL_NOEXCEPT;

GBL_EXPORT GblStringRef* GblScanner_delimeters    (GBL_CSELF)                  GBL_NOEXCEPT;

GBL_EXPORT void          GblScanner_setDelimeters (GBL_SELF,
                                                   const char* pPattern)       GBL_NOEXCEPT;

GBL_EXPORT void          GblScanner_clearError    (GBL_SELF)                   GBL_NOEXCEPT;

GBL_EXPORT void          GblScanner_raiseError    (GBL_SELF,
                                                   GblFlags    flags,
                                                   const char* pFmt,
                                                   ...)                        GBL_NOEXCEPT;

GBL_EXPORT void          GblScanner_reset         (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT size_t        GblScanner_tell          (GBL_CSELF)                  GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_seek          (GBL_SELF, int whence)       GBL_NOEXCEPT;

GBL_EXPORT const GblScannerCursor*
                         GblScanner_cursor        (GBL_CSELF, size_t depth)    GBL_NOEXCEPT;
GBL_EXPORT size_t        GblScanner_cursorDepth   (GBL_CSELF)                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblScanner_pushCursor    (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblScanner_popCursor     (GBL_SELF)                   GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblScanner_scanToken     (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanMatch     (GBL_SELF, const char* pPat) GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanLines     (GBL_SELF, size_t lines)     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanBytes     (GBL_SELF, size_t bytes)     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanBool      (GBL_SELF, GblBool* pBool)   GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanChar      (GBL_SELF, char* pChar)      GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanUint8     (GBL_SELF, uint8_t* pUint)   GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanUint16    (GBL_SELF, uint16_t* pUint)  GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanInt16     (GBL_SELF, int16_t* pInt)    GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanInt32     (GBL_SELF, int32_t* pInt)    GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanUint32    (GBL_SELF, uint32_t* pUint)  GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanInt64     (GBL_SELF, int64_t* pInt)    GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanUint64    (GBL_SELF, uint64_t* pUint)  GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanFloat     (GBL_SELF, float* pFloat)    GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanDouble    (GBL_SELF, double* pDouble)  GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_scanType      (GBL_SELF, GblType t, ...)   GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblScanner_peekToken     (GBL_SELF)                   GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekMatch     (GBL_SELF, const char* pPat) GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekLines     (GBL_SELF, size_t lines)     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekBytes     (GBL_SELF, size_t bytes)     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekBool      (GBL_SELF, GblBool* pBool)   GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekChar      (GBL_SELF, char* pChar)      GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekUint8     (GBL_SELF, uint8_t* pUint)   GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekUint16    (GBL_SELF, uint16_t* pUint)  GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekInt16     (GBL_SELF, int16_t* pInt)    GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekInt32     (GBL_SELF, int32_t* pInt)    GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekUint32    (GBL_SELF, uint32_t* pUint)  GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekInt64     (GBL_SELF, int64_t* pInt)    GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekUint64    (GBL_SELF, uint64_t* pUint)  GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekFloat     (GBL_SELF, float* pFloat)    GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekDouble    (GBL_SELF, double* pDouble)  GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_peekType      (GBL_SELF, GblType t, ...)   GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblScanner_skipTokens    (GBL_SELF, size_t count)     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_skipMatch     (GBL_SELF, const char* pStr) GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_skipToMatch   (GBL_SELF, const char* pStr) GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_skipLines     (GBL_SELF, size_t count)     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblScanner_skipBytes     (GBL_SELF, size_t count)     GBL_NOEXCEPT;

GBL_EXPORT int           GblScanner_scanf         (GBL_SELF,
                                                   const char* pFmt,
                                                   ...)                        GBL_NOEXCEPT;

GBL_EXPORT int           GblScanner_vscanf        (GBL_SELF,
                                                   const char* pFmt,
                                                   va_list*    pList)          GBL_NOEXCEPT;

#define                  GblScanner_create(...)   GblScanner_createDefault_(__VA_ARGS__)
#define                  GblScanner_cursor(...)   GblScanner_cursorDefault_(__VA_ARGS__)
#define                  GblScanner_setInput(...) GblScanner_setInputDefault_(__VA_ARGS__)

// ========== Implementation ==========
///\cond

#define GblScanner_createDefault_(...) \
    GblScanner_createDefault__(__VA_ARGS__, 0)
#define GblScanner_createDefault__(string, count, ...) \
    (GblScanner_create)(string, count)

#define GblScanner_cursorDefault_(...) \
    GblScanner_cursorDefault__(__VA_ARGS__, 0)
#define GblScanner_cursorDefault__(self, depth, ...) \
    (GblScanner_cursor)(self, depth)

#define GblScanner_setInputDefault_(...) \
    GblScanner_setInputDefault__(__VA_ARGS__, 0)
#define GblScanner_setInputDefault__(self, string, count, ...) \
    (GblScanner_setInput)(self, string, count)

///\endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_SCANNER_H
