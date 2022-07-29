#ifndef GIMBAL_ENUM_H
#define GIMBAL_ENUM_H

#include "gimbal_primitives.h"
#include "../strings/gimbal_quark.h"

/// \ingroup metaBuiltinTypes
#define GBL_ENUM_TYPE                       (GBL_BUILTIN_TYPE(ENUM))
#define GBL_ENUM_CLASS_STRUCT               GblEnumClass
#define GBL_ENUM_TYPE_CHECK(other)          (GblType_check(other, GBL_ENUM_TYPE))

#define GBL_ENUM_CLASS(klass)               (GBL_CLASS_CAST_PREFIX(klass, GBL_ENUM))
#define GBL_ENUM_CLASS_CHECK(klass)         (GBL_CLASS_CHECK_PREFIX(klass, GBL_ENUM))
#define GBL_ENUM_CLASS_TRY(klass)           (GBL_CLASS_TRY_PREFIX(klass, GBL_ENUM))

#define GBL_ENUM_GET_CLASS(variant)         (GBL_ENUM_CLASS(GblClass_weakRefDefault(GblVariant_typeOf(&variant))))
#define GBL_ENUM_TRY_CLASS(variant)         (GBL_ENUM_CLASS_TRY(GblClass_weakRefDefault(GblVariant_typeOf(&variant))))

#define GBL_ENUM_ENTRY(enumValue, nick)     { enumValue, #enumValue, nick }
#define GBL_ENUM_ENTRY_LAST()               { 0, NULL, NULL }

#define SELF    GblEnumClass* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

typedef struct GblEnumEntry {
    GblEnum         value;
    const char*     pName;
    const char*     pNick;
} GblEnumEntry;

typedef struct GblEnumClass {
    GblPrimitiveClass   base;
    GblEnum             valueMax;
    GblEnum             valueMin;
    uint16_t            entryCount;
} GblEnumClass;

// ========== GblEnumClass ==========

GBL_INLINE const char*          GblEnumClass_nameFromIndex      (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_INLINE const char*          GblEnumClass_nameFromValue      (CSELF, GblEnum value)              GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblEnumClass_nameQuarkFromIndex (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblEnumClass_nameQuarkFromValue (CSELF, GblEnum value)              GBL_NOEXCEPT;

GBL_INLINE const char*          GblEnumClass_nickFromIndex      (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_INLINE const char*          GblEnumClass_nickFromValue      (CSELF, GblEnum value)              GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblEnumClass_nickQuarkFromIndex (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblEnumClass_nickQuarkFromValue (CSELF, GblEnum value)              GBL_NOEXCEPT;

GBL_EXPORT GblEnum              GblEnumClass_valueFromIndex     (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_INLINE GblEnum              GblEnumClass_valueFromName      (CSELF, const char* pString)        GBL_NOEXCEPT;
GBL_INLINE GblEnum              GblEnumClass_valueFromNick      (CSELF, const char* pString)        GBL_NOEXCEPT;
GBL_EXPORT GblEnum              GblEnumClass_valueFromNameQuark (CSELF, GblQuark quark)             GBL_NOEXCEPT;
GBL_EXPORT GblEnum              GblEnumClass_valueFromNickQuark (CSELF, GblQuark quark)             GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblEnumClass_valueCheck         (CSELF, GblEnum value)              GBL_NOEXCEPT;

// ========== GblEnum ==========

GBL_EXPORT GblType              GblEnum_register                (const char*         pName,
                                                                 const GblEnumEntry* pValidEntries) GBL_NOEXCEPT;

GBL_INLINE GblEnum              GblEnum_fromName                (const char* pName, GblType type)   GBL_NOEXCEPT;
GBL_INLINE GblEnum              GblEnum_fromNameQuark           (GblQuark name, GblType type)       GBL_NOEXCEPT;
GBL_INLINE GblEnum              GblEnum_fromNick                (const char* pNick, GblType type)   GBL_NOEXCEPT;
GBL_INLINE GblEnum              GblEnum_fromNickQuark           (GblQuark nick, GblType type)       GBL_NOEXCEPT;

GBL_INLINE const char*          GblEnum_name                    (GblEnum value, GblType type)       GBL_NOEXCEPT;
GBL_INLINE GblQuark             GblEnum_nameQuark               (GblEnum value, GblType type)       GBL_NOEXCEPT;
GBL_INLINE const char*          GblEnum_nick                    (GblEnum value, GblType type)       GBL_NOEXCEPT;
GBL_INLINE GblQuark             GblEnum_nickQuark               (GblEnum value, GblType type)       GBL_NOEXCEPT;
GBL_INLINE GblBool              GblEnum_check                   (GblEnum value, GblType type)       GBL_NOEXCEPT;

// =============== IMPL ================

// ---------- GblEnumClass ----------

GBL_INLINE const char* GblEnumClass_nameFromIndex(CSELF, uint16_t index) GBL_NOEXCEPT {
    return GblQuark_toString(GblEnumClass_nameQuarkFromIndex(pSelf, index));
}
GBL_INLINE const char* GblEnumClass_nickFromIndex(CSELF, uint16_t index) GBL_NOEXCEPT {
    return GblQuark_toString(GblEnumClass_nickQuarkFromIndex(pSelf, index));
}
GBL_INLINE const char* GblEnumClass_nameFromValue(CSELF, GblEnum value) GBL_NOEXCEPT {
    return GblQuark_toString(GblEnumClass_nameQuarkFromValue(pSelf, value));
}
GBL_INLINE const char* GblEnumClass_nickFromValue(CSELF, GblEnum value) GBL_NOEXCEPT {
    return GblQuark_toString(GblEnumClass_nickQuarkFromValue(pSelf, value));
}
GBL_INLINE GblEnum GblEnumClass_valueFromName(CSELF, const char* pString) GBL_NOEXCEPT {
    GblEnum value = 0;
    GblQuark quark = GblQuark_tryString(pString);
    if(quark != GBL_QUARK_INVALID) {
        value = GblEnumClass_valueFromNameQuark(pSelf, quark);
    }
    return value;
}
GBL_INLINE GblEnum GblEnumClass_valueFromNick(CSELF, const char* pString) GBL_NOEXCEPT {
    GblEnum value = 0;
    GblQuark quark = GblQuark_tryString(pString);
    if(quark != GBL_QUARK_INVALID) {
        value = GblEnumClass_valueFromNickQuark(pSelf, quark);
    }
    return value;
}
// ---------- GblEnum ----------

GBL_INLINE GblEnum GblEnum_fromName(const char* pName, GblType type) GBL_NOEXCEPT {
    GblEnumClass* pClass = GBL_ENUM_CLASS(GblClass_refDefault(type));
    GblEnum value = GblEnumClass_valueFromName(pClass, pName);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return value;
}
GBL_INLINE GblEnum GblEnum_fromNameQuark(GblQuark name, GblType type) GBL_NOEXCEPT {
    GblEnumClass* pClass = GBL_ENUM_CLASS(GblClass_refDefault(type));
    GblEnum value = GblEnumClass_valueFromNameQuark(pClass, name);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return value;
}
GBL_INLINE GblEnum GblEnum_fromNick(const char* pNick, GblType type) GBL_NOEXCEPT {
    GblEnumClass* pClass = GBL_ENUM_CLASS(GblClass_refDefault(type));
    GblEnum value = GblEnumClass_valueFromNick(pClass, pNick);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return value;
}
GBL_INLINE GblEnum GblEnum_fromNickQuark(GblQuark nick, GblType type) GBL_NOEXCEPT {
    GblEnumClass* pClass = GBL_ENUM_CLASS(GblClass_refDefault(type));
    GblEnum value = GblEnumClass_valueFromNickQuark(pClass, nick);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return value;
}

GBL_INLINE const char* GblEnum_name(GblEnum value, GblType type) GBL_NOEXCEPT {
    GblEnumClass* pClass = GBL_ENUM_CLASS(GblClass_refDefault(type));
    const char* pName = GblEnumClass_nameFromValue(pClass, value);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return pName;
}
GBL_INLINE GblQuark GblEnum_nameQuark(GblEnum value, GblType type) GBL_NOEXCEPT {
    GblEnumClass* pClass = GBL_ENUM_CLASS(GblClass_refDefault(type));
    GblQuark name = GblEnumClass_nameQuarkFromValue(pClass, value);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return name;
}
GBL_INLINE const char* GblEnum_nick(GblEnum value, GblType type) GBL_NOEXCEPT {
    GblEnumClass* pClass = GBL_ENUM_CLASS(GblClass_refDefault(type));
    const char* pNick = GblEnumClass_nickFromValue(pClass, value);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return pNick;
}
GBL_INLINE GblQuark GblEnum_nickQuark(GblEnum value, GblType type) GBL_NOEXCEPT {
    GblEnumClass* pClass = GBL_ENUM_CLASS(GblClass_refDefault(type));
    GblQuark nick = GblEnumClass_nickQuarkFromValue(pClass, value);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return nick;
}
GBL_INLINE GblBool GblEnum_check(GblEnum value, GblType type) GBL_NOEXCEPT {
    GblEnumClass* pClass = GBL_ENUM_CLASS(GblClass_refDefault(type));
    GblBool result = GblEnumClass_valueCheck(pClass, value);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return result;
}


#if 0
// ====== GblEnumVariant ======
// Experimental API I'm thinking about...

typedef GblVariant GblEnumVariant;

#define GBL_ENUM_VARIANT(variant)           (GblEnumVariant_cast(variant))
#define GBL_ENUM_VARIANT_CHECK(variant)     (GblEnumVariant_check(variant))
#define GBL_ENUM_VARIANT_TRY(variant)       (GblEnumVariant_try(variant))

GBL_EXPORT GBL_RESULT           GblEnumVariant_construct        (GblEnumVariant* pVariant,
                                                                 GblType type,
                                                                 GblEnum value)                     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblEnumVariant_constructFromName(GblEnumVariant* pVariant,
                                                                 GblType type,
                                                                 const char* pName)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblEnumVariant_constructFromNameQuark
                                                                (GblEnumVariant* pVariant,
                                                                 GblType type,
                                                                 GblQuark quark)                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblEnumVariant_constructFromNick(GblEnumVariant* pVariant,
                                                                 GblType type,
                                                                 const char* pName)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblEnumVariant_constructFromNickQuark
                                                                (GblEnumVariant* pVariant,
                                                                 GblType type,
                                                                 GblQuark quark)                    GBL_NOEXCEPT;


GBL_EXPORT const char*          GblEnumVariant_name             (const GblVariant* pVariant)        GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblEnumVariant_nameQuark        (const GblVariant* pVariant)        GBL_NOEXCEPT;
GBL_EXPORT const char*          GblEnumVariant_nick             (const GblVariant* pVariant)        GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblEnumVariant_nickQuark        (const GblVariant* pVariant)        GBL_NOEXCEPT;
GBL_EXPORT GblEnum              GblEnumVariant_value            (const GblVariant* pVariant)        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblEnumVariant_valueSet         (GblVariant* pVariant,
                                                                 GblEnum value)                     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblEnumVariant_valueSetFromName (GblVariant* pVariant,
                                                                 const char* pName)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblEnumVariant_valueSetFromNameQuark
                                                                (GblVariant* pVariant,
                                                                 GblQuark quark)                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblEnumVariant_valueSetFromNick (GblVariant* pVariant,
                                                                 const char* pName)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblEnumVariant_valuaSetFromNickQuark
                                                                (GblVariant* pVariant,
                                                                 GblQuark quark)                    GBL_NOEXCEPT;

GBL_EXPORT GblBool              GblEnumVariant_valueCheck       (const GblVariant* pVariant)        GBL_NOEXCEPT;

GBL_EXPORT GblBool              GblEnumVariant_check            (const GblVariant* pVariant)        GBL_NOEXCEPT;
GBL_EXPORT GblEnumVariant*      GblEnumVariant_cast             (GblVariant* pVariant)              GBL_NOEXCEPT;
GBL_EXPORT GblEnumVariant*      GblEnumVariant_try              (GblVariant* pVariant)              GBL_NOEXCEPT;

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
#endif
GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_ENUM_H
