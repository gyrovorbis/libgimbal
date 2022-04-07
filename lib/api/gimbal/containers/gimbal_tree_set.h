#ifndef GIMBAL_BTREE_H
#define GIMBAL_BTREE_H

#include <btree.c/btree.h>
#include <time.h>
#include "gimbal_container.h"
#include "../types/gimbal_typedefs.h"

#define SELF    GblTreeSet* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTreeSet);

typedef GblInt  (*GblTreeSetEntryCompareFn) (CSELF, const void*, const void*);
typedef void    (*GblTreeSetEntryDestructFn)(CSELF, void*);


typedef struct GblTreeSetNode {
    uint16_t                    entryCount;
    GblBool                     leaf;
    uint8_t                     padding[sizeof(void*)-3];
    void*                       pEntries;
    struct GblTreeSetNode*      pChildren[1];
} GblTreeSetNode;

typedef struct GblTreeSetGroup {
    GblTreeSetNode**            ppNodes;
    GblSize                     length;
    GblSize                     capacity;
} GblTreeSetGroup;

typedef struct GblTreeSetPool {
    GblTreeSetGroup             leaves;
    GblTreeSetGroup             branches;
 } GblTreeSetPool;

typedef struct GblTreeSet {
    GblContext                  hCtx;
    GblSize                     entrySize;
    GblTreeSetEntryCompareFn    pFnCompare;
    GblTreeSetEntryDestructFn   pFnDestruct;
    GblTreeSetPool              pool;
    GblTreeSetNode*             pRoot;
    GblSize                     count;
    GblSize                     height;
    GblSize                     maxCount;
    GblSize                     minCount;
    void*                       pSpares[3];
    void*                       pLastEntry;
    void*                       pLastNode;
    void*                       pUserdata;
} GblTreeSet;

typedef struct GblTreeSetIterator {
    GblTreeSet*         pContainer;
    GblTreeSetNode*     pNode;
    uint16_t            index;
} GblTreeSetIterator;


GBL_API                 GblTreeSet_construct_7  (SELF,
                                                GblSize                   entrySize,
                                                GblTreeSetEntryCompareFn  pFnCompare,
                                                GblTreeSetEntryDestructFn pFnDestruct,
                                                GblSize                   maxEntries,
                                                GblContext                hCtx,
                                                void*                     pUserdata)    GBL_NOEXCEPT;
GBL_API                 GblTreeSet_construct_6  (SELF,
                                                GblSize                   entrySize,
                                                GblTreeSetEntryCompareFn  pFnCompare,
                                                GblTreeSetEntryDestructFn pFnDestruct,
                                                GblSize                   maxEntries,
                                                GblContext                hCtx)         GBL_NOEXCEPT;
GBL_API                 GblTreeSet_construct_5  (SELF,
                                                GblSize                   entrySize,
                                                GblTreeSetEntryCompareFn  pFnCompare,
                                                GblTreeSetEntryDestructFn pFnDestruct,
                                                GblSize                   maxEntries)   GBL_NOEXCEPT;
GBL_API                 GblTreeSet_construct_4  (SELF,
                                                GblSize                   entrySize,
                                                GblTreeSetEntryCompareFn  pFnCompare,
                                                GblTreeSetEntryDestructFn pFnDestruct)  GBL_NOEXCEPT;
GBL_API                 GblTreeSet_construct_3  (SELF,
                                                GblSize                   entrySize,
                                                GblTreeSetEntryCompareFn  pFnCompare)   GBL_NOEXCEPT;
#define                 GblTreeSet_construct    (...)                                               \
                                                  GBL_VA_OVERLOAD_SELECT(GblTreeSet_construct,      \
                                                  GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)
GBL_API                 GblTreeSet_destruct     (SELF)                                  GBL_NOEXCEPT;

GBL_INLINE GblSize      GblTreeSet_size         (CSELF)                                 GBL_NOEXCEPT;
GBL_INLINE GblSize      GblTreeSet_height       (CSELF)                                 GBL_NOEXCEPT;
GBL_INLINE GblContext   GblTreeSet_context      (CSELF)                                 GBL_NOEXCEPT;
GBL_INLINE GblBool      GblTreeSet_empty        (CSELF)                                 GBL_NOEXCEPT;
GBL_INLINE void*        GblTreeSet_userdata     (CSELF)                                 GBL_NOEXCEPT;

GBL_EXPORT void*        GblTreeSet_get          (CSELF, const void* pKey)               GBL_NOEXCEPT;

GBL_EXPORT void*        GblTreeSet_set          (SELF,  const void* pEntry)             GBL_NOEXCEPT;

GBL_EXPORT void*        GblTreeSet_extract      (SELF,  const void* pKey)               GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblTreeSet_erase        (SELF,  const void* pKey)               GBL_NOEXCEPT;
GBL_EXPORT void         GblTreeSet_clear        (SELF)                                  GBL_NOEXCEPT;





// ======== INLINE IMPLEMENTATION =========

GBL_INLINE GblSize GblTreeSet_size(CSELF) GBL_NOEXCEPT {
    return pSelf->count;
}

GBL_INLINE GblSize GblTreeSet_height(CSELF) GBL_NOEXCEPT {
    return pSelf->height;
}

GBL_INLINE GblContext GblTreeSet_context(CSELF) GBL_NOEXCEPT {
    return pSelf->hCtx;
}

GBL_INLINE GblBool GblTreeSet_empty(CSELF) GBL_NOEXCEPT {
    return pSelf->count ? GBL_FALSE : GBL_TRUE;
}

GBL_INLINE void* GblTreeSet_userdata(CSELF) GBL_NOEXCEPT {
    return pSelf->pUserdata;
}


#undef CSELF
#undef SELF

GBL_DECLS_END

#endif // GIMBAL_TREE_SET_H