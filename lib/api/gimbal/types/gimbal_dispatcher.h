#ifndef GIMBAL_DISPATCHER_H
#define GIMBAL_DISPATCHER_H

#include "../containers/gimbal_hashmap.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef GBL_RESULT (*GblBoxedDestructFn)(void* pBoxed);
typedef GBL_RESULT (*GblBoxedCopyFn)    (const void* pSrc, void** ppDst);


//GblString constructor so move semantics can be used
GBL_RESULT gblTypeRegisterBoxed(const char*         pTypeName,
                                GblBoxedDestructFn  pFnDestructor,
                                GblBoxedCopyFn      pFnCopy);

GBL_RESULT gblBoxedFree         (GblType boxedType, void* pBoxed);
GBL_RESULT gblBoxedCopy         (GblType boxedType, const void* pSrc, void** ppDst);


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_DISPATCHER_H
