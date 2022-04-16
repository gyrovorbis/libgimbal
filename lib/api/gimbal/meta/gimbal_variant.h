#ifndef GIMBAL_VARIANT_H
#define GIMBAL_VARIANT_H

#include "../types/gimbal_typedefs.h"


#define SELF    GblVariant* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

#define GBL_VARIANT_INIT { .type = GBL_TYPE_NIL }


GBL_EXPORT GblType  GblVariant_type                (CSELF)                                           GBL_NOEXCEPT;

GBL_API             GblVariant_constructCopy       (SELF,  const GblVariant* pOther)                 GBL_NOEXCEPT;
GBL_API             GblVariant_constructMove       (SELF,  GblVariant* pOther)                       GBL_NOEXCEPT;
GBL_API             GblVariant_setCopy             (SELF,  const GblVariant* pOther)                 GBL_NOEXCEPT;
GBL_API             GblVariant_setMove             (SELF,  GblVariant* pOther)                       GBL_NOEXCEPT;
GBL_API             GblVariant_destruct            (SELF)                                            GBL_NOEXCEPT;
GBL_API             GblVariant_save                (CSELF, GblString* pString)                       GBL_NOEXCEPT;
GBL_API             GblVariant_load                (SELF, const GblString* pString)                  GBL_NOEXCEPT;

GBL_API             GblVariant_constructDefault    (SELF,  GblType type)                             GBL_NOEXCEPT;

GBL_API             GblVariant_constructValueCopy  (SELF,  GblType type, ...)                        GBL_NOEXCEPT;
GBL_API             GblVariant_constructValueCopyVaList(SELF, GblType type, va_list* pList)            GBL_NOEXCEPT;

GBL_API             GblVariant_constructValueMove  (SELF,  GblType type, ...)                        GBL_NOEXCEPT;
GBL_API             GblVariant_setValueCopy        (SELF,  GblType type, ...)                        GBL_NOEXCEPT;
GBL_API             GblVariant_setValueMove        (SELF,  GblType type, ...)                        GBL_NOEXCEPT;
GBL_API             GblVariant_getValueCopy        (CSELF,  ...)                                     GBL_NOEXCEPT;
GBL_API             GblVariant_getValueCopyVaList  (CSELF, va_list* pVarArgs)                        GBL_NOEXCEPT;
GBL_API             GblVariant_getValuePeek        (CSELF,  ...)                                     GBL_NOEXCEPT;
GBL_API             GblVariant_getValuePeekVaList  (SELF, va_list* pVarArgs)                         GBL_NOEXCEPT;
GBL_API             GblVariant_getValueTake        (SELF,  ...)                                      GBL_NOEXCEPT;
GBL_API             GblVariant_getValueTakeVaList  (SELF, va_list* pVarArgs)                         GBL_NOEXCEPT;

GBL_EXPORT GblInt   GblVariant_compare             (CSELF, const GblVariant* pOther)                 GBL_NOEXCEPT;
GBL_API             GblVariant_convert             (CSELF, GblType toType, GblVariant* pToVariant)   GBL_NOEXCEPT;





































#if 0// === HELPER UTILS ====
GBL_API             GblVariant_constructFloat(GblVariant* pSelf, float val);
GBL_API             GblVariant_setFloat(GblVariant* pSelf, float val);
GBL_EXPORT float    GblVariant_getFloat(GblVariant* pSelf, GBL_RESULT* pResult);

GBL_API             GblVariant_constructStringCopy(GblVariant* pSelf, const GblString* pString)     GBL_NOEXCEPT;
GBL_API             GblVariant_constructStringMove(GblVariant* pSelf, GblString* pString)           GBL_NOEXCEPT;

GBL_API             GblVariant_constructCStrCopy(GblVariant* pSelf, const char* pString)            GBL_NOEXCEPT;
GBL_API             GblVariant_constructCStrMove(GblVariant* pSelf, const char* pString)            GBL_NOEXCEPT;
GBL_API             GblVariant_setCStrCopy(GblVariant* pSelf, const char* pString)                  GBL_NOEXCEPT;
GBL_API             GblVariant_setCStrMove(GblVariant* pSelf, const char* pString)                  GBL_NOEXCEPT;

GBL_EXPORT const char* GblVariant_getCStrCopy(const GblVariant* pSelf, GBL_RESULT* pResult)         GBL_NOEXCEPT;
GBL_EXPORT const char* GblVariant_getCStrMove(GblVariant* pSelf, GBL_RESULT* pResult)               GBL_NOEXCEPT;

GBL_EXPORT GblString* GblVariant_getStringCopy(const GblVariant* pSelf)                     GBL_NOEXCEPT;
GBL_EXPORT GblString* GblVariant_getStringPeek(const GblVariant* pSelf) GBL_NOEXCEPT;

GblVariant_takeString
#endif

/*
 * GblSize size = 0;
 * const char* pBuff = malloc(size);
 * GblVariant buffVariant = GBL_VARIANT_INIT;
 * GblVariant_constructValueMove(size, pBuff);
 *
 * GblBuffer* pBuffer;
 * GblVariant_getValue(&buffVariant, &pBuffer);
 *
 * GblBuffer* pBuffer;
 * GblVariant_takeValue(&buffVariant, &pBuffer);
 */

GBL_EXPORT

#if 0

//GBL_API GblVariant_setString();
//GBL_API GblVariant_getInt();

GblVariant_getValue()


GBL_EXPORT GblBool  GblVariant_typeCompatible       (CSELF, const GblVariant* pOther)                GBL_NOEXCEPT;

GblVariant_assignString();
GblVariant_assignUint();

GBL_VARIANT_OP_CMP_TYPE


GBL_API GblVariant_initNil(SELF) {
    return GblVariant_constructDefault(pSelf, GBL_TYPE_NIL);
}
GBL_API GblVariant_fromString(SELF, const char* pString, GblSize count) {
    return GblVariant_constructValue(pSelf, GBL_TYPE_STRING, pString, count);
}
GBL_API GblVariant_constructNil();
GBL_API GblVariant_valueGetString();
GBL_API GblVariant_valueSetString();

const char* GblVariant_asString(SELF) {
    const char* pResult = NULL;
    if(GblVariant_type(pSelf) == GBL_TYPE_STRING);
    GblVariant_valueGet(pSelf, &pResult);
    return pResult;
}
#endif

#if 0
GBL_API GblVariant_constructWithValues(SELF, GblType type, ...);
GBL_EXPORT GblVariant_setValues(SELF, ...);
GBL_EXPORT GblVariant_getValues(CSELF, ...);

GblVariant fVar = GBL_VARIANT_INIT;
GblVariant_constructWithValue(&fVar, GBL_TYPE_FLOAT, 32.0f); //pVaFmt
GblVariant_setValue(&fVar, 48.0f);
float floater;
GblVariant_getValue(&fVar, &floater);

GblVariant sVar = GBL_VARIANT_INIT;
GblVariant_constructWithValue(&sVar, GBL_TYPE_STRING, "char array", 4); //array + length
GblVariant_setValue(&sVar, "nuther array", 21);
GblString pStr;
const char* pArray;
GblVariant_getValue(&sVar, &charArray);




#define GblVariant_constructValue   VA_ARG_OVERLOAD_SHIT
GBL_API GblVariant_constructValue_uint16();
GBL_API GblVariant_constructValue_bool();
GBL_API GblVariant_constructValue_string();


GBL_API GblVariant_assignValue_uint16()
GBL_API GblVariant_assignValue_string();
GBL_API GblVariant_assignValue_instance()

uint16_t GblVariant_getValue_uint16();
string   GblVariant_getValue_string();

bool GblVariant_toValue_bool()
uint16_t GblVariant_toValue_u16();


GBL_EXPORT GblInt GblVariant_getString()




#endif

//GBL_API GblIVariant_compare(GblVariant* pSelf, const GblVariant* pOther, GblInt* pCmpResult) GBL_NOEXCEPT;


/*
GblVariant v = GblVariant_init(GBL_TYPE_INTEGER, 3);



*/


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_VARIANT_H
