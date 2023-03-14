/*! \file
 *  \brief GblUri: URI parsing, formatting, validation, and management
 *  \ingroup utils
 */
#ifndef GIMBAL_URI_H
#define GIMBAL_URI_H

#include "../strings/gimbal_string_ref.h"

#define GBL_SELF_TYPE GblUri

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblStringBuffer);

GBL_DECLARE_FLAGS(GBL_URI_FLAGS) {
    None = 0x0,
/*
    RemoveScheme = 0x1,
    RemovePassword = 0x2,
    RemoveUserInfo = RemovePassword | 0x4,
    RemovePort = 0x8,
    RemoveAuthority = RemoveUserInfo | RemovePort | 0x10,
    RemovePath = 0x20,
    RemoveQuery = 0x40,
    RemoveFragment = 0x80,
    // 0x100 was a private code in Qt 4, keep unused for a while
    PreferLocalFile = 0x200,
    StripTrailingSlash = 0x400,
    RemoveFilename = 0x800,
    NormalizePathSegments = 0x1000
  */
};

GBL_FORWARD_DECLARE_STRUCT(GblUri);

GBL_EXPORT GblUri*       GblUri_create        (void)                         GBL_NOEXCEPT;

GBL_EXPORT GblUri*       GblUri_createParsed  (const char* pToParse,
                                               GblBool     relaxed)          GBL_NOEXCEPT;

GBL_EXPORT GblUri*       GblUri_ref           (GBL_SELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblRefCount   GblUri_unref         (GBL_SELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblRefCount   GblUri_refCount      (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT GblUri*       GblUri_copy          (GBL_SELF, GblUri* pDest)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblUri_parse         (GBL_SELF,
                                               const char* pStr,
                                               GblBool     relaxed)          GBL_NOEXCEPT;

GBL_EXPORT const char*   GblUri_format        (GBL_CSELF,
                                               GBL_URI_FLAGS    flags,
                                               GblStringBuffer* pStrBuff)    GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblUri_isValid       (GBL_CSELF)                    GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblUri_error         (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblUri_isEmpty       (GBL_CSELF)                    GBL_NOEXCEPT;
GBL_EXPORT void          GblUri_clear         (GBL_SELF)                     GBL_NOEXCEPT;

GBL_EXPORT void          GblUri_setScheme     (GBL_SELF, const char* pStr)   GBL_NOEXCEPT;
GBL_EXPORT void          GblUri_moveScheme    (GBL_SELF, GblStringRef* pRef) GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblUri_scheme        (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT void          GblUri_setAuthority  (GBL_SELF, const char* pStr)   GBL_NOEXCEPT;
GBL_EXPORT void          GblUri_moveAuthority (GBL_SELF, GblStringRef* pRef) GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblUri_authority     (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT void          GblUri_setUserInfo   (GBL_SELF, const char* pStr)   GBL_NOEXCEPT;
GBL_EXPORT void          GblUri_moveUserInfo  (GBL_SELF, GblStringRef* pRef) GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblUri_userInfo      (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT void          GblUri_setUserName   (GBL_SELF, const char* pStr)   GBL_NOEXCEPT;
GBL_EXPORT void          GblUri_moveUserName  (GBL_SELF, GblStringRef* pRef) GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblUri_userName      (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT void          GblUri_setPassword   (GBL_SELF, const char* pStr)   GBL_NOEXCEPT;
GBL_EXPORT void          GblUri_movePassword  (GBL_SELF, GblStringRef* pRef) GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblUri_password      (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT void          GblUri_setHost       (GBL_SELF, const char* pStr)   GBL_NOEXCEPT;
GBL_EXPORT void          GblUri_moveHost      (GBL_SELF, GblStringRef* pRef) GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblUri_host          (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT void          GblUri_setPort       (GBL_SELF, int port)           GBL_NOEXCEPT;
GBL_EXPORT int           GblUri_port          (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT void          GblUri_setPath       (GBL_SELF, const char* pStr)   GBL_NOEXCEPT;
GBL_EXPORT void          GblUri_movePath      (GBL_SELF, GblStringRef* pRef) GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblUri_path          (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT GblStringView GblUri_fileName      (GBL_CSELF)                    GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblUri_extension     (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT void          GblUri_setQuery      (GBL_SELF, const char* pStr)   GBL_NOEXCEPT;
GBL_EXPORT void          GblUri_moveQuery     (GBL_SELF, GblStringRef* pRef) GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblUri_query         (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT void          GblUri_setFragment   (GBL_SELF, const char* pStr)   GBL_NOEXCEPT;
GBL_EXPORT void          GblUri_moveFragment  (GBL_SELF, GblStringRef* pRef) GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblUri_fragment      (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblUri_isRelative    (GBL_CSELF)                    GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblUri_isParentOf    (GBL_CSELF,
                                               const GblUri* pUri)           GBL_NOEXCEPT;

GBL_EXPORT int           GblUri_compare       (GBL_CSELF,
                                               const GblUri* pUri)           GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblUri_matches       (GBL_CSELF,
                                               const GblUri* pOther,
                                               GBL_URI_FLAGS flags)          GBL_NOEXCEPT;
GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_URI_H
