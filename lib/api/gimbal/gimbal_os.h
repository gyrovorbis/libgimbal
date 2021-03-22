#ifndef EVMU_OS_H
#define EVMU_OS_H

#include <evmu/evmu_defines.h>

#ifdef __cplusplus
extern "C" {
#endif

EVMU_FORWARD_DECLARE_STRUCT(EVMUContext);


EVMU_API EVMU_RESULT    evmuEventHandler_(const EVMUContext* pCtx,
                                  const EVMUEvent* pEvent,
                                  EVMU_DEVICE_EVENT_STATUS* pStatus);


EVMU_API EVMU_RESULT    evmuMalloc_(const EVMUContext* pCtx,
                                    EVMUSize_t size,
                                    EVMUSize_t alignment,
                                    void** ppData);
EVMU_API EVMU_RESULT    evmuRealloc_(const EVMUContext* pCtx,
                                     void* pExistingData,
                                     EVMUSize_t newSize,
                                     EVMUSize_t newAlignment,
                                     void* ppNewData);
EVMU_API EVMU_RESULT    evmuFree_(const EVMUContext* pCtx,
                                  void* pData);

EVMU_API EVMU_RESULT    evmuLogWrite_(const EVMUContext* pCtx,
                                      const char* pFile,
                                      const char* pFunction,
                                      EVMUSize_t line,
                                      EVMU_LOG_LEVEL level,
                                      const char* pFmt, ...);

EVMU_API EVMU_RESULT    evmuLogPush_(const EVMUContext* pCtx);
EVMU_API EVMU_RESULT    evmuLogPop_(const EVMUContext* pCtx,
                                    uint32_t count);




EVMU_FORWARD_DECLARE_STRUCT(EVMUContext);
EVMU_FORWARD_DECLARE_STRUCT(EVMUDevice);

typedef struct EVMUSourceLoc {
    const char* pFile;
    const char* pFunc;
    uint64_t    line;
} EVMUSourceLoc;

typedef struct EVMUApiCookie {
#ifdef EVMU_RESULT_CALL_STACK_TRACKING
    EVMUSourceLoc           sourceLoc;
#endif
    VMUContext*             pContext;
    VMUDevice*              pDevice;
    struct EVMUApiCookie*   pParent;
    EVMU_RESULT             result;
    uint32_t                stackDepth;
} EVMUApiCookie;

// override all of these to take a context?




#ifdef __cplusplus
}
#endif

#endif // EVMU_OS_H
