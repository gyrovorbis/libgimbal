/*! \file
 *  \brief GblScanner, generic text tokenizer and lexer
 *  \ingroup utils
 *
 *  The main operation is to set the delimeter pattern (regex, powered by the
 *  TinyCRegex submodule), and then use GblScanner_peekToken() (and friends)
 *  to "view" the upcoming token without actually advancing the stream or do
 *  GblScanner_scanToken() (and friends) to take the next token and advance the
 *  buffer.
 *
 *  This basically lets GblScanner operate with a one-token lookahead at all time.
 *  If you need to inspect more tokens than just that without modifying the buffer
 *  state, that's when you would use GblScanner_pushCursor() to save your state,
 *  then peek/scan, and when you need to back up and restore an old state,
 *  use GblScanner_popCursor().
 *
 *  The main two operations are peek and scan, with their results being immediately
 *  stored in the public GblStringView variables GblScanner::next and GblScanner::token
 *  respectively. The variations of peek/scan are simply using GblStringView to handle
 *  type conversions for you, like GblScanner_scanUint32() for example, scans into
 *  GblScanner::token and then calls GblStringView_toUint32() on the token for you
 *  automatically.
 *
 *  \todo
 *      - continue testing type-specific peek/scan functions
 *      - write own scanf() implementation that knows how
 *        many characters have been read
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_SCANNER_H
#define GIMBAL_SCANNER_H

#include "../meta/instances/gimbal_object.h"
#include "../strings/gimbal_pattern.h"
#include "../meta/signals/gimbal_signal.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_SCANNER_TYPE            (GBL_TYPEID(GblScanner))            //!< Type UUID for GblScanner
#define GBL_SCANNER(self)           (GBL_CAST(GblScanner, self))        //!< Function-style cast for GblInstance
#define GBL_SCANNER_CLASS(klass)    (GBL_CLASS_CAST(GblScanner, klass)) //!< Function-style cast for GblClass
#define GBL_SCANNER_GET_CLASS(self) (GBL_CLASSOF(GblScanner, self))     //!< Get a GblScannerClass from GblInstance
//! @}

//! Default delimeters used with GblScanner for tokenizing the input stream
#define GBL_SCANNER_DELIMETERS_DEFAULT  "[ \t\n\r]"

#define GBL_SELF_TYPE GblScanner

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblScanner);

/*! \enum  GBL_SCANNER_FLAGS
 *  \brief Extensible enum of flags used for GblScanner::status.
 */
GBL_DECLARE_ENUM(GBL_SCANNER_FLAGS) {
    // Builtin Non-error state flags (bit 0)
    GBL_SCANNER_OK         = 0x00000000,    //!< No errors present
    GBL_SCANNER_EOF        = 0x00000001,    //!< End-of-file
    // Builtin Error flags (bits 29-31)
    GBL_SCANNER_SCAN_ERROR = 0x20000000,    //!< Failed to scan the previous token
    GBL_SCANNER_PEEK_ERROR = 0x40000000,    //!< Failed to peek the previous token
    GBL_SCANNER_SKIP_ERROR = 0x80000000,    //!< Failed previous skip call
    // User masks (bits 1-28)
    GBL_SCANNER_USER_STATE = 0x00fffff2,    //!< Mask of non-error user flags
    GBL_SCANNER_USER_ERROR = 0x1f000000,    //!< Mask of error user flags
    // Masks for combined state and error flags
    GBL_SCANNER_STATE      = 0x00ffffff,    //!< Mask of all scanner state flags
    GBL_SCANNER_ERROR      = 0xff000000,    //!< Mask of all scanner error flags
};

//! Represents the current region of interest for a GblScanner
typedef struct GblScannerCursor {
    size_t position;    //!< Current character position
    size_t line;        //!< Current line number
    size_t column;      //!< Current column number
    size_t length;      //!< Length of current region of interest
} GblScannerCursor;

/*! \struct  GblScannerClass
 *  \extends GblObjectClass
 *  \brief   GblClass VTable structure for GblScanner
 *
 *  GblScannerClass provides a virtual function table for
 *  polymorphically overriding the tokenization logic.
 *
 *  \sa GblScanner
 */
GBL_CLASS_DERIVE(GblScanner, GblObject)
    //! Called every time the next token is to be extracted from the stream
    GBL_RESULT (*pFnNextToken)(GBL_SELF, GblStringView* pToken);
GBL_CLASS_END

/*! \struct  GblScanner
 *  \extends GblObject
 *  \ingroup utils
 *  \brief   Generic text stream scanner object
 *
 *  GblScanner offers a generic way to scan through a stream
 *  of text, parsing its contents into variables of any type
 *  supported by the type system.
 *
 *  \sa GblScannerClass
 */
GBL_INSTANCE_DERIVE(GblScanner, GblObject)
    GblStringView     token;    //!< Current token in the stream
    GblStringView     next;     //!< Peeked-at next token in the stream
    GblStringRef*     pError;   //!< Pending error message
    GBL_SCANNER_FLAGS status;   //!< Status after the last operation
GBL_INSTANCE_END

//! \cond
GBL_PROPERTIES(GblScanner,
    (input,      GBL_GENERIC, (READ, WRITE, CONSTRUCT), GBL_STRING_TYPE),
    (delimeters, GBL_GENERIC, (READ, WRITE, CONSTRUCT), GBL_STRING_TYPE),
    (token,      GBL_GENERIC, (READ),                   GBL_STRING_TYPE),
    (next,       GBL_GENERIC, (READ),                   GBL_STRING_TYPE),
    (status,     GBL_GENERIC, (READ),                   GBL_FLAGS_TYPE),
    (error,      GBL_GENERIC, (READ, WRITE),            GBL_STRING_TYPE)
)

GBL_SIGNALS(GblScanner,
    (reset,   (GBL_INSTANCE_TYPE, pReceiver)),
    (eof,     (GBL_INSTANCE_TYPE, pReceiver)),
    (peeked,  (GBL_INSTANCE_TYPE, pReceiver)),
    (scanned, (GBL_INSTANCE_TYPE, pReceiver)),
    (raised,  (GBL_INSTANCE_TYPE, pReceiver))
)
//! \endcond

/*! \name Default Delimeters
 *  \brief Accessor methods for the default delimeter pattern
 *  \relatesalso GblScannerClass
 * @{
 */
//! Returns a string reference to the current default delimeter pattern on the given class
GBL_EXPORT const GblStringRef*
                 GblScannerClass_defaultDelimeters
                                        (GBL_CKLASS)                   GBL_NOEXCEPT;
//! Sets the default delimeter pattern on the given class to a copy of \p pStr
GBL_EXPORT void  GblScannerClass_setDefaultDelimeters
                                        (GBL_KLASS,
                                         const char* pPattern)         GBL_NOEXCEPT;
//! Moves the given string reference to be owned by the given class as its delimeter pattern
GBL_EXPORT void  GblScannerClass_setDefaultDelimetersRef
                                        (GBL_KLASS,
                                         const GblStringRef* pPattern) GBL_NOEXCEPT;
//! @}

//! Returns the GblType UUID associated with GblScanner
GBL_EXPORT GblType GblScanner_type (void) GBL_NOEXCEPT;

/*! \name Lifetime Management
 *  \brief Methods for managing GblScanner creation/destruction
 *  @{
 */
//! Creates and returns a new GblScanner, setting its input to the given string with optional length
GBL_EXPORT GblScanner* GblScanner_create (const char* pStr,
                                          size_t      len/*=0*/) GBL_NOEXCEPT;
//! Acquires a reference to the given scanner, incrementing its reference count and returning its address
GBL_EXPORT GblScanner* GblScanner_ref    (GBL_SELF)              GBL_NOEXCEPT;
//! Releases a reference to the given scanner, returning the remaining count, and destroying the scanner when it hits 0
GBL_EXPORT GblRefCount GblScanner_unref  (GBL_SELF)              GBL_NOEXCEPT;
//! @}

/*! \name Input and Delimeters
 *  \brief Methods for setting input and delimeter pattern
 *  \relatesalso GblScanner
 *  @{
 */
//! Returns a string reference to the string current being used as the input stream
GBL_EXPORT const GblStringRef*
                 GblScanner_input         (GBL_CSELF)                GBL_NOEXCEPT;
//! Sets the input stream to the given string, optionally taking its size, and resetting the stream
GBL_EXPORT void  GblScanner_setInput      (GBL_SELF,
                                           const char* pStr,
                                           size_t      count/*=0*/)  GBL_NOEXCEPT;
//! Sets the input stream to given string reference, taking ownership of it, and resetting the sream
GBL_EXPORT void  GblScanner_setInputRef   (GBL_SELF,
                                           const GblStringRef* pRef) GBL_NOEXCEPT;
//! Returns a reference to the string used as the delimeter regular expression pattern
GBL_EXPORT const GblStringRef*
                 GblScanner_delimeters    (GBL_CSELF)                GBL_NOEXCEPT;
//! Sets the stream's delimeter regular expression pattern to \p pPattern
GBL_EXPORT void  GblScanner_setDelimeters (GBL_SELF,
                                           const char* pPattern)     GBL_NOEXCEPT;
//! @}

/*! \name Errors
 *  \brief Methods for managing errors
 *  \relatesalso GblScanner
 */
//! Clears the pending error message string and any error flags which are set on the given scanner
GBL_EXPORT void GblScanner_clearError (GBL_SELF) GBL_NOEXCEPT;
//! Raises an error, setting the given flags and message for the given scanner
GBL_EXPORT void GblScanner_raiseError (GBL_SELF,
                                       GblFlags    flags,
                                       const char* pFmt,
                                       ...)      GBL_NOEXCEPT;
//! @}

/*! \name Stream
 *  \brief Methods for controlling the input stream
 *  \relatesalso GblScanner
 *  @{
 */
//! Resets the state of the input stream, moving the cursor back to the beginning
GBL_EXPORT void    GblScanner_reset (GBL_SELF)   GBL_NOEXCEPT;
//! Fetches the current position of the cursor into the input stream
GBL_EXPORT size_t  GblScanner_tell  (GBL_CSELF)  GBL_NOEXCEPT;
//! Moves the cursor's current stream position by the given positive or negative offset
GBL_EXPORT GblBool GblScanner_seek  (GBL_SELF,
                                     int whence) GBL_NOEXCEPT;
//! @}

/*! \name Cursor
 *  \brief Methods for managing the stream cursor
 *  \relatesalso GblScanner
 *  @{
 */
//! Returns the top cursor on the stack, which maintains the current position into the input stream
GBL_EXPORT const GblScannerCursor*
                      GblScanner_cursor      (GBL_CSELF, size_t depth/*=0*/) GBL_NOEXCEPT;
//! Returns the depth of the cursor stack for the given scanner object
GBL_EXPORT size_t     GblScanner_cursorDepth (GBL_CSELF)                     GBL_NOEXCEPT;
//! Pushes the value of the current cursor onto the stack, saving its state
GBL_EXPORT GBL_RESULT GblScanner_pushCursor  (GBL_SELF)                      GBL_NOEXCEPT;
//! Pops the value of the current cursor from the top of the stack, reloading its state
GBL_EXPORT GBL_RESULT GblScanner_popCursor   (GBL_SELF)                      GBL_NOEXCEPT;
//! @}

/*! \name Scanning
 *  \brief Methods for scanning the next token
 *  \relatesalso GblScanner
 *  @{
 */
//! Scans the next token, storing it to GblScanner::token or returning GBL_FALSE upon failure
GBL_EXPORT GblBool GblScanner_scanToken   (GBL_SELF)                          GBL_NOEXCEPT;
//! Scans the next token to GblScanner::token if it matches the given pattern or returning GBL_FALSE upon failure
GBL_EXPORT GblBool GblScanner_scanMatch   (GBL_SELF, const char* pPat)        GBL_NOEXCEPT;
//! \todo implement me: GblScanner_scanMatch() overload
GBL_EXPORT GblBool GblScanner_scanPattern (GBL_SELF, const GblPattern* pPat)  GBL_NOEXCEPT;
//! Scans the given number of lines to GblScanner::token or returns GBL_FALSE upon failure
GBL_EXPORT GblBool GblScanner_scanLines   (GBL_SELF, size_t lines)            GBL_NOEXCEPT;
//! Scans the given number of bytes to GblScanner::token or returns GBL_FALSE upon failure
GBL_EXPORT GblBool GblScanner_scanBytes   (GBL_SELF, size_t bytes)            GBL_NOEXCEPT;
//! Scans the next token as a boolean to GblScanner::token, returning GBL_TRUE and setting \p pBool to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanBool    (GBL_SELF, GblBool* pBool)          GBL_NOEXCEPT;
//! Scans the next token as a character to GblScanner::token, returning GBL_TRUE and setting \p pChar to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanChar    (GBL_SELF, char* pChar)             GBL_NOEXCEPT;
//! Scans the next token as an 8-bit unsigned integer to GblScanner::token, returning GBL_TRUE and setting \p pUint to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanUint8   (GBL_SELF, uint8_t* pUint)          GBL_NOEXCEPT;
//! Scans the next token as a 16-bit unsigned integer to GblScanner::token, returning GBL_TRUE and setting \p pUint to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanUint16  (GBL_SELF, uint16_t* pUint)         GBL_NOEXCEPT;
//! Scans the next token as a 16-bit integer to GblScanner::token, returning GBL_TRUE and setting \p pInt to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanInt16   (GBL_SELF, int16_t* pInt)           GBL_NOEXCEPT;
//! Scans the next token as a 32-bit integer to GblScanner::token, returning GBL_TRUE and setting \p pInt to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanInt32   (GBL_SELF, int32_t* pInt)           GBL_NOEXCEPT;
//! Scans the next token as a 32-bit unsigned integer to GblScanner::token, returning GBL_TRUE and setting \p pUint to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanUint32  (GBL_SELF, uint32_t* pUint)         GBL_NOEXCEPT;
//! Scans the next token as a 64-bit integer to GblScanner::token, returning GBL_TRUE and setting \p pInt to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanInt64   (GBL_SELF, int64_t* pInt)           GBL_NOEXCEPT;
//! Scans the next token as a 64-bit unsigned integer to GblScanner::token, returning GBL_TRUE and setting \p pUint to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanUint64  (GBL_SELF, uint64_t* pUint)         GBL_NOEXCEPT;
//! Scans the next token as a float to GblScanner::token, returning GBL_TRUE and setting \p pFloat to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanFloat   (GBL_SELF, float* pFloat)           GBL_NOEXCEPT;
//! Scans the next token as a double to GblScanner::token, returning GBL_TRUE and setting \p pDouble to the scanned value upon success
GBL_EXPORT GblBool GblScanner_scanDouble  (GBL_SELF, double* pDouble)         GBL_NOEXCEPT;
//! Scans the next token and attempts to convert it from a string to the given type, \p t, moving its value through the variadic argument list
GBL_EXPORT GblBool GblScanner_scanType    (GBL_SELF, GblType t, ...)          GBL_NOEXCEPT;
//! Same as GblScanner_scanType(), except taking a va_list* rather than (a) variadic argument(s)
GBL_EXPORT GblBool GblScanner_scanTypeVa  (GBL_SELF, GblType t, va_list* pVa) GBL_NOEXCEPT;
//! Generic overload macro which automatically calls the respective scan function for the type of \p ptr
#define            GblScanner_scan(self, ptr) \
    GBL_META_GENERIC_MACRO_GENERATE(GBL_SCANNER_SCAN_TABLE_, ptr)(self, ptr)
//! @}

/*! \name Peeking
 *  \brief Methods for peeking at the next token
 *  \relatesalso GblScanner
 *  @{
 */
//! Peeks at the next token, without advancing the stream, storing it to GblScanner::next or returning GBL_FALSE upon failure
GBL_EXPORT GblBool GblScanner_peekToken   (GBL_SELF)                          GBL_NOEXCEPT;
//! Peeks at the next token, storing it to GblScanner::next if it exactly matches the given pattern or returning GBL_FALSE upon failure
GBL_EXPORT GblBool GblScanner_peekMatch   (GBL_SELF, const char* pMatch)      GBL_NOEXCEPT;
//! \todo implement me: GblScanner_peekMatch() overload
GBL_EXPORT GblBool GblScanner_peekPattern (GBL_SELF, const GblPattern* pPat)  GBL_NOEXCEPT;
//! Peeks at the next \p count lines, storing them to GblScanner::next or returning GBL_FALSE upon encountering EOF
GBL_EXPORT GblBool GblScanner_peekLines   (GBL_SELF, size_t count)            GBL_NOEXCEPT;
//! Peeks at the next \p count bytes, storing them to GblScanner::next or returning GBL_FALSE upon encountering EOF
GBL_EXPORT GblBool GblScanner_peekBytes   (GBL_SELF, size_t count)            GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pBool and returning GBL_TRUE if it's boolean-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekBool    (GBL_SELF, GblBool* pBool)          GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pChar and returning GBL_TRUE if it's character-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekChar    (GBL_SELF, char* pChar)             GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pUint and returning GBL_TRUE if it's uint8-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekUint8   (GBL_SELF, uint8_t* pUint)          GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pUint and returning GBL_TRUE if it's uint16-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekUint16  (GBL_SELF, uint16_t* pUint)         GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pInt and returning GBL_TRUE if it's int16-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekInt16   (GBL_SELF, int16_t* pInt)           GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pInt and returning GBL_TRUE if it's int32-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekInt32   (GBL_SELF, int32_t* pInt)           GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pUint and returning GBL_TRUE if it's uint32-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekUint32  (GBL_SELF, uint32_t* pUint)         GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pInt and returning GBL_TRUE if it's int64-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekInt64   (GBL_SELF, int64_t* pInt)           GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pUint and returning GBL_TRUE if it's uint64-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekUint64  (GBL_SELF, uint64_t* pUint)         GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pFloat and returning GBL_TRUE if it's float-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekFloat   (GBL_SELF, float* pFloat)           GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, setting \p pDouble and returning GBL_TRUE if it's double-convertible, or returning GBL_FALSE otherwise
GBL_EXPORT GblBool GblScanner_peekDouble  (GBL_SELF, double* pDouble)         GBL_NOEXCEPT;
//! Peeks at the next token, without advancing, attempting to convert it to the given variant type, moving its value to the variadic argument, returning GBL_TRUE upon success
GBL_EXPORT GblBool GblScanner_peekType    (GBL_SELF, GblType t, ...)          GBL_NOEXCEPT;
//! Same as GblScanner_peekType(), except taking a va_list* rather than (a) variadic argument(s)
GBL_EXPORT GblBool GblScanner_peekTypeVa  (GBL_SELF, GblType t, va_list* pVa) GBL_NOEXCEPT;
//! Generic overload macro which automatically calls the respective peek function for the type of \p ptr
#define            GblScanner_peek(self, ptr) \
    GBL_META_GENERIC_MACRO_GENERATE(GBL_SCANNER_PEEK_TABLE_, ptr)(self, ptr)
//! @}

/*! \name Skipping
 *  \brief Methods for skipping over the input steram
 *  \relatesalso GblScanner
 *  @{
 */
//! Attempts to skip the next \p count tokens from the stream, returning GBL_TRUE upon success or GBL_FALSE upon EOF
GBL_EXPORT GblBool GblScanner_skipTokens    (GBL_SELF, size_t count)           GBL_NOEXCEPT;
//! Attempts to skip to just past the given regex \p pPattern, returning GBL_TRUE upon success or GBL_FALSE if not found or upon EOF
GBL_EXPORT GblBool GblScanner_skipMatch     (GBL_SELF, const char* pPattern)   GBL_NOEXCEPT;
//! \todo implement me: GblScanner_skipMatch() overload
GBL_EXPORT GblBool GblScanner_skipPattern   (GBL_SELF, const GblPattern* pPat) GBL_NOEXCEPT;
//! Attemps to skip to the matching regex \p pPattern, returning GBL_TRUE upon success or GBL_FALSE if not found or upon EOF
GBL_EXPORT GblBool GblScanner_skipToMatch   (GBL_SELF, const char* pPattern)   GBL_NOEXCEPT;
//! \todo implement me: GblScanner_skipToMatch() overload
GBL_EXPORT GblBool GblScanner_skipToPattern (GBL_SELF, const GblPattern* pPat) GBL_NOEXCEPT;
//! Attempts to skip \p count lines, returning GBL_TRUE upon success or GBL_FALSE upon EOF
GBL_EXPORT GblBool GblScanner_skipLines     (GBL_SELF, size_t count)           GBL_NOEXCEPT;
//! Attempts to skip \p count bytes, returning GBL_TRUE upon success or GBL_FALSE upon EOF
GBL_EXPORT GblBool GblScanner_skipBytes     (GBL_SELF, size_t count)           GBL_NOEXCEPT;
//! @}

/*! \name Utilities
 *  \brief Methods providing other utilities
 *  \relatesalso GblScanner
 *  @{
 */
//! Invokes the C stdlib routine, sscanf(), at the current position of the input stream
GBL_EXPORT int GblScanner_scanf  (GBL_SELF, const char* pFmt,  ...)           GBL_NOEXCEPT;
//! Invokes the C stdlib routine, vscanf(), at the current position of the input stream
GBL_EXPORT int GblScanner_vscanf (GBL_SELF, const char* pFmt, va_list* pList) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

///\cond
#define GBL_SCANNER_PEEK_TABLE_ (           \
    GBL_META_GENERIC_MACRO_NO_DEFAULT,      \
    (                                       \
        (char*,     GblScanner_peekChar),   \
        (uint8_t*,  GblScanner_peekUint8),  \
        (uint16_t*, GblScanner_peekUint16), \
        (int16_t*,  GblScanner_peekInt16),  \
        (uint32_t*, GblScanner_peekUint32), \
        (int32_t*,  GblScanner_peekInt32),  \
        (uint64_t*, GblScanner_peekUint64), \
        (int64_t*,  GblScanner_peekInt64),  \
        (float*,    GblScanner_peekFloat),  \
        (double*,   GblScanner_peekDouble)  \
    )                                       \
)

#define GBL_SCANNER_SCAN_TABLE_ (           \
    GBL_META_GENERIC_MACRO_NO_DEFAULT,      \
    (                                       \
        (char*,     GblScanner_scanChar),   \
        (uint8_t*,  GblScanner_scanUint8),  \
        (uint16_t*, GblScanner_scanUint16), \
        (int16_t*,  GblScanner_scanInt16),  \
        (uint32_t*, GblScanner_scanUint32), \
        (int32_t*,  GblScanner_scanInt32),  \
        (uint64_t*, GblScanner_scanUint64), \
        (int64_t*,  GblScanner_scanInt64),  \
        (float*,    GblScanner_scanFloat),  \
        (double*,   GblScanner_scanDouble)  \
    )                                       \
)

#define GblScanner_create(...) \
    GblScanner_createDefault_(__VA_ARGS__)
#define GblScanner_createDefault_(...) \
    GblScanner_createDefault__(__VA_ARGS__, 0)
#define GblScanner_createDefault__(string, count, ...) \
    (GblScanner_create)(string, count)

#define GblScanner_cursor(...) \
    GblScanner_cursorDefault_(__VA_ARGS__)
#define GblScanner_cursorDefault_(...) \
    GblScanner_cursorDefault__(__VA_ARGS__, 0)
#define GblScanner_cursorDefault__(self, depth, ...) \
    (GblScanner_cursor)(self, depth)

#define GblScanner_setInput(...) \
    GblScanner_setInputDefault_(__VA_ARGS__)
#define GblScanner_setInputDefault_(...) \
    GblScanner_setInputDefault__(__VA_ARGS__, 0)
#define GblScanner_setInputDefault__(self, string, count, ...) \
    (GblScanner_setInput)(self, string, count)
///\endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_SCANNER_H
