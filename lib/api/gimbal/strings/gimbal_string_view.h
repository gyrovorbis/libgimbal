/*! \file
 *  \brief GblStringView structure and related functions
 *  \ingroup strings
 *
 *  This file contains the GblStringView structure and related
 *  API. A GblStringView is an immutable slice of a string,
 *  containing simply a pointer to the string and its size.
 *  When doing non-mutating operations on strings, such as
 *  searching, this is the preferred string type for the job
 *  with the most detailed API.
 *
 *  \note
 *  GblStringView also reserves one bit of its length field to
 *  store whether its NULL-terminated or not. This allows for
 *  more efficient conversions between string_view types and
 *  NULL-terminated C strings, as a copy can be ellided when
 *  the view is already of a NULL-terminated C string.
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */

#ifndef GIMBAL_STRING_VIEW_H
#define GIMBAL_STRING_VIEW_H

#include "../core/gimbal_ctx.h"
#include "../algorithms/gimbal_hash.h"
#include "../preprocessor/gimbal_compiler.h"
#include "../preprocessor/gimbal_macro_composition.h"
#include "gimbal_quark.h"
#include <ctype.h>
#include <stdint.h>
#include <limits.h>

#define GBL_STRING_VIEW_NPOS                GBL_NPOS
#define GBL_STRV(...)                       GblStringView_fromString(__VA_ARGS__)
#define GBL_STRING_VIEW_CSTR(view)          ((view).nullTerminated? (view).pData : GBL_STRING_VIEW_CSTR_ALLOCA((view)))
#define GBL_STRING_VIEW_CSTR_ALLOCA(view)   GblStringView_toCString(view, (char*)GBL_ALLOCA(view.length + 1), view.length + 1)

#define GBL_SELF_TYPE GblStringView

GBL_DECLS_BEGIN

/*! \brief Immutable substring type
 *  \ingroup strings
 */
typedef struct GblStringView {
    const char* pData;
    size_t      nullTerminated  : 1;
#ifdef GBL_64BIT
    size_t      length          : 63;
#elif defined(GBL_32BIT)
    size_t      length          : 31;
#endif
} GblStringView;

GBL_EXPORT GblStringView GblStringView_fromEmpty            (void)                                                                        GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringView_fromString           (const char* pString, size_t length/*=0*/)                                    GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringView_fromQuark            (GblQuark quark)                                                              GBL_NOEXCEPT;
GBL_EXPORT int           GblStringView_compare              (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT int           GblStringView_compareIgnoreCase    (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_equals               (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_equalsIgnoreCase     (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblStringView_copy                 (GBL_VSELF, void* pDst, size_t offset, size_t bytes)                          GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_empty                (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_blank                (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT char          GblStringView_at                   (GBL_VSELF, size_t index)                                                     GBL_NOEXCEPT;
GBL_EXPORT char          GblStringView_first                (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT char          GblStringView_last                 (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringView_removePrefix         (GBL_VSELF, size_t length)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringView_removeSuffix         (GBL_VSELF, size_t length)                                                    GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringView_chomp                (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringView_substr               (GBL_VSELF, size_t offset, size_t length)                                     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_contains             (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_containsIgnoreCase   (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_count                (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_countIgnoreCase      (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_find                 (GBL_VSELF, const char* pStr, size_t len/*=0*/, size_t offset/*=0*/)          GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_findIgnoreCase       (GBL_VSELF, const char* pStr, size_t len/*=0*/, size_t offset/*=0*/)          GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_rfind                (GBL_VSELF, const char* pStr, size_t len/*=0*/, size_t offset/*=GBL_NPOS*/)   GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_rfindIgnoreCase      (GBL_VSELF, const char* pStr, size_t len/*=0*/, size_t offset/*=GBL_NPOS*/)   GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_startsWith           (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_startsWithIgnoreCase (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_endsWith             (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_endsWithIgnoreCase   (GBL_VSELF, const char* pString, size_t len/*=0*/)                            GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_findFirstOf          (GBL_VSELF, const char* pChars, size_t len/*=0*/, size_t offset/*=0*/)        GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_findLastOf           (GBL_VSELF, const char* pChars, size_t len/*=0*/, size_t offset/*=GBL_NPOS*/) GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_findFirstNotOf       (GBL_VSELF, const char* pChars, size_t len/*=0*/, size_t offset/*=0*/)        GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_findLastNotOf        (GBL_VSELF, const char* pChars, size_t len/*=0*/, size_t offset/*=GBL_NPOS*/) GBL_NOEXCEPT;
GBL_EXPORT GblQuark      GblStringView_quark                (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT GblQuark      GblStringView_tryQuark             (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT const char*   GblStringView_intern               (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT GblHash       GblStringView_hash                 (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT char*         GblStringView_strdup               (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT int           GblStringView_scanf                (GBL_VSELF, const char* pFmt, ...)                                            GBL_NOEXCEPT;
GBL_EXPORT int           GblStringView_scanfVa              (GBL_VSELF, const char* pFmt, va_list* pVarArgs)                              GBL_NOEXCEPT;
GBL_EXPORT char*         GblStringView_toCString            (GBL_VSELF, char* pDest, size_t destSize)                                     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_toNil                (GBL_VSELF)                                                                   GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_toBool               (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT char          GblStringView_toChar               (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT uint8_t       GblStringView_toUint8              (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT uint16_t      GblStringView_toUint16             (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT int16_t       GblStringView_toInt16              (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT uint32_t      GblStringView_toUint32             (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT int32_t       GblStringView_toInt32              (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT uint64_t      GblStringView_toUint64             (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT int64_t       GblStringView_toInt64              (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT float         GblStringView_toFloat              (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT double        GblStringView_toDouble             (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;
GBL_EXPORT void*         GblStringView_toPointer            (GBL_VSELF, GblBool* pSuccess)                                                GBL_NOEXCEPT;

GBL_DECLS_END

//! \cond
#define GblStringView_fromString(...) \
    GblStringView_fromStringDefault_(__VA_ARGS__)
#define GblStringView_fromStringDefault_(...) \
    GblStringView_fromStringDefault__(__VA_ARGS__, 0)
#define GblStringView_fromStringDefault__(string, length, ...) \
    (GblStringView_fromString)(string, length)

#define GblStringView_compare(...) \
    GblStringView_compareDefault_(__VA_ARGS__)
#define GblStringView_compareDefault_(...) \
    GblStringView_compareDefault__(__VA_ARGS__, 0)
#define GblStringView_compareDefault__(view, string, length, ...) \
    (GblStringView_compare)(view, string, length)

#define GblStringView_compareIgnoreCase(...) \
    GblStringView_compareIgnoreCaseDefault_(__VA_ARGS__)
#define GblStringView_compareIgnoreCaseDefault_(...) \
    GblStringView_compareIgnoreCaseDefault__(__VA_ARGS__, 0)
#define GblStringView_compareIgnoreCaseDefault__(view, string, length, ...) \
    (GblStringView_compareIgnoreCase)(view, string, length)

#define GblStringView_equals(...) \
    GblStringView_equalsDefault_(__VA_ARGS__)
#define GblStringView_equalsDefault_(...) \
    GblStringView_equalsDefault__(__VA_ARGS__, 0)
#define GblStringView_equalsDefault__(view, string, length, ...) \
    (GblStringView_equals)(view, string, length)

#define GblStringView_equalsIgnoreCase(...) \
    GblStringView_equalsIgnoreCaseDefault_(__VA_ARGS__)
#define GblStringView_equalsIgnoreCaseDefault_(...) \
    GblStringView_equalsIgnoreCaseDefault__(__VA_ARGS__, 0)
#define GblStringView_equalsIgnoreCaseDefault__(view, string, length, ...) \
    (GblStringView_equalsIgnoreCase)(view, string, length)

#define GblStringView_contains(...) \
    GblStringView_containsDefault_(__VA_ARGS__)
#define GblStringView_containsDefault_(...) \
    GblStringView_containsDefault__(__VA_ARGS__, 0)
#define GblStringView_containsDefault__(view, string, length, ...) \
    (GblStringView_contains)(view, string, length)

#define GblStringView_containsIgnoreCase(...) \
    GblStringView_containsIgnoreCaseDefault_(__VA_ARGS__)
#define GblStringView_containsIgnoreCaseDefault_(...) \
    GblStringView_containsIgnoreCaseDefault__(__VA_ARGS__, 0)
#define GblStringView_containsIgnoreCaseDefault__(view, string, length, ...) \
    (GblStringView_containsIgnoreCase)(view, string, length)

#define GblStringView_count(...) \
    GblStringView_countDefault_(__VA_ARGS__)
#define GblStringView_countDefault_(...) \
    GblStringView_countDefault__(__VA_ARGS__, 0)
#define GblStringView_countDefault__(view, string, length, ...) \
    (GblStringView_count)(view, string, length)

#define GblStringView_startsWith(...) \
    GblStringView_startsWithDefault_(__VA_ARGS__)
#define GblStringView_startsWithDefault_(...) \
    GblStringView_startsWithDefault__(__VA_ARGS__, 0)
#define GblStringView_startsWithDefault__(view, string, length, ...) \
    (GblStringView_startsWith)(view, string, length)

#define GblStringView_startsWithIgnoreCase(...) \
    GblStringView_startsWithIgnoreCaseDefault_(__VA_ARGS__)
#define GblStringView_startsWithIgnoreCaseDefault_(...) \
    GblStringView_startsWithIgnoreCaseDefault__(__VA_ARGS__, 0)
#define GblStringView_startsWithIgnoreCaseDefault__(view, string, length, ...) \
    (GblStringView_startsWithIgnoreCase)(view, string, length)

#define GblStringView_endsWith(...) \
    GblStringView_endsWithDefault_(__VA_ARGS__)
#define GblStringView_endsWithDefault_(...) \
    GblStringView_endsWithDefault__(__VA_ARGS__, 0)
#define GblStringView_endsWithDefault__(view, string, length, ...) \
    (GblStringView_endsWith)(view, string, length)

#define GblStringView_endsWithIgnoreCase(...) \
    GblStringView_endsWithIgnoreCaseDefault_(__VA_ARGS__)
#define GblStringView_endsWithIgnoreCaseDefault_(...) \
    GblStringView_endsWithIgnoreCaseDefault__(__VA_ARGS__, 0)
#define GblStringView_endsWithIgnoreCaseDefault__(view, string, length, ...) \
    (GblStringView_endsWithIgnoreCase)(view, string, length)

#define GblStringView_find(...) \
    GblStringView_findDefault_(__VA_ARGS__)
#define GblStringView_findDefault_(...) \
    GblStringView_findDefault__(__VA_ARGS__, 0, 0)
#define GblStringView_findDefault__(view, string, length, offset, ...) \
    (GblStringView_find)(view, string, length, offset)

#define GblStringView_findIgnoreCase(...) \
    GblStringView_findIgnoreCaseDefault_(__VA_ARGS__)
#define GblStringView_findIgnoreCaseDefault_(...) \
    GblStringView_findIgnoreCaseDefault__(__VA_ARGS__, 0, 0)
#define GblStringView_findIgnoreCaseDefault__(view, string, length, offset, ...) \
    (GblStringView_findIgnoreCase)(view, string, length, offset)

#define GblStringView_rfind(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringView_rfind, __VA_ARGS__)
#define GblStringView_rfind_2(view, str) \
    GblStringView_rfind_3(view, str, 0)
#define GblStringView_rfind_3(view, str, len) \
    GblStringView_rfind_4(view, str, len, GBL_STRING_VIEW_NPOS)
#define GblStringView_rfind_4(view, str, len, offset) \
    (GblStringView_rfind)(view, str, len, offset)

#define GblStringView_rfindIgnoreCase(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringView_rfindIgnoreCase, __VA_ARGS__)
#define GblStringView_rfindIgnoreCase_2(view, str) \
    GblStringView_rfindIgnoreCase_3(view, str, 0)
#define GblStringView_rfindIgnoreCase_3(view, str, len) \
    GblStringView_rfindIgnoreCase_4(view, str, len, GBL_STRING_VIEW_NPOS)
#define GblStringView_rfindIgnoreCase_4(view, str, len, offset) \
    (GblStringView_rfindIgnoreCase)(view, str, len, offset)

#define GblStringView_findFirstOf(...) \
    GblStringView_findFirstOfDefault_(__VA_ARGS__)
#define GblStringView_findFirstOfDefault_(...) \
    GblStringView_findFirstOfDefault__(__VA_ARGS__, 0, 0)
#define GblStringView_findFirstOfDefault__(view, string, length, offset, ...) \
    (GblStringView_findFirstOf)(view, string, length, offset)

#define GblStringView_findFirstNotOf(...) \
    GblStringView_findFirstNotOfDefault_(__VA_ARGS__)
#define GblStringView_findFirstNotOfDefault_(...) \
    GblStringView_findFirstNotOfDefault__(__VA_ARGS__, 0, 0)
#define GblStringView_findFirstNotOfDefault__(view, string, length, offset, ...) \
    (GblStringView_findFirstNotOf)(view, string, length, offset)

#define GblStringView_findLastOf(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringView_findLastOf, __VA_ARGS__)
#define GblStringView_findLastOf_2(view, str) \
    GblStringView_findLastOf_3(view, str, 0)
#define GblStringView_findLastOf_3(view, str, len) \
    GblStringView_findLastOf_4(view, str, len, GBL_STRING_VIEW_NPOS)
#define GblStringView_findLastOf_4(view, str, len, offset) \
    (GblStringView_findLastOf)(view, str, len, offset)

#define GblStringView_findLastNotOf(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringView_findLastNotOf, __VA_ARGS__)
#define GblStringView_findLastNotOf_2(view, str) \
    GblStringView_findLastNotOf_3(view, str, 0)
#define GblStringView_findLastNotOf_3(view, str, len) \
    GblStringView_findLastNotOf_4(view, str, len, GBL_STRING_VIEW_NPOS)
#define GblStringView_findLastNotOf_4(view, str, len, offset) \
    (GblStringView_findLastNotOf)(view, str, len, offset)

#define GblStringView_toBool(...) \
    GblStringView_toBoolDefault_(__VA_ARGS__)
#define GblStringView_toBoolDefault_(...) \
    GblStringView_toBoolDefault__(__VA_ARGS__, NULL)
#define GblStringView_toBoolDefault__(self, success, ...) \
    ((GblStringView_toBool)(self, success))

#define GblStringView_toChar(...) \
    GblStringView_toCharDefault_(__VA_ARGS__)
#define GblStringView_toCharDefault_(...) \
    GblStringView_toCharDefault__(__VA_ARGS__, NULL)
#define GblStringView_toCharDefault__(self, success, ...) \
    ((GblStringView_toChar)(self, success))

#define GblStringView_toUint8(...) \
    GblStringView_toUint8Default_(__VA_ARGS__)
#define GblStringView_toUint8Default_(...) \
    GblStringView_toUint8Default__(__VA_ARGS__, NULL)
#define GblStringView_toUint8Default__(self, success, ...) \
    ((GblStringView_toUint8)(self, success))

#define GblStringView_toUint16(...) \
    GblStringView_toUint16Default_(__VA_ARGS__)
#define GblStringView_toUint16Default_(...) \
    GblStringView_toUint16Default__(__VA_ARGS__, NULL)
#define GblStringView_toUint16Default__(self, success, ...) \
    ((GblStringView_toUint16)(self, success))

#define GblStringView_toInt16(...) \
    GblStringView_toInt16Default_(__VA_ARGS__)
#define GblStringView_toInt16Default_(...) \
    GblStringView_toInt16Default__(__VA_ARGS__, NULL)
#define GblStringView_toInt16Default__(self, success, ...) \
    ((GblStringView_toInt16)(self, success))

#define GblStringView_toUint32(...) \
    GblStringView_toUint32Default_(__VA_ARGS__)
#define GblStringView_toUint32Default_(...) \
    GblStringView_toUint32Default__(__VA_ARGS__, NULL)
#define GblStringView_toUint32Default__(self, success, ...) \
    ((GblStringView_toUint32)(self, success))

#define GblStringView_toInt32(...) \
    GblStringView_toInt32Default_(__VA_ARGS__)
#define GblStringView_toInt32Default_(...) \
    GblStringView_toInt32Default__(__VA_ARGS__, NULL)
#define GblStringView_toInt32Default__(self, success, ...) \
    ((GblStringView_toInt32)(self, success))

#define GblStringView_toUint64(...) \
    GblStringView_toUint64Default_(__VA_ARGS__)
#define GblStringView_toUint64Default_(...) \
    GblStringView_toUint64Default__(__VA_ARGS__, NULL)
#define GblStringView_toUint64Default__(self, success, ...) \
    ((GblStringView_toUint64)(self, success))

#define GblStringView_toInt64(...) \
    GblStringView_toInt64Default_(__VA_ARGS__)
#define GblStringView_toInt64Default_(...) \
    GblStringView_toInt64Default__(__VA_ARGS__, NULL)
#define GblStringView_toInt64Default__(self, success, ...) \
    ((GblStringView_toInt64)(self, success))

#define GblStringView_toFloat(...) \
    GblStringView_toFloatDefault_(__VA_ARGS__)
#define GblStringView_toFloatDefault_(...) \
    GblStringView_toFloatDefault__(__VA_ARGS__, NULL)
#define GblStringView_toFloatDefault__(self, success, ...) \
    ((GblStringView_toFloat)(self, success))

#define GblStringView_toDouble(...) \
    GblStringView_toDoubleDefault_(__VA_ARGS__)
#define GblStringView_toDoubleDefault_(...) \
    GblStringView_toDoubleDefault__(__VA_ARGS__, NULL)
#define GblStringView_toDoubleDefault__(self, success, ...) \
    ((GblStringView_toDouble)(self, success))

#define GblStringView_toPointer(...) \
    GblStringView_toPointerDefault_(__VA_ARGS__)
#define GblStringView_toPointerDefault_(...) \
    GblStringView_toPointerDefault__(__VA_ARGS__, NULL)
#define GblStringView_toPointerDefault__(self, success, ...) \
    ((GblStringView_toPointer)(self, success))
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_STRING_VIEW_H
