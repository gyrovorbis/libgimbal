/*! \file
 *  \brief GblHashSet structure and related functions
 *  \ingroup containers
 */

#ifndef GIMBAL_BTREE_H
#define GIMBAL_BTREE_H

#include <time.h>
#include "gimbal_container.h"
#include "../core/gimbal_typedefs.h"

#define GBL_SELF_TYPE GblTreeSet

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTreeSet);

typedef GblInt  (*GblTreeSetEntryCompareFn) (GBL_CSELF, const void*, const void*);
typedef void    (*GblTreeSetEntryDestructFn)(GBL_CSELF, void*);

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

/*! \brief Binary tree based abstract associative container with C++-style STL API
 *  \ingroup containers
 */
typedef struct GblTreeSet {
    GblContext*                 pCtx;
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


GBL_EXPORT GBL_RESULT   GblTreeSet_construct_7  (GBL_SELF,
                                                 GblSize                   entrySize,
                                                 GblTreeSetEntryCompareFn  pFnCompare,
                                                 GblTreeSetEntryDestructFn pFnDestruct,
                                                 GblSize                   maxEntries,
                                                 GblContext*               pCtx,
                                                 void*                     pUserdata)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblTreeSet_construct_6  (GBL_SELF,
                                                 GblSize                   entrySize,
                                                 GblTreeSetEntryCompareFn  pFnCompare,
                                                 GblTreeSetEntryDestructFn pFnDestruct,
                                                 GblSize                   maxEntries,
                                                 GblContext*               pCtx)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblTreeSet_construct_5  (GBL_SELF,
                                                 GblSize                   entrySize,
                                                 GblTreeSetEntryCompareFn  pFnCompare,
                                                 GblTreeSetEntryDestructFn pFnDestruct,
                                                 GblSize                   maxEntries)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblTreeSet_construct_4  (GBL_SELF,
                                                 GblSize                   entrySize,
                                                 GblTreeSetEntryCompareFn  pFnCompare,
                                                 GblTreeSetEntryDestructFn pFnDestruct) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblTreeSet_construct_3  (GBL_SELF,
                                                 GblSize                   entrySize,
                                                 GblTreeSetEntryCompareFn  pFnCompare)  GBL_NOEXCEPT;
#define                 GblTreeSet_construct(...)                                                 \
                                                  GBL_VA_OVERLOAD_CALL(GblTreeSet_construct,      \
                                                  GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

GBL_EXPORT GBL_RESULT   GblTreeSet_destruct     (GBL_SELF)                              GBL_NOEXCEPT;

GBL_INLINE GblSize      GblTreeSet_size         (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_INLINE GblSize      GblTreeSet_height       (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_INLINE GblContext*  GblTreeSet_context      (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_INLINE GblBool      GblTreeSet_empty        (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_INLINE void*        GblTreeSet_userdata     (GBL_CSELF)                             GBL_NOEXCEPT;

GBL_EXPORT void*        GblTreeSet_get          (GBL_CSELF, const void* pKey)           GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_getHint      (GBL_CSELF,
                                                 const void* pKey,
                                                 uint64_t* pHint)                       GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_at           (GBL_CSELF, const void* pKey)           GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_atHint       (GBL_CSELF,
                                                 const void* pKey,
                                                 uint64_t* pHint)                       GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblTreeSet_contains     (GBL_CSELF, const void* pKey)           GBL_NOEXCEPT;
GBL_EXPORT GblSize      GblTreeSet_count        (GBL_CSELF, const void* pKey)           GBL_NOEXCEPT;
//GBL_EXPORT ???        GblTreeSet_find         (GBL_CSELF, const void* pKey)               GBL_NOEXCEPT;

GBL_EXPORT void*        GblTreeSet_set          (GBL_SELF, const void* pEntry)          GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_setHint      (GBL_SELF,
                                                 const void* pEntry,
                                                 uint64_t* pHint)                       GBL_NOEXCEPT;
//GBL_EXPORT GblBool      GblTreeSet_insert       (GBL_SELF, const void* pEntry)              GBL_NOEXCEPT;
//GBL_EXPORT void         GblTreeSet_insertOrAssign(GBL_SELF, const void* pEntry)             GBL_NOEXCEPT;
//GBL_EXPORT void*        GblTreeSet_emplace      (GBL_SELF, const void* pEntry)              GBL_NOEXCEPT;
//GBL_EXPORT void*        GblTreeSet_tryEmplace   (GBL_SELF, const void* pEntry)              GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblTreeSet_erase        (GBL_SELF,  const void* pKey)           GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_extract      (GBL_SELF,  const void* pKey)           GBL_NOEXCEPT;
GBL_EXPORT void         GblTreeSet_clear        (GBL_SELF)                              GBL_NOEXCEPT;

// probe, foreach, or next for iteration



// ======== INLINE IMPLEMENTATION =========

GBL_INLINE GblSize GblTreeSet_size(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->count;
}

GBL_INLINE GblSize GblTreeSet_height(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->height;
}

GBL_INLINE GblContext* GblTreeSet_context(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->pCtx;
}

GBL_INLINE GblBool GblTreeSet_empty(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->count ? GBL_FALSE : GBL_TRUE;
}

GBL_INLINE void* GblTreeSet_userdata(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->pUserdata;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_TREE_SET_H
