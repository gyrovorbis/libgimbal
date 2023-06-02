/*! \file
 *  \brief GblArenaAllocator zone/region/area-based allocator + API
 *  \ingroup allocators
 *  \copydoc GblArenaAllocator
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_ARENA_ALLOCATOR_H
#define GIMBAL_ARENA_ALLOCATOR_H

#include "../containers/gimbal_linked_list.h"
#include "../core/gimbal_result.h"

#define GBL_SELF_TYPE GblArenaAllocator

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblContext);

/*! \brief Represents a single arena allocation page
 *
 *  GblArenaAllocatorPage is the meta data header and
 *  memory payload buffer for a preset number of bytes
 *  which can be allocated from a GblArenaAllocator
 *  \ingroup allocators
 *  \sa GblArenaAllocator
 */
typedef struct GblArenaAllocatorPage {
    union {
        struct GblArenaAllocatorPage* pNext;    ///< Next (used) allocator page
        GblLinkedListNode             listNode; ///< Linked list node base
    };
    size_t          capacity;                   ///< Page capacity
    size_t          used;                       ///< # of bytes filled on page
    union {
        GblBool     staticAlloc;                ///< Whether this page is static or heap allocated
        size_t      padding;
    };
    unsigned char bytes[1];                     ///< first byte of data segment
} GblArenaAllocatorPage;

/*! \brief Arena/zone/region/area-based paged allocator
 *
 *  GblArenaAllocator is a custom allocator which is useful
 *  for when you are allocating a bunch of data dynamically
 *  which all has the same shared lifetime. The allocator is
 *  created once, then as allocations are requested, it
 *  only allocates new memory in per-page incremements rather
 *  than for individual allocations. Finally, when you're
 *  done, rather than calling delete, you simply destruct
 *  the allocator, which frees its pages all at once. This
 *  can be SUBSTANTIALLY faster than calling malloc() or
 *  new for backing data structures with such allocation
 *  patterns.
 *  \note
 *  It's often useful to create the first allocation page
 *  as a static buffer somewhere in a source file, so that
 *  heap memory isn't even used until it's necessary. Be sure
 *  to set the GblArenaAllocatorPage::staticAlloc flag to
 *  ensure the allocator does not attempt to free it!
 *  \ingroup dataTypes
 *  \sa GblArenaAllocatorPage, GblPoolAllocator
 */
typedef struct GblArenaAllocator {
    union {
        GblArenaAllocatorPage* pActivePage; ///< Active (unfilled) page at list head
        GblLinkedListNode      listNode;    ///< Linked list node base
    };
    GblContext* pCtx;                       ///< Custom context associated with allocator
    size_t      pageSize;                   ///< Default page size for all new pages
    size_t      pageAlign;                  ///< Alignment of each page, also maximum requestable alignment
    size_t      allocCount;                 ///< Total # of allocations across all pages
} GblArenaAllocator;

/*! \brief Represents the current state of a GblArenaAllocator
 *
 *  GblArenaAllocatorState is an auxiliary struct
 *  whose purpose is to capture the current page
 *  state associated with a given arena allocator,
 *  so that it may be use to "restore" the arena's
 *  state later.
 *
 *  This is useful for wanting to make temporary
 *  use of an arena's resources, allowing you to
 *  free them when you're done.
 *  \sa GblArenaAllocator GblArenaAllocatorPage
 */
typedef struct GblArenaAllocatorState {
    GblArenaAllocatorPage* pActivePage;
    size_t                 bytesUsed;
} GblArenaAllocatorState;

// ===== Public methods =====
GBL_EXPORT GBL_RESULT GblArenaAllocator_construct       (GBL_SELF,
                                                         size_t                 pageSize,
                                                         size_t                 pageAlign,
                                                         GblArenaAllocatorPage* pInitialPage,
                                                         GblContext*            pCtx)                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblArenaAllocator_destruct        (GBL_SELF)                                       GBL_NOEXCEPT;

GBL_EXPORT size_t     GblArenaAllocator_pageCount       (GBL_CSELF)                                      GBL_NOEXCEPT;
GBL_EXPORT size_t     GblArenaAllocator_bytesUsed       (GBL_CSELF)                                      GBL_NOEXCEPT;
GBL_EXPORT size_t     GblArenaAllocator_bytesAvailable  (GBL_CSELF)                                      GBL_NOEXCEPT;

GBL_EXPORT size_t     GblArenaAllocator_totalCapacity   (GBL_CSELF)                                      GBL_NOEXCEPT;
GBL_EXPORT size_t     GblArenaAllocator_fragmentedBytes (GBL_CSELF)                                      GBL_NOEXCEPT;
GBL_EXPORT float      GblArenaAllocator_utilization     (GBL_CSELF)                                      GBL_NOEXCEPT;

GBL_EXPORT void       GblArenaAllocator_saveState       (GBL_CSELF, GblArenaAllocatorState* pState)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblArenaAllocator_loadState       (GBL_SELF, const GblArenaAllocatorState* pState) GBL_NOEXCEPT;

GBL_EXPORT void*      GblArenaAllocator_alloc           (GBL_SELF, size_t size, size_t alignment)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblArenaAllocator_freeAll         (GBL_SELF)                                       GBL_NOEXCEPT;

// ===== Macro overloads =====
#define GblArenaAllocator_construct(...)    GblArenaAllocator_constructDefault_(__VA_ARGS__)
#define GblArenaAllocator_alloc(...)        GblArenaAllocator_allocDefault_(__VA_ARGS__)

// ===== IMPL =====
///\cond
#define GblArenaAllocator_constructDefault_(...) \
    GblArenaAllocator_constructDefault__(__VA_ARGS__, 16, GBL_NULL, GBL_NULL)
#define GblArenaAllocator_constructDefault__(self, size, align, initial, ctx, ...) \
    (GblArenaAllocator_construct)(self, size, align, initial, ctx)

#define GblArenaAllocator_allocDefault_(...) \
    GblArenaAllocator_allocDefault__(__VA_ARGS__, 0)
#define GblArenaAllocator_allocDefault__(self, size, align, ...) \
    (GblArenaAllocator_alloc)(self, size, align)
///\endcond
GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_ARENA_ALLOCATOR_H
