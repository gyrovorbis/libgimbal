/*! \file
 *  \brief GblEnumClass and API for managing enumerated types
 *  \ingroup meta
 *
 *  \todo
 *  - Macro DSL for auto registering and declaring enum at the same time
 *  - Finish docs (method descriptions)
 *
 *  \author 2023 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_ENUM_H
#define GIMBAL_ENUM_H

#include "gimbal_primitives.h"
#include "../../strings/gimbal_quark.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_ENUM_TYPE           (GBL_TYPEID(GblEnum))            //!< Type UUID of GblEnumClass
#define GBL_ENUM_CLASS(klass)   (GBL_CLASS_CAST(GblEnum, klass)) //!< Function-style GblClass cast
//! @}

/*! \name Helper Macros
 *  \brief Utility macros to aid in representing GblEnumEntry lists
 *  @{
 */
#define GBL_ENUM_ENTRY(enumValue, nick) { enumValue, #enumValue, nick } //!< Utility GblEnumEntry initializer, using the \p enumValue
#define GBL_ENUM_ENTRY_LAST()           { 0, NULL, NULL }               //!< Utility GblEnumEntry initializer for end list designator
//! @}

#define GBL_SELF_TYPE GblEnumClass

GBL_DECLS_BEGIN

//! Attributes for a single value of an enumeration
typedef struct GblEnumEntry {
    GblEnum     value;  //!< Value of the enumeration
    const char* pName;  //!< String name of the enumeration value
    const char* pNick;  //!< Alternate name of the enumeration value
} GblEnumEntry;

/*! \struct GblEnumClass
 *  \extends GblPrimitiveClass
 *  \brief Static GblClass for storing type data for enumerations
 *
 *  GblEnumClass is a static/persistent class which is created
 *  from a list of GblEnumEntry structures, representing all of
 *  the valid values for the enumerated type.
 */
GBL_CLASS_DERIVE(GblEnum, GblPrimitive)
    GblEnum  valueMax;     //!< Cached maximum value of the enum (used for validation)
    GblEnum  valueMin;     //!< Cached minimum value of the enum (used for validation)
    uint16_t entryCount;   //!< Cached number of values in the entry table
GBL_CLASS_END

// ========== GblEnumClass ==========
/*! \name Name Queries
 *  \brief Methods for querying name of enum values
 *  \relatesalso GblEnumClass
 *  @{
 */
GBL_EXPORT const char* GblEnumClass_nameFromIndex      (GBL_CSELF, uint16_t index) GBL_NOEXCEPT;
GBL_EXPORT const char* GblEnumClass_nameFromValue      (GBL_CSELF, GblEnum value)  GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblEnumClass_nameQuarkFromIndex (GBL_CSELF, uint16_t index) GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblEnumClass_nameQuarkFromValue (GBL_CSELF, GblEnum value)  GBL_NOEXCEPT;
//! @}

/*! \name Nick Queries
 *  \brief Methods for querying nick of enum values
 *  \relatesalso GblEnumClass
 *  @{
 */
GBL_EXPORT const char* GblEnumClass_nickFromIndex      (GBL_CSELF, uint16_t index) GBL_NOEXCEPT;
GBL_EXPORT const char* GblEnumClass_nickFromValue      (GBL_CSELF, GblEnum value)  GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblEnumClass_nickQuarkFromIndex (GBL_CSELF, uint16_t index) GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblEnumClass_nickQuarkFromValue (GBL_CSELF, GblEnum value)  GBL_NOEXCEPT;
//! @}

/*! \name Value Queries
 *  \brief Methods for querying value of enum values
 *  \relatesalso GblEnumClass
 *  @{
 */
GBL_EXPORT GblEnum GblEnumClass_valueFromIndex     (GBL_CSELF, uint16_t index)      GBL_NOEXCEPT;
GBL_EXPORT GblEnum GblEnumClass_valueFromName      (GBL_CSELF, const char* pString) GBL_NOEXCEPT;
GBL_EXPORT GblEnum GblEnumClass_valueFromNick      (GBL_CSELF, const char* pString) GBL_NOEXCEPT;
GBL_EXPORT GblEnum GblEnumClass_valueFromNameQuark (GBL_CSELF, GblQuark quark)      GBL_NOEXCEPT;
GBL_EXPORT GblEnum GblEnumClass_valueFromNickQuark (GBL_CSELF, GblQuark quark)      GBL_NOEXCEPT;
GBL_EXPORT GblBool GblEnumClass_valueCheck         (GBL_CSELF, GblEnum value)       GBL_NOEXCEPT;
//! @}

// ========== GblEnum ==========
GBL_EXPORT GblType GblEnum_type     (void)                         GBL_NOEXCEPT;
GBL_EXPORT GblType GblEnum_register (const char*         pName,
                                     const GblEnumEntry* pEntries) GBL_NOEXCEPT;

/*! \name Enums from Strings
 *  \brief Methods for getting an enum value from string
 *  @{
 */
GBL_EXPORT GblEnum GblEnum_fromName      (const char* pName, GblType type) GBL_NOEXCEPT;
GBL_EXPORT GblEnum GblEnum_fromNameQuark (GblQuark name, GblType type)     GBL_NOEXCEPT;
GBL_EXPORT GblEnum GblEnum_fromNick      (const char* pNick, GblType type) GBL_NOEXCEPT;
GBL_EXPORT GblEnum GblEnum_fromNickQuark (GblQuark nick, GblType type)     GBL_NOEXCEPT;
//! @}

/*! \name Enums to Strings
 *  \brief Methods for getting a string from an enum value
 *  @{
 */
GBL_EXPORT const char* GblEnum_name      (GblEnum value, GblType type) GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblEnum_nameQuark (GblEnum value, GblType type) GBL_NOEXCEPT;
GBL_EXPORT const char* GblEnum_nick      (GblEnum value, GblType type) GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblEnum_nickQuark (GblEnum value, GblType type) GBL_NOEXCEPT;
//! @}

GBL_EXPORT GblBool     GblEnum_check     (GblEnum value, GblType type) GBL_NOEXCEPT;

GBL_DECLS_END

#if 0
#define GBL_ENUM(name, ...)     GBL_ENUM_(name, __VA_ARGS__)

#define GBL_ENUM_ENTRY_(name, ...) \
    GBL_ENUM_ENTRY(__VA_ARGS__)

#define GBL_ENUM_(name, ...) \
    GBL_DECLARE_ENUM(name) { \
        GBL_TUPLE_FOREACH(GBL_ENUM_ENTRY_, name, __VA_ARGS__) \
        GBL_ENUM_ENTRY_LAST()
    };
#endif

#undef GBL_SELF_TYPE

#endif // GIMBAL_ENUM_H
