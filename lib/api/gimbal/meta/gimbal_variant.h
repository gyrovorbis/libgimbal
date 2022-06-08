#ifndef GIMBAL_VARIANT_H
#define GIMBAL_VARIANT_H

#include "../types/gimbal_typedefs.h"
#include "../core/gimbal_api_frame.h"

#define SELF    GblVariant* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

#define GBL_VARIANT_INIT { .type = GBL_NIL_TYPE }


GBL_EXPORT GblType      GblVariant_type                     (CSELF)                         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_constructDefault         (SELF,  GblType type)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_constructCopy            (SELF,
                                                             const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_constructMove            (SELF,
                                                             GblVariant* pOther)            GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_constructValueCopy       (SELF,  GblType type, ...)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_constructValueMove       (SELF,  GblType type, ...)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_constructValueCopyVaList (SELF,
                                                             GblType type,
                                                             va_list* pList)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_destruct                 (SELF)                          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_setCopy                  (SELF,
                                                             const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_setMove                  (SELF,  GblVariant* pOther)     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVariant_setValueCopy             (SELF,  GblType type, ...)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVariant_setValueMove             (SELF,  GblType type, ...)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_getValueCopy             (CSELF,  ...)                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVariant_getValueCopyVaList       (CSELF, va_list* pVarArgs)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVariant_getValuePeek             (CSELF,  ...)                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVariant_getValuePeekVaList       (SELF, va_list* pVarArgs)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVariant_getValueTake             (SELF,  ...)                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVariant_getValueTakeVaList       (SELF, va_list* pVarArgs)       GBL_NOEXCEPT;

GBL_EXPORT GblInt       GblVariant_compare                  (CSELF,
                                                             const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_convert                  (CSELF,
                                                             GblType toType,
                                                             GblVariant* pToVariant)        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_save                     (CSELF,
                                                             GblStringBuffer* pString)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVariant_load                     (SELF,
                                                             const GblStringBuffer* pStr)   GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblVariant_constructNil             (SELF)                          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructBool            (SELF, GblBool value)           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructChar            (SELF, char value)              GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructInt             (SELF, GblInt value)            GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructUint            (SELF, GblUint value)           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructEnum            (SELF, GblEnum value)           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructFlags           (SELF, GblFlags value)          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructFloat           (SELF, float value)             GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructtDouble         (SELF, double value)            GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructPointer         (SELF, void* pValue)            GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructString          (SELF, const char* pValue)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructInstance        (SELF, GblInstance* pValue)     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructObject          (SELF, GblObject* pValue)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_constructObjectMove      (SELF, GblObject* pValue)       GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblVariant_constructBoxed           (SELF,
                                                             GblType type,
                                                             void* pBoxed)                  GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblVariant_constructBoxedMove       (SELF,
                                                             GblType type,
                                                             void* pBoxed)                  GBL_NOEXCEPT;

GBL_INLINE GblBool      GblVariant_getBool                  (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE char         GblVariant_getChar                  (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblInt       GblVariant_getInt                   (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblUint      GblVariant_getUint                  (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblEnum      GblVariant_getEnum                  (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblFlags     GblVariant_getFlags                 (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE float        GblVariant_getFloat                 (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE double       GblVariant_getDouble                (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE void*        GblVariant_getPointer               (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE const char*  GblVariant_getString                (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblInstance* GblVariant_getInstance              (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblObject*   GblVariant_getObjectCopy            (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblObject*   GblVariant_getObjectTake            (SELF)                          GBL_NOEXCEPT;
GBL_INLINE GblObject*   GblVariant_getObjectPeek            (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE void*        GblVariant_getBoxedCopy             (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE void*        GblVariant_getBoxedTake             (SELF)                          GBL_NOEXCEPT;
GBL_INLINE void*        GblVariant_getBoxedPeek             (CSELF)                         GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblVariant_setNil                   (SELF)                          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setBool                  (SELF, GblBool value)           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setChar                  (SELF, char value)              GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setInt                   (SELF, GblInt value)            GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setUint                  (SELF, GblUint value)           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setEnum                  (SELF, GblEnum value)           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setFlags                 (SELF, GblFlags value)          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setFloat                 (SELF, float value)             GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setDouble                (SELF, double value)            GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setPointer               (SELF, void* pValue)            GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setString                (SELF, const char* pValue)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setInstance              (SELF, GblInstance* pValue)     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setObjectCopy            (SELF, GblObject* pValue)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setObjectMove            (SELF, GblObject* pValue)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setBoxedCopy             (SELF, void* pValue)            GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVariant_setBoxedtMove            (SELF, void* pValue)            GBL_NOEXCEPT;

// ========== IMPL ==========
GBL_INLINE GBL_RESULT GblVariant_constructNil(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    //pSelf->
    GBL_API_END();
}
GBL_INLINE GBL_RESULT GblVariant_constructBool(SELF, GblBool value) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructChar(SELF, char value) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructInt(SELF, GblInt value) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructUint(SELF, GblUint value) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructEnum(SELF, GblEnum value) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructFlags(SELF, GblFlags value) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructFloat(SELF, float value) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructtDouble(SELF, double value) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructPointer(SELF, void* pValue) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructString(SELF, const char* pValue) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructInstance(SELF, GblInstance* pValue) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructObject(SELF, GblObject* pValue) GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT GblVariant_constructObjectMove(SELF, GblObject* pValue) GBL_NOEXCEPT;


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_VARIANT_H
