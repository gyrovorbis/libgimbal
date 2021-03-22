#ifndef GIMBAL_EXT_H
#define GIMBAL_EXT_H

#include "gimbal_types.h"

typedef GBL_RESULT (*GblExtLogWriteFn)(void*, GBL_LOG_LEVEL, const char*, va_list);
typedef GBL_RESULT (*GblExtLogPushFn)(void*);
typedef GBL_RESULT (*GblExtLogPopFn)(void*, uint32_t);

//Custom allocators
typedef GBL_RESULT (*GblExtMallocFn)(void*, GblSize, GblSize, void**);
typedef GBL_RESULT (*GblExtReallocFn)(void*, const void*, GblSize, GblSize, void**);
typedef GBL_RESULT (*GblExtFreeFn)(void*, void*);

//define all extension points and shit in one centralized location



/*Call everything through these top-level macros so that user
  is able to override them at compile-time + use inline functions



GBL_EXT_EVENT_HANDLER
    a. user-defined macro (compile-time polymorphic)
    b. gblExtEventHandler
        1. user-defined implementation (link-time polymorphic)
        2. dynamic implementation   (run-time polymorphic)
            a. calls context function pointer
            b. calls event handler
            c. falls back to default */

#ifndef GBL_EXT_EVENT_HANDLER
#   define GBL_EXT_EVENT_HANDLER    gblExtEventHandler
#endif

/* ==== SERIOUSLY CONSIDER MAKING THIS SHIT TAKE SOURCE CONTEXT INFO
    actually give context a pointer to latest
*/
#ifndef GBL_EXT_MALLOC
#   define GBL_EXT_MALLOC           gblExtMalloc
#endif

#ifndef GBL_EXT_REALLOC
#   define GBL_EXT_REALLOC          gblExtRealloc
#endif

#ifndef GBL_EXT_FREE
#   define GBL_EXT_FREE             gblExtFree
#endif

#ifndef GBL_EXT_LOG_WRITE
#   define GBL_EXT_WRITE            gblExtLogWrite
#endif

#ifndef GBL_EXT_LOG_PUSH
#   define GBL_EXT_LOG_PUSH         gblExtLogPush
#endif

#ifndef GBL_EXT_LOG_POP
#   define GBL_EXT_LOG_POP          gblExtLogPop
#endif

#ifndef GBL_EXT_RESULT_STRING
#   define GBL_EXT_RESULT_STRING    gblExtResultString
#endif

#if 0


typedef struct gblEventExtLogWrite {
    gblEvent _base;

} gblEventogWrite;

GBL_API gblExtEventHandlerLogWrite(gblContext* pCtx,
                                      gblEvent* pEvent)
{


}

typedef struct gblEventExtMemory {
    gblEvent _base;
    void**    ppData;        // nullptr for malloc, valid for free/realloc
    gblSize_t size;         // 0 for free, valid for malloc/realloc
    gblSize_t align;        // 0 for free, valid for rest
} gblEventExtMemory



// Do vtable lookup for callback handler
GBL_API gblExtEventHandlergblContext* pCtx,
                                      gblEvent* pEvent)
{
    GBL_API_BEGIN(pCtx);
    gblEventExtMemory* pMemEvent = (gblEventExtMemory*)pEvent;

    switch(pMemEvent->_base.eventType) {
    case GBL_EVENT_EXT_MEM_MALLOC:
        GBL_API_CTX_DISPATCH(CTX_MEM_MALLOC, pMemEvent->size, pMemEvent->align, pMemEvent->ppData);
    break;

    case GBL_EVENT_EXT_MEM_REALLOC:

    case GBL_EVENT_EXT_MEM_FREE:

    default: GBL_ASSERT(false);

    }



}

typedef EVMU_RESULT (*EVMUMallocFn)(EVMUContextUd, EVMUSize_t, EVMUSize_t, void**);


GBL_API gblExtEventHandlerDefault(gblContext* pCtx,
                              gblEvent* pEvent)
{
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_HANDLE(pCtx);
    GBL_API_VERIFY_ARG(pEvent);

    if(pEvent->eventType < gbl_EVENT_BUILTIN_COUNT) {
        switch(pEvent->eventType) {
            GBL_API_CTX_DISPATCH(CTX_MEM_MALLOC, );
        default:

        }
    }


    GBL_API_END()
}


GBL_API gblExtEventHandler(const gblContext* pCtx,
                              gblEvent* pEvent)
{
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_HANDLE(pCtx);
    GBL_API_VERIFY_ARG(pEvent);



    // check whether event is listened to
        //should've happened before handler was even called!


    // check member function pointer
    if(GBL_API_CTX_VTABLE(EXT_EVENT_HANDLER)) {
        GBL_API_CTX_DISPATCH(EVENT_HANDLER, pEvent);
    }

    //


    //log unhandled event? log events?

    GBL_API_END();
}


GBL_API  gblExtMalloc(const gblContext* pCtx,
                        gblSize_t size,
                        gblSize_t alignment,
                        void** ppData)
{
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_HANDLE(pCtx);
    GBL_API_VERIFY_ARG(size);
    GBL_API_VERIFY_ARG(alignment);
    GBL_API_VERIFY_ARG(ppData);

#ifdef GBL_EXT_CALLBACKS
    // check member function pointer
    if(GBL_API_CTX_VTABLE(EXT_MALLOC)) {
        GBL_API_CTX_DISPATCH(EVENT_HANDLER, pEvent);
    }
#endif

#ifdef GBL_EXT_EVENTS
    if(GBL_API_CTX_VTABLE(EXT_EVENT_HANDLER)) {


    }
#endif

#ifdef GBL_EXT_DEFAULTS

#endif

    //log unhandled event? log events?

    GBL_API_END();

}
GBL_API  gblExtRealloc(const EVMUContext* pCtx,
                                     void* pExistingData,
                                     EVMUSize_t newSize,
                                     EVMUSize_t newAlignment,
                                     void* ppNewData);
GBL_API  gblExtFree(const EVMUContext* pCtx,
                                  void* pData);

GBL_API  gblExtLogWrite(const EVMUContext* pCtx,
                          const char* pFile,
                          const char* pFunction,#ifndef GBL_VERSION_STRING
                        #   define GBL_VERSION_STRING GBL_STRINGIFY_VERSION(GBL_VERSION_MAJOR, GBL_VERSION_MINOR, GBL_VERSION_PATCH)

                          EVMUSize_t line,
                          EVMU_LOG_LEVEL level,
                          const char* pFmt, ...);

GBL_API   gblExtLogPush(const EVMUContext* pCtx);
GBL_API   gblExtLogPop(const EVMUContext* pCtx, uint32_t count);


// Function pointers and all  of context dyanmic shit is literally only existing with static private internal dynamic impl lib?
// actually static at the core level?


typedef EVMU_RESULT (*EVMUEventHandlerFn)(EVMUContextUd, EVMUEvent*, EVMU_DEVICE_EVENT_STATUS*)

typedef EVMU_RESULT (*EVMUErrorStringFn)(EVMUContextUd, EVMU_RESULT, const char**);

// Fixme to use va_list!!!
typedef EVMU_RESULT (*EVMULogWriteFn)(EVMUContextUd, const char*, const char*, EVMUSize_t, EVMU_LOG_LEVEL, const char*);
typedef EVMU_RESULT (*EVMULogPushFn)(EVMUContextUd);
typedef EVMU_RESULT (*EVMULogPopFn)(EVMUContextUd, uint32_t);

//Custom allocators
typedef EVMU_RESULT (*EVMUMallocFn)(EVMUContextUd, EVMUSize_t, EVMUSize_t, void**);
typedef EVMU_RESULT (*EVMUReallocFn)(EVMUContextUd, void*, EVMUSize_t, EVMUSize_t, void**);
typedef EVMU_RESULT (*EVMUFreeFn)(EVMUContextUd, void*);

                        #endif


#endif // gblBAL_EXT_H
