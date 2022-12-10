/*! \file
 *  \brief GblVariant structure and related functions
 *  \ingroup meta
 */

#ifndef GIMBAL_VARIANT_H
#define GIMBAL_VARIANT_H

#include "../../core/gimbal_typedefs.h"
#include "../../core/gimbal_ctx.h"
#include "../../strings/gimbal_string_ref.h"
#include "gimbal_pointer.h"
#include "../classes/gimbal_primitives.h"
#include "../classes/gimbal_enum.h"
#include "../classes/gimbal_flags.h"
#include "../classes/gimbal_opaque.h"
#include "../instances/gimbal_instance.h"

#define GBL_VARIANT_INIT    { .type = GBL_INVALID_TYPE }
#define GBL_VARIANT(name)   GblVariant name = GBL_VARIANT_INIT

#define GBL_SELF_TYPE GblVariant

GBL_DECLS_BEGIN

typedef GBL_RESULT (*GblVariantConverterFn)(GBL_CSELF, GblVariant* pOther);

/*! \brief Tagged union generalizing all value-compatible meta types
 * \ingroup meta
*/
//need to postfix everything with '_' for private
typedef struct GblVariant {
    GblType             type;
    union {
        char            character;
        GblBool         boolean;
        uint8_t         u8;
        uint16_t        u16;
        int16_t         i16;
        uint32_t        u32;
        int32_t         i32;
        uint64_t        u64;
        int64_t         i64;
        GblEnum         enumeration;
        GblFlags        flags;
        float           f32;
        double          f64;
        void*           pVoid;
        GblFnPtr        pFnPtr;
        GblStringRef*   pString;
        GblType         typeValue;
        GblInstance*    pInstance;
        GblObject*      pObject;
    };
} GblVariant;

GBL_INLINE GblBool       GblVariant_checkTypeCompatible      (GblType type)                  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_constructDefault         (GBL_SELF,  GblType type)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_constructCopy            (GBL_SELF,
                                                              const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_constructMove            (GBL_SELF,
                                                              GblVariant* pOther)            GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_constructValueCopy       (GBL_SELF,  GblType type, ...)  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_constructValueCopyVaList (GBL_SELF,
                                                              GblType type,
                                                              va_list* pList)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_constructValueMove       (GBL_SELF,  GblType type, ...)  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_constructValueMoveVaList (GBL_SELF,
                                                              GblType type,
                                                              va_list* pList)                GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_constructNil             (GBL_SELF)                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructBool            (GBL_SELF, GblBool value)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructChar            (GBL_SELF, char value)          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructUint8           (GBL_SELF, uint8_t value)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructUint16          (GBL_SELF, uint16_t value)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructInt16           (GBL_SELF, int16_t value)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructUint32          (GBL_SELF, uint32_t value)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructInt32           (GBL_SELF, int32_t value)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructUint64          (GBL_SELF, uint64_t value)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructInt64           (GBL_SELF, int64_t value)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructFloat           (GBL_SELF, float value)         GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructDouble          (GBL_SELF, double value)        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructString          (GBL_SELF, const char* pValue)  GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructStringView      (GBL_SELF, GblStringView value) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructTypeValue       (GBL_SELF, GblType type)        GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_constructEnum            (GBL_SELF,
                                                              GblType type,
                                                              GblEnum value)                 GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_constructFlags           (GBL_SELF,
                                                              GblType type,
                                                              GblFlags value)                GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_constructPointer         (GBL_SELF,
                                                              GblType ptrType,
                                                              void* pValue)                  GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_constructOpaqueCopy      (GBL_SELF,
                                                              GblType opaqueType,
                                                              void*  pValue)                 GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_constructOpaqueMove      (GBL_SELF,
                                                              GblType opaqueType,
                                                              void*  pValue)                 GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_constructInstance        (GBL_SELF, GblInstance* pValue) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructObjectCopy      (GBL_SELF, GblObject* pValue)   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_constructObjectMove      (GBL_SELF, GblObject* pValue)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_destruct                 (GBL_SELF)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_setCopy                  (GBL_SELF,
                                                              const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_setMove                  (GBL_SELF,  GblVariant* pOther) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_setValueCopy             (GBL_SELF,  GblType type, ...)  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_setValueCopyVaList       (GBL_SELF,
                                                              GblType  type,
                                                              va_list* pVarArgs)             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_setValueMove             (GBL_SELF,  GblType type, ...)  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_setValueMoveVaList       (GBL_SELF,
                                                              GblType  type,
                                                              va_list* pVarArgs)             GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_invalidate               (GBL_SELF)                      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_setNil                   (GBL_SELF)                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setBool                  (GBL_SELF, GblBool value)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setChar                  (GBL_SELF, char value)          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setUint8                 (GBL_SELF, uint8_t value)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setUint16                (GBL_SELF, uint16_t value)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setInt16                 (GBL_SELF, int16_t value)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setUint32                (GBL_SELF, uint32_t value)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setInt32                 (GBL_SELF, int32_t value)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setUint64                (GBL_SELF, uint64_t value)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setInt64                 (GBL_SELF, int64_t value)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setFloat                 (GBL_SELF, float value)         GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setDouble                (GBL_SELF, double value)        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setString                (GBL_SELF, const char* pValue)  GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setStringView            (GBL_SELF, GblStringView value) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setTypeValue             (GBL_SELF, GblType value)       GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_setEnum                  (GBL_SELF,
                                                              GblType enumType,
                                                              GblEnum value)                 GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_setFlags                 (GBL_SELF,
                                                              GblType flagsType,
                                                              GblFlags value)                GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_setPointer               (GBL_SELF,
                                                              GblType ptrType,
                                                              void* pValue)                  GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_setOpaqueCopy            (GBL_SELF,
                                                              GblType opaqueType,
                                                              void*  pValue)                 GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_setOpaqueMove            (GBL_SELF,
                                                              GblType opaqueType,
                                                              void*  pValue)                 GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT    GblVariant_setInstance              (GBL_SELF, GblInstance* pValue) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setObjectCopy            (GBL_SELF, GblObject* pValue)   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT    GblVariant_setObjectMove            (GBL_SELF, GblObject* pValue)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_getValueCopy             (GBL_CSELF,  ...)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_getValueCopyVaList       (GBL_CSELF, va_list* pVarArgs)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_getValuePeek             (GBL_CSELF,  ...)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_getValuePeekVaList       (GBL_SELF, va_list* pVarArgs)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_getValueMove             (GBL_SELF,  ...)                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_getValueMoveVaList       (GBL_SELF, va_list* pVarArgs)   GBL_NOEXCEPT;

GBL_INLINE GblType       GblVariant_typeOf                   (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE const char*   GblVariant_typeName                 (GBL_CSELF)                     GBL_NOEXCEPT;

GBL_INLINE GblBool       GblVariant_isValid                  (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblBool       GblVariant_isNil                    (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblBool       GblVariant_getBool                  (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE char          GblVariant_getChar                  (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE uint8_t       GblVariant_getUint8                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE uint16_t      GblVariant_getUint16                (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE int16_t       GblVariant_getInt16                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE uint32_t      GblVariant_getUint32                (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE int32_t       GblVariant_getInt32                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE uint64_t      GblVariant_getUint64                (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE int64_t       GblVariant_getInt64                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblEnum       GblVariant_getEnum                  (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblFlags      GblVariant_getFlags                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE float         GblVariant_getFloat                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE double        GblVariant_getDouble                (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE const char*   GblVariant_getString                (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblStringView GblVariant_getStringView            (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblType       GblVariant_getTypeValue             (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE void*         GblVariant_getPointer               (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE void*         GblVariant_getOpaqueCopy            (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE void*         GblVariant_getOpaqueMove            (GBL_SELF)                      GBL_NOEXCEPT;
GBL_INLINE void*         GblVariant_getOpaquePeek            (GBL_CSELF)                     GBL_NOEXCEPT;

GBL_INLINE GblInstance*  GblVariant_getInstance              (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblObject*    GblVariant_getObjectCopy            (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblObject*    GblVariant_getObjectMove            (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblObject*    GblVariant_getObjectPeek            (GBL_CSELF)                     GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblVariant_toBool                   (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT char          GblVariant_toChar                   (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT uint8_t       GblVariant_toUint8                  (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT uint16_t      GblVariant_toUint16                 (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT int16_t       GblVariant_toInt16                  (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT uint32_t      GblVariant_toUint32                 (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT int32_t       GblVariant_toInt32                  (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT uint64_t      GblVariant_toUint64                 (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT int64_t       GblVariant_toInt64                  (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblEnum       GblVariant_toEnum                   (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblFlags      GblVariant_toFlags                  (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT float         GblVariant_toFloat                  (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT double        GblVariant_toDouble                 (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT void*         GblVariant_toPointer                (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT const char*   GblVariant_toString                 (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblVariant_toStringView             (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblType       GblVariant_toTypeValue              (GBL_SELF)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_registerConverter        (GblType fromType,
                                                              GblType toType,
                                                              GblVariantConverterFn pFnConv) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_unregisterConverter      (GblType fromType,
                                                              GblType toType)                GBL_NOEXCEPT;

GBL_EXPORT GblSize       GblVariant_converterCount           (void)                          GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblVariant_canConvert               (GblType fromType,
                                                              GblType toType)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_convert                  (GBL_CSELF,
                                                              GblVariant* pToVariant)        GBL_NOEXCEPT;

GBL_EXPORT int           GblVariant_compare                  (GBL_CSELF,
                                                              const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_INLINE GblBool       GblVariant_equals                   (GBL_CSELF,
                                                              const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_save                     (GBL_CSELF,
                                                              GblStringBuffer* pString)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_load                     (GBL_SELF,
                                                              const GblStringBuffer* pStr)   GBL_NOEXCEPT;

//========== MACRO OVERLOADS ==========
/// \cond
#define GBL_VARIANT_CONSTRUCT_TABLE  (                                  \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,                              \
        (                                                               \
            (char,                   GblVariant_constructChar),         \
            (uint8_t,                GblVariant_constructUint8),        \
            (uint16_t,               GblVariant_constructUint16),       \
            (int16_t,                GblVariant_constructInt16),        \
            (uint32_t,               GblVariant_constructUint32),       \
            (int32_t,                GblVariant_constructInt32),        \
            (uint64_t,               GblVariant_constructUint64),       \
            (int64_t,                GblVariant_constructInt64),        \
            (float,                  GblVariant_constructFloat),        \
            (double,                 GblVariant_constructDouble),       \
            (const char*,            GblVariant_constructString),       \
            (char*,                  GblVariant_constructString),       \
            (GblStringView,          GblVariant_constructStringView),   \
            (void*,                  GblVariant_constructPointer),      \
            (const void*,            GblVariant_constructPointer),      \
            (const GblVariant*,      GblVariant_constructCopy),         \
            (GblVariant*,            GblVariant_constructCopy)          \
        )                                                               \
    )
#define GblVariant_construct_N(pVariant, ...)   GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_CONSTRUCT_TABLE, GBL_TUPLE_FIRST (__VA_ARGS__))(pVariant, __VA_ARGS__)
#define GblVariant_construct_1(pVariant)        GblVariant_constructNil(pVariant)
/// \endcond
#define GblVariant_construct(...)               GBL_VA_OVERLOAD_CALL(GblVariant_construct, GBL_VA_OVERLOAD_SUFFIXER_1_N, __VA_ARGS__)

/// \cond
#define GBL_VARIANT_SET_TABLE  (                                    \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,                          \
        (                                                           \
            (char,                   GblVariant_setChar),           \
            (uint8_t,                GblVariant_setUint8),          \
            (uint16_t,               GblVariant_setUint16),         \
            (int16_t,                GblVariant_setInt16),          \
            (uint32_t,               GblVariant_setUint32),         \
            (int32_t,                GblVariant_setInt32),          \
            (uint64_t,               GblVariant_setUint64),         \
            (int64_t,                GblVariant_setInt64),          \
            (float,                  GblVariant_setFloat),          \
            (double,                 GblVariant_setDouble),         \
            (char*,                  GblVariant_setString),         \
            (const char*,            GblVariant_setString),         \
            (GblStringView,          GblVariant_setStringView),     \
            (const void*,            GblVariant_setPointer),        \
            (void*,                  GblVariant_setPointer),        \
            (const GblVariant*,      GblVariant_setCopy),           \
            (GblVariant*,            GblVariant_setCopy)            \
        )                                                           \
    )
/// \endcond
#define GblVariant_set(pVariant, ...) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_SET_TABLE, GBL_TUPLE_FIRST(__VA_ARGS__))(pVariant, __VA_ARGS__)



// ========== IMPL ==========
GBL_INLINE GblBool GblVariant_checkTypeCompatible(GblType type) GBL_NOEXCEPT {
    return type != GBL_INVALID_TYPE? GblType_check(type, GBL_IVARIANT_TYPE) : GBL_TRUE;
}

GBL_INLINE GblType GblVariant_typeOf(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->type;
}

GBL_INLINE const char* GblVariant_typeName(GBL_CSELF) GBL_NOEXCEPT {
    return GblType_name(GblVariant_typeOf(pSelf));
}

GBL_INLINE GBL_RESULT GblVariant_constructNil(GBL_SELF) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(pSelf, GBL_NIL_TYPE));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructBool(GBL_SELF, GblBool value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_BOOL_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructChar(GBL_SELF, char value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_CHAR_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructUint8(GBL_SELF, uint8_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_UINT8_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructUint16(GBL_SELF, uint16_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_UINT16_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructInt16(GBL_SELF, int16_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_INT16_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructUint32(GBL_SELF, uint32_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_UINT32_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructInt32(GBL_SELF, int32_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_INT32_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructUint64(GBL_SELF, uint64_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_UINT64_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructInt64(GBL_SELF, int64_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_INT64_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructEnum(GBL_SELF, GblType type, GblEnum value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(type, GBL_ENUM_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, type, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructFlags(GBL_SELF, GblType type, GblFlags value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(type, GBL_FLAGS_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, type, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructFloat(GBL_SELF, float value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_FLOAT_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructDouble(GBL_SELF, double value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_DOUBLE_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructPointer(GBL_SELF, GblType ptrType, void* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY(GBL_POINTER_TYPE_CHECK(ptrType),
                   GBL_RESULT_ERROR_TYPE_MISMATCH);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, ptrType, pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructString(GBL_SELF, const char* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_STRING_TYPE, pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructOpaqueCopy(GBL_SELF, GblType opaqueType, void*  pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(opaqueType, GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, opaqueType, pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructOpaqueMove(GBL_SELF, GblType opaqueType, void* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(opaqueType, GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(pSelf, opaqueType, pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructStringView(GBL_SELF, GblStringView view) GBL_NOEXCEPT {
    return GblVariant_constructString(pSelf, GBL_STRING_VIEW_CSTR(view));
}

GBL_INLINE GBL_RESULT GblVariant_constructTypeValue(GBL_SELF, GblType type) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_TYPE_TYPE, type));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructInstance(GBL_SELF, GblInstance* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_INSTANCE_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructObjectCopy(GBL_SELF, GblObject* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueCopy(pSelf, GBL_INSTANCE_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_constructObjectMove(GBL_SELF, GblObject* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(pSelf, GBL_INSTANCE_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_INLINE GblBool GblVariant_isValid(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf && GblVariant_typeOf(pSelf) != GBL_INVALID_TYPE;
}

GBL_INLINE GblBool GblVariant_isNil(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf && GblVariant_typeOf(pSelf) == GBL_NIL_TYPE;
}

GBL_INLINE GblBool GblVariant_getBool(GBL_CSELF) GBL_NOEXCEPT {
    GblBool value = GBL_FALSE;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_BOOL_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE char GblVariant_getChar(GBL_CSELF) GBL_NOEXCEPT {
    char value = '\0';;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_CHAR_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE uint8_t GblVariant_getUint8(GBL_CSELF) GBL_NOEXCEPT {
    uint8_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_UINT8_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE uint16_t GblVariant_getUint16(GBL_CSELF) GBL_NOEXCEPT {
    uint16_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_UINT16_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE int16_t GblVariant_getInt16(GBL_CSELF) GBL_NOEXCEPT {
    int16_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_INT16_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE uint32_t GblVariant_getUint32(GBL_CSELF) GBL_NOEXCEPT {
    uint32_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_UINT32_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE int32_t GblVariant_getInt32(GBL_CSELF) GBL_NOEXCEPT {
    int32_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_INT32_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE uint64_t GblVariant_getUint64(GBL_CSELF) GBL_NOEXCEPT {
    uint64_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_UINT64_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE int64_t GblVariant_getInt64(GBL_CSELF) GBL_NOEXCEPT {
    int64_t value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_INT64_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE GblEnum GblVariant_getEnum(GBL_CSELF) GBL_NOEXCEPT {
    GblEnum value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_ENUM_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE GblEnum GblVariant_getFlags(GBL_CSELF) GBL_NOEXCEPT {
    GblFlags value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_FLAGS_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE float GblVariant_getFloat(GBL_CSELF) GBL_NOEXCEPT {
    float value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_FLOAT_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE double GblVariant_getDouble(GBL_CSELF) GBL_NOEXCEPT {
    double value = 0;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_DOUBLE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE void* GblVariant_getPointer(GBL_CSELF) GBL_NOEXCEPT {
    void* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_POINTER_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValuePeek(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_INLINE void* GblVariant_getOpaqueCopy(GBL_CSELF) GBL_NOEXCEPT {
    void* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_INLINE void* GblVariant_getOpaqueMove(GBL_SELF) GBL_NOEXCEPT {
    void* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValueMove(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_INLINE void* GblVariant_getOpaquePeek(GBL_CSELF) GBL_NOEXCEPT {
    void* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValuePeek(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_INLINE const char* GblVariant_getString(GBL_CSELF) GBL_NOEXCEPT {
    const char* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_STRING_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValuePeek(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_INLINE GblStringView GblVariant_getStringView(GBL_CSELF) GBL_NOEXCEPT {
    return GblStringRef_view(GblVariant_getString(pSelf));
}

GBL_INLINE GblType GblVariant_getTypeValue(GBL_CSELF) GBL_NOEXCEPT {
    GblType value = GBL_INVALID_TYPE;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_TYPE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValuePeek(pSelf, &value));
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_INLINE GblInstance* GblVariant_getInstance(GBL_CSELF) GBL_NOEXCEPT {
    GblInstance* pValue = GBL_NULL;
    GBL_CTX_BEGIN(GBL_NULL);
    GBL_CTX_VERIFY_TYPE(GblVariant_typeOf(pSelf), GBL_INSTANCE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_getValuePeek(pSelf, &pValue));
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_INLINE GBL_RESULT GblVariant_invalidate(GBL_SELF) GBL_NOEXCEPT {
    if(GblVariant_typeOf(pSelf) != GBL_INVALID_TYPE)
        GblClass_unrefDefault(GblClass_weakRefDefault(GblVariant_typeOf(pSelf)));
    memset(pSelf, 0, sizeof(GblVariant));
    pSelf->type = GBL_INVALID_TYPE;
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_RESULT GblVariant_setNil(GBL_SELF) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(pSelf));
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(pSelf, GBL_NIL_TYPE));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setBool(GBL_SELF, GblBool value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_BOOL_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setChar(GBL_SELF, char value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_CHAR_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setUint8(GBL_SELF, uint8_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_UINT8_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setUint16(GBL_SELF, uint16_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_UINT16_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setInt16(GBL_SELF, int16_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_INT16_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setUint32(GBL_SELF, uint32_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_UINT32_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setInt32(GBL_SELF, int32_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_INT32_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setUint64(GBL_SELF, uint64_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_UINT64_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setInt64(GBL_SELF, int64_t value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_INT64_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setEnum(GBL_SELF,
                                         GblType enumType,
                                         GblEnum value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(enumType, GBL_ENUM_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, enumType, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setFlags(GBL_SELF,
                                          GblType flagsType,
                                          GblFlags value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(flagsType, GBL_FLAGS_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, flagsType, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setFloat(GBL_SELF, float value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_FLOAT_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setDouble(GBL_SELF, double value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_DOUBLE_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setPointer(GBL_SELF,
                                            GblType ptrType,
                                            void* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(ptrType, GBL_POINTER_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, ptrType, pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setOpaqueCopy(GBL_SELF, GblType opaqueType, void*  pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(opaqueType, GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, opaqueType, pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setOpaqueMove(GBL_SELF, GblType opaqueType, void*  pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(opaqueType, GBL_OPAQUE_TYPE);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pSelf, opaqueType, pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setString(GBL_SELF, const char* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_STRING_TYPE, pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setStringView(GBL_SELF, GblStringView value) GBL_NOEXCEPT {
    return GblVariant_setString(pSelf, GBL_STRING_VIEW_CSTR(value));
}

GBL_INLINE GBL_RESULT GblVariant_setTypeValue(GBL_SELF, GblType value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_TYPE_TYPE, value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setInstance(GBL_SELF, GblInstance* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_INSTANCE_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setObjectCopy(GBL_SELF, GblObject* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueCopy(pSelf, GBL_INSTANCE_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblVariant_setObjectMove(GBL_SELF, GblObject* pValue) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pSelf, GBL_INSTANCE_TYPEOF(pValue), pValue));
    GBL_CTX_END();
}

GBL_INLINE GblBool GblVariant_equals(GBL_CSELF, const GblVariant* pOther) GBL_NOEXCEPT {
    return GblVariant_compare(pSelf, pOther) == 0;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_VARIANT_H
