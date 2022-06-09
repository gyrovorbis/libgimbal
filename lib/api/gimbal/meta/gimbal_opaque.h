#ifndef GIMBAL_OPAQUE_H
#define GIMBAL_OPAQUE_H

#include "gimbal_primitives.h"

#define GBL_OPAQUE(variant)
#//define

GBL_DECLS_BEGIN


/*
 * Right now a boxed is fucking bigger than a value!!
 *
 *
 * GblVariant v = GBL_VARIANT_INIT;
 *
 * GblOpaque* pOpaque = GBL_OPAQUE(&v); // NULL IF NOT OKAY
 * void* pDupe = GblOpaque_copy(pOpaque);
 *
 * GblEnum* pEnum = GBL_ENUM(&v);
 * if(pEnum) {
 *      std::cout << GblEnum_name(pEnum);
 *      if(GblEnum_check(pEnum)) {
 *          std:: << "fucked.";
 *      }
 *
 *      GblVariant v2;
 *      GblVariant_constructValueCopy(&v, GBL_STRING_TYPE, "lolol");
 *
 *      GblVariant_setMove(&v, &v2);
 *
 *      GblStringBuffer* pString = GBL_STRING(&v);
 *      if(pString) {
 *          GblStringBuffer_append(pString, "trolololol");
 *      }
 *
 *
 *
 * }
 *
 * GblVariant_destruct(&v);
 *
 */

typedef GBL_RESULT (*GblBoxedCopyFn)(void* pSrc, void** ppDst);
typedef GBL_RESULT (*GblBoxedFreeFn)(void* pBoxed);

GblType GblOpaque_registerPointer(const char* pName);
GblType GblOpaque_registerBox(const char* pName,
                              GblBoxedCopyFn pFnCopy,
                              GblBoxedFreeFn pFnFree);


void*       GblOpaque_copy(GblType type, void* pPtr);
GBL_RESULT  GblOpaque_free(GblType type, void* pPtr);


GBL_DECLS_END


#endif // GIMBAL_OPAQUE_H
