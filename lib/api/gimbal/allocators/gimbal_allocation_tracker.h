/*! \file
 *  \brief GblAllocationTracker, counters, and related API
 *  \ingroup allocators
 *
 *  \author 2023 Falco Girgis
 *  \copyright MIT License
 */

#ifndef GIMBAL_ALLOCATION_TRACKER_H
#define GIMBAL_ALLOCATION_TRACKER_H

#include "../core/gimbal_typedefs.h"
#include "gimbal/core/gimbal_stack_frame.h"

#define GBL_SELF_TYPE GblAllocationTracker

GBL_DECLS_BEGIN

//! Counters for memory allocation and event statistics
typedef struct GblAllocationCounters {
    size_t    allocEvents;      //!< Total number of allocations
    size_t    reallocEvents;    //!< Total number of reallocations
    size_t    freeEvents;       //!< Total number of frees
    size_t    bytesAllocated;   //!< Total number of bytes allocated
    size_t    bytesFreed;       //!< Total number of bytes freed
    ptrdiff_t bytesActive;      //!< Current number of bytes active
    ptrdiff_t allocsActive;     //!< Current number of allocations active
} GblAllocationCounters;

//! Structure used for tracking allocation events and statistics
typedef struct GblAllocationTracker {
    size_t                maxAllocations;    //!< Maximum number of active allocations
    size_t                maxBytes;          //!< Maximum number of allocated bytes
    size_t                maxAllocationSize; //!< Maximum size of a single allocation
    GblAllocationCounters counters;          //!< Current allocation counters
} GblAllocationTracker;

/*! \name Lifetime Management
 *  \brief Methods for creating and destroying
 *  \relatesalso GblAllocationTracker
 *  @{
 */
//! Creates a GblAllocationTracker and returns a pointer to it
GBL_EXPORT GBL_SELF_TYPE* GblAllocationTracker_create  (GblContext* pCtx) GBL_NOEXCEPT;
//! Destroys the given GblAllocation tracker, returning the result
GBL_EXPORT GBL_RESULT     GblAllocationTracker_destroy (GBL_SELF)         GBL_NOEXCEPT;
//! @}

/*! \name Allocation Events
 *  \brief Methods for capturing allocation events
 *  \relatesalso GblAllocationTracker
 *  @{
 */
//! To be called every time an allocation has been requested, to track the event.
GBL_EXPORT GBL_RESULT GblAllocationTracker_allocEvent   (GBL_SELF,
                                                         const void*       pPtr,
                                                         size_t            size,
                                                         size_t            align,
                                                         const char*       pDbg,
                                                         GblSourceLocation srcLoc) GBL_NOEXCEPT;
//! To be called every time a reallocation has been requested, to track the event
GBL_EXPORT GBL_RESULT GblAllocationTracker_reallocEvent (GBL_SELF,
                                                         const void*       pExisting,
                                                         const void*       pNew,
                                                         size_t            newSize,
                                                         size_t            newAlign,
                                                         GblSourceLocation srcLoc) GBL_NOEXCEPT;
//! To be called every time a free has been requested, to track the event
GBL_EXPORT GBL_RESULT GblAllocationTracker_freeEvent    (GBL_SELF,
                                                         const void* pPtr,
                                                         GblSourceLocation srcLoc) GBL_NOEXCEPT;
//! @}

/*! \name Utilities
 *  \brief General and other methods
 *  \relatesalso GblAllocationTracker
 *  @{
 */
//! Returns GBL_TRUE if the given pointer points to an active, tracked allocation
GBL_EXPORT GblBool    GblAllocationTracker_validatePointer (GBL_CSELF, const void* pPtr) GBL_NOEXCEPT;
//! Dumps all of the active allocations and their context information to the log

GBL_EXPORT GBL_RESULT GblAllocationTracker_logActive       (GBL_CSELF)                   GBL_NOEXCEPT;
//! @}

/*! \name Counter Operations
 *  \brief Methods involving GblAllocationCountes
 *  \relatesalso GblAllocationTracker
 *  @{
 */
//! Saves the current value of GblAllocationTracker::counters to the given structure
GBL_EXPORT void GblAllocationTracker_captureCounters (GBL_CSELF,
                                                      GblAllocationCounters* pCount)     GBL_NOEXCEPT;
//! Takes the difference between GblAllocationTracker::counters and pSrc, storing the result in the pDst
GBL_EXPORT void GblAllocationTracker_diffCounters    (GBL_CSELF,
                                                      const GblAllocationCounters* pSrc,
                                                      GblAllocationCounters*       pDst) GBL_NOEXCEPT;
//! @}
GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_ALLOCATION_TRACKER_H
