/*! \file
 *  \brief Low-level C string and character-based functions
 *  \ingroup strings
 *
 *  This file contains the lowest-level string API for working
 *  with individual characters or C strings.
 *
 *  \note
 *  Typically, it's much easier to work with higher-level string
 *  APIs built around these utilities.
 *
 *  \sa GblStringView, GblStringRef, GblStringBuffer
 *
 *  \todo
 *      - gblStrdupa()
 *      - gblStrndupa()
 *      - Unit tests
 *      - Consider moving bulk of GblStringView operations
 *
 *  \author    2023 Falco Girgis
 *  \copyright MIT License
 */

#ifndef GIMBAL_STRING_H
#define GIMBAL_STRING_H

#include "../core/gimbal_decls.h"

GBL_DECLS_BEGIN

/*! \name  Per-Character
 *  \brief Methods for dealing with individual ASCII characters
 *  @{
 */
//! Returns the numeric value of the given base-10 ASCII digit
GBL_EXPORT int gblAsciiDigitValue    (char c) GBL_NOEXCEPT;
//! Returns the numeric value of the given hexadecimal ASCII digit
GBL_EXPORT int gblAsciiHexDigitValue (char c) GBL_NOEXCEPT;
//! @}

/*! \name C Strings
 *  \brief Methods for operating on NULL-terminated C strings
 *  @{
 */
//! Returns the length of the given NULL-terminated C string or \p len, if it's less than it's length
GBL_EXPORT size_t gblStrnlen     (const char* pStr1,
                                  size_t      length)  GBL_NOEXCEPT;
//! Compares the two NULL-terminaed C strings without case-sensitivity
GBL_EXPORT int    gblStrCaseCmp  (const char* pStr1,
                                  const char* pStr2)   GBL_NOEXCEPT;
//! Compares up to \p length bytes of the two C strings without case-sensitivity
GBL_EXPORT int    gblStrnCaseCmp (const char* pStr1,
                                  const char* pStr2,
                                  size_t      length)  GBL_NOEXCEPT;
//! Searches for the beginning of the \p pSub substring within \p pStr
GBL_EXPORT char*  gblStrCaseStr  (const char* pStr,
                                  const char* pSub)    GBL_NOEXCEPT;
//! Duplicates the given string, returning a new one allocatedon the heap
GBL_EXPORT char*  gblStrdup      (const char* pString) GBL_NOEXCEPT;
//! Duplicates up to \p length bytes of the given string on the heap
GBL_EXPORT char*  gblStrndup     (const char* pString,
                                  size_t      length)  GBL_NOEXCEPT;
//! Reverses up to \p length bytes within the given character array
GBL_EXPORT char*  gblStrnReverse (char*  pString,
                                  size_t length)       GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#endif // GIMBAL_STRING_H
