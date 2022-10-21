/*! \file
 *  \brief GblAllocationTracker, counters, and related API
 *  \ingroup allocators
 */

#ifndef GIMBAL_ALLOCATION_TRACKER_H
#define GIMBAL_ALLOCATION_TRACKER_H

#include "../core/gimbal_typedefs.h"
#include "gimbal/core/gimbal_stack_frame.h"

#define GBL_SELF_TYPE GblAllocationTracker

GBL_DECLS_BEGIN

typedef struct GblAllocationCounters {
    GblSize     allocEvents;
    GblSize     reallocEvents;
    GblSize     freeEvents;
    GblSize     bytesAllocated;
    GblSize     bytesFreed;
    ptrdiff_t   bytesActive;
    ptrdiff_t   allocsActive;
} GblAllocationCounters;

typedef struct GblAllocationTracker {
    GblSize                 maxAllocations;
    GblSize                 maxBytes;
    GblSize                 maxAllocationSize;
    GblAllocationCounters   counters;
} GblAllocationTracker;

GBL_EXPORT GBL_SELF_TYPE* GblAllocationTracker_create          (GblContext* pCtx)                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblAllocationTracker_destroy         (GBL_SELF)                          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblAllocationTracker_allocEvent      (GBL_SELF,
                                                                const void*        pPtr,
                                                                GblSize            size,
                                                                GblSize            align,
                                                                const char*        pDbg,
                                                                GblSourceLocation  srcLoc)         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblAllocationTracker_reallocEvent    (GBL_SELF,
                                                                const void*        pExisting,
                                                                const void*        pNew,
                                                                GblSize            newSize,
                                                                GblSize            newAlign,
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
