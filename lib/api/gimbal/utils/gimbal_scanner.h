/*! \file
 *  \brief GblScanner, generic text tokenizer and lexer
 *  \ingroup utils
 */
#ifndef GIMBAL_SCANNER_H
#define GIMBAL_SCANNER_H

#include "../meta/instances/gimbal_object.h"
#include "../strings/gimbal_pattern.h"

#define GBL_SCANNER_TYPE                (GBL_TYPEOF(GblScanner))
#define GBL_SCANNER(self)               (GBL_INSTANCE_CAST(self, GblScanner))
#define GBL_SCANNER_CLASS(klass)        (GBL_CLASS_CAST(klass, GblScanner))
#define GBL_SCANNER_GET_CLASS(self)     (GBL_INSTANCE_GET_CLASS(self, GblScanner))

#define GBL_SCANNER_DELIMETERS_DEFAULT  " \t\n\r"

#define GBL_SELF_TYPE GblScanner

GBL_FORWARD_DECLARE_STRUCT(GblScanner);

GBL_DECLARE_ENUM(GBL_SCANNER_FLAGS) {
    GBL_SCANNER_FLAG_READ_ERROR = 0x1,
    GBL_SCANNER_FLAG_EOF        = 0x2
};

GBL_CLASS_DERIVE(GblScanner, GblObject)
    GBL_RESULT (*pFnNextToken)(GBL_SELF);
    GBL_RESULT (*pFnPeekToken)(GBL_SELF);
    GBL_PRIVATE()
        GblStringRef* pDefaultDelimeters;
    GBL_PRIVATE_END
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblScanner, GblObject)
    GBL_PRIVATE()
        GblStringView streamBuffer;
        GblStringRef* pInputString;
        GblStringRef* pDelimeters;
    GBL_PRIVATE_END

    GblStringView token;
    GblStringView nextToken;

    GBL_SCANNER_FLAGS status;
GBL_INSTANCE_END

GBL_PROPERTIES(GblScanner,
    (input,      GBL_GENERIC, (READ, WRITE, CONSTRUCT), GBL_STRING_TYPE),
    (delimeters, GBL_GENERIC, (READ, WRITE, CONSTRUCT), GBL_STRING_TYPE),
    (token,      GBL_GENERIC, (READ),                   GBL_STRING_TYPE),
    (nextToken,  GBL_GENERIC, (READ),                   GBL_STRING_TYPE),
    (status,     GBL_GENERIC, (READ),                   GBL_ENUM_TYPE)
)

GBL_EXPORT GblType     GblScanner_type          (void)                    GBL_NOEXCEPT;

GBL_EXPORT GblScanner* GblScanner_create        (const char* pStr,
                                                 size_t      count)       GBL_NOEXCEPT;

GBL_EXPORT GblRefCount GblScanner_unref         (GBL_SELF)                GBL_NOEXCEPT;

GBL_EXPORT void        GblScanner_setInput      (GBL_SELF,
                                                 const char* pStr,
                                                 size_t      count)       GBL_NOEXCEPT;

GBL_EXPORT void        GblScanner_setDelimeters (GBL_SELF,
                                                 const char* pDel)        GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblScanner_nextToken     (GBL_SELF)                GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblScanner_peekToken     (GBL_SELF)                GBL_NOEXCEPT;

// ======== WIP ========
//GBL_EXPORT void        GblScanner_reset         (GBL_SELF)                GBL_NOEXCEPT;
//GBL_EXPORT size_t      GblScanner_tell          (GBL_CSELF)               GBL_NOEXCEPT;
//GBL_EXPORT GBL_RESULT  GblScanner_seek          (GBL_SELF, size_t offset) GBL_NOEXCEPT;
//GBL_EXPORT GblBool     GblScanner_eof           (GBL_CSELF)               GBL_NOEXCEPT;
//GBL_EXPORT GblBool     GblScanner_skipTokens    (GBL_SELF, size_t count)  GBL_NOEXCEPT;
//GBL_EXPORT GblBool     GblScanner_skipLines     (GBL_SELF, size_t count)  GBL_NOEXCEPT;
//GBL_EXPORT GblBool     GblScanner_skipBytes     (GBL_SELF, size_t count)  GBL_NOEXCEPT;

#undef GBL_SELF_TYPE

#endif // GIMBAL_SCANNER_H
