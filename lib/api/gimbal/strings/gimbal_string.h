/*! \todo
gblStrdup()
gblStrndup()
gblStrdupa()
gblStrndupa()
Unit tests
*/

/*! \file
 *  \brief C char and char array manipulation
 *  \ingroup strings
 */

#ifndef GIMBAL_STRING_H
#define GIMBAL_STRING_H

#include "../core/gimbal_typedefs.h"
#include <ctype.h>

GBL_DECLS_BEGIN

GBL_INLINE int gblAsciiDigitValue   (char c) GBL_NOEXCEPT;
GBL_INLINE int gblAsciiXDigitValue  (char c) GBL_NOEXCEPT;

// ===== IMPL =====

GBL_INLINE int gblAsciiDigitValue(char c) GBL_NOEXCEPT {
    return isdigit(c)? c-'0' : -1;
}

GBL_INLINE int gblAsciiXDigitValue(char c) GBL_NOEXCEPT {
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return gblAsciiDigitValue(c);
}


GBL_DECLS_END


#endif // GIMBAL_STRING_H
