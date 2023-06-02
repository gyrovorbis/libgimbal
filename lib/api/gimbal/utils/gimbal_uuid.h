/*! \file
 *  \brief GblUuid generator, stringifier, validator, etc
 *  \ingroup utils
 *
 *  LibGimbal provides basic UUID functionality by allowing
 *  you to deserialize existing UUIDs from strings, generate
 *  new version 4 UUIDs, serialize UUIDs, and compare two UUIDs,
 *  based on RFC 4122.
 *
 *  \todo
 *      - fix reliance on apparently determinsitic random mechanism
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_UUID_H
#define GIMBAL_UUID_H

#include "../core/gimbal_ctx.h"

#define GBL_UUID_BYTE_COUNT     16  ///< Total number of bytes to representa a UUID, per RFC 4122.
#define GBL_UUID_NODE_COUNT     6   ///< Number of node bytes in a UUID, per RFC 4122.
#define GBL_UUID_STRING_SIZE    37  ///< Byte size of a string needed to represent a UUID, including NULL terminator.
#define GBL_UUID_STRING_LENGTH  36  ///< Length of string needed to represent a UUID, excluding NULL terminator.

#define GBL_SELF_TYPE GblUuid

GBL_DECLS_BEGIN

/*! \brief Universally unique identifier
 *
 *  RFC 4122-based UUID type.
 *  \ingroup utils
 */
typedef struct GblUuid {
    union {
        GBL_PRIVATE()
            uint32_t  time_low;
            uint16_t  time_mid;
            uint16_t  time_hi_and_version;
            uint8_t   clock_seq_hi_and_reserved;
            uint8_t   clock_seq_low;
            uint8_t   node[GBL_UUID_NODE_COUNT];
        GBL_PRIVATE_END
        uint8_t       bytes[GBL_UUID_BYTE_COUNT];
    };
} GblUuid;

/// Initializes the given GblUuid structure to a "nil" UUID, per RFC 4122.
GBL_EXPORT GBL_RESULT GblUuid_initNil        (GBL_SELF)                         GBL_NOEXCEPT;
/// Initializes the given GblUuid structure to a new, randomly generated UUID per RFC 4122 (version 4).
GBL_EXPORT GBL_RESULT GblUuid_initV4         (GBL_SELF)                         GBL_NOEXCEPT;
/// Initializes the given GblUuid structure by deserializing the string representation given by pStrBuffer.
GBL_EXPORT GBL_RESULT GblUuid_initFromString (GBL_SELF, const char* pStrBuffer) GBL_NOEXCEPT;
/// Fills in the given 37-byte string buffer with the standardized string representation of the given UUID.
GBL_EXPORT GBL_RESULT GblUuid_toString       (GBL_CSELF, char* pStrBuffer)      GBL_NOEXCEPT;
/// Lexicographically Compares the given UUID to another, returning 0 if equal, -1 if less than, or 1 if greater than/
GBL_EXPORT int        GblUuid_compare        (GBL_CSELF, const GblUuid* pOther) GBL_NOEXCEPT;
/// Returns the UUID version type, according to RFC 4122.
GBL_EXPORT unsigned   GblUuid_version        (GBL_CSELF)                        GBL_NOEXCEPT;
/// Returns true if the given UUID is equal to the NIL UUID, per RFC 4122 (all zeroes),
GBL_EXPORT GblBool    GblUuid_isNil          (GBL_CSELF)                        GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_UUID_H

/*!
 * \fn GBL_RESULT GblUuid_initNil(GblUuid* pSelf)
 * Returns GBL_RESULT_SUCCESS upon success, or GBL_RESULT_ERROR_INVALID_POINTER,
 * if a valid pointer was not passed as pSelf.
 * \param pSelf pointer to a GblUuid
 * \returns success or invalid pointer
 * \relatesalso GblUuid
*/
/*!
 * \fn GBL_RESULT GblUuid_initV4(GblUuid* pSelf)
 * Returns GBL_RESULT_SUCCESS upon success, or
 * GBL_RESULT_ERROR_INVALID_POINTER, if a valid pointer was not passed as pSelf.
 * \param pSelf pointer to a GblUuid
 * \returns success or invalid pointer
 * \relatesalso GblUuid
*/
/*!
 * \fn GBL_RESULT GblUuid_initFromString(GblUuid* pSelf, const char* pStrBuffer)
 * Returns GBL_RESULT_SUCCESS upon success, or an error if the provided
 * arguments were not valid.
 * \param pSelf pointer to a GblUuid
 * \param pStrBuffer Standard 36-char UUID string format (XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX)
 * \returns success or error message
 * \relatesalso GblUuid
*/
/*!
 * \fn GBL_RESULT GblUuid_toString(const GblUuid* pSelf, char* pStrBuffer)
 * Returns success or an error code upon failure.
 * \param pSelf pointer to a GblUuid
 * \param pStrBuffer string buffer of at least 37 bytes (36 plus NULL terminator)
 * \returns success or error message
 * \relatesalso GblUuid
*/
/*!
 * \fn int GblUuid_compare(const GblUuid* pSelf, const GblUuid* pOther)
 * Errors are thrown if either argument is invalid.
 * \param pSelf pointer to a GblUuid
 * \param pOther pointer to another GblUuid
 * \returns 0, -1, or 1
 * \relatesalso GblUuid
*/
/*!
 * \fn unsigned GblUuid_version(const GblUuid* pSelf)
 * Throws an error if the given pointer is invalid.
 * \param pSelf pointer to a GblUuid
 * \returns UUID version
 * \relatesalso GblUuid
*/
/*!
 * \fn GblBool GblUuid_isNil(const GblUuid* pSelf)
 * Throws an error if pSelf is invalid.
 * \param pSelf pointer to a GblUuid
 * \returns whether the given UUID is nil or not
 * \relatesalso GblUuid
*/
