#ifndef GIMBAL_SCOPE_ALLOCATOR_H
#define GIMBAL_SCOPE_ALLOCATOR_H

#include "gimbal_arena_allocator.h"

#define GBL_SELF_TYPE GblScopeAllocator

GBL_DECLS_BEGIN

typedef GBL_RESULT (*GblScopeAllocatorDtorFn)(void* pPtr);

typedef struct GblScopeAllocator {
    GblArenaAllocator*      pArena;
    GblArenaAllocatorState  rewindState;
    GblLinkedListNode       dtorList;
} GblScopeAllocator;

GBL_EXPORT GblScopeAllocator* GblScopeAllocator_create    (GblArenaAllocator* pArena)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblScopeAllocator_construct (GBL_SELF,
                                                           GblArenaAllocator* pArena)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT         GblScopeAllocator_destroy   (GBL_SELF)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblScopeAllocator_destruct  (GBL_SELF)                        GBL_NOEXCEPT;

GBL_EXPORT void*              GblScopeAllocator_alloc     (GBL_SELF,
                                                           GblSize                 size,
                                                           GblSize                 align,
                                                           GblScopeAllocatorDtorFn pFnDtor) GBL_NOEXCEPT;

GBL_EXPORT GblSize            GblScopeAllocator_dtorCount (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_EXPORT GblSize            GblScopeAllocator_bytesUsed (GBL_CSELF)                       GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_SCOPE_ALLOCATOR_H
