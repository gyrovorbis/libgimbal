#include <gimbal/meta/ifaces/gimbal_iallocator.h>
#include <gimbal/meta/instances/gimbal_instance.h>
#include <gimbal/strings/gimbal_quark.h>
#include "../types/gimbal_type_.h"

static GBL_RESULT GblIAllocatorClass_alloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, size_t  size, size_t  align, const char* pDbgStr, void** ppData) GBL_NOEXCEPT {
    GBL_UNUSED(pIAllocator);
    GBL_UNUSED(pFrame);
    GBL_UNUSED(pDbgStr);
    GBL_CTX_BEGIN(NULL);
    *ppData = GBL_ALIGNED_ALLOC(align, size);
    GBL_CTX_END();
}

static GBL_RESULT GblIAllocatorClass_realloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData, size_t  newSize, size_t  newAlign, void** ppNewData) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame && pIAllocator && newAlign);
    GBL_CTX_BEGIN(NULL);
    *ppNewData = GBL_ALIGNED_REALLOC(pData, newAlign, newSize);
    GBL_CTX_END();
}

static GBL_RESULT GblIAllocatorClass_free_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame && pIAllocator);
    GBL_CTX_BEGIN(NULL);
    GBL_ALIGNED_FREE(pData);
    GBL_CTX_END();
}

static GBL_RESULT GblIAllocatorClass_init_(GblIAllocatorClass* pIFace, void* pData) {
    GBL_UNUSED(pData);
    GBL_CTX_BEGIN(NULL);
    pIFace->pFnAlloc    = GblIAllocatorClass_alloc_;
    pIFace->pFnRealloc  = GblIAllocatorClass_realloc_;
    pIFace->pFnFree     = GblIAllocatorClass_free_;
    GBL_CTX_END();
}

GBL_RESULT GblIAllocator_alloc  (GblIAllocator* pSelf,
                                 const GblStackFrame* pStackFrame,
                                 size_t  size,
                                 size_t  alignment,
                                 const char* pDebugString,
                                 void** ppData)                     GBL_NOEXCEPT
{
    GBL_CTX_BEGIN(NULL);
    GBL_VCALL(GblIAllocator, pFnAlloc,
                       pSelf, pStackFrame, size, alignment, pDebugString, ppData);
    GBL_CTX_END();
}

GBL_RESULT GblIAllocator_realloc(GblIAllocator* pSelf,
                                 const GblStackFrame* pStackFrame,
                                 void* pData,
                                 size_t  newSize,
                                 size_t  newAlign,
                                 void** ppNewData)                  GBL_NOEXCEPT
{
    GBL_CTX_BEGIN(NULL);
    GBL_VCALL(GblIAllocator, pFnRealloc,
                       pSelf, pStackFrame, pData, newSize, newAlign, ppNewData);
    GBL_CTX_END();
}

GBL_RESULT GblIAllocator_free   (GblIAllocator* pSelf,
                                 const GblStackFrame* pStackFrame,
                                 void* pData)                       GBL_NOEXCEPT
{
    GBL_CTX_BEGIN(NULL);
    GBL_VCALL(GblIAllocator, pFnFree,
                       pSelf, pStackFrame, pData);
    GBL_CTX_END();
}

GBL_EXPORT GblType GblIAllocator_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .pFnClassInit = (GblClassInitFn)GblIAllocatorClass_init_,
        .classSize = sizeof(GblIAllocatorClass)
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        type = GblType_register(GblQuark_internStringStatic("GblIAllocator"),
                                GBL_INTERFACE_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}
