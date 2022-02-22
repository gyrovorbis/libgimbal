#ifndef GIMBAL_ILAST_ERROR_H
#define GIMBAL_ILAST_ERROR_H

#include "gimbal_icomponent.h"

#define ILastError_IFACE \
    vfunc(GBL_RESULT, recordGet, const VSelf, const GblCallRecord** ppRecord) \
    vfunc(GBL_RESULT, recordSet, VSelf,       const GblCallRecord* pRecord)

#define ILastError_EXTENDS (IComponent)

interface(ILastError)

#endif // GIMBAL_ILAST_ERROR_H
