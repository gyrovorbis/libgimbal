/*! \file
 *  \brief GblStringView structure and related functions
 *  \ingroup strings
 */

#ifndef GIMBAL_STRING_VIEW_H
#define GIMBAL_STRING_VIEW_H

#include "../core/gimbal_ctx.h"
#include "../algorithms/gimbal_hash.h"
#include "../preprocessor/gimbal_compiler.h"
#include "gimbal_quark.h"
#include <ctype.h>
#include <stdint.h>
#include <limits.h>

#define GBL_STRING_VIEW_NPOS                GBL_NPOS
#define GBL_STRING_VIEW_2(data, length)     GblStringView_fromStringSized(data, length)
#define GBL_STRING_VIEW_1(cstring)          GblStringView_fromString(cstring)
#define GBL_STRING_VIEW(...)                GBL_VA_OVERLOAD_SELECT(GBL_STRING_VIEW, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)
#define GBL_STRV(...)                       GBL_STRING_VIEW(__VA_ARGS__)
#define GBL_STRING_VIEW_CSTR_ALLOCA(view)   GblStringView_toCString(view, (char*)GBL_ALLOCA(view.length+1), view.length+1)
#define GBL_STRING_VIEW_CSTR(view)          (view.nullTerminated? view.pData : GBL_STRING_VIEW_CSTR_ALLOCA(view))

#define GBL_SELF_TYPE GblStringView

GBL_DECLS_BEGIN

/*! \brief Immutable substring type
 *  \ingroup strings
 */
typedef struct GblStringView {
    const char* pData;
    GblSize     nullTerminated  : 1;
#ifdef GBL_64BIT
    GblSize     length          : 63;
#elif defined(GBL_32BIT)
    GblSize     length          : 31;
#endif
} GblStringView;

GBL_INLINE GblStringView GblStringView_fromEmpty         (void)                                                 GBL_NOEXCEPT;
GBL_INLINE GblStringView GblStringView_fromString        (const char* pString)                                  GBL_NOEXCEPT;
GBL_INLINE GblStringView GblStringView_fromStringSized   (const char* pData, GblSize length)                    GBL_NOEXCEPT;
GBL_INLINE GblStringView GblStringView_fromQuark         (GblQuark quark)                                       GBL_NOEXCEPT;
GBL_INLINE int           GblStringView_compare           (GBL_VSELF, GblStringView other)                       GBL_NOEXCEPT;
GBL_INLINE int           GblStringView_compareIgnoreCase (GBL_VSELF, GblStringView other)                       GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_equals            (GBL_VSELF, GblStringView other)                       GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_equalsIgnoreCase  (GBL_VSELF, GblStringView other)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblStringView_copy              (GBL_VSELF, void* pDst, GblSize offset, GblSize bytes) GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_empty             (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_blank             (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE char          GblStringView_at                (GBL_VSELF, GblSize index)                             GBL_NOEXCEPT;
GBL_INLINE char          GblStringView_first             (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE char          GblStringView_last              (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE GblStringView GblStringView_removePrefix      (GBL_VSELF, GblSize length)                            GBL_NOEXCEPT;
GBL_INLINE GblStringView GblStringView_removeSuffix      (GBL_VSELF, GblSize length)                            GBL_NOEXCEPT;
GBL_INLINE GblStringView GblStringView_chomp             (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE GblStringView GblStringView_substr            (GBL_VSELF, GblSize offset, GblSize length)            GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_contains          (GBL_VSELF, GblStringView subStr)                      GBL_NOEXCEPT;
//GBL_INLINE GblBool     GblStringView_containsIgnoreCase(GBL_VSELF, GblStringView substr)                      GBL_NOEXCEPT;
GBL_INLINE GblSize       GblStringView_count             (GBL_VSELF, GblStringView substr)                      GBL_NOEXCEPT;
//GBL_INLINE GblSize     GblStringView_countIgnoreCase   (GBL_VSELF, GblStringView substr)                      GBL_NOEXCEPT;
GBL_INLINE GblSize       GblStringView_find              (GBL_VSELF, GblStringView substr, GblSize offset)      GBL_NOEXCEPT;
//GBL_INILNE GblSize     GblStringView_findIgnoreCase    (GBL_VSELF, GblStringView substr, GblSize offset)      GBL_NOEXCEPT;
GBL_INLINE GblSize       GblStringView_rfind             (GBL_VSELF, GblStringView substr, GblSize offset)      GBL_NOEXCEPT;
//GBL_INLINE GblSize     GblStringView_rfindIgnoreCase   (GBL_VSELF, GblStringView substr, GblSize offset)      GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_startsWith        (GBL_VSELF, GblStringView substr)                      GBL_NOEXCEPT;
//GBL_INLINE GblBool     GblStringView_startsWithIgnoreCase(GBL_VSELF, GblStringView substr)                    GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_endsWith          (GBL_VSELF, GblStringView substr)                      GBL_NOEXCEPT;
//GBL_INLINE GblBool     GblStringView_endsWithIgnoreCase(GBL_VSELF, GblStringView substr)                      GBL_NOEXCEPT;
GBL_INLINE GblSize       GblStringView_findFirstOf       (GBL_VSELF, GblStringView chars, GblSize offset)       GBL_NOEXCEPT;
GBL_INLINE GblSize       GblStringView_findLastOf        (GBL_VSELF, GblStringView chars, GblSize end)          GBL_NOEXCEPT;
GBL_INLINE GblSize       GblStringView_findFirstNotOf    (GBL_VSELF, GblStringView chars, GblSize offset)       GBL_NOEXCEPT;
GBL_INLINE GblSize       GblStringView_findLastNotOf     (GBL_VSELF, GblStringView chars, GblSize end)          GBL_NOEXCEPT;
GBL_INLINE GblQuark      GblStringView_quark             (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE GblQuark      GblStringView_quarkTry          (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE const char*   GblStringView_intern            (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE GblHash       GblStringView_hash              (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE char*         GblStringView_strdup            (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE char*         GblStringView_toCString         (GBL_VSELF, char* pDest, GblSize destSize)             GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_toNil             (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE GblBool       GblStringView_toBool            (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE int           GblStringView_toInt             (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE unsigned      GblStringView_toUInt            (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE float         GblStringView_toFloat           (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE double        GblStringView_toDouble          (GBL_VSELF)                                            GBL_NOEXCEPT;
GBL_INLINE void*         GblStringView_toPointer         (GBL_VSELF)                                            GBL_NOEXCEPT;


//========== IMPL ==========

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

GBL_INLINE GblStringView GblStringView_fromStringSized(const char* pString, GblSize length) GBL_NOEXCEPT {
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

GBL_INLINE GblStringView GblStringView_fromQuark(GblQuark quark) GBL_NOEXCEPT {
    return GblStringView_fromString(GblQuark_toString(quark));
}

GBL_INLINE int GblStringView_compare(GBL_VSELF, GblStringView other) GBL_NOEXCEPT {
    if(!self.length && !other.length)   return 0;
    else if(self.length > other.length) return INT_MAX;
    else if(self.length < other.length) return INT_MIN;
    else GBL_LIKELY{
        return memcmp(self.pData, other.pData, self.length);
    }
}

GBL_INLINE int GblStringView_compareIgnoreCase(GBL_VSELF, GblStringView other) GBL_NOEXCEPT {
    if(!self.length && other.length) return INT_MIN;
    else if(self.length && !other.length) return INT_MAX;
    else if(!self.length && !other.length) return 0;
    else GBL_LIKELY {
        char* pString1 = GBL_STRING_VIEW_CSTR_ALLOCA(self);
        char* pString2 = GBL_STRING_VIEW_CSTR_ALLOCA(other);
        for(GblSize i = 0; i < self.length; ++i) {
            pString1[i] = toupper(pString1[i]);
        }
        for(GblSize i = 0; i < other.length; ++i) {
            pString2[i] = toupper(pString2[i]);
        }
        return strcmp(pString1, pString2);
    }
}

GBL_INLINE GblBool GblStringView_equals(GBL_VSELF, GblStringView other) GBL_NOEXCEPT {
    return GblStringView_compare(self, other) == 0;
}

GBL_INLINE GblBool GblStringView_equalsIgnoreCase(GBL_VSELF, GblStringView other) GBL_NOEXCEPT {
    return GblStringView_compareIgnoreCase(self, other) == 0;
}

GBL_INLINE GBL_RESULT GblStringView_copy(GBL_VSELF, void* pDst, GblSize offset, GblSize bytes) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_POINTER(pDst);
    GBL_CTX_VERIFY(offset + bytes < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(bytes) {
        memcpy(pDst, self.pData+offset, bytes);
    }
    GBL_CTX_END();
}

GBL_INLINE GblBool GblStringView_empty(GBL_VSELF) GBL_NOEXCEPT {
    return self.length == 0;
}

GBL_INLINE GblBool GblStringView_blank(GBL_VSELF) GBL_NOEXCEPT {
    if(GblStringView_empty(self)) return GBL_TRUE;
    else return GblStringView_findFirstNotOf(self, GBL_STRING_VIEW(" \t\n\r"), 0) == GBL_STRING_VIEW_NPOS;
}

GBL_INLINE char GblStringView_at(GBL_VSELF, GblSize index) GBL_NOEXCEPT {
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

GBL_INLINE char GblStringView_first(GBL_VSELF) GBL_NOEXCEPT {
    char value = '\0';
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = self.pData[0];
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE char GblStringView_last(GBL_VSELF) GBL_NOEXCEPT {
    char value = '\0';
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = self.pData[self.length-1];
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE GblStringView GblStringView_removePrefix(GBL_VSELF, GblSize length) GBL_NOEXCEPT {
    GblStringView view = {
        .pData          = GBL_NULL,
        .nullTerminated = 0,
        .length         = 0
    };
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(length <= self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    view.pData          = (length == self.length && !self.nullTerminated)?
                                GBL_NULL : self.pData + length;
    view.length         = self.length - length;
    view.nullTerminated = self.nullTerminated;
    GBL_CTX_END_BLOCK();
    return view;
}

GBL_INLINE GblStringView GblStringView_removeSuffix(GBL_VSELF, GblSize length) GBL_NOEXCEPT {
    GblStringView view = {
        .pData          = GBL_NULL,
        .nullTerminated = 0,
        .length         = 0
    };
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(length <= self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    view.pData          = (self.length || self.nullTerminated)? self.pData : GBL_NULL;
    view.length         = self.length - length;
    view.nullTerminated = (!length && self.nullTerminated)? 1 : 0;
    GBL_CTX_END_BLOCK();
    return view;
}

GBL_INLINE GblStringView GblStringView_chomp(GBL_VSELF) GBL_NOEXCEPT {
    GblSize subStrLen = self.length;
    if(self.length && self.pData[self.length-1] == '\n') --subStrLen;
    if(self.length >= 2 && self.pData[self.length-2] == '\r') --subStrLen;
    return GblStringView_substr(self, 0, subStrLen);
}

GBL_INLINE GblStringView GblStringView_substr(GBL_VSELF, GblSize offset, GblSize length) GBL_NOEXCEPT {
    GblStringView view = {
        .pData          = GBL_NULL,
        .nullTerminated = 0,
        .length         = 0
    };
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(offset + length <= self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    view.pData          = self.pData + offset;
    view.length         = length;
    view.nullTerminated = (offset+length == self.length && self.nullTerminated)? 1 : 0;
    GBL_CTX_END_BLOCK();
    return view;
}

GBL_INLINE GblBool GblStringView_contains(GBL_VSELF, GblStringView substr) GBL_NOEXCEPT {
    GblBool result = GBL_FALSE;
    if(!self.length && !substr.length) {
        result = GBL_TRUE;
    } else if(self.length && substr.length) {
        const char* pCStr1 = GBL_STRING_VIEW_CSTR(self);
        const char* pCStr2 = GBL_STRING_VIEW_CSTR(substr);
        result = (strstr(pCStr1, pCStr2) != GBL_NULL);
    }
    return result;
}

GBL_INLINE GblSize GblStringView_count(GBL_VSELF, GblStringView substr) GBL_NOEXCEPT {
    GblSize count = 0;
    GblSize offset = 0;
    while((offset = GblStringView_find(self, substr, offset)) != GBL_STRING_VIEW_NPOS) {
        ++count;
        offset += substr.length;
        if(offset >= self.length) break;
    }
    return count;
}

GBL_INLINE GblSize GblStringView_find(GBL_VSELF, GblStringView substr, GblSize offset) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(NULL);
    if(!self.length && !offset) GBL_CTX_DONE();
    GBL_CTX_VERIFY(offset < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(self.length && substr.length) {
        const char* pCStr1  = GBL_STRING_VIEW_CSTR(self);
        const char* pCStr2  = GBL_STRING_VIEW_CSTR(substr);
        const char* pSubstr = strstr(pCStr1+offset, pCStr2);
        if(pSubstr) {
            pos = (GblSize)(pSubstr - pCStr1);
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_INLINE GblSize GblStringView_rfind(GBL_VSELF, GblStringView substr, GblSize end) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(NULL);
    if(!self.length && end == GBL_STRING_VIEW_NPOS)
        GBL_CTX_DONE();
    if(end == GBL_STRING_VIEW_NPOS) end = self.length-1;
    GBL_CTX_VERIFY(end < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(self.length && substr.length) {
        char* pCStr1 = (char*)GBL_ALLOCA(end + 2);
        char* pCStr2 = (char*)GBL_ALLOCA(substr.length + 1);

        for(GblSize i = end+1; i > 0; --i) {
            pCStr1[i-1] = self.pData[end+1 - i];
        }
        pCStr1[end+1] = '\0';

        for(GblSize i = substr.length; i > 0; --i) {
            pCStr2[i-1] = substr.pData[substr.length - i];
        }
        pCStr2[substr.length] = '\0';

        const char* pSubstr = strstr(pCStr1, pCStr2);
        if(pSubstr) {
            pos = end-(GblSize)(pSubstr - pCStr1)-(substr.length-1);
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_INLINE GblBool GblStringView_startsWith(GBL_VSELF, GblStringView substr) GBL_NOEXCEPT {
    if(substr.length > self.length)
        return GBL_FALSE;
    for(GblSize i = 0; i < substr.length; ++i) {
        if(self.pData[i] != substr.pData[i])
            return GBL_FALSE;
    }
    return GBL_TRUE;
}

GBL_INLINE GblBool GblStringView_endsWith(GBL_VSELF, GblStringView substr) GBL_NOEXCEPT {
    if(substr.length > self.length)
        return GBL_FALSE;
    for(GblSize i = 0; i < substr.length; ++i) {
        if(self.pData[self.length-substr.length+i] != substr.pData[i])
            return GBL_FALSE;
    }
    return GBL_TRUE;
}

GBL_INLINE GblSize GblStringView_findFirstOf(GBL_VSELF, GblStringView chars, GblSize offset) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(offset < self.length || (!self.length && !offset),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    for(GblSize i = offset; i < self.length; ++i) {
        for(GblSize c = 0; c < chars.length; ++c) {
            if(self.pData[i] == chars.pData[c]) {
                pos = i;
                GBL_CTX_DONE();
            }
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_INLINE GblSize GblStringView_findLastOf(GBL_VSELF, GblStringView chars, GblSize end) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(GBL_NULL);
    if(!self.length && (!end || end == GBL_STRING_VIEW_NPOS)) {
        GBL_CTX_DONE();
    }
    if(end == GBL_STRING_VIEW_NPOS) end = self.length-1;
    GBL_CTX_VERIFY(end < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    for(GblSize i = end+1; i >= 1; --i) {
        for(GblSize c = 0; c < chars.length; ++c) {
            if(self.pData[i-1] == chars.pData[c]) {
                pos = i-1;
                GBL_CTX_DONE();
            }
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_INLINE GblSize GblStringView_findFirstNotOf(GBL_VSELF, GblStringView chars, GblSize offset) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(offset < self.length || (!self.length && !offset),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(!chars.length) {
        pos = 0;
    } else {
        for(GblSize i = 0; i < self.length; ++i) {
            GblBool found = GBL_FALSE;
            for(GblSize c = 0; c < chars.length; ++c) {
                if(self.pData[i] == chars.pData[c]) {
                    found = GBL_TRUE;
                    break;
                }
            }
            if(!found) {
                pos = i;
                break;
            }
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_INLINE GblSize GblStringView_findLastNotOf(GBL_VSELF, GblStringView chars, GblSize end) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(GBL_NULL);
    if(!self.length && (!end || end == GBL_STRING_VIEW_NPOS)) {
        GBL_CTX_DONE();
    }
    if(end == GBL_STRING_VIEW_NPOS) end = self.length-1;
    GBL_CTX_VERIFY(end < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    for(GblSize i = end+1; i >= 1; --i) {
        GblBool found = GBL_FALSE;
        for(GblSize c = 0; c < chars.length; ++c) {
            if(self.pData[i-1] == chars.pData[c]) {
                found = GBL_TRUE;
                break;
            }
        }
        if(!found) {
            pos = i-1;
            break;
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
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

GBL_INLINE char* GblStringView_strdup(GBL_VSELF) GBL_NOEXCEPT {
    char* pStr = NULL;
    GBL_CTX_BEGIN(NULL);
    pStr = (char*)GBL_CTX_MALLOC(sizeof(char) * self.length + 1);
    if(self.length) memcpy(pStr, self.pData, self.length);
    pStr[self.length] = '\0';
    GBL_CTX_END_BLOCK();
    return pStr;
}

GBL_INLINE char* GblStringView_toCString(GBL_VSELF, char* pDst, GblSize destSize) GBL_NOEXCEPT {
    if(!destSize) return GBL_NULL;
    GblSize length = destSize-1 < self.length? destSize-1 : self.length;
    memcpy(pDst, self.pData, length);
    pDst[length] = '\0';
    return pDst;
}

GBL_INLINE GblBool GblStringView_toNil(GBL_VSELF) GBL_NOEXCEPT  {
    const char* pCStr = GBL_STRING_VIEW_CSTR(self);
    if(!self.length || strncmp(pCStr, "nil", sizeof("nil")) == 0) {
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_INLINE GblBool GblStringView_toBool(GBL_VSELF) GBL_NOEXCEPT {
    const char* pCStr = GBL_STRING_VIEW_CSTR(self);
    if(strncmp(pCStr, "true", sizeof("true")) == 0) {
        return GBL_TRUE;
    } else {
        return (GblStringView_toInt(self) != 0)? GBL_TRUE : GBL_FALSE;
    }
}

GBL_INLINE int GblStringView_toInt(GBL_VSELF) GBL_NOEXCEPT {
    return atoi(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE unsigned GblStringView_toUint(GBL_VSELF) GBL_NOEXCEPT {
    return strtoul(GBL_STRING_VIEW_CSTR(self), GBL_NULL, 10);
}

GBL_INLINE float GblStringView_toFloat(GBL_VSELF) GBL_NOEXCEPT {
    return atof(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE double GblStringView_toDouble(GBL_VSELF) GBL_NOEXCEPT  {
    return atof(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE void* GblStringView_toPointer(GBL_VSELF) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    const char* pCStr = GBL_STRING_VIEW_CSTR(self);
    unsigned int tempUint = 0;
    GBL_CTX_VERIFY(sscanf(pCStr, "0x%x", &tempUint) == 1,
                   GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_CTX_END_BLOCK();
    return (void*)(uintptr_t)tempUint;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_STRING_VIEW_H
