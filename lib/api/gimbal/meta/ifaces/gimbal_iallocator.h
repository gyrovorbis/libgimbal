/*! \file
 *  \brief GblIAllocator abstract allocator interface
 *  \ingroup interfaces
 */

#ifndef GIMBAL_IALLOCATOR_H
#define GIMBAL_IALLOCATOR_H

#include "gimbal_interface.h"
#include "../../core/gimbal_api_frame.h"

#define GBL_IALLOCATOR_TYPE                 GBL_BUILTIN_TYPE(IALLOCATOR)
#define GBL_IALLOCATOR_STRUCT               GblIAllocator
#define GBL_IALLOCATOR_CLASS_STRUCT         GblIAllocatorIFace
#define GBL_IALLOCATOR(inst)                (GBL_INSTANCE_CAST_PREFIX  (inst,  GBL_IALLOCATOR))
#define GBL_IALLOCATOR_CHECK(inst)          (GBL_INSTANCE_CHECK_PREFIX (insta, GBL_IALLOCATOR))
#define GBL_IALLOCATOR_IFACE(klass)         (GBL_CLASS_CAST_PREFIX     (klass, GBL_IALLOCATOR))
#define GBL_IALLOCATOR_IFACE_CHECK(klass)   (GBL_CLASS_CHECK_PREFIX    (klass, GBL_IALLOCATOR))
#define GBL_IALLOCATOR_GET_IFACE(inst)      (GBL_INSTANCE_GET_CLASS_PREFIX (inst,  GBL_IALLOCATOR))

#define GBL_SELF    GblIAllocator* pSelf
#define GBL_CSELF   const GBL_SELF

GBL_DECLS_BEGIN

GBL_INTERFACE_DERIVE(GblIAllocator)
    GBL_RESULT (*pFnAlloc)  (GBL_SELF, const GblStackFrame* pStackFrame, GblSize size, GblSize align, const char* pDbgStr, void** ppData);
    GBL_RESULT (*pFnRealloc)(GBL_SELF, const GblStackFrame* pStackFrame, void* pData, GblSize newSize, GblSize newAlign, void** ppNewData);
    GBL_RESULT (*pFnFree)   (GBL_SELF, const GblStackFrame* pStackFrame, void* pData);
GBL_INTERFACE_END

GBL_RESULT GblIAllocator_alloc  (GBL_SELF,
                                 const GblStackFrame* pStackFrame,
                                 GblSize size,
                                 GblSize alignment,
                                 const char* pDebugString,
                                 void** ppData)                     GBL_NOEXCEPT;

GBL_RESULT GblIAllocator_realloc(GBL_SELF,
                                 const GblStackFrame* pStackFrame,
                                 void* pData,
                                 GblSize newSize,
                                 GblSize newAlign,
                                 void** ppNewData)                  GBL_NOEXCEPT;

GBL_RESULT GblIAllocator_free   (GBL_SELF,
                                 const GblStackFrame* pStackFrame,
                                 void* pData)                       GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_CSELF
#undef GBL_SELF


#endif // GIMBAL_IALLOCATOR_H
