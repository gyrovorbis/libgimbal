#ifndef GIMBAL_FLAGS_H
#define GIMBAL_FLAGS_H

#include "gimbal_primitives.h"
#include "../strings/gimbal_quark.h"

#define GBL_FLAGS_TYPE                      (GBL_BUILTIN_TYPE(FLAGS))
#define GBL_FLAGS_CLASS_STRUCT              GblFlagsClass
#define GBL_FLAGS_TYPE_CHECK(type)          (GblType_check(type, GBL_FLAGS_TYPE))
#define GBL_FLAGS_CHECK(variant)            (GblType_check(GblVariant_type(&variant). GBL_FLAGS_TYPE))

#define GBL_FLAGS_CLASS(klass)              (GBL_CLASS_CAST_PREFIX(klass, GBL_FLAGS))
#define GBL_FLAGS_CLASS_CHECK(klass)        (GBL_CLASS_CHECK_PREFIX(klass, GBL_FLAGS))
#define GBL_FLAGS_CLASS_TRY(klass)          (GBL_CLASS_TRY_PREFIX(klass, GBL_FLAGS))

#define GBL_FLAGS_GET_CLASS(variant)        (GBL_FLAGS_CLASS(GblClass_peek(GblVariant_type(&variant))))
#define GBL_FLAGS_TRY_CLASS(variant)        (GBL_FLAGS_CLASS_TRY(GblClass_peek(GblVariant_type(&variant))))

#define GBL_FLAGS_ENTRY(flagsValue, nick)   { flagsValue, #flagsValue, nick }
#define GBL_FLAGS_ENTRY_LAST()              { 0, NULL, NULL }

#define SELF    GblFlagsClass* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

typedef struct GblFlagEntry {
    GblFlags        value;
    const char*     pName;
    const char*     pNick;
} GblFlagEntry;

typedef struct GblFlagsClass {
    GblPrimitiveClass   base;
    GblFlags            valueMask;
    uint16_t            entryCount;
} GblFlagsClass;

// ========== GblFlagsClass ==========

GBL_INLINE const char*  GblFlagsClass_nameFromIndex     (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_INLINE const char*  GblFlagsClass_nameFromValue     (CSELF, GblFlags value)             GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblFlagsClass_nameQuarkFromIndex(CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblFlagsClass_nameQuarkFromValue(CSELF, GblFlags value)             GBL_NOEXCEPT;

GBL_INLINE const char*  GblFlagsClass_nickFromIndex     (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_INLINE const char*  GblFlagsClass_nickFromValue     (CSELF, GblFlags value)             GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblFlagsClass_nickQuarkFromIndex(CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblFlagsClass_nickQuarkFromValue(CSELF, GblFlags value)             GBL_NOEXCEPT;

GBL_EXPORT GblFlags     GblFlagsClass_valueFromIndex    (CSELF, uint16_t index)             GBL_NOEXCEPT;
GBL_INLINE GblFlags     GblFlagsClass_valueFromName     (CSELF, const char* pName)          GBL_NOEXCEPT;
GBL_INLINE GblFlags     GblFlagsClass_valueFromNick     (CSELF, const char* pNick)          GBL_NOEXCEPT;
GBL_EXPORT GblFlags     GblFlagsClass_valueFromNameQuark(CSELF, GblQuark name)              GBL_NOEXCEPT;
GBL_EXPORT GblFlags     GblFlagsClass_valueFromNickQuark(CSELF, GblQuark nick)              GBL_NOEXCEPT;
GBL_EXPORT GblFlags     GblFlagsClass_valueFromString   (CSELF, const char* pString)        GBL_NOEXCEPT;

GBL_INLINE GblBool      GblFlagsClass_valueCheck        (CSELF, GblFlags value)             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblFlagsClass_valueAppendString (CSELF,
                                                         GblFlags value,
                                                         GblStringBuffer* pStr)             GBL_NOEXCEPT;

// ========== GblFlags ==========

GBL_EXPORT GblType      GblFlags_register               (const char*          pName,
                                                         const GblFlagEntry*  pEntries)     GBL_NOEXCEPT;

GBL_INLINE GblFlags     GblFlags_fromName               (const char* pName, GblType type)   GBL_NOEXCEPT;
GBL_INLINE GblFlags     GblFlags_fromNameQuark          (GblQuark name, GblType type)       GBL_NOEXCEPT;
GBL_INLINE GblFlags     GblFlags_fromNick               (const char* pNick, GblType type)   GBL_NOEXCEPT;
GBL_INLINE GblFlags     GblFlags_fromNickQuark          (GblQuark nick, GblType type)       GBL_NOEXCEPT;
GBL_INLINE GblFlags     GblFlags_fromString             (const char* pName, GblType type)   GBL_NOEXCEPT;

GBL_INLINE const char*  GblFlags_name                   (GblFlags value, GblType type)      GBL_NOEXCEPT;
GBL_INLINE GblQuark     GblFlags_nameQuark              (GblFlags value, GblType type)      GBL_NOEXCEPT;
GBL_INLINE const char*  GblFlags_nick                   (GblFlags value, GblType type)      GBL_NOEXCEPT;
GBL_INLINE GblQuark     GblFlags_nickQuark              (GblFlags value, GblType type)      GBL_NOEXCEPT;

GBL_INLINE GblBool      GblFlags_check                  (GblFlags value, GblType type)      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblFlags_appendString           (GblFlags value,
                                                         GblType type,
                                                         GblStringBuffer* pBuffer)          GBL_NOEXCEPT;

// ========== IMPL ==========

// ------ GblFlagsClass ------

GBL_INLINE const char* GblFlagsClass_nameFromIndex(CSELF, uint16_t index) GBL_NOEXCEPT {
    return GblQuark_toString(GblFlagsClass_nameQuarkFromIndex(pSelf, index));
}
GBL_INLINE const char* GblFlagsClass_nickFromIndex(CSELF, uint16_t index) GBL_NOEXCEPT {
    return GblQuark_toString(GblFlagsClass_nickQuarkFromIndex(pSelf, index));
}
GBL_INLINE const char* GblFlagsClass_nameFromValue(CSELF, GblFlags value) GBL_NOEXCEPT {
    return GblQuark_toString(GblFlagsClass_nameQuarkFromValue(pSelf, value));
}
GBL_INLINE const char* GblFlagsClass_nickFromValue(CSELF, GblFlags value) GBL_NOEXCEPT {
    return GblQuark_toString(GblFlagsClass_nickQuarkFromValue(pSelf, value));
}
GBL_INLINE GblFlags GblFlagsClass_valueFromName(CSELF, const char* pString) GBL_NOEXCEPT {
    GblFlags value = 0;
    GblQuark quark = GblQuark_tryString(pString);
    if(quark != GBL_QUARK_INVALID) {
        value = GblFlagsClass_valueFromNameQuark(pSelf, quark);
    }
    return value;
}
GBL_INLINE GblFlags GblFlagsClass_valueFromNick(CSELF, const char* pString) GBL_NOEXCEPT {
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
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    GblFlags value = GblFlagsClass_valueFromName(pClass, pName);
    GblClass_unref(GBL_CLASS(pClass));
    return value;
}
GBL_INLINE GblFlags GblFlags_fromNameQuark(GblQuark name, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    GblFlags value = GblFlagsClass_valueFromNameQuark(pClass, name);
    GblClass_unref(GBL_CLASS(pClass));
    return value;
}
GBL_INLINE GblFlags GblFlags_fromNick(const char* pNick, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    GblFlags value = GblFlagsClass_valueFromNick(pClass, pNick);
    GblClass_unref(GBL_CLASS(pClass));
    return value;
}
GBL_INLINE GblFlags GblFlags_fromNickQuark(GblQuark nick, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    GblFlags value = GblFlagsClass_valueFromNickQuark(pClass, nick);
    GblClass_unref(GBL_CLASS(pClass));
    return value;
}

GBL_INLINE GblFlags GblFlags_fromString(const char* pName, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    GblFlags value = GblFlagsClass_valueFromString(pClass, pName);
    GblClass_unref(GBL_CLASS(pClass));
    return value;
}

GBL_INLINE const char* GblFlags_name(GblFlags value, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    const char* pName = GblFlagsClass_nameFromValue(pClass, value);
    GblClass_unref(GBL_CLASS(pClass));
    return pName;
}
GBL_INLINE GblQuark GblFlags_nameQuark(GblFlags value, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    GblQuark name = GblFlagsClass_nameQuarkFromValue(pClass, value);
    GblClass_unref(GBL_CLASS(pClass));
    return name;
}
GBL_INLINE const char* GblFlags_nick(GblFlags value, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    const char* pNick = GblFlagsClass_nickFromValue(pClass, value);
    GblClass_unref(GBL_CLASS(pClass));
    return pNick;
}
GBL_INLINE GblQuark GblFlags_nickQuark(GblFlags value, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    GblQuark nick = GblFlagsClass_nickQuarkFromValue(pClass, value);
    GblClass_unref(GBL_CLASS(pClass));
    return nick;
}
GBL_INLINE GblBool GblFlags_check(GblFlags value, GblType type) GBL_NOEXCEPT {
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    GblBool result = GblFlagsClass_valueCheck(pClass, value);
    GblClass_unref(GBL_CLASS(pClass));
    return result;
}

GBL_INLINE GBL_RESULT GblFlags_appendString(GblFlags value,
                                            GblType type,
                                            GblStringBuffer* pBuffer) GBL_NOEXCEPT
{
    GBL_API_BEGIN(NULL);
    GblFlagsClass* pClass = GBL_FLAGS_CLASS(GblClass_ref(type));
    GBL_API_CALL(GblFlagsClass_valueAppendString(pClass, value, pBuffer));
    GblClass_unref(GBL_CLASS(pClass));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_FLAGS_H
