#ifndef GIMBAL_WEAK_REF_H
#define GIMBAL_WEAK_REF_H

#include "gimbal_type.h"

#ifdef __cplusplus
extern "C" {
#endif



/*
  Refcounting
*/
GBL_RESULT gblObjectRef(                GblObject* pObject);
GBL_RESULT gblObjectUnref               (GblObject* pObject);

/*
 * Weak References
 */
typedef void (*GblWeakRefNotifyFn)      (void* pData, GblObject* pPrevAddress);

GBL_RESULT gblObjectWeakRef             (GblObject*          pObject,
                                        GblWeakRefNotifyFn  pFnNotifier,
                                        void*               pData);

GBL_RESULT gblObjectWeakUnref           (GblObject*        pObject,
                                        GblWeakRefNotifyFn  pFnNotifier,
                                        void*               pData);

GBL_RESULT gblObjectAddWeakPointer      (GblObject*   pObject,
                                        void*        pWeakPointerLocation);

GBL_RESULT gblObjectRemoveWeakPointer   (GblObject*   pObject,
                                        void**        ppWeakPointerLocation);
/*
 * Cycle handling
 */
GBL_RESULT gblObjectRunDispose          (GblObject*   pObject);


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_WEAK_REF_H
