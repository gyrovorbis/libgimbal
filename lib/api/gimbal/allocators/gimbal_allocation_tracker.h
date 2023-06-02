/*! \file
 *  \brief GblAllocationTracker, counters, and related API
 *  \ingroup allocators
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_ALLOCATION_TRACKER_H
#define GIMBAL_ALLOCATION_TRACKER_H

#include "../core/gimbal_typedefs.h"
#include "gimbal/core/gimbal_stack_frame.h"

#define GBL_SELF_TYPE GblAllocationTracker

GBL_DECLS_BEGIN

typedef struct GblAllocationCounters {
    size_t      allocEvents;
    size_t      reallocEvents;
    size_t      freeEvents;
    size_t      bytesAllocated;
    size_t      bytesFreed;
    ptrdiff_t   bytesActive;
    ptrdiff_t   allocsActive;
} GblAllocationCounters;

typedef struct GblAllocationTracker {
    size_t                  maxAllocations;
    size_t                  maxBytes;
    size_t                  maxAllocationSize;
    GblAllocationCounters   counters;
} GblAllocationTracker;

GBL_EXPORT GBL_SELF_TYPE* GblAllocationTracker_create          (GblContext* pCtx)                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblAllocationTracker_destroy         (GBL_SELF)                          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblAllocationTracker_allocEvent      (GBL_SELF,
                                                                const void*        pPtr,
                                                                size_t             size,
                                                                size_t             align,
                                                                const char*        pDbg,
                                                                GblSourceLocation  srcLoc)         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblAllocationTracker_reallocEvent    (GBL_SELF,
                                                                const void*        pExisting,
                                                                const void*        pNew,
                                                                size_t             newSize,
                                                                size_t             newAlign,
                                                                GblSourceLocation  srcLoc)         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblAllocationTracker_freeEvent       (GBL_SELF,
                                                                const void* pPtr,
                                                                GblSourceLocation  srcLoc)         GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblAllocationTracker_validatePointer (GBL_CSELF, const void* pPtr)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblAllocationTracker_logActive       (GBL_CSELF)                         GBL_NOEXCEPT;

GBL_EXPORT void           GblAllocationTracker_captureCounters (GBL_CSELF,
                                                                GblAllocationCounters* pCount)     GBL_NOEXCEPT;

GBL_EXPORT void           GblAllocationTracker_diffCounters    (GBL_CSELF,
                                                                const GblAllocationCounters* pSrc,
                                                                GblAllocationCounters*       pDst) GBL_NOEXCEPT;
GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_ALLOCATION_TRACKER_H
