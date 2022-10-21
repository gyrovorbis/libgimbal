/*! \file
 *  \brief GblScopeAllocator stack-like arena sub allocator
 *  \ingroup allocators
 *  \copydoc GblScopeAllocator
 */

#ifndef GIMBAL_SCOPE_ALLOCATOR_H
#define GIMBAL_SCOPE_ALLOCATOR_H

#include "gimbal_arena_allocator.h"

#define GBL_SELF_TYPE GblScopeAllocator

GBL_DECLS_BEGIN

typedef GBL_RESULT (*GblScopeAllocatorDtorFn)(void* pPtr);

typedef struct GblScopeAllocatorDtorEntry {
    union {
        struct GblScopeAllocatorDtorEntry* pNext;
        GblLinkedListNode                  node;
    };
    GblScopeAllocatorDtorFn                pFnDtor;
    void*                                  pData;
} GblScopeAllocatorDtorEntry;

typedef struct GblScopeAllocator {
    GblArenaAllocator*      pArena;
    GblArenaAllocatorState  rewindState;
    GblLinkedListNode       dtorList;
    GblSize                 bytesUsed;
} GblScopeAllocator;

// ===== Public API =====
GBL_EXPORT GblScopeAllocator* GblScopeAllocator_create    (GblArenaAllocator* pArena)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblScopeAllocator_construct (GBL_SELF,
                                                           GblArenaAllocator* pArena)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT         GblScopeAllocator_destroy   (GBL_SELF)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblScopeAllocator_destruct  (GBL_SELF)                        GBL_NOEXCEPT;

GBL_EXPORT void*              GblScopeAllocator_alloc     (GBL_SELF,
                                                           GblSize                 size,
                                                           GblSize                 align,
                                                           GblScopeAllocatorDtorFn pFnDtor) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT         GblScopeAllocator_pushDtor (GBL_SELF,
                                                          GblScopeAllocatorDtorFn pFnDtor,
                                                          void*                   pData)    GBL_NOEXCEPT;

GBL_EXPORT GblSize            GblScopeAllocator_dtorCount (GBL_CSELF)                       GBL_NOEXCEPT;

// ===== Macro Overrides =====
#define GblScopeAllocator_alloc(...)                      GblScopeAllocator_allocDefault_(__VA_ARGS__)
#define GblScopeAllocator_pushDtor(...)                   GblScopeAllocator_pushDtorDefault_(__VA_ARGS__)

// ===== Implementation =====
#define GblScopeAllocator_allocDefault_(...) \
    GblScopeAllocator_allocDefault__(__VA_ARGS__, 0, GBL_NULL)
#define GblScopeAllocator_allocDefault__(self, size, align, dtor, ...) \
    (GblScopeAllocator_alloc)(self, size, align, dtor)

#define GblScopeAllocator_pushDtorDefault_(...) \
    GblScopeAllocator_pushDtorDefault__(__VA_ARGS__, GBL_NULL)
#define GblScopeAllocator_pushDtorDefault__(self, dtor, data, ...) \
    (GblScopeAllocator_pushDtor)(self, dtor, data)

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_SCOPE_ALLOCATOR_H
