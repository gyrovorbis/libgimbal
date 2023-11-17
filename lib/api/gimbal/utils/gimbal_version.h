/*! \file
 *  \brief   GblVersion type, utlities and query functions
 *  \ingroup utils
 *
 *  This file contains the GblVersion typedef and its associated API.
 *  A GblVersion is a 32-bit unsigned integer, encoded as:
 *
 *  |Bits    |24-31|9-23 |0-8  |
 *  |--------|-----|-----|-----|
 *  |\b Field|Major|Minor|Patch|
 *
 *  \note
 *  Since it's just an unsigned integer, two versions can directly
 *  be compared to one-another for version checks.
 *
 *  \todo
 *      - implement fuzzy comparison: GblVersion_compare(version, ">=3.23.0")
 *      - registered GblType?
 *
 *  \author    2023 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_VERSION_H
#define GIMBAL_VERSION_H

#include "../core/gimbal_result.h"

/*! \name Bitfields
 *  \brief Bitfield positions and masks
 *  @{
 */
#define GBL_VERSION_MAJOR_BITPOS    24u     //!< Bit position of the MAJOR field
#define GBL_VERSION_MAJOR_MASK      0xff    //!< Bit mask of the MAJOR field

#define GBL_VERSION_MINOR_BITPOS    8u      //!< Bit position of the MINOR field
#define GBL_VERSION_MINOR_MASK      0xffff  //!< Bit mask of the MINOR field

#define GBL_VERSION_PATCH_BITPOS    0u      //!< Bit position of the PATCH field
#define GBL_VERSION_PATCH_MASK      0xff    //!< Bit mask of the PATCH field
//! @}

//!Size required for a string buffer to hold a version, used with GblVersion_string()
 #define GBL_VERSION_STRING_SIZE    (2 * 3 + 5 + 2 + 1)

/*! \name Compile-Time Versioning
 *  \brief Macros for compile-time encoding
 *  @{
 */
//! Encodes a GblVersion from its constituents
#define GBL_VERSION_MAKE(major, minor, patch)       GBL_VERSION_MAKE_(major, minor, patch)
//! Creates a compile-time stringified version from its constituents
#define GBL_VERSION_STRING(major, minor, patch)     GBL_VERSION_STRING_(major, minor, patch)
//! @}

#define GBL_SELF_TYPE GblVersion

GBL_DECLS_BEGIN

/*! 32-bit unsigned integer representing a packed version in the form (MAJOR.MINOR.PATCH)
 *  \ingroup utils
 */
typedef uint32_t GblVersion;

/*! \name Creation
 *  \brief Methods for creating and encoding
 *  @{
 */
//! Creates and returns a GblVersion from its packed constituent values
GBL_EXPORT GblVersion GblVersion_make  (uint8_t  major,
                                        uint16_t minor,
                                        uint8_t  patch)      GBL_NOEXCEPT;
//! Parses and returns a GblVersion from an encoded version string in the format "MAJOR.MINOR.PATCH"
GBL_EXPORT GblVersion GblVersion_parse (const char* pString) GBL_NOEXCEPT;
//! @}

//! Stringifies a GblVersion, storing it within \p pBuffer and returning \p pBuffer
GBL_EXPORT char* GblVersion_string (GBL_VSELF, char* pBuffer) GBL_NOEXCEPT;

/*! \name Field Extraction
 *  \brief Methods for getting field values
 *  @{
 */
//! Extracts the 8-bit MAJOR version number field
GBL_EXPORT uint8_t  GblVersion_major (GBL_VSELF) GBL_NOEXCEPT;
//! Extracts the 16-bit MINOR version number field
GBL_EXPORT uint16_t GblVersion_minor (GBL_VSELF) GBL_NOEXCEPT;
//! Extracts the 8-bit PATCH version number field
GBL_EXPORT uint8_t  GblVersion_patch (GBL_VSELF) GBL_NOEXCEPT;
//! @}

/*! \name Field Encoding
 *  \brief Methods for setting field values
 *  @{
 */
//! Sets the 8-bit MAJOR version number field to \p major
GBL_EXPORT void GblVersion_setMajor (GBL_SELF, uint8_t major)  GBL_NOEXCEPT;
//! Sets the 16-bit MINOR version number field to \p minor
GBL_EXPORT void GblVersion_setMinor (GBL_SELF, uint16_t minor) GBL_NOEXCEPT;
//! Sets the 8-bit PATCH version number field to \p patch
GBL_EXPORT void GblVersion_setPatch (GBL_SELF, uint8_t patch)  GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond
#define GBL_VERSION_MAKE_(major, minor, patch)              \
    ((uint32_t)((major & (uint8_t)GBL_VERSION_MAJOR_MASK)   \
                << GBL_VERSION_MAJOR_BITPOS) |              \
    (uint32_t)((minor & (uint16_t)GBL_VERSION_MINOR_MASK)   \
                << GBL_VERSION_MINOR_BITPOS) |              \
    (uint32_t)((patch & (uint8_t)GBL_VERSION_PATCH_MASK)    \
               << GBL_VERSION_PATCH_BITPOS))

#define GBL_VERSION_STRING_(major, minor, patch) \
    GBL_STRINGIFY(major) "."                     \
    GBL_STRINGIFY(minor) "."                     \
    GBL_STRINGIFY(patch)
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_VERSION_H
