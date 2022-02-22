#ifndef GIMBAL_ICONTEXT_H
#define GIMBAL_ICONTEXT_H

#include "gimbal_iallocator.h"
#include "gimbal_ievent_handler.h"
#include "gimbal_ilast_error.h"
#include "gimbal_ilogger.h"
#include "gimbal_iparent.h"
#include "gimbal_ichild.h"
#include "gimbal_inamed.h"
#include "gimbal_iuserdata.h"

#define IContext_IFACE

#define IContext_EXTENDS (IAllocator)
#define IContext_EXTENDS (IEventHandler)
#define IContext_EXTENDS (ILastError)
#define IContext_EXTENDS (ILogger)
#define IContext_EXTENDS (IParent)
#define IContext_EXTENDS (IChild)
#define IContext_EXTENDS (INamed)
#define IContext_EXTENDS (IUserdata)

interface(IContext)

// CONSIDER PUTTING RANDOM NUMBER GENERATOR SEED IN THIS BITCH
typedef struct GblContext_ {
    GblContextObject_           baseContextObject;
    const char                  name[200]; //GblString?
    GblCallRecord           lastRecord;
    //GblModule
    //GblRuntime
#if GBL_CONFIG_EXT_CONTEXT_DEFAULT_ENABLED
    uint32_t logStackDepth;
#endif
} GblContext;


GBL_API gblVersion         (GblVersion* pVersion, const char** ppString); //hard-compiled


// anything that doesn't return data could just respond to event?!?!


// combine
GBL_API gblContextLogWrite       (GblContext hCtx, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs);
GBL_API gblContextLogPush        (GblContext hCtx, const GblStackFrame* pFrame);
GBL_API gblContextLogPop         (GblContext hCtx, const GblStackFrame* pFrame, uint32_t count);

// combine
GBL_API gblContextMemAlloc       (GblContext hCtx, const GblStackFrame* pFrame, GblSize size, GblSize alignment, const char* pDebugInfoStr, void** ppData);
GBL_API gblContextMemRealloc     (GblContext hCtx, const GblStackFrame* pFrame, void* pData, GblSize newSize, GblSize newAlign, void** pNewData);
GBL_API gblContextMemFree        (GblContext hCtx, const GblStackFrame* pFrame, void* pData);



#endif // GIMBAL_ICONTEXT_H
