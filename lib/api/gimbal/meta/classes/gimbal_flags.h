/*! \file
 *  \brief GblFlagsClass and API for managing flag types
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_FLAGS_H
#define GIMBAL_FLAGS_H

#include "gimbal_primitives.h"
#include "../../strings/gimbal_quark.h"

/// \ingroup metaBuiltinTypes
#define GBL_FLAGS_TYPE                      (GblFlags_type())
#define GBL_FLAGS_CLASS(klass)              (GBL_CLASS_CAST(klass, GblFlags))

#define GBL_FLAGS_ENTRY(flagsValue, nick)   { flagsValue, #flagsValue, nick }
#define GBL_FLAGS_ENTRY_LAST()              { 0, NULL, NULL }

#define GBL_SELF_TYPE GblFlagsClass

GBL_DECLS_BEGIN

typedef struct GblFlagEntry {
    GblFlags        value;
    const char*     pName;
    const char*     pNick;
} GblFlagEntry;

GBL_CLASS_DERIVE(GblFlags, GblPrimitive)
    GblFlags            valueMask;
    uint16_t            entryCount;
GBL_CLASS_END

// ========== GblFlagsClass ==========

GBL_INLINE const char* GblFlagsClass_nameFromIndex      (GBL_CSELF, uint16_t index)       GBL_NOEXCEPT;
GBL_INLINE const char* GblFlagsClass_nameFromValue      (GBL_CSELF, GblFlags value)       GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblFlagsClass_nameQuarkFromIndex (GBL_CSELF, uint16_t index)       GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblFlagsClass_nameQuarkFromValue (GBL_CSELF, GblFlags value)       GBL_NOEXCEPT;

GBL_INLINE const char* GblFlagsClass_nickFromIndex      (GBL_CSELF, uint16_t index)       GBL_NOEXCEPT;
GBL_INLINE const char* GblFlagsClass_nickFromValue      (GBL_CSELF, GblFlags value)       GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblFlagsClass_nickQuarkFromIndex (GBL_CSELF, uint16_t index)       GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblFlagsClass_nickQuarkFromValue (GBL_CSELF, GblFlags value)       GBL_NOEXCEPT;

GBL_EXPORT GblFlags    GblFlagsClass_valueFromIndex     (GBL_CSELF, uint16_t index)       GBL_NOEXCEPT;
GBL_INLINE GblFlags    GblFlagsClass_valueFromName      (GBL_CSELF, const char* pName)    GBL_NOEXCEPT;
GBL_INLINE GblFlags    GblFlagsClass_valueFromNick      (GBL_CSELF, const char* pNick)    GBL_NOEXCEPT;
GBL_EXPORT GblFlags    GblFlagsClass_valueFromNameQuark (GBL_CSELF, GblQuark name)        GBL_NOEXCEPT;
GBL_EXPORT GblFlags    GblFlagsClass_valueFromNickQuark (GBL_CSELF, GblQuark nick)        GBL_NOEXCEPT;
GBL_EXPORT GblFlags    GblFlagsClass_valueFromString    (GBL_CSELF, const char* pString)  GBL_NOEXCEPT;

GBL_INLINE GblBool     GblFlagsClass_valueCheck         (GBL_CSELF, GblFlags value)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblFlagsClass_valueAppendString  (GBL_CSELF,
                                                         GblFlags value,
                                                         GblStringBuffer* pStr)           GBL_NOEXCEPT;

// ========== GblFlags ==========

GBL_EXPORT GblType     GblFlags_type                    (void)                            GBL_NOEXCEPT;

GBL_EXPORT GblType     GblFlags_register                (const char*          pName,
                                                         const GblFlagEntry*  pEntries)   GBL_NOEXCEPT;

GBL_INLINE GblFlags    GblFlags_fromName                (const char* pName, GblType type) GBL_NOEXCEPT;
GBL_INLINE GblFlags    GblFlags_fromNameQuark           (GblQuark name, GblType type)     GBL_NOEXCEPT;
GBL_INLINE GblFlags    GblFlags_fromNick                (const char* pNick, GblType type) GBL_NOEXCEPT;
GBL_INLINE GblFlags    GblFlags_fromNickQuark           (GblQuark nick, GblType type)     GBL_NOEXCEPT;
GBL_INLINE GblFlags    GblFlags_fromString              (const char* pName, GblType type) GBL_NOEXCEPT;

GBL_INLINE const char* GblFlags_name                    (GblFlags value, GblType type)    GBL_NOEXCEPT;
GBL_INLINE GblQuark    GblFlags_nameQuark               (GblFlags value, GblType type)    GBL_NOEXCEPT;
GBL_INLINE const char* GblFlags_nick                    (GblFlags value, GblType type)    GBL_NOEXCEPT;
GBL_INLINE GblQuark    GblFlags_nickQuark               (GblFlags value, GblType type)    GBL_NOEXCEPT;

GBL_INLINE GblBool     GblFlags_check                   (GblFlags value, GblType type)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblFlags_appendString            (GblFlags value,
                                                         GblType type,
                                                         GblStringBuffer* pBuffer)        GBL_NOEXCEPT;

// ========== IMPL ==========

// ------ GblFlagsClass ------

GBL_INLINE const char* GblFlagsClass_nameFromIndex(GBL_CSELF, uint16_t index) GBL_NOEXCEPT {
    return GblQuark_toString(GblFlagsClass_nameQuarkFromIndex(pSelf, index));
}
GBL_INLINE const char* GblFlagsClass_nickFromIndex(GBL_CSELF, uint16_t index) GBL_NOEXCEPT {
    return GblQuark_toString(GblFlagsClass_nickQuarkFromIndex(pSelf, index));
}
GBL_INLINE const char* GblFlagsClass_nameFromValue(GBL_CSELF, GblFlags value) GBL_NOEXCEPT {
    return GblQuark_toString(GblFlagsClass_nameQuarkFromValue(pSelf, value));
}
GBL_INLINE const char* GblFlagsClass_nickFromValue(GBL_CSELF, GblFlags value) GBL_NOEXCEPT {
    return GblQuark_toString(GblFlagsClass_nickQuarkFromValue(pSelf, value));
}
GBL_INLINE GblFlags GblFlagsClass_valueFromName(GBL_CSELF, const char* pString) GBL_NOEXCEPT {
    GblFlags value = 0;
    GblQuark quark = GblQuark_tryString(pString);
    if(quark != GBL_QUARK_INVALID) {
        value = GblFlagsClass_valueFromNameQuark(pSelf, quark);
    }
    return value;
}
GBL_INLINE GblFlags GblFlagsClass_valueFromNick(GBL_CSELF, const char* pString) GBL_NOEXCEPT {
    GblFlags value = 0;
    GblQuark quark = GblQuark_tryString(pString);
    if(quark != GBL_QUARK_INVALID) {
        value = GblFlagsClass_valueFromNickQuark(pSelf, quark);
    }
    return value;
}

GBL_INLINE GblBool GblFlagsClass_valueCheck(const GblFlagsClass* pSelf, GblFlags value) GBL_NOEXCEPT {
    return ((value | pSelf->valueMask) == pSelf->valueMask)? GBL_TRUE : GBL_FALSE;
}

// -------- GblFlags --------


GBL_INLINE GblFlags GblFlags_fromName(const char* pName, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    GblFlags value = GblFlagsClass_valueFromName(pClass, pName);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return value;
}
GBL_INLINE GblFlags GblFlags_fromNameQuark(GblQuark name, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    GblFlags value = GblFlagsClass_valueFromNameQuark(pClass, name);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return value;
}
GBL_INLINE GblFlags GblFlags_fromNick(const char* pNick, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    GblFlags value = GblFlagsClass_valueFromNick(pClass, pNick);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return value;
}
GBL_INLINE GblFlags GblFlags_fromNickQuark(GblQuark nick, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    GblFlags value = GblFlagsClass_valueFromNickQuark(pClass, nick);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return value;
}

GBL_INLINE GblFlags GblFlags_fromString(const char* pName, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    GblFlags value = GblFlagsClass_valueFromString(pClass, pName);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return value;
}

GBL_INLINE const char* GblFlags_name(GblFlags value, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    const char* pName = GblFlagsClass_nameFromValue(pClass, value);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return pName;
}
GBL_INLINE GblQuark GblFlags_nameQuark(GblFlags value, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    GblQuark name = GblFlagsClass_nameQuarkFromValue(pClass, value);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return name;
}
GBL_INLINE const char* GblFlags_nick(GblFlags value, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    const char* pNick = GblFlagsClass_nickFromValue(pClass, value);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return pNick;
}
GBL_INLINE GblQuark GblFlags_nickQuark(GblFlags value, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    GblQuark nick = GblFlagsClass_nickQuarkFromValue(pClass, value);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return nick;
}
GBL_INLINE GblBool GblFlags_check(GblFlags value, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_refDefault(type));
    GblBool result = GblFlagsClass_valueCheck(pClass, value);
    GblClass_unrefDefault(GBL_CLASS(pClass));
    return result;
}


GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_FLAGS_H
