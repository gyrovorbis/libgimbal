/*! \file
 *  \brief GblIAllocator abstract allocator interface
 *  \ingroup interfaces
 */

#ifndef GIMBAL_IALLOCATOR_H
#define GIMBAL_IALLOCATOR_H

#include "gimbal_interface.h"
#include "../../core/gimbal_ctx.h"

#define GBL_IALLOCATOR_TYPE                 (GBL_TYPEOF(GblIAllocator))
#define GBL_IALLOCATOR(instance)            (GBL_INSTANCE_CAST(instance,  GblIAllocator))
#define GBL_IALLOCATOR_CLASS(klass)         (GBL_CLASS_CAST(klass, GblIAllocator))
#define GBL_IALLOCATOR_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GblIAllocator))

#define GBL_SELF_TYPE GblIAllocator

GBL_DECLS_BEGIN

GBL_INTERFACE_DERIVE(GblIAllocator)
    GBL_RESULT (*pFnAlloc)  (GBL_SELF, const GblStackFrame* pStackFrame, GblSize size, GblSize align, const char* pDbgStr, void** ppData);
    GBL_RESULT (*pFnRealloc)(GBL_SELF, const GblStackFrame* pStackFrame, void* pData, GblSize newSize, GblSize newAlign, void** ppNewData);
    GBL_RESULT (*pFnFree)   (GBL_SELF, const GblStackFrame* pStackFrame, void* pData);
GBL_INTERFACE_END

GBL_EXPORT GblType    GblIAllocator_type    (void)                              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblIAllocator_alloc   (GBL_SELF,
                                             const GblStackFrame* pStackFrame,
                                             GblSize              size,
                                             GblSize              alignment,
                                             const char*          pDebugString,
                                             void**               ppData)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblIAllocator_realloc (GBL_SELF,
                                             const GblStackFrame* pStackFrame,
                                             void*                pData,
                                             GblSize              newSize,
                                             GblSize              newAlign,
                                             void**               ppNewData)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblIAllocator_free    (GBL_SELF,
                                             const GblStackFrame* pStackFrame,
                                             void*                pData)        GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_IALLOCATOR_H
