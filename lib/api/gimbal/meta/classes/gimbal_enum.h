/*! \file
 *  \brief GblEnumClass and API for managing enumerated types
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_ENUM_H
#define GIMBAL_ENUM_H

#include "gimbal_primitives.h"
#include "../../strings/gimbal_quark.h"

/// \ingroup metaBuiltinTypes
#define GBL_ENUM_TYPE                       (GblEnum_type())
#define GBL_ENUM_CLASS(klass)               (GBL_CLASS_CAST(klass, GblEnum))

#define GBL_ENUM_ENTRY(enumValue, nick)     { enumValue, #enumValue, nick }
#define GBL_ENUM_ENTRY_LAST()               { 0, NULL, NULL }

#define GBL_SELF_TYPE GblEnumClass

GBL_DECLS_BEGIN

typedef struct GblEnumEntry {
    GblEnum     value;
    const char* pName;
    const char* pNick;
} GblEnumEntry;

GBL_CLASS_DERIVE(GblEnum, GblPrimitive)
    GblEnum   valueMax;
    GblEnum   valueMin;
    uint16_t  entryCount;
GBL_CLASS_END

// ========== GblEnumClass ==========#

GBL_INLINE const char* GblEnumClass_nameFromIndex      (GBL_CSELF, uint16_t index)       GBL_NOEXCEPT;
GBL_INLINE const char* GblEnumClass_nameFromValue      (GBL_CSELF, GblEnum value)        GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblEnumClass_nameQuarkFromIndex (GBL_CSELF, uint16_t index)       GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblEnumClass_nameQuarkFromValue (GBL_CSELF, GblEnum value)        GBL_NOEXCEPT;

GBL_INLINE const char* GblEnumClass_nickFromIndex      (GBL_CSELF, uint16_t index)       GBL_NOEXCEPT;
GBL_INLINE const char* GblEnumClass_nickFromValue      (GBL_CSELF, GblEnum value)        GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblEnumClass_nickQuarkFromIndex (GBL_CSELF, uint16_t index)       GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblEnumClass_nickQuarkFromValue (GBL_CSELF, GblEnum value)        GBL_NOEXCEPT;

GBL_EXPORT GblEnum     GblEnumClass_valueFromIndex     (GBL_CSELF, uint16_t index)       GBL_NOEXCEPT;
GBL_INLINE GblEnum     GblEnumClass_valueFromName      (GBL_CSELF, const char* pString)  GBL_NOEXCEPT;
GBL_INLINE GblEnum     GblEnumClass_valueFromNick      (GBL_CSELF, const char* pString)  GBL_NOEXCEPT;
GBL_EXPORT GblEnum     GblEnumClass_valueFromNameQuark (GBL_CSELF, GblQuark quark)       GBL_NOEXCEPT;
GBL_EXPORT GblEnum     GblEnumClass_valueFromNickQuark (GBL_CSELF, GblQuark quark)       GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblEnumClass_valueCheck         (GBL_CSELF, GblEnum value)        GBL_NOEXCEPT;

// ========== GblEnum ==========

GBL_EXPORT GblType     GblEnum_type                    (void)                            GBL_NOEXCEPT;

GBL_EXPORT GblType     GblEnum_register                (const char*         pName,
                                                        const GblEnumEntry* pEntries)    GBL_NOEXCEPT;

GBL_INLINE GblEnum     GblEnum_fromName                (const char* pName, GblType type) GBL_NOEXCEPT;
GBL_INLINE GblEnum     GblEnum_fromNameQuark           (GblQuark name, GblType type)     GBL_NOEXCEPT;
GBL_INLINE GblEnum     GblEnum_fromNick                (const char* pNick, GblType type) GBL_NOEXCEPT;
GBL_INLINE GblEnum     GblEnum_fromNickQuark           (GblQuark nick, GblType type)     GBL_NOEXCEPT;

GBL_INLINE const char* GblEnum_name                    (GblEnum value, GblType type)     GBL_NOEXCEPT;
GBL_INLINE GblQuark    GblEnum_nameQuark               (GblEnum value, GblType type)     GBL_NOEXCEPT;
GBL_INLINE const char* GblEnum_nick                    (GblEnum value, GblType type)     GBL_NOEXCEPT;
GBL_INLINE GblQuark    GblEnum_nickQuark               (GblEnum value, GblType type)     GBL_NOEXCEPT;
GBL_INLINE GblBool     GblEnum_check                   (GblEnum value, GblType type)     GBL_NOEXCEPT;

// =============== IMPL ================

// ---------- GblEnumClass ----------

GBL_INLINE const char* GblEnumClass_nameFromIndex(GBL_CSELF, uint16_t index) GBL_NOEXCEPT {
    return GblQuark_toString(GblEnumClass_nameQuarkFromIndex(pSelf, index));
}
GBL_INLINE const char* GblEnumClass_nickFromIndex(GBL_CSELF, uint16_t index) GBL_NOEXCEPT {
    return GblQuark_toString(GblEnumClass_nickQuarkFromIndex(pSelf, index));
}
GBL_INLINE const char* GblEnumClass_nameFromValue(GBL_CSELF, GblEnum value) GBL_NOEXCEPT {
    return GblQuark_toString(GblEnumClass_nameQuarkFromValue(pSelf, value));
}
GBL_INLINE const char* GblEnumClass_nickFromValue(GBL_CSELF, GblEnum value) GBL_NOEXCEPT {
    return GblQuark_toString(GblEnumClass_nickQuarkFromValue(pSelf, value));
}
GBL_INLINE GblEnum GblEnumClass_valueFromName(GBL_CSELF, const char* pString) GBL_NOEXCEPT {
    GblEnum value = 0;
    GblQuark quark = GblQuark_tryString(pString);
    if(quark != GBL_QUARK_INVALID) {
        value = GblEnumClass_valueFromNameQuark(pSelf, quark);
    }
    return value;
}
GBL_INLINE GblEnum GblEnumClass_valueFromNick(GBL_CSELF, const char* pString) GBL_NOEXCEPT {
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


GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_ENUM_H
