/*! \file
 *  \brief GblHashSet structure and related functions
 *  \ingroup containers
 */

#ifndef GIMBAL_HASHSET_H
#define GIMBAL_HASHSET_H

#include "../types/gimbal_typedefs.h"
#include "../core/gimbal_api_frame.h"

#define SELF    GblHashSet*         pSelf
#define CSELF   const GblHashSet*   pSelf

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblHashSet);

typedef GblHash       (*GblHashSetEntryHashFn)    (CSELF, const void*);
typedef GblBool       (*GblHashSetEntryCompareFn) (CSELF, const void*, const void*);
typedef void          (*GblHashSetEntryDestructFn)(CSELF, void*);
typedef GblBool       (*GblHashSetIterateFn)      (CSELF, void*, void*);

/*! \brief Hash table based abstract associative container with C++-style STL API
 *  \details
 *      Robinhood hashing
 *  \ingroup containers
 */
typedef struct GblHashSet {
    GblContext*                 pCtx;
    GblSize                     entrySize;
    GblSize                     capacity;
    GblHashSetEntryHashFn       pFnHash;
    GblHashSetEntryCompareFn    pFnCompare;
    GblHashSetEntryDestructFn   pFnDestruct;
    GblSize                     bucketSize;
    GblSize                     bucketCount;
    GblSize                     count;
    GblSize                     mask;
    void*                       pBuckets;
    void*                       pSpare;
    void*                       pUserdata;
} GblHashSet;

typedef struct GblHashSetIterator {
    GblHashSet*         pSet;
    GblSize             bucketIdx;
} GblHashSetIterator;

GBL_API             GblHashSet_construct_8(SELF,
                                           GblSize                     entrySize,
                                           GblHashSetEntryHashFn       pFnHash,
                                           GblHashSetEntryCompareFn    pFnCompare,
                                           GblHashSetEntryDestructFn   pFnDestruct,
                                           GblSize                     capacity,
                                           GblContext*                 pCtx,
                                           void*                       pUserdata)   GBL_NOEXCEPT;
GBL_API             GblHashSet_construct_7(SELF,
                                           GblSize                     entrySize,
                                           GblHashSetEntryHashFn       pFnHash,
                                           GblHashSetEntryCompareFn    pFnCompare,
                                           GblHashSetEntryDestructFn   pFnDestruct,
                                           GblSize                     capacity,
                                           GblContext*                 pCtx)        GBL_NOEXCEPT;
GBL_API             GblHashSet_construct_6(SELF,
                                           GblSize                     entrySize,
                                           GblHashSetEntryHashFn       pFnHash,
                                           GblHashSetEntryCompareFn    pFnCompare,
                                           GblHashSetEntryDestructFn   pFnDestruct,
                                           GblSize                     capacity)    GBL_NOEXCEPT;
GBL_API             GblHashSet_construct_5(SELF,
                                           GblSize                     entrySize,
                                           GblHashSetEntryHashFn       pFnHash,
                                           GblHashSetEntryCompareFn    pFnCompare,
                                           GblHashSetEntryDestructFn   pFnDestruct) GBL_NOEXCEPT;
GBL_API             GblHashSet_construct_4(SELF,
                                           GblSize                     entrySize,
                                           GblHashSetEntryHashFn       pFnHash,
                                           GblHashSetEntryCompareFn    pFnCompare)  GBL_NOEXCEPT;
#define GblHashSet_construct(...) \
    GBL_VA_OVERLOAD_SELECT(GblHashSet_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

GBL_API                 GblHashSet_clone            (SELF, const GblHashSet* pRhs,
                                                     GblContext* hCtx)              GBL_NOEXCEPT;

GBL_API                 GblHashSet_constructMove    (SELF, GblHashSet* pRhs,
                                                     GblContext* hCtx)              GBL_NOEXCEPT;

GBL_API                 GblHashSet_constructClone   (SELF, GblHashSet* pRhs)        GBL_NOEXCEPT;
GBL_API                 GblHashSet_assignMove       (SELF, GblHashSet* pRhs)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblHashSet_destruct         (SELF)                          GBL_NOEXCEPT;

GBL_INLINE GblSize      GblHashSet_size             (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblSize      GblHashSet_bucketCount      (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblSize      GblHashSet_bucketSize       (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblContext*  GblHashSet_context          (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblBool      GblHashSet_empty            (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE void*        GblHashSet_userdata         (CSELF)                         GBL_NOEXCEPT;

GBL_EXPORT void*        GblHashSet_get              (CSELF, const void* pKey)       GBL_NOEXCEPT; //raw get, no error
GBL_EXPORT void*        GblHashSet_at               (CSELF, const void* pKey)       GBL_NOEXCEPT; //throws range error
GBL_EXPORT GblBool      GblHashSet_contains         (CSELF, const void* pKey)       GBL_NOEXCEPT; //true if entry exists
GBL_EXPORT GblSize      GblHashSet_count            (CSELF, const void* pKey)       GBL_NOEXCEPT; //# of matching entries (0 or 1)
GBL_EXPORT GblHashSetIterator
                        GblHashSet_find             (CSELF, const void* pKey)       GBL_NOEXCEPT;

GBL_EXPORT void*        GblHashSet_set              (SELF, const void* pEntry)      GBL_NOEXCEPT; //raw set, returns existing item w/o deleting
GBL_EXPORT GblBool      GblHashSet_insert           (SELF, const void* pEntry)      GBL_NOEXCEPT; //throws duplicate error
GBL_EXPORT void         GblHashSet_insertOrAssign   (SELF, const void* pEntry)      GBL_NOEXCEPT; //deletes any overwritten value
// Returns a raw data pointer to the buffer that you have to emplace over!
GBL_EXPORT void*        GblHashSet_emplace          (SELF, const void* pKey)        GBL_NOEXCEPT; //throws duplicate error
GBL_EXPORT void*        GblHashSet_tryEmplace       (SELF, const void* pKey)        GBL_NOEXCEPT; //gracefully returns NULL if already exists

GBL_EXPORT GblBool      GblHashSet_erase            (SELF, const void* pKey)        GBL_NOEXCEPT; //deletes entry, not found is fine
GBL_EXPORT void*        GblHashSet_extract          (SELF, const void* pKey)        GBL_NOEXCEPT; //removes entry, no deletion, not found is fine
GBL_EXPORT void         GblHashSet_clear            (SELF)                          GBL_NOEXCEPT; //deletes entries

GBL_EXPORT void*        GblHashSet_probe            (CSELF, GblSize position)       GBL_NOEXCEPT; //returns entry at slot or NULL, sparse
GBL_EXPORT GblBool      GblHashSet_foreach          (CSELF, GblHashSetIterateFn iter,              //iterates over every non-null position
                                                     void* pUdata)                  GBL_NOEXCEPT;
GBL_EXPORT GblHashSetIterator
                        GblHashSet_next(CSELF, const GblHashSetIterator* pPrevIt)   GBL_NOEXCEPT; //[Lua-style] Returns iterator after given
                                                                                                  //   NULL previous it => return first entry
                                                                                                  //   invalid return iterator => conatainer end
#undef  CSELF
#undef  SELF

#define SELF    GblHashSetIterator*         pSelf
#define CSELF   const GblHashSetIterator*   pSelf

GBL_EXPORT const GblHashSet*    GblHashSetIterator_container(CSELF)                 GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblHashSetIterator_valid    (CSELF)                 GBL_NOEXCEPT;
GBL_EXPORT void*                GblHashSetIterator_value    (CSELF)                 GBL_NOEXCEPT;

// ======== INLINE IMPL ======


GBL_INLINE void* GblHashSet_userdata(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return pSelf->pUserdata;
}

GBL_INLINE GblBool GblHashSet_empty(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return pSelf->count? GBL_FALSE : GBL_TRUE;
}

GBL_INLINE GblContext* GblHashSet_context(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return pSelf->pCtx;
}

GBL_INLINE GblSize  GblHashSet_bucketSize(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return pSelf->bucketSize;
}

GBL_INLINE GblSize GblHashSet_size(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return pSelf->count;
}

GBL_INLINE GblSize GblHashSet_bucketCount(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return pSelf->bucketCount;
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_HASHSET_H
