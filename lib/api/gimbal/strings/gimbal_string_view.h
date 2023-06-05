/*! \file
 *  \brief GblStringView structure and related functions
 *  \ingroup strings
 *
 *  \todo
 *  Move larger inline functions to source file
 *
 *  \author Falco Girgis
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

#define GBL_STRING_VIEW_NPOS            GBL_NPOS
#define GBL_STRING_VIEW(...)            GBL_VA_OVERLOAD_SELECT(GBL_STRING_VIEW, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)
#define GBL_STRV(...)                   GBL_STRING_VIEW(__VA_ARGS__)
#define GBL_STRING_VIEW_CSTR(view)      ((view).nullTerminated? (view).pData : GBL_STRING_VIEW_CSTR_ALLOCA((view)))

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

GBL_INLINE GblStringView GblStringView_fromEmpty         (void)                                               GBL_NOEXCEPT;
GBL_INLINE GblStringView GblStringView_fromString        (const char* pString)                                GBL_NOEXCEPT;
GBL_INLINE GblStringView GblStringView_fromStringSized   (const char* pData, size_t length)                   GBL_NOEXCEPT;
GBL_INLINE GblStringView GblStringView_fromQuark         (GblQuark quark)                                     GBL_NOEXCEPT;
GBL_EXPORT int           GblStringView_compare           (GBL_VSELF, GblStringView other)                     GBL_NOEXCEPT;
GBL_EXPORT int           GblStringView_compareIgnoreCase (GBL_VSELF, GblStringView other)                     GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_equals            (GBL_VSELF, GblStringView other)                     GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_equalsIgnoreCase  (GBL_VSELF, GblStringView other)                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblStringView_copy              (GBL_VSELF, void* pDst, size_t offset, size_t bytes) GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_empty             (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_blank             (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_INLINE char          GblStringView_at                (GBL_VSELF, size_t index)                            GBL_NOEXCEPT;
GBL_EXPORT char          GblStringView_first             (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_EXPORT char          GblStringView_last              (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringView_removePrefix      (GBL_VSELF, size_t length)                           GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringView_removeSuffix      (GBL_VSELF, size_t length)                           GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringView_chomp             (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringView_substr            (GBL_VSELF, size_t offset, size_t length)            GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_contains          (GBL_VSELF, GblStringView subStr)                    GBL_NOEXCEPT;
//GBL_INLINE GblBool     GblStringView_containsIgnoreCase(GBL_VSELF, GblStringView substr)                    GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_count             (GBL_VSELF, GblStringView substr)                    GBL_NOEXCEPT;
//GBL_INLINE size_t      GblStringView_countIgnoreCase   (GBL_VSELF, GblStringView substr)                    GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_find              (GBL_VSELF, GblStringView substr, size_t offset)     GBL_NOEXCEPT;
//GBL_INILNE size_t      GblStringView_findIgnoreCase    (GBL_VSELF, GblStringView substr, size_t offset)     GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_rfind             (GBL_VSELF, GblStringView substr, size_t offset)     GBL_NOEXCEPT;
//GBL_INLINE size_t      GblStringView_rfindIgnoreCase   (GBL_VSELF, GblStringView substr, size_t offset)     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_startsWith        (GBL_VSELF, GblStringView substr)                    GBL_NOEXCEPT;
//GBL_INLINE GblBool     GblStringView_startsWithIgnoreCase(GBL_VSELF, GblStringView substr)                  GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_endsWith          (GBL_VSELF, GblStringView substr)                    GBL_NOEXCEPT;
//GBL_INLINE GblBool     GblStringView_endsWithIgnoreCase(GBL_VSELF, GblStringView substr)                    GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_findFirstOf       (GBL_VSELF, GblStringView chars, size_t offset)      GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_findLastOf        (GBL_VSELF, GblStringView chars, size_t end)         GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_findFirstNotOf    (GBL_VSELF, GblStringView chars, size_t offset)      GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringView_findLastNotOf     (GBL_VSELF, GblStringView chars, size_t end)         GBL_NOEXCEPT;
GBL_INLINE GblQuark      GblStringView_quark             (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_INLINE GblQuark      GblStringView_quarkTry          (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_INLINE const char*   GblStringView_intern            (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_INLINE GblHash       GblStringView_hash              (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_EXPORT char*         GblStringView_strdup            (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_EXPORT char*         GblStringView_toCString         (GBL_VSELF, char* pDest, size_t destSize)            GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_toNil             (GBL_VSELF)                                          GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringView_toBool            (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;
GBL_EXPORT uint8_t       GblStringView_toUint8           (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;
GBL_EXPORT uint16_t      GblStringView_toUint16          (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;
GBL_EXPORT int16_t       GblStringView_toInt16           (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;
GBL_EXPORT uint32_t      GblStringView_toUint32          (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;
GBL_EXPORT int32_t       GblStringView_toInt32           (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;
GBL_EXPORT uint64_t      GblStringView_toUint64          (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;
GBL_EXPORT int64_t       GblStringView_toInt64           (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;
GBL_EXPORT float         GblStringView_toFloat           (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;
GBL_EXPORT double        GblStringView_toDouble          (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;
GBL_EXPORT void*         GblStringView_toPointer         (GBL_VSELF, GblBool* pSuccess)                       GBL_NOEXCEPT;

#define                  GblStringView_toBool(...)       GblStringView_toBoolDefault_(__VA_ARGS__)
#define                  GblStringView_toUint8(...)      GblStringView_toUint8Default_(__VA_ARGS__)
#define                  GblStringView_toUint16(...)     GblStringView_toUint16Default_(__VA_ARGS__)
#define                  GblStringView_toInt16(...)      GblStringView_toInt16Default_(__VA_ARGS__)
#define                  GblStringView_toUint32(...)     GblStringView_toUint32Default_(__VA_ARGS__)
#define                  GblStringView_toInt32(...)      GblStringView_toInt32Default_(__VA_ARGS__)
#define                  GblStringView_toUint64(...)     GblStringView_toUint64Default_(__VA_ARGS__)
#define                  GblStringView_toInt64(...)      GblStringView_toInt64Default_(__VA_ARGS__)
#define                  GblStringView_toFloat(...)      GblStringView_toFloatDefault_(__VA_ARGS__)
#define                  GblStringView_toDouble(...)     GblStringView_toDoubleDefault_(__VA_ARGS__)
#define                  GblStringView_toPointer(...)    GblStringView_toPointerDefault_(__VA_ARGS__)

//========== IMPL ==========
///\cond
#define GBL_STRING_VIEW_2(data, length)     GblStringView_fromStringSized(data, length)
#define GBL_STRING_VIEW_1(cstring)          GblStringView_fromString(cstring)
#define GBL_STRING_VIEW_CSTR_ALLOCA(view)   GblStringView_toCString(view, (char*)GBL_ALLOCA(view.length+1), view.length+1)

#define GblStringView_toBoolDefault_(...) \
    GblStringView_toBoolDefault__(__VA_ARGS__, NULL)
#define GblStringView_toBoolDefault__(self, success, ...) \
    ((GblStringView_toBool)(self, success))

#define GblStringView_toUint8Default_(...) \
    GblStringView_toUint8Default__(__VA_ARGS__, NULL)
#define GblStringView_toUint8Default__(self, success, ...) \
    ((GblStringView_toUint8)(self, success))

#define GblStringView_toUint16Default_(...) \
    GblStringView_toUint16Default__(__VA_ARGS__, NULL)
#define GblStringView_toUint16Default__(self, success, ...) \
    ((GblStringView_toUint16)(self, success))
#define GblStringView_toInt16Default_(...) \
    GblStringView_toInt16Default__(__VA_ARGS__, NULL)
#define GblStringView_toInt16Default__(self, success, ...) \
    ((GblStringView_toInt16)(self, success))

#define GblStringView_toUint32Default_(...) \
    GblStringView_toUint32Default__(__VA_ARGS__, NULL)
#define GblStringView_toUint32Default__(self, success, ...) \
    ((GblStringView_toUint32)(self, success))
#define GblStringView_toInt32Default_(...) \
    GblStringView_toInt32Default__(__VA_ARGS__, NULL)
#define GblStringView_toInt32Default__(self, success, ...) \
    ((GblStringView_toInt32)(self, success))

#define GblStringView_toUint64Default_(...) \
    GblStringView_toUint64Default__(__VA_ARGS__, NULL)
#define GblStringView_toUint64Default__(self, success, ...) \
    ((GblStringView_toUint64)(self, success))
#define GblStringView_toInt64Default_(...) \
    GblStringView_toInt64Default__(__VA_ARGS__, NULL)
#define GblStringView_toInt64Default__(self, success, ...) \
    ((GblStringView_toInt64)(self, success))

#define GblStringView_toFloatDefault_(...) \
    GblStringView_toFloatDefault__(__VA_ARGS__, NULL)
#define GblStringView_toFloatDefault__(self, success, ...) \
    ((GblStringView_toFloat)(self, success))

#define GblStringView_toDoubleDefault_(...) \
    GblStringView_toDoubleDefault__(__VA_ARGS__, NULL)
#define GblStringView_toDoubleDefault__(self, success, ...) \
    ((GblStringView_toDouble)(self, success))

#define GblStringView_toPointerDefault_(...) \
    GblStringView_toPointerDefault__(__VA_ARGS__, NULL)
#define GblStringView_toPointerDefault__(self, success, ...) \
    ((GblStringView_toPointer)(self, success))

///\endcond

GBL_INLINE GblStringView GblStringView_fromEmpty(void) GBL_NOEXCEPT {
    return GblStringView_fromString(GBL_NULL);
}

GBL_INLINE GblStringView GblStringView_fromString(const char* pString) GBL_NOEXCEPT {
    GblStringView view = {
        .pData  = pString
    };
    if(pString) {
        view.nullTerminated = 1;
        view.length         = strlen(pString);
    }
    return view;
}

GBL_INLINE GblStringView GblStringView_fromStringSized(const char* pString, size_t length) GBL_NOEXCEPT {
    GblStringView view = {
        .pData  = pString,
        .length = length
    };
    if(pString) {
        if(!length) {
            view.length = strlen(pString);
            view.nullTerminated = 1;
        } else if(pString[length-1] == '\0') {
            view.length--;
            view.nullTerminated = 1;
        }
    }
    return view;
}

GBL_INLINE char GblStringView_at(GblStringView self, size_t index) GBL_NOEXCEPT {
    char value = '\0';
    if(index >= self.length) GBL_UNLIKELY {
            GBL_CTX_BEGIN(GBL_NULL);
            GBL_CTX_VERIFY(GBL_FALSE,
                           GBL_RESULT_ERROR_OUT_OF_RANGE);
            GBL_CTX_END_BLOCK();
        } else GBL_LIKELY {
            value = self.pData[index];
        }
    return value;
}

GBL_INLINE GblStringView GblStringView_fromQuark(GblQuark quark) GBL_NOEXCEPT {
    return GblStringView_fromString(GblQuark_toString(quark));
}

GBL_INLINE GblBool GblStringView_equals(GBL_VSELF, GblStringView other) GBL_NOEXCEPT {
    return GblStringView_compare(self, other) == 0;
}

GBL_INLINE GblBool GblStringView_equalsIgnoreCase(GBL_VSELF, GblStringView other) GBL_NOEXCEPT {
    return GblStringView_compareIgnoreCase(self, other) == 0;
}

GBL_INLINE GblBool GblStringView_empty(GBL_VSELF) GBL_NOEXCEPT {
    return self.length == 0;
}

GBL_INLINE GblBool GblStringView_blank(GBL_VSELF) GBL_NOEXCEPT {
    if(GblStringView_empty(self)) return GBL_TRUE;
    else return GblStringView_findFirstNotOf(self, GBL_STRING_VIEW(" \t\n\r"), 0) == GBL_STRING_VIEW_NPOS;
}

GBL_INLINE GblQuark GblStringView_quark(GBL_VSELF) GBL_NOEXCEPT {
    return GblQuark_fromString(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE GblQuark GblStringView_quarkTry(GBL_VSELF) GBL_NOEXCEPT {
    return GblQuark_tryString(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE const char* GblStringView_intern(GBL_VSELF) GBL_NOEXCEPT {
    return GblQuark_internString(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE GblHash GblStringView_hash(GBL_VSELF) GBL_NOEXCEPT {
    return self.length? gblHash(self.pData, self.length) : 0;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_STRING_VIEW_H
