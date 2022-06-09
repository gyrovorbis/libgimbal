#ifndef GIMBAL_ENUM_H
#define GIMBAL_ENUM_H

#include "gimbal_primitives.h"

#define GBL_ENUM_TYPE                       (GBL_BUILTIN_TYPE(ENUM))
#define GBL_ENUM_CLASS_STRUCT               GblEnumClass
#define GBL_ENUM_TYPE_CHECK(other)          (GblType_check(other, GBL_ENUM_TYPE))
#define GBL_ENUM_CHECK(variant)             (GblType_check(GblVariant_type(), GBL_ENUM_TYPE))
#define GBL_ENUM_CLASS(klass)               (GBL_CLASS_CAST_PREFIX(klass, GBL_ENUM))
#define GBL_ENUM_CLASS_CHECK(klass)         (GBL_CLASS_CHECK_PREFIX(klass, GBL_ENUM))
#define GBL_ENUM_CLASS_TRY(klass)           (GBL_CLASS_TRY_PREFIX(klass, GBL_ENUM))
#define GBL_ENUM_GET_CLASS(variant)         (GBL_ENUM_CLASS(GblClass_peek(GblVariant_type(&variant))))
#define GBL_ENUM_TRY_CLASS(variant)         (GBL_ENUM_CLASS_TRY(GblClass_peek(GblVariant_type(&variant))))

#define GBL_ENUM_ENTRY(enumValue, nick)     { enumValue, GblQuark_fromStatic(#enumValue), GblQuark_fromStatic(nick) }

#define SELF    GblEnumClass* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

/*
 * #define GBL_LOG_LEVEL_ENUM_TABLE (                                       \
 *      ( GBL_LOG_LEVEL, LogLevel, "Level of Loggery", gblLogLevelString ), \
 *      (                                                                   \
 *          (GBL_LOG_LEVEL_VERBOSE, 0x1, Verbose,   "Verbose Shit"),        \
 *          (GBL_LOG_LEVEL_WARNING, 0x2, Warning,   "Warning Shit"),        \
 *          (GBL_LOG_LEVEL_ERROR,   0x3, Error,     "Error   Shit")         \
 *      )                                                                   \
 *  )
 *  // automatically shit out C enum definition
 *  GBL_ENUM_TABLE_DECLARE(GBL_LOG_LEVEL_ENUM_TABLE);
 *  // automatically shit out C++ enum definition
 *  GBL_ENUM_TABLE_DECLARE_CPP(GBL_LOG_LEVEL_ENUM_TABLE);
 *  // automatically shit out GblType registration code
 *  GBL_ENUM_REGISTER_TABLE(GBL_LOG_LEVEL_ENUM_TABLE);
 */

typedef struct GblEnumEntry {
    GblEnum     value;
    GblQuark    name;    // So that walking table to go from string to value is cheap
    GblQuark    nick;
} GblEnumEntry;

typedef struct GblEnumClass {
    GblPrimitiveClass   base;
    GblEnum             valueMax;
    GblEnum             valueMin;
    uint16_t            entryCount; //so we can just pointer offset jump and know we're fine
    GblEnumEntry*       pEntries;
} GblEnumClass;

GBL_EXPORT const GblEnumEntry*  GblEnumClass_entries            (CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT uint16_t             GblEnumClass_entryCount         (CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT const GblEnumEntry*  GblEnumClass_entryFromIndex     (CSELF, uint16_t index)             GBL_NOEXCEPT;

GBL_EXPORT const char*          GblEnumClass_nameFromIndex      (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblEnumClass_nameQuarkFromIndex (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_EXPORT const char*          GblEnumClass_nameFromValue      (CSELF, GblEnum value)              GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblEnuMClass_nameQuarkFromValue (CSELF, GblEnum value)              GBL_NOEXCEPT;

GBL_EXPORT const char*          GblEnumClass_nickFromIndex      (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblEnumClass_nickQuarkFromIndex (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_EXPORT const char*          GblEnumClass_nickFromValue      (CSELF, GblEnum value)              GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblEnumClass_nickQuarkFromValue (CSELF, GblEnum value)              GBL_NOEXCEPT;

GBL_EXPORT GblEnum              GblEnumClass_valueFromIndex     (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_EXPORT GblEnum              GblEnumClass_valueFromName      (CSELF, const char* pString)        GBL_NOEXCEPT;
GBL_EXPORT GblEnum              GblEnumClass_valueFromNameQuark (CSELF, GblQuark quark)             GBL_NOEXCEPT;
GBL_EXPORT GblEnum              GblEnumClass_valueFromNick      (CSELF, const char* pString)        GBL_NOEXCEPT;
GBL_EXPORT GblEnum              GblEnumClass_valueFromNickQuark (CSELF, GblQuark quark)             GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblEnumClass_valueCheck         (CSELF, GblEnum value)              GBL_NOEXCEPT;

GBL_EXPORT GblType              GblEnumClass_register           (const char*         pName,
                                                                 const GblEnumEntry* pValidEntries) GBL_NOEXCEPT;

/* wrapper around GBL_VARIANT
GBL_EXPORT GblEnum              GblEnum_value(GblEnum* pEnum);
GBL_EXPORT GblBool              GblEnum_check(GblEnum* pEnum);
GBL_EXPORT const char*          GblEnum_name(GblEnum* pEnum);
GBL_EXPORT GblQuark             GblEnum_nameQuark(GblEnum* pEnum);
GBL_EXPORT const char*          GblEnum_nick(GblEnum* pEnum);
GBL_EXPORT GblQuark             GblEnum_nickQuark(GblEnum* pEnum);
GBL_EXPORT uint16_t             GblEnum_entryIndex(GblEnum* pEnum);
*/

GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_ENUM_H
