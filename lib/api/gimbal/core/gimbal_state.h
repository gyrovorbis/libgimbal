#ifndef GIMBAL_STATE_H
#define GIMBAL_STATE_H

#include <gimbal/types/gimbal_typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

GBL_API gblStateContextGet(GblContext* phCtx);
GBL_API gblStateContextSet(GblContext hCtx);



#ifdef __cplusplus
}
#endif

#endif // GIMBAL_STATE_H
