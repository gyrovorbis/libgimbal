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

// ===== Public API =====

GBL_EXPORT GblPattern* GblPattern_compile     (const char* pRegExp)       GBL_NOEXCEPT;

GBL_EXPORT int         GblPattern_firstMatchc (GBL_CSELF,
                                               const char* pString,
                                               int*        pLen/*=NULL*/) GBL_NOEXCEPT;

GBL_EXPORT int         GblPattern_firstMatch  (const char* pRegExp,
                                               const char* pText,
                                               int*        pLen/*=NULL*/) GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblPattern_exactMatchc (GBL_CSELF,
                                               const char* pString)       GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblPattern_exactMatch  (const char* pRegExp,
                                               const char* pString)       GBL_NOEXCEPT;
GBL_DECLS_END

// ===== Implementation =====

#define GblPattern_firstMatchc(...) \
    (GblPattern_firstMatchc_(__VA_ARGS__, GBL_NULL))
#define GblPattern_firstMatchc_(pat, self, len, ...) \
    ((GblPattern_firstMatchc)(pat, self, len))

#define GblPattern_firstMatch(...) \
    (GblPattern_firstMatch_(__VA_ARGS__, GBL_NULL))
#define GblPattern_firstMatch_(pat, self, len, ...) \
((GblPattern_firstMatch)(pat, self, len))

#undef GBL_SELF_TYPE

#endif // GIMBAL_PATTERN_H
