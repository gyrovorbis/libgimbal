#ifndef GIMBAL_STRING_VIEW_H
#define GIMBAL_STRING_VIEW_H

#include "../core/gimbal_api_frame.h"
#include "../algorithms/gimbal_hash.h"
#include "gimbal_quark.h"
#include <ctype.h>

#define GBL_STRING_VIEW_NPOS                GBL_NPOS
#define GBL_STRING_VIEW_2(data, length)     GblStringView_fromStringSized(data, length)
#define GBL_STRING_VIEW_1(cstring)          GblStringView_fromString(cstring)
#define GBL_STRING_VIEW(...)                GBL_VA_OVERLOAD_SELECT(GBL_STRING_VIEW, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)
#define GBL_STRV(cstring)                   GBL_STRING_VIEW(cstring)
#define GBL_STRING_VIEW_CSTR_ALLOCA(view)   GblStringView_toCString(view, (char*)GBL_ALLOCA(view.length+1), view.length+1)
#define GBL_STRING_VIEW_CSTR(view)          (view.nullTerminated? view.pData : GBL_STRING_VIEW_CSTR_ALLOCA(view))

#define SELF    GblStringView self

GBL_DECLS_BEGIN

typedef struct GblStringView {
    const char* pData;
    GblSize     nullTerminated  : 1;
#ifdef GBL_64BIT
    GblSize     length          : 63;
#elif defined(GBL_32BIT)
    GblSize     length          : 31;
#endif
} GblStringView;

GBL_INLINE GblStringView    GblStringView_fromEmpty         (void)                                              GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringView_fromString        (const char* pString)                               GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringView_fromStringSized   (const char* pData, GblSize length)                 GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringView_fromQuark         (GblQuark quark)                                    GBL_NOEXCEPT;
GBL_INLINE GblInt           GblStringView_compare           (SELF, GblStringView other)                         GBL_NOEXCEPT;
GBL_INLINE GblInt           GblStringView_compareIgnoreCase (SELF, GblStringView other)                         GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringView_equals            (SELF, GblStringView other)                         GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringView_equalsIgnoreCase  (SELF, GblStringView other)                         GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringView_copy              (SELF, void* pDst, GblSize offset, GblSize bytes)   GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringView_empty             (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringView_blank             (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE char             GblStringView_at                (SELF, GblSize index)                               GBL_NOEXCEPT;
GBL_INLINE char             GblStringView_first             (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE char             GblStringView_last              (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringView_removePrefix      (SELF, GblSize length)                              GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringView_removeSuffix      (SELF, GblSize length)                              GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringView_chomp             (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringView_substr            (SELF, GblSize offset, GblSize length)              GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringView_contains          (SELF, GblStringView subStr)                        GBL_NOEXCEPT;
//GBL_INLINE GblBool          GblStringView_containsIgnoreCase(SELF, GblStringView substr)                        GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringView_count             (SELF, GblStringView substr)                        GBL_NOEXCEPT;
//GBL_INLINE GblSize          GblStringView_countIgnoreCase   (SELF, GblStringView substr)                        GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringView_find              (SELF, GblStringView substr, GblSize offset)        GBL_NOEXCEPT;
//GBL_INILNE GblSize          GblStringView_findIgnoreCase    (SELF, GblStringView substr, GblSize offset)        GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringView_rfind             (SELF, GblStringView substr, GblSize offset)        GBL_NOEXCEPT;
//GBL_INLINE GblSize          GblStringView_rfindIgnoreCase   (SELF, GblStringView substr, GblSize offset)        GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringView_startsWith        (SELF, GblStringView substr)                        GBL_NOEXCEPT;
//GBL_INLINE GblBool        GblStringView_startsWithIgnoreCase(SELF, GblStringView substr)                       GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringView_endsWith          (SELF, GblStringView substr)                        GBL_NOEXCEPT;
//GBL_INLINE GblBool        GblStringView_endsWithIgnoreCase(SELF, GblStringView substr)                         GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringView_findFirstOf       (SELF, GblStringView chars, GblSize offset)         GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringView_findLastOf        (SELF, GblStringView chars, GblSize end)            GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringView_findFirstNotOf    (SELF, GblStringView chars, GblSize offset)         GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringView_findLastNotOf     (SELF, GblStringView chars, GblSize end)            GBL_NOEXCEPT;
GBL_INLINE GblQuark         GblStringView_quark             (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblQuark         GblStringView_quarkTry          (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE const char*      GblStringView_intern            (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblHash          GblStringView_hash              (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE char*            GblStringView_strdup            (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE char*            GblStringView_toCString         (SELF, char* pDest, GblSize destSize)               GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringView_toNil             (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringView_toBool            (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblInt           GblStringView_toInt             (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblUint          GblStringView_toUInt            (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblFloat         GblStringView_toFloat           (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE double           GblStringView_toDouble          (SELF)                                              GBL_NOEXCEPT;
GBL_INLINE void*            GblStringView_toPointer         (SELF)                                              GBL_NOEXCEPT;


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

GBL_INLINE GblInt GblStringView_compare(SELF, GblStringView other) GBL_NOEXCEPT {
    if(!self.length && !other.length)   return 0;
    else if(self.length > other.length) return INT_MAX;
    else if(self.length < other.length) return INT_MIN;
    else GBL_LIKELY{
        return memcmp(self.pData, other.pData, self.length);
    }
}

GBL_INLINE GblInt GblStringView_compareIgnoreCase(SELF, GblStringView other) GBL_NOEXCEPT {
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

GBL_INLINE GblBool GblStringView_equals(SELF, GblStringView other) GBL_NOEXCEPT {
    return GblStringView_compare(self, other) == 0;
}

GBL_INLINE GblBool GblStringView_equalsIgnoreCase(SELF, GblStringView other) GBL_NOEXCEPT {
    return GblStringView_compareIgnoreCase(self, other) == 0;
}

GBL_INLINE GBL_RESULT GblStringView_copy(SELF, void* pDst, GblSize offset, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pDst);
    GBL_API_VERIFY(offset + bytes < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(bytes) {
        memcpy(pDst, self.pData+offset, bytes);
    }
    GBL_API_END();
}

GBL_INLINE GblBool GblStringView_empty(SELF) GBL_NOEXCEPT {
    return self.length == 0;
}

GBL_INLINE GblBool GblStringView_blank(SELF) GBL_NOEXCEPT {
    if(GblStringView_empty(self)) return GBL_TRUE;
    else return GblStringView_findFirstNotOf(self, GBL_STRING_VIEW(" \t\n\r"), 0) == GBL_STRING_VIEW_NPOS;
}

GBL_INLINE char GblStringView_at(SELF, GblSize index) GBL_NOEXCEPT {
    char value = '\0';
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY(index < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = self.pData[index];
    GBL_API_END_BLOCK();
    return value;
}

GBL_INLINE char GblStringView_first(SELF) GBL_NOEXCEPT {
    char value = '\0';
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY(self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = self.pData[0];
    GBL_API_END_BLOCK();
    return value;
}

GBL_INLINE char GblStringView_last(SELF) GBL_NOEXCEPT {
    char value = '\0';
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY(self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = self.pData[self.length-1];
    GBL_API_END_BLOCK();
    return value;
}

GBL_INLINE GblStringView GblStringView_removePrefix(SELF, GblSize length) GBL_NOEXCEPT {
    GblStringView view = {
        .pData          = GBL_NULL,
        .nullTerminated = 0,
        .length         = 0
    };
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY(length <= self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    view.pData          = (length == self.length && !self.nullTerminated)?
                                GBL_NULL : self.pData + length;
    view.length         = self.length - length;
    view.nullTerminated = self.nullTerminated;
    GBL_API_END_BLOCK();
    return view;
}

GBL_INLINE GblStringView GblStringView_removeSuffix(SELF, GblSize length) GBL_NOEXCEPT {
    GblStringView view = {
        .pData          = GBL_NULL,
        .nullTerminated = 0,
        .length         = 0
    };
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY(length <= self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    view.pData          = (self.length || self.nullTerminated)? self.pData : GBL_NULL;
    view.length         = self.length - length;
    view.nullTerminated = (!length && self.nullTerminated)? 1 : 0;
    GBL_API_END_BLOCK();
    return view;
}

GBL_INLINE GblStringView GblStringView_chomp(SELF) GBL_NOEXCEPT {
    GblSize subStrLen = self.length;
    if(self.length && self.pData[self.length-1] == '\n') --subStrLen;
    if(self.length >= 2 && self.pData[self.length-2] == '\r') --subStrLen;
    return GblStringView_substr(self, 0, subStrLen);
}

GBL_INLINE GblStringView GblStringView_substr(SELF, GblSize offset, GblSize length) GBL_NOEXCEPT {
    GblStringView view = {
        .pData          = GBL_NULL,
        .nullTerminated = 0,
        .length         = 0
    };
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY(offset + length <= self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    view.pData          = self.pData + offset;
    view.length         = length;
    view.nullTerminated = (offset+length == self.length && self.nullTerminated)? 1 : 0;
    GBL_API_END_BLOCK();
    return view;
}

GBL_INLINE GblBool GblStringView_contains(SELF, GblStringView substr) GBL_NOEXCEPT {
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

GBL_INLINE GblSize GblStringView_count(SELF, GblStringView substr) GBL_NOEXCEPT {
    GblSize count = 0;
    GblSize offset = 0;
    while((offset = GblStringView_find(self, substr, offset)) != GBL_STRING_VIEW_NPOS) {
        ++count;
        offset += substr.length;
        if(offset >= self.length) break;
    }
    return count;
}

GBL_INLINE GblSize GblStringView_find(SELF, GblStringView substr, GblSize offset) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_API_BEGIN(NULL);
    if(!self.length && !offset) GBL_API_DONE();
    GBL_API_VERIFY(offset < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(self.length && substr.length) {
        const char* pCStr1  = GBL_STRING_VIEW_CSTR(self);
        const char* pCStr2  = GBL_STRING_VIEW_CSTR(substr);
        const char* pSubstr = strstr(pCStr1+offset, pCStr2);
        if(pSubstr) {
            pos = (GblSize)(pSubstr - pCStr1);
        }
    }
    GBL_API_END_BLOCK();
    return pos;
}

GBL_INLINE GblSize GblStringView_rfind(SELF, GblStringView substr, GblSize end) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_API_BEGIN(NULL);
    if(!self.length && end == GBL_STRING_VIEW_NPOS)
        GBL_API_DONE();
    if(end == GBL_STRING_VIEW_NPOS) end = self.length-1;
    GBL_API_VERIFY(end < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(self.length && substr.length) {
        char* pCStr1 = (char*)GBL_ALLOCA(end + 1);
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
    GBL_API_END_BLOCK();
    return pos;
}

GBL_INLINE GblBool GblStringView_startsWith(SELF, GblStringView substr) GBL_NOEXCEPT {
    if(substr.length > self.length)
        return GBL_FALSE;
    for(GblSize i = 0; i < substr.length; ++i) {
        if(self.pData[i] != substr.pData[i])
            return GBL_FALSE;
    }
    return GBL_TRUE;
}

GBL_INLINE GblBool GblStringView_endsWith(SELF, GblStringView substr) GBL_NOEXCEPT {
    if(substr.length > self.length)
        return GBL_FALSE;
    for(GblSize i = 0; i < substr.length; ++i) {
        if(self.pData[self.length-substr.length+i] != substr.pData[i])
            return GBL_FALSE;
    }
    return GBL_TRUE;
}

GBL_INLINE GblSize GblStringView_findFirstOf(SELF, GblStringView chars, GblSize offset) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY(offset < self.length || (!self.length && !offset),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    for(GblSize i = offset; i < self.length; ++i) {
        for(GblSize c = 0; c < chars.length; ++c) {
            if(self.pData[i] == chars.pData[c]) {
                pos = i;
                GBL_API_DONE();
            }
        }
    }
    GBL_API_END_BLOCK();
    return pos;
}

GBL_INLINE GblSize GblStringView_findLastOf(SELF, GblStringView chars, GblSize end) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_API_BEGIN(GBL_NULL);
    if(!self.length && (!end || end == GBL_STRING_VIEW_NPOS)) {
        GBL_API_DONE();
    }
    if(end == GBL_STRING_VIEW_NPOS) end = self.length-1;
    GBL_API_VERIFY(end < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    for(GblSize i = end+1; i >= 1; --i) {
        for(GblSize c = 0; c < chars.length; ++c) {
            if(self.pData[i-1] == chars.pData[c]) {
                pos = i-1;
                GBL_API_DONE();
            }
        }
    }
    GBL_API_END_BLOCK();
    return pos;
}

GBL_INLINE GblSize GblStringView_findFirstNotOf(SELF, GblStringView chars, GblSize offset) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY(offset < self.length || (!self.length && !offset),
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
    GBL_API_END_BLOCK();
    return pos;
}

GBL_INLINE GblSize GblStringView_findLastNotOf(SELF, GblStringView chars, GblSize end) GBL_NOEXCEPT {
    GblSize pos = GBL_STRING_VIEW_NPOS;
    GBL_API_BEGIN(GBL_NULL);
    if(!self.length && (!end || end == GBL_STRING_VIEW_NPOS)) {
        GBL_API_DONE();
    }
    if(end == GBL_STRING_VIEW_NPOS) end = self.length-1;
    GBL_API_VERIFY(end < self.length,
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
    GBL_API_END_BLOCK();
    return pos;
}

GBL_INLINE GblQuark GblStringView_quark(SELF) GBL_NOEXCEPT {
    return GblQuark_fromString(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE GblQuark GblStringView_quarkTry(SELF) GBL_NOEXCEPT {
    return GblQuark_tryString(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE const char* GblStringView_intern(SELF) GBL_NOEXCEPT {
    return GblQuark_internString(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE GblHash GblStringView_hash(SELF) GBL_NOEXCEPT {
    return self.length? gblHashMurmur(self.pData, self.length) : 0;
}

GBL_INLINE char* GblStringView_strdup(SELF) GBL_NOEXCEPT {
    char* pStr = NULL;
    GBL_API_BEGIN(NULL);
    pStr = (char*)GBL_API_MALLOC(sizeof(char) * self.length + 1);
    if(self.length) memcpy(pStr, self.pData, self.length);
    pStr[self.length] = '\0';
    GBL_API_END_BLOCK();
    return pStr;
}

GBL_INLINE char* GblStringView_toCString(SELF, char* pDst, GblSize destSize) GBL_NOEXCEPT {
    if(!destSize) return GBL_NULL;
    GblSize length = destSize-1 < self.length? destSize-1 : self.length;
    memcpy(pDst, self.pData, length);
    pDst[length] = '\0';
    return pDst;
}

GBL_INLINE GblBool GblStringView_toNil(SELF) GBL_NOEXCEPT  {
    const char* pCStr = GBL_STRING_VIEW_CSTR(self);
    if(!self.length || strncmp(pCStr, "nil", sizeof("nil")) == 0) {
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_INLINE GblBool GblStringView_toBool(SELF) GBL_NOEXCEPT {
    const char* pCStr = GBL_STRING_VIEW_CSTR(self);
    if(strncmp(pCStr, "true", sizeof("true")) == 0) {
        return GBL_TRUE;
    } else {
        return (GblStringView_toInt(self) != 0)? GBL_TRUE : GBL_FALSE;
    }
}

GBL_INLINE GblInt GblStringView_toInt(SELF) GBL_NOEXCEPT {
    return atoi(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE GblUint GblStringView_toUint(SELF) GBL_NOEXCEPT {
    return strtoul(GBL_STRING_VIEW_CSTR(self), GBL_NULL, 10);
}

GBL_INLINE GblFloat GblStringView_toFloat(SELF) GBL_NOEXCEPT {
    return atof(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE double GblStringView_toDouble(SELF) GBL_NOEXCEPT  {
    return atof(GBL_STRING_VIEW_CSTR(self));
}

GBL_INLINE void* GblStringView_toPointer(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    const char* pCStr = GBL_STRING_VIEW_CSTR(self);
    unsigned int tempUint = 0;
    GBL_API_VERIFY(sscanf(pCStr, "0x%x", &tempUint) == 1,
                   GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_API_END_BLOCK();
    return (void*)(uintptr_t)tempUint;
}

GBL_DECLS_END

#undef SELF

#endif // GIMBAL_STRING_VIEW_H
