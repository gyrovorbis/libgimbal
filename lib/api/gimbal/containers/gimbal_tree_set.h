/*! \file
 *  \brief GblHashSet structure and related functions
 *  \ingroup containers
 *
 *  \author Josh Baker
 *  \author 2023 Falco Girgis
 */

#ifndef GIMBAL_BTREE_H
#define GIMBAL_BTREE_H

#include <time.h>
#include "../core/gimbal_ctx.h"

#define GBL_SELF_TYPE GblTreeSet

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTreeSet);

typedef int  (*GblTreeSetCompareFn) (GBL_CSELF, const void*, const void*);
typedef void (*GblTreeSetDestructFn)(GBL_CSELF, void*);

//! Internal structure representing a single node within a GblTreeSet
typedef struct GblTreeSetNode {
    uint16_t                entryCount;
    GblBool                 leaf;
    uint8_t                 padding[sizeof(void*)-3];
    void*                   pEntries;
    struct GblTreeSetNode*  pChildren[1];
} GblTreeSetNode;

//! Internal structure representing a group of nodes within a GblTreeSet
typedef struct GblTreeSetGroup {
    GblTreeSetNode**        ppNodes;
    size_t                  length;
    size_t                  capacity;
} GblTreeSetGroup;

//! Internal structure representing a pool of \ref GblTreeSetGroup items within a GblTreeSet
typedef struct GblTreeSetPool {
    GblTreeSetGroup         leaves;
    GblTreeSetGroup         branches;
 } GblTreeSetPool;

/*! \brief Binary tree based abstract associative container with C++-style STL API
 *  \ingroup containers
 */
typedef struct GblTreeSet {
    GblContext*             pCtx;
    size_t                  entrySize;
    GblTreeSetCompareFn     pFnCompare;
    GblTreeSetDestructFn    pFnDestruct;
    GblTreeSetPool          pool;
    GblTreeSetNode*         pRoot;
    size_t                  count;
    size_t                  height;
    size_t                  maxCount;
    size_t                  minCount;
    void*                   pSpares[3];
    void*                   pLastEntry;
    void*                   pLastNode;
    void*                   pUserdata;
} GblTreeSet;

//! Represents an iterator for iterating over a GblTreeSet
typedef struct GblTreeSetIterator {
    GblTreeSet*         pContainer;
    GblTreeSetNode*     pNode;
    uint16_t            index;
} GblTreeSetIterator;


GBL_EXPORT GBL_RESULT   GblTreeSet_construct_7  (GBL_SELF,
                                                 size_t                    entrySize,
                                                 GblTreeSetCompareFn  pFnCompare,
                                                 GblTreeSetDestructFn pFnDestruct,
                                                 size_t                    maxEntries,
                                                 GblContext*               pCtx,
                                                 void*                     pUserdata)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblTreeSet_construct_6  (GBL_SELF,
                                                 size_t                    entrySize,
                                                 GblTreeSetCompareFn  pFnCompare,
                                                 GblTreeSetDestructFn pFnDestruct,
                                                 size_t                    maxEntries,
                                                 GblContext*               pCtx)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblTreeSet_construct_5  (GBL_SELF,
                                                 size_t                    entrySize,
                                                 GblTreeSetCompareFn  pFnCompare,
                                                 GblTreeSetDestructFn pFnDestruct,
                                                 size_t                    maxEntries)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblTreeSet_construct_4  (GBL_SELF,
                                                 size_t                    entrySize,
                                                 GblTreeSetCompareFn  pFnCompare,
                                                 GblTreeSetDestructFn pFnDestruct) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblTreeSet_construct_3  (GBL_SELF,
                                                 size_t                    entrySize,
                                                 GblTreeSetCompareFn  pFnCompare)  GBL_NOEXCEPT;
#define                 GblTreeSet_construct(...) \
                                                  GBL_VA_OVERLOAD_CALL_ARGC(GblTreeSet_construct, __VA_ARGS__)

GBL_EXPORT GBL_RESULT   GblTreeSet_destruct     (GBL_SELF)                              GBL_NOEXCEPT;

GBL_EXPORT size_t       GblTreeSet_size         (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT size_t       GblTreeSet_height       (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT GblContext*  GblTreeSet_context      (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblTreeSet_empty        (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_userdata     (GBL_CSELF)                             GBL_NOEXCEPT;

GBL_EXPORT void*        GblTreeSet_get          (GBL_CSELF, const void* pKey)           GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_getHint      (GBL_CSELF,
                                                 const void* pKey,
                                                 uint64_t* pHint)                       GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_at           (GBL_CSELF, const void* pKey)           GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_atHint       (GBL_CSELF,
                                                 const void* pKey,
                                                 uint64_t* pHint)                       GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblTreeSet_contains     (GBL_CSELF, const void* pKey)           GBL_NOEXCEPT;
GBL_EXPORT size_t       GblTreeSet_count        (GBL_CSELF, const void* pKey)           GBL_NOEXCEPT;
//GBL_EXPORT ???        GblTreeSet_find         (GBL_CSELF, const void* pKey)               GBL_NOEXCEPT;

GBL_EXPORT void*        GblTreeSet_set          (GBL_SELF, const void* pEntry)          GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_setHint      (GBL_SELF,
                                                 const void* pEntry,
                                                 uint64_t* pHint)                       GBL_NOEXCEPT;
//GBL_EXPORT GblBool      GblTreeSet_insert       (GBL_SELF, const void* pEntry)              GBL_NOEXCEPT;
//GBL_EXPORT void         GblTreeSet_insertOrAssign(GBL_SELF, const void* pEntry)             GBL_NOEXCEPT;
//GBL_EXPORT void*        GblTreeSet_emplace      (GBL_SELF, const void* pEntry)              GBL_NOEXCEPT;
//GBL_EXPORT void*        GblTreeSet_tryEmplace   (GBL_SELF, const void* pEntry)              GBL_NOEXCEPT;

GBL_EXPORT void*        GblTreeSet_popMax       (GBL_SELF)                              GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_popMin       (GBL_SELF)                              GBL_NOEXCEPT;

GBL_EXPORT void*        GblTreeSet_min          (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_max          (GBL_CSELF)                             GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblTreeSet_erase        (GBL_SELF,  const void* pKey)           GBL_NOEXCEPT;
GBL_EXPORT void*        GblTreeSet_extract      (GBL_SELF,  const void* pKey)           GBL_NOEXCEPT;
GBL_EXPORT void         GblTreeSet_clear        (GBL_SELF)                              GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_TREE_SET_H
