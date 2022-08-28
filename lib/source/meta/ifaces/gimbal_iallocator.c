#include <gimbal/meta/ifaces/gimbal_iallocator.h>
#include <gimbal/meta/instances/gimbal_instance.h>
#include <gimbal/strings/gimbal_quark.h>
#include "../types/gimbal_type_.h"

static GBL_RESULT GblIAllocatorIFace_alloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, GblSize size, GblSize align, const char* pDbgStr, void** ppData) GBL_NOEXCEPT {
    GBL_UNUSED(pIAllocator);
    GBL_UNUSED(pFrame);
    GBL_UNUSED(pDbgStr);
    GBL_API_BEGIN(NULL);
    *ppData = GBL_ALIGNED_ALLOC(align, size);
    GBL_API_END();
}

static GBL_RESULT GblIAllocatorIFace_realloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData, GblSize newSize, GblSize newAlign, void** ppNewData) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame && pIAllocator && newAlign);
    GBL_API_BEGIN(NULL);
    *ppNewData = GBL_ALIGNED_REALLOC(pData, newAlign, newSize);
    GBL_API_END();
}

static GBL_RESULT GblIAllocatorIFace_free_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame && pIAllocator);
    GBL_API_BEGIN(NULL);
    GBL_ALIGNED_FREE(pData);
    GBL_API_END();
}

static GBL_RESULT GblIAllocatorIFace_init_(GblIAllocatorIFace* pIFace, void* pData, GblContext* pCtx) GBL_NOEXCEPT {
    GBL_UNUSED(pData);
    GBL_API_BEGIN(pCtx);
    pIFace->pFnAlloc    = GblIAllocatorIFace_alloc_;
    pIFace->pFnRealloc  = GblIAllocatorIFace_realloc_;
    pIFace->pFnFree     = GblIAllocatorIFace_free_;
    GBL_API_END();
}

GBL_RESULT GblIAllocator_alloc  (GblIAllocator* pSelf,
                                 const GblStackFrame* pStackFrame,
                                 GblSize size,
                                 GblSize alignment,
                                 const char* pDebugString,
                                 void** ppData)                     GBL_NOEXCEPT
{
    GBL_API_BEGIN(NULL);
    GBL_INSTANCE_VCALL_PREFIX(GBL_IALLOCATOR, pFnAlloc,
                              pSelf, pStackFrame, size, alignment, pDebugString, ppData);
    GBL_API_END();
}

GBL_RESULT GblIAllocator_realloc(GblIAllocator* pSelf,
                                 const GblStackFrame* pStackFrame,
                                 void* pData,
                                 GblSize newSize,
                                 GblSize newAlign,
                                 void** ppNewData)                  GBL_NOEXCEPT
{
    GBL_API_BEGIN(NULL);
    GBL_INSTANCE_VCALL_PREFIX(GBL_IALLOCATOR, pFnRealloc,
                              pSelf, pStackFrame, pData, newSize, newAlign, ppNewData);
    GBL_API_END();
}

GBL_RESULT GblIAllocator_free   (GblIAllocator* pSelf,
                                 const GblStackFrame* pStackFrame,
                                 void* pData)                       GBL_NOEXCEPT
{
    GBL_API_BEGIN(NULL);
    GBL_INSTANCE_VCALL_PREFIX(GBL_IALLOCATOR, pFnFree,
                              pSelf, pStackFrame, pData);
    GBL_API_END();
}

GBL_EXPORT GblType GblIAllocator_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .pFnClassInit = (GblTypeClassInitializeFn)GblIAllocatorIFace_init_,
        .classSize = sizeof(GblIAllocatorIFace)
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("IAllocator"),
                                      GBL_INTERFACE_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}
