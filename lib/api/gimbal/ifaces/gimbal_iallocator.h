#ifndef GIMBAL_IALLOCATOR_H
#define GIMBAL_IALLOCATOR_H

#include "gimbal_icomponent.h"

#define IAllocator_IFACE \
    vfunc(GBL_RESULT, alloc,    VSelf, const GblStackFrame* pFrame, GblSize size, GblSize align, const char* pDbgStr, void** ppData) \
    vfunc(GBL_RESULT, realloc,  VSelf, const GblStackFrame* pFrame, void* pData, GblSize newSize, GblSize newAlign, void** ppNewPtr) \
    vfunc(GBL_RESULT, free,     VSelf, const GblStackFrame* pFrame, void* pData)

#define IAllocator_EXTENDS (IComponent)

interface(IAllocator)

#endif // GIMBAL_IALLOCATOR_H
