#ifndef GIMBAL_FLAGS_H
#define GIMBAL_FLAGS_H

#include "gimbal_primitives.h"

#define GBL_FLAGS_TYPE                      (GBL_BUILTIN_TYPE(FLAGS))
#define GBL_FLAGS_CLASS_STRUCT              GblFlagsClass
#define GBL_FLAGS_TYPE_CHECK(type)          (GblType_check(type, GBL_FLAGS_TYPE))
#define GBL_FLAGS_CHECK(variant)            (GblVariant_type(&variant) == GBL_FLAGS_TYPE)
#define GBL_FLAGS_CLASS(klass)              (GBL_CLASS_CAST_PREFIX(klass, GBL_FLAGS))
#define GBL_FLAGS_CLASS_CHECK(klass)        (GBL_CLASS_CHECK_PREFIX(klass, GBL_FLAGS))
#define GBL_FLAGS_CLASS_TRY(klass)          (GBL_CLASS_TRY_PREFIX(klass, GBL_FLAGS))
#define GBL_FLAGS_GET_CLASS(variant)        (GBL_FLAGS_CLASS(GblClass_peek(GblVariant_type(&variant))))
#define GBL_FLAGS_TRY_CLASS(variant)        (GBL_FLAGS_CLASS_TRY(GblClass_peek(GblVariant_type(&variant))))

#define GBL_FLAGS_ENTRY(flagsValue, nick)   { flagsValue, GblQuark_fromStatic(#flagsValue), GblQuark_fromStatic(nick) }

#define SELF    GblFlagsClass* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

typedef struct GblFlagsEntry {
    GblFlags    value;
    GblQuark    name;
    GblQuark    nick;
} GblFlagsEntry;

typedef struct GblFlagsClass {
    GblPrimitiveClass   base;
    GblFlags            mask;
    uint16_t            entryCount;
    GblFlagsEntry*      pEntries;
} GblFlagsClass;

GBL_EXPORT uint16_t     GblFlagsClass_entryCount        (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT const GblFlagsEntry*
                        GblFlagsClass_entries           (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT const GblFlagsEntry*
                        GblFlagsClass_entryFromIndex    (CSELF, uint16_t index)         GBL_NOEXCEPT;

GBL_EXPORT const char*  GblFlagsClass_nameFromIndex     (CSELF, uint16_t index)         GBL_NOEXCEPT;
GBL_EXPORT const char*  GblFlagsClass_nameFromValue     (CSELF, GblFlags value)         GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblFlagsClass_nameQuarkFromIndex(CSELF, uint16_t index)         GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblFlagsClass_nameQuarkFromValue(CSELF, GblFlags value)         GBL_NOEXCEPT;

GBL_EXPORT const char*  GblFlagsClass_nickFromIndex     (CSELF, uint16_t index)         GBL_NOEXCEPT;
GBL_EXPORT const char*  GblFlagsClass_nickFromvalue     (CSELF, GblFlags value)         GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblFlagsClass_nickQuarkFromIndex(CSELF, uint16_t index)         GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblFlagsClass_nickQuarkFromValue(CSELF, GblFlags value)         GBL_NOEXCEPT;

GBL_EXPORT GblFlags     GblFlagsClass_valueFromIndex    (CSELF, uint16_t index)         GBL_NOEXCEPT;
GBL_EXPORT GblFlags     GblFlagsClass_valueFromName     (CSELF, const char* pName)      GBL_NOEXCEPT;
GBL_EXPORT GblFlags     GblFlagsClass_valueFromNick     (CSELF, const char* pNick)      GBL_NOEXCEPT;
GBL_EXPORT GblFlags     GblFlagsClass_valueFromNameQuark(CSELF, GblQuark name)          GBL_NOEXCEPT;
GBL_EXPORT GblFlags     GblFlagsClass_valueFromNickQuark(CSELF, GblQuark nick)          GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblFlagsClass_valueCheck        (CSELF, GblFlags flag)          GBL_NOEXCEPT;

GBL_EXPORT GblType      GblFlagsClass_register          (const char*          pName,
                                                         const GblFlagsEntry* pEntries) GBL_NOEXCEPT;

GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_FLAGS_H
