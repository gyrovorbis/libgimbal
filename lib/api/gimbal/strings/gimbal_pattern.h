/*! \file
 *  \brief GblPattern RegExp-style pattern matching
 *  \ingroup strings
 */

#ifndef GIMBAL_PATTERN_H
#define GIMBAL_PATTERN_H

#include "../core/gimbal_ctx.h"

#define GBL_SELF_TYPE GblPattern

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblPattern);
GBL_FORWARD_DECLARE_STRUCT(GblStringView);

// ===== Public API =====
GBL_EXPORT const GblPattern*
                   GblPattern_compile       (const char* pRegExp)            GBL_NOEXCEPT;

GBL_EXPORT GblBool GblPattern_match         (GBL_CSELF,
                                             const char*    pString,
                                             GblStringView* pMatch/*=NULL*/,
                                             int*           pCount/*=NULL*/) GBL_NOEXCEPT;

GBL_EXPORT GblBool GblPattern_matchStr      (const char*    pRegExp,
                                             const char*    pString,
                                             GblStringView* pMatch/*=NULL*/,
                                             int*           pCount/*=NULL*/) GBL_NOEXCEPT;

GBL_EXPORT GblBool GblPattern_matchNot      (GBL_CSELF,
                                             const char*    pString,
                                             GblStringView* pMatch/*=NULL*/,
                                             int*           pCount/*=NULL*/) GBL_NOEXCEPT;

GBL_EXPORT GblBool GblPattern_matchNotStr   (const char*    pRegExp,
                                             const char*    pString,
                                             GblStringView* pMatch/*=NULL*/,
                                             int*           pCount/*=NULL*/) GBL_NOEXCEPT;

GBL_EXPORT GblBool GblPattern_matchExact    (GBL_CSELF, const char* pString) GBL_NOEXCEPT;

GBL_EXPORT GblBool GblPattern_matchExactStr (const char* pRegExp,
                                             const char* pString)            GBL_NOEXCEPT;

GBL_EXPORT size_t  GblPattern_matchCount    (GBL_CSELF, const char* pString) GBL_NOEXCEPT;

GBL_EXPORT size_t  GblPattern_matchCountStr (const char* pRegExp,
                                             const char* pString)            GBL_NOEXCEPT;

// ===== Default Argument Wrapper Macros ====-
#define GblPattern_match(...)               (GblPattern_matchDefault_(__VA_ARGS__))
#define GblPattern_matchStr(...)            (GblPattern_matchStrDefault_(__VA_ARGS__))
#define GblPattern_matchNot(...)            (GblPattern_matchNotDefault_(__VA_ARGS__))
#define GblPattern_matchNotStr(...)         (GblPattern_matchNotStrDefault_(__VA_ARGS__))

// ===== Implementation =====
///\cond

#define GblPattern_matchDefault_(...) \
    (GblPattern_matchDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL))
#define GblPattern_matchDefault__(pat, str, match, count, ...) \
    ((GblPattern_match)(pat, str, match, count))

#define GblPattern_matchStrDefault_(...) \
    (GblPattern_matchStrDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL))
#define GblPattern_matchStrDefault__(exp, str, match, count, ...) \
    ((GblPattern_matchStr)(exp, str, match, count))

#define GblPattern_matchNotDefault_(...) \
    (GblPattern_matchNotDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL))
#define GblPattern_matchNotDefault__(pat, str, match, count, ...) \
    ((GblPattern_matchNot)(pat, str, match, count))

#define GblPattern_matchNotStrDefault_(...) \
    (GblPattern_matchNotStrDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL))
#define GblPattern_matchNotStrDefault__(exp, str, match, count, ...) \
    ((GblPattern_matchNotStr)(exp, str, match, count))

///\endcond

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_PATTERN_H
