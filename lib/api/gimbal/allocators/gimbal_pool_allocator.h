/*! \file
 *  \brief GblPoolAllocator chunked/paged pool based allocator + API
 *  \ingroup allocators
 *  \copydoc GblPoolAllocator
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_POOL_ALLOCATOR_H
#define GIMBAL_POOL_ALLOCATOR_H

#include "gimbal_arena_allocator.h"

#define GBL_SELF_TYPE GblPoolAllocator

GBL_DECLS_BEGIN

/*! \brief Pool allocator for ultra-fast fixed-size allocations
 *
 *  GblPoolAllocator is a custsom allocator providing
 *  extremely efficient dynamic allocations and deallocations
 *  for data of a fixed size. Allocations are done in bulk
 *  as single pages which are then split up into individual
 *  allocation entries. Since the pool allocator is built
 *  upon an arena allocator, when the initial page chunk
 *  overflows, the allocator will gracefully allocate another,
 *  so it's still able to dynamically accomodate arbitrary
 *  numbers of allocations.
 *
 *  The pool allocator constructs and maintains a free list
 *  of deleted entries (embedded within the unused allocations
 *  themselves), so a previously freed entry can be immediately
 *  recycled when a new allocation is requested.
 *
 *  The allocator also supports enforcing custom alignment
 *  constraints on each requested allocation.
 *  \ingroup allocators
 *  \sa GblArenaAllocator
 */
typedef struct GblPoolAllocator {
    GblArenaAllocator arena;            ///< Arena allocator instance providing backing allocations
    GblLinkedListNode freeList;         ///< Link list of deleted entries for efficient recycling
    size_t            entrySize;        ///< Base struct size of each entry
    size_t            entryAlign;       ///< Alignment requirement for each entry
    size_t            activeEntries;    ///< Number of allocated, used entries
} GblPoolAllocator;

// ===== Public methods =====
GBL_EXPORT GBL_RESULT GblPoolAllocator_construct    (GBL_SELF,
                                                     size_t                 entrySize,
                                                     size_t                 entriesPerPage,
                                                     size_t                 entryAlign,
                                                     GblArenaAllocatorPage* pInitialPage,
                                                     GblContext*            pCtx)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblPoolAllocator_destruct     (GBL_SELF)                              GBL_NOEXCEPT;

GBL_EXPORT size_t     GblPoolAllocator_freeListSize (GBL_CSELF)                             GBL_NOEXCEPT;

GBL_EXPORT void*      GblPoolAllocator_new          (GBL_SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblPoolAllocator_delete       (GBL_SELF, void* pEntry)                GBL_NOEXCEPT;

// ===== Macro Overloads =====
#define GblPoolAllocator_construct(...)     GblPoolAllocator_constructDefault_(__VA_ARGS__)

// ===== IMPL =====

///\cond
#define GblPoolAllocator_constructDefault_(...) \
    GblPoolAllocator_constructDefault__(__VA_ARGS__, 0, GBL_NULL, GBL_NULL)
#define GblPoolAllocator_constructDefault__(self, size, perPage, align, initial, ctx, ...) \
    (GblPoolAllocator_construct)(self, size, perPage, align, initial, ctx)
///\endcond
GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_POOL_ALLOCATOR_H
