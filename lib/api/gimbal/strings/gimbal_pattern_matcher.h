#ifndef GIMBAL_PATTERN_MATCHER_H
#define GIMBAL_PATTERN_MATCHER_H

#include "../core/gimbal_typedefs.h"

#define GBL_SELF_TYPE GblPatternMatcher

GBL_DECLS_BEGIN


GBL_FORWARD_DECLARE_STRUCT(GblPatternMatcher);


typedef enum GBL_PATTERN_CLASS {
    GBL_PATTERN_CLASS_CHAR,
    GBL_PATTERN_CLASS_PERIOD,
    GBL_PATTERN_CLASS_CARROT,
    GBL_PATTERN_CLASS_DOLLAR,
    GBL_PATTERN_CLASS_ASTERISK,
    GBL_PATTERN_CLASS_COUNT
} GBL_PATTERN_CLASS;

GBL_INLINE GblBool GblPatternMatcher_match(const char* pRegExp, const char* pText) GBL_NOEXCEPT;


// ===== Implementation =====
GBL_INLINE GblBool GblPatternMatcher_matchHere_(const char* pRegExp, const char* pText) GBL_NOEXCEPT;

/* matchplus: search for c*pRegExp at beginning of text */
GBL_INLINE GblBool GblPatternMatcher_matchQuestion_(int c, const char *pRegExp, const char* pText) GBL_NOEXCEPT {
    GBL_UNUSED(pRegExp);
    GblSize matchCount = 0;
    do {    /* a * matches zero or 1 instances */
      //if (GblPatternMatcher_matchHere_(pRegExp, pText))
          if(++matchCount >= 2) return GBL_FALSE;
  } while (*pText != '\0' && (*pText++ == c || c == '.'));
  return GBL_TRUE;
}

/* matchplus: search for c*pRegExp at beginning of text */
GBL_INLINE GblBool GblPatternMatcher_matchPlus_(int c, const char *pRegExp, const char* pText) GBL_NOEXCEPT {
    GblSize matchCount = 0;
    do {    /* a * matches one or more instances */
      if (GblPatternMatcher_matchHere_(pRegExp, pText))
          if(++matchCount >= 2) return GBL_TRUE;
  } while (*pText != '\0' && (*pText++ == c || c == '.'));
  return GBL_FALSE;
}

/* matchstar: search for c*pRegExp at beginning of text */
GBL_INLINE GblBool GblPatternMatcher_matchStar_(int c, const char *pRegExp, const char* pText) GBL_NOEXCEPT {
  do {    /* a * matches zero or more instances */
      if (GblPatternMatcher_matchHere_(pRegExp, pText))
          return GBL_TRUE;
  } while (*pText != '\0' && (*pText++ == c || c == '.'));
  return GBL_FALSE;
}
// \d+ to match integer, use gblAsciiDigit
/* matchhere: search for pRegExp at beginning of text */
GBL_INLINE GblBool GblPatternMatcher_matchHere_(const char* pRegExp, const char* pText) GBL_NOEXCEPT {
    if (pRegExp[0] == '\0')
        return GBL_TRUE;
    if (pRegExp[1] == '*')
        return GblPatternMatcher_matchStar_(pRegExp[0], pRegExp+2, pText);
    if (pRegExp[1] == '+')
        return GblPatternMatcher_matchPlus_(pRegExp[0], pRegExp+2, pText);
    if (pRegExp[1] == '?')
        return GblPatternMatcher_matchQuestion_(pRegExp[0], pRegExp+2, pText);
    if (pRegExp[0] == '$' && pRegExp[1] == '\0')
        return *pText == '\0';
    if (*pText!='\0' && (pRegExp[0]=='.' || pRegExp[0]== *pText))
        return GblPatternMatcher_matchHere_(pRegExp+1, pText+1);
    return GBL_FALSE;
}

GBL_INLINE GblBool GblPatternMatcher_match(const char* pRegExp, const char* pText) GBL_NOEXCEPT {
    if(!pRegExp || !pText) return GBL_FALSE;
    if (pRegExp[0] == '^')
        return GblPatternMatcher_matchHere_(pRegExp+1, pText);
    do {    /* must look even if string is empty */
        if (GblPatternMatcher_matchHere_(pRegExp, pText))
            return GBL_TRUE;
    } while (*pText++ != '\0');
    return GBL_FALSE;
}


GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_PATTERN_MATCHER_H
