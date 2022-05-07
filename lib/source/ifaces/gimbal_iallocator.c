#include <gimbal/ifaces/gimbal_iallocator.h>
#include <gimbal/meta/gimbal_instance.h>

static GBL_RESULT GblIAllocatorIFace_alloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, GblSize size, GblSize align, const char* pDbgStr, void** ppData) GBL_NOEXCEPT {
    GBL_UNUSED(pIAllocator);
    GBL_UNUSED(pFrame);
    GBL_UNUSED(pDbgStr);
    GBL_API_BEGIN(NULL);
    *ppData = GBL_ALLOC_ALIGNED(align, size);
    GBL_API_END();
}

static GBL_RESULT GblIAllocatorIFace_realloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData, GblSize newSize, GblSize newAlign, void** ppNewData) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame && pIAllocator && newAlign);
    GBL_API_BEGIN(NULL);
    *ppNewData = realloc(pData, newSize);
    GBL_API_END();
}

static GBL_RESULT GblIAllocatorIFace_free_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame && pIAllocator);
    GBL_API_BEGIN(NULL);
    free(pData);
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

extern GBL_RESULT GblIAllocator_typeRegister_(GblIAllocator* pCtx) GBL_NOEXCEPT {
    GBL_API_BEGIN(pCtx);
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_IALLOCATOR,
      GBL_INTERFACE_TYPE,
      "IAllocator",
      &((const GblTypeInfo) {
          .pFnClassInit     = (GblTypeClassInitializeFn)GblIAllocatorIFace_init_,
          .classSize        = sizeof(GblIAllocatorIFace)
      }),
      GBL_TYPE_FLAGS_NONE);
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
