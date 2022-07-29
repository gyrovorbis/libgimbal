/*! \file
 *  \brief GblAllocationTracker, counters, and related API
 *  \ingroup testing
 */

#ifndef GIMBAL_ALLOCATION_TRACKER_H
#define GIMBAL_ALLOCATION_TRACKER_H

#include "../types/gimbal_typedefs.h"
#include "gimbal/core/gimbal_call_stack.h"

#define SELF    GblAllocationTracker* pSelf
#define CSELF   const SELF

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

GBL_EXPORT GblAllocationTracker*
                        GblAllocationTracker_create         (GblContext* pCtx)                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblAllocationTracker_destroy        (SELF)                              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblAllocationTracker_allocEvent     (SELF,
                                                             const void*        pPtr,
                                                             GblSize            size,
                                                             GblSize            align,
                                                             const char*        pDbg,
                                                             GblSourceLocation  srcLoc)         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblAllocationTracker_reallocEvent   (SELF,
                                                             const void*        pExisting,
                                                             const void*        pNew,
                                                             GblSize            newSize,
                                                             GblSize            newAlign,
                                                             GblSourceLocation  srcLoc)         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblAllocationTracker_freeEvent      (SELF,
                                                             const void* pPtr,
                                                             GblSourceLocation  srcLoc)         GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblAllocationTracker_validatePointer(CSELF, const void* pPtr)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblAllocationTracker_logActive      (CSELF)                             GBL_NOEXCEPT;

GBL_EXPORT void         GblAllocationTracker_captureCounters(CSELF,
                                                             GblAllocationCounters* pCount)     GBL_NOEXCEPT;

GBL_EXPORT void         GblAllocationTracker_diffCounters   (CSELF,
                                                             const GblAllocationCounters* pSrc,
                                                             GblAllocationCounters*       pDst) GBL_NOEXCEPT;

GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_ALLOCATION_TRACKER_H
