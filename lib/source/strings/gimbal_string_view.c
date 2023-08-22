#include <gimbal/strings/gimbal_string_view.h>
#include <gimbal/strings/gimbal_string.h>

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#define GblStringView_toInt_(postfix, type, min, max, func, tempType) \
    GBL_EXPORT type GblStringView_to##postfix(GblStringView self, GblBool* pSuccess) { \
        type        result      = 0; \
        GblBool     valid       = GBL_TRUE; \
        char*       pEnd        = NULL; \
        const char* pCString    = GBL_STRING_VIEW_CSTR(self); \
        \
        if(min == 0 && GblStringView_startsWith(self, "-")) { \
            result = 0; \
            valid = GBL_FALSE; \
        } else { \
        \
            errno = 0; \
            tempType retVal = func(pCString, &pEnd, 0); \
         \
            if(pEnd != pCString + self.length || retVal > max || retVal < min || errno != 0) { \
                result = 0; \
                valid = GBL_FALSE; \
            } else result = (type)retVal; \
        } \
        \
        if(pSuccess) \
            *pSuccess = valid; \
     \
        return result; \
    }

GblStringView_toInt_(Uint8, uint8_t, 0, UINT8_MAX, strtoul, unsigned long)
GblStringView_toInt_(Int8, int8_t, INT8_MIN, INT8_MAX, strtol, long)
GblStringView_toInt_(Uint16, uint16_t, 0, UINT16_MAX, strtoul, unsigned long)
GblStringView_toInt_(Int16, int16_t, INT16_MIN, INT16_MAX, strtol, long)
GblStringView_toInt_(Uint32, uint32_t, 0, UINT32_MAX, strtoul, unsigned long)
GblStringView_toInt_(Int32, int32_t, INT32_MIN, INT32_MAX, strtol, long)
GblStringView_toInt_(Uint64, uint64_t, 0, UINT64_MAX, strtoumax, uintmax_t)
GblStringView_toInt_(Int64, int64_t, INT64_MIN, INT64_MAX, strtoimax, intmax_t)

GBL_EXPORT char GblStringView_toChar(GblStringView self, GblBool* pSuccess) {
    if(self.length != 1) {
        if(pSuccess)
            *pSuccess = GBL_FALSE;

        return '\0';
    } else {
        if(pSuccess)
            *pSuccess = GBL_TRUE;

        return GblStringView_at(self, 0);
    }
}

GBL_EXPORT float GblStringView_toFloat(GblStringView self, GblBool* pSuccess) {
    float       retVal      = 0.0f;
    GblBool     valid       = GBL_TRUE;
    char*       pEnd        = NULL;
    const char* pCString    = GBL_STRING_VIEW_CSTR(self);

    errno = 0;
    retVal = strtof(pCString, &pEnd);

    if(pEnd != pCString + self.length) {
        retVal = 0.0f;
        valid = GBL_FALSE;
    }

    if(pSuccess)
        *pSuccess = !!(valid && errno == 0);

    return retVal;
}

GBL_EXPORT double GblStringView_toDouble(GblStringView self, GblBool* pSuccess) {
    double      retVal      = 0.0;
    GblBool     valid       = GBL_TRUE;
    char*       pEnd        = NULL;
    const char* pCString    = GBL_STRING_VIEW_CSTR(self);

    errno = 0;
    retVal = strtod(pCString, &pEnd);

    if(pEnd != pCString + self.length) {
        retVal = 0.0;
        valid = GBL_FALSE;
    }

    if(pSuccess)
        *pSuccess = !!(valid && errno == 0);

    return retVal;
}

GBL_EXPORT GblBool GblStringView_toNil(GblStringView self) {
    const char* pCStr = GBL_STRING_VIEW_CSTR(self);
    if(!self.length ||
       gblStrnCaseCmp(pCStr, "nil", sizeof("nil")) == 0 ||
       gblStrnCaseCmp(pCStr, "null", sizeof("null")) == 0) {
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_EXPORT GblBool GblStringView_toBool(GblStringView self, GblBool* pSuccess) {
    const char* pCStr = GBL_STRING_VIEW_CSTR(self);
    if(gblStrnCaseCmp(pCStr, "true", sizeof("true")) == 0 ||
       gblStrnCaseCmp(pCStr, "yes",  sizeof("yes"))  == 0) {
        if(pSuccess)
            *pSuccess = GBL_TRUE;

        return GBL_TRUE;
    } else if(gblStrnCaseCmp(pCStr, "false", sizeof("false")) == 0 ||
              gblStrnCaseCmp(pCStr, "no", sizeof("no")) == 0) {
        if(pSuccess)
            *pSuccess = GBL_TRUE;

        return GBL_FALSE;

    } else {
        return (GblStringView_toUint32(self, pSuccess) != 0)?
                   GBL_TRUE : GBL_FALSE;
    }
}

GBL_EXPORT void* GblStringView_toPointer(GblStringView self, GblBool* pSuccess) {
    const char* pCStr = GBL_STRING_VIEW_CSTR(self);
    unsigned int tempUint = 0;

    const GblBool success = !!(sscanf(pCStr, "0x%x", &tempUint) == 1);

    if(pSuccess)
        *pSuccess = success;

    return (void*)(uintptr_t)tempUint;
}

GBL_EXPORT char* GblStringView_strdup(GblStringView self) {
    char* pStr = NULL;
    GBL_CTX_BEGIN(NULL);
    pStr = (char*)GBL_CTX_MALLOC(sizeof(char) * self.length + 1);
    if(self.length) memcpy(pStr, self.pData, self.length);
    pStr[self.length] = '\0';
    GBL_CTX_END_BLOCK();
    return pStr;
}

GBL_EXPORT char* GblStringView_toCString(GblStringView self, char* pDst, size_t  destSize) {
    if(!destSize) return GBL_NULL;
    size_t length = destSize-1 < self.length? destSize-1 : self.length;
    memcpy(pDst, self.pData, length);
    pDst[length] = '\0';
    return pDst;
}

GBL_EXPORT size_t (GblStringView_findLastNotOf)(GblStringView self, const char* pChars, size_t len, size_t end) {
    GblStringView chars = GblStringView_fromString(pChars, len);

    size_t  pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(GBL_NULL);
    if(!self.length && (!end || end == GBL_STRING_VIEW_NPOS)) {
        GBL_CTX_DONE();
    }
    if(end == GBL_STRING_VIEW_NPOS) end = self.length - 1;
    GBL_CTX_VERIFY(end < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    for(size_t i = end + 1; i >= 1; --i) {
        GblBool found = GBL_FALSE;
        for(size_t c = 0; c < chars.length; ++c) {
            if(self.pData[i - 1] == chars.pData[c]) {
                found = GBL_TRUE;
                break;
            }
        }
        if(!found) {
            pos = i - 1;
            break;
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_EXPORT GBL_RESULT GblStringView_copy(GblStringView self, void* pDst, size_t  offset, size_t  bytes) {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_POINTER(pDst);
    GBL_CTX_VERIFY(offset + bytes < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(bytes) {
        memcpy(pDst, self.pData + offset, bytes);
    }
    GBL_CTX_END();
}

GBL_EXPORT int (GblStringView_compare)(GblStringView self, const char* pString, size_t len) {
    GblStringView other = GblStringView_fromString(pString, len);
    if(!self.length && !other.length)   return 0;
    else if(self.length > other.length) return INT_MAX;
    else if(self.length < other.length) return INT_MIN;
    else GBL_LIKELY{
            return memcmp(self.pData, other.pData, self.length);
        }
}

GBL_EXPORT int (GblStringView_compareIgnoreCase)(GblStringView self, const char* pString, size_t len) {
    GblStringView other = GblStringView_fromString(pString, len);

    if(!self.length && other.length) return INT_MIN;
    else if(self.length && !other.length) return INT_MAX;
    else if(!self.length && !other.length) return 0;
    else GBL_LIKELY {
            char* pString1 = GBL_STRING_VIEW_CSTR_ALLOCA(self);
            char* pString2 = GBL_STRING_VIEW_CSTR_ALLOCA(other);
            for(size_t  i = 0; i < self.length; ++i) {
                pString1[i] = toupper(pString1[i]);
            }
            for(size_t  i = 0; i < other.length; ++i) {
                pString2[i] = toupper(pString2[i]);
            }
            return strcmp(pString1, pString2);
        }
}

GBL_EXPORT size_t (GblStringView_findFirstNotOf)(GblStringView self, const char* pChars, size_t len, size_t offset) {
    GblStringView chars = GblStringView_fromString(pChars, len);

    size_t  pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(offset < self.length || (!self.length && !offset),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(!chars.length) {
        pos = 0;
    } else {
        for(size_t i = 0; i < self.length; ++i) {
            GblBool found = GBL_FALSE;
            for(size_t c = 0; c < chars.length; ++c) {
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

GBL_EXPORT char GblStringView_first(GblStringView self) {
    char value = '\0';
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = self.pData[0];
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT char GblStringView_last(GblStringView self) {
    char value = '\0';
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = self.pData[self.length-1];
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT GblStringView GblStringView_removePrefix(GblStringView self, size_t  length) {
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

GBL_EXPORT GblStringView GblStringView_removeSuffix(GblStringView self, size_t  length) {
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

GBL_EXPORT GblStringView GblStringView_chomp(GblStringView self) {
    size_t  subStrLen = self.length;
    if(self.length && self.pData[self.length-1] == '\n') --subStrLen;
    if(self.length >= 2 && self.pData[self.length-2] == '\r') --subStrLen;
    return GblStringView_substr(self, 0, subStrLen);
}

GBL_EXPORT GblStringView GblStringView_substr(GblStringView self, size_t  offset, size_t  length) {
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

GBL_EXPORT GblBool (GblStringView_contains)(GblStringView self, const char* pStr, size_t len) {
    GblStringView substr = GblStringView_fromString(pStr, len);
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

GBL_EXPORT GblBool (GblStringView_containsIgnoreCase)(GblStringView self, const char* pStr, size_t len) {
    GblStringView substr = GblStringView_fromString(pStr, len);
    GblBool result = GBL_FALSE;
    if(!self.length && !substr.length) {
        result = GBL_TRUE;
    } else if(self.length && substr.length) {
        const char* pCStr1 = GBL_STRING_VIEW_CSTR(self);
        const char* pCStr2 = GBL_STRING_VIEW_CSTR(substr);
        result = (gblStrCaseStr(pCStr1, pCStr2) != GBL_NULL);
    }
    return result;
}

GBL_EXPORT size_t (GblStringView_count)(GblStringView self, const char* pStr, size_t len) {
    GblStringView substr = GblStringView_fromString(pStr, len);
    size_t  count = 0;
    size_t  offset = 0;
    while((offset = GblStringView_find(self, substr.pData, substr.length, offset)) != GBL_STRING_VIEW_NPOS) {
        ++count;
        offset += substr.length;
        if(offset >= self.length) break;
    }
    return count;
}

#if 0
GBL_EXPORT size_t (GblStringView_countIgnoreCase)(GblStringView self, const char* pStr, size_t len) {
    GblStringView substr = GblStringView_fromString(pStr, len);
    size_t  count = 0;
    size_t  offset = 0;
    while((offset = GblStringView_find(self, substr, offset)) != GBL_STRING_VIEW_NPOS) {
        ++count;
        offset += substr.length;
        if(offset >= self.length) break;
    }
    return count;
}
#endif

GBL_EXPORT size_t (GblStringView_find)(GblStringView self, const char* pStr, size_t len, size_t offset) {
    GblStringView substr = GblStringView_fromString(pStr, len);

    size_t  pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(NULL);
    if(!self.length && !offset) GBL_CTX_DONE();
    GBL_CTX_VERIFY(offset < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(self.length && substr.length) {
        const char* pCStr1  = GBL_STRING_VIEW_CSTR(self);
        const char* pCStr2  = GBL_STRING_VIEW_CSTR(substr);
        const char* pSubstr = strstr(pCStr1 + offset, pCStr2);
        if(pSubstr) {
            pos = (size_t )(pSubstr - pCStr1);
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_EXPORT size_t (GblStringView_findIgnoreCase)(GblStringView self, const char* pStr, size_t len, size_t offset) {
    GblStringView substr = GblStringView_fromString(pStr, len);

    size_t  pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(NULL);
    if(!self.length && !offset) GBL_CTX_DONE();
    GBL_CTX_VERIFY(offset < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(self.length && substr.length) {
        const char* pCStr1  = GBL_STRING_VIEW_CSTR(self);
        const char* pCStr2  = GBL_STRING_VIEW_CSTR(substr);
        const char* pSubstr = gblStrCaseStr(pCStr1 + offset, pCStr2);
        if(pSubstr) {
            pos = (size_t )(pSubstr - pCStr1);
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_EXPORT size_t GblStringView_rfind_(GblStringView self, const char* pStr, size_t len, size_t end, GblBool caseSensitive) {
    GblStringView substr = GblStringView_fromString(pStr, len);

    size_t  pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(NULL);
    if(!self.length && end == GBL_STRING_VIEW_NPOS)
        GBL_CTX_DONE();
    if(end == GBL_STRING_VIEW_NPOS) end = self.length-1;
    GBL_CTX_VERIFY(end < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(self.length && substr.length) {
        char* pCStr1 = (char*)GBL_ALLOCA(end + 2);
        char* pCStr2 = (char*)GBL_ALLOCA(substr.length + 1);

        for(size_t  i = end+1; i > 0; --i) {
            pCStr1[i-1] = self.pData[end+1 - i];
        }
        pCStr1[end+1] = '\0';

        for(size_t  i = substr.length; i > 0; --i) {
            pCStr2[i-1] = substr.pData[substr.length - i];
        }
        pCStr2[substr.length] = '\0';

        const char* pSubstr = caseSensitive? strstr(pCStr1, pCStr2):
                                             gblStrCaseStr(pCStr1, pCStr2);
        if(pSubstr) {
            pos = end-(size_t )(pSubstr - pCStr1)-(substr.length-1);
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_EXPORT size_t (GblStringView_rfind)(GblStringView self, const char* pStr, size_t len, size_t end) {
    return GblStringView_rfind_(self, pStr, len, end, GBL_TRUE);
}

GBL_EXPORT size_t (GblStringView_rfindIgnoreCase)(GblStringView self, const char* pStr, size_t len, size_t end) {
    return GblStringView_rfind_(self, pStr, len, end, GBL_FALSE);
}

GBL_EXPORT GblBool (GblStringView_startsWith)(GblStringView self, const char* pStr, size_t len) {
    GblStringView substr = GblStringView_fromString(pStr, len);

    if(substr.length > self.length)
        return GBL_FALSE;
    for(size_t  i = 0; i < substr.length; ++i) {
        if(self.pData[i] != substr.pData[i])
            return GBL_FALSE;
    }
    return GBL_TRUE;
}

GBL_EXPORT GblBool (GblStringView_startsWithIgnoreCase)(GblStringView self, const char* pStr, size_t len) {
    GblStringView substr = GblStringView_fromString(pStr, len);

    if(substr.length > self.length)
        return GBL_FALSE;
    for(size_t  i = 0; i < substr.length; ++i) {
        if((int)tolower((int)self.pData[i]) != (int)tolower((int)substr.pData[i]))
            return GBL_FALSE;
    }
    return GBL_TRUE;
}

GBL_EXPORT GblBool (GblStringView_endsWith)(GblStringView self, const char* pStr, size_t len) {
    GblStringView substr = GblStringView_fromString(pStr, len);

    if(substr.length > self.length)
        return GBL_FALSE;
    for(size_t  i = 0; i < substr.length; ++i) {
        if(self.pData[self.length-substr.length + i] != substr.pData[i])
            return GBL_FALSE;
    }
    return GBL_TRUE;
}

GBL_EXPORT GblBool (GblStringView_endsWithIgnoreCase)(GblStringView self, const char* pStr, size_t len) {
    GblStringView substr = GblStringView_fromString(pStr, len);

    if(substr.length > self.length)
        return GBL_FALSE;
    for(size_t  i = 0; i < substr.length; ++i) {
        if((int)tolower((int)self.pData[self.length - substr.length + i]) !=
           (int)tolower((int)substr.pData[i]))
            return GBL_FALSE;
    }
    return GBL_TRUE;
}

GBL_EXPORT size_t (GblStringView_findFirstOf)(GblStringView self, const char* pChars, size_t len, size_t offset) {
    GblStringView chars = GblStringView_fromString(pChars, len);

    size_t  pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY(offset < self.length || (!self.length && !offset),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    for(size_t i = offset; i < self.length; ++i) {
        for(size_t c = 0; c < chars.length; ++c) {
            if(self.pData[i] == chars.pData[c]) {
                pos = i;
                GBL_CTX_DONE();
            }
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_EXPORT size_t (GblStringView_findLastOf)(GblStringView self, const char* pChars, size_t len, size_t end) {
    GblStringView chars = GblStringView_fromString(pChars, len);

    size_t  pos = GBL_STRING_VIEW_NPOS;
    GBL_CTX_BEGIN(GBL_NULL);
    if(!self.length && (!end || end == GBL_STRING_VIEW_NPOS)) {
        GBL_CTX_DONE();
    }
    if(end == GBL_STRING_VIEW_NPOS) end = self.length - 1;
    GBL_CTX_VERIFY(end < self.length,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    for(size_t i = end + 1; i >= 1; --i) {
        for(size_t c = 0; c < chars.length; ++c) {
            if(self.pData[i - 1] == chars.pData[c]) {
                pos = i - 1;
                GBL_CTX_DONE();
            }
        }
    }
    GBL_CTX_END_BLOCK();
    return pos;
}

GBL_EXPORT int GblStringView_scanf(GblStringView self, const char* pFmt, ...) {
    va_list varArgs;
    va_start(varArgs, pFmt);
    const int retVal = GblStringView_scanfVa(self, pFmt, &varArgs);
    va_end(varArgs);
    return retVal;
}

GBL_EXPORT int GblStringView_scanfVa(GblStringView self, const char* pFmt, va_list* pVarArgs) {
    const char* pCString = GBL_STRING_VIEW_CSTR(self);
    return vsscanf(pCString, pFmt, *pVarArgs);
}


GBL_EXPORT GblStringView GblStringView_fromEmpty(void) GBL_NOEXCEPT {
    return GblStringView_fromString(GBL_NULL);
}

GBL_EXPORT GblStringView (GblStringView_fromString)(const char* pString, size_t length) GBL_NOEXCEPT {
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

GBL_EXPORT char GblStringView_at(GblStringView self, size_t index) GBL_NOEXCEPT {
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

GBL_EXPORT GblStringView GblStringView_fromQuark(GblQuark quark) {
    return GblStringView_fromString(GblQuark_toString(quark));
}

GBL_EXPORT GblBool (GblStringView_equals)(GblStringView self, const char* pString, size_t len) {
    return GblStringView_compare(self, pString, len) == 0;
}

GBL_EXPORT GblBool (GblStringView_equalsIgnoreCase)(GblStringView self, const char* pString, size_t len) {
    return GblStringView_compareIgnoreCase(self, pString, len) == 0;
}

GBL_EXPORT GblBool GblStringView_empty(GblStringView self) {
    return self.length == 0;
}

GBL_EXPORT GblBool GblStringView_blank(GblStringView self) {
    if(GblStringView_empty(self)) return GBL_TRUE;
    else return GblStringView_findFirstNotOf(self, " \t\n\r") == GBL_STRING_VIEW_NPOS;
}

GBL_EXPORT GblQuark GblStringView_quark(GblStringView self) {
    return GblQuark_fromString(GBL_STRING_VIEW_CSTR(self));
}

GBL_EXPORT GblQuark GblStringView_tryQuark(GblStringView self) {
    return GblQuark_tryString(GBL_STRING_VIEW_CSTR(self));
}

GBL_EXPORT const char* GblStringView_intern(GblStringView self) {
    return GblQuark_internString(GBL_STRING_VIEW_CSTR(self));
}

GBL_EXPORT GblHash GblStringView_hash(GblStringView self) {
    return self.length? gblHash(self.pData, self.length) : 0;
}
