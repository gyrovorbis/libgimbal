/*! \file
 *  \brief GblHashSet structure and related functions
 *  \ingroup containers
 *
 *  \author Josh Baker
 *  \author Falco Girgis
 */

#ifndef GIMBAL_HASHSET_H
#define GIMBAL_HASHSET_H

#include "../core/gimbal_typedefs.h"
#include "../core/gimbal_ctx.h"

#define GBL_SELF_TYPE GblHashSet

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblHashSet);

typedef GblHash (*GblHashSetHashFn)(GBL_CSELF, const void*);              ///< User-defined hasher function, hashing two entries
typedef GblBool (*GblHashSetCmpFn) (GBL_CSELF, const void*, const void*); ///< User-defined comparator function, comparing two entries
typedef void    (*GblHashSetDtorFn)(GBL_CSELF, void*);                    ///< User-defined destructor function, destroying an entry
typedef GblBool (*GblHashSetIterFn)(GBL_CSELF, void*, void*);             ///< User-defined iterator function for traversal

/*! \brief Hash-table based abstract associative container with C++-style STL std::unoredered_set API
 *  \details
 *  GblHashSet uses open-addressing ans is implemented using a Robin Hood hashing algorithm.
 *  Using it requires providing a custom hasher function (which typically uses one of
 *  the libGimbal hashing algorithms such as gblHashMurmur()) as well as a custom comparator function.
 *
 *  \note
 *  Performance is pretty darn good. Read speed is faster than both C++'s std::unordered_pSelf and
 *  Qt's QHash (despite being runtime polymorphic with C function pointers) while write speed
 *  is slightly slower than both.
 *
 *  \ingroup containers
 */
typedef struct GblHashSet {
    GBL_PRIVATE_BEGIN
        GblContext*        pCtx;
        size_t             entrySize;
        size_t             capacity;
        GblHashSetHashFn   pFnHash;
        GblHashSetCmpFn    pFnCompare;
        GblHashSetDtorFn   pFnDestruct;
        size_t             bucketSize;
        size_t             bucketCount;
        size_t             count;
        size_t             mask;
        void*              pBuckets;
        void*              pSpare;
        void*              pUserdata;
    GBL_PRIVATE_END
} GblHashSet;

/*! \brief Iterator structure used for iterating over GblHashSet
 *  \sa GblHashSet
 */
typedef struct GblHashSetIter {
    GBL_PRIVATE_BEGIN
        GblHashSet*        pSet;
        size_t             bucketIdx;
    GBL_PRIVATE_END
} GblHashSetIter;

GBL_EXPORT GBL_RESULT        GblHashSet_construct_8   (GBL_SELF,
                                                       size_t             entrySize,
                                                       GblHashSetHashFn   pFnHash,
                                                       GblHashSetCmpFn    pFnCompare,
                                                       GblHashSetDtorFn   pFnDestruct,
                                                       size_t             capacity,
                                                       GblContext*        pCtx,
                                                       void*              pUserdata)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_construct_7   (GBL_SELF,
                                                       size_t             entrySize,
                                                       GblHashSetHashFn   pFnHash,
                                                       GblHashSetCmpFn    pFnCompare,
                                                       GblHashSetDtorFn   pFnDestruct,
                                                       size_t             capacity,
                                                       GblContext*        pCtx)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_construct_6   (GBL_SELF,
                                                       size_t             entrySize,
                                                       GblHashSetHashFn   pFnHash,
                                                       GblHashSetCmpFn    pFnCompare,
                                                       GblHashSetDtorFn   pFnDestruct,
                                                       size_t             capacity)     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_construct_5   (GBL_SELF,
                                                       size_t             entrySize,
                                                       GblHashSetHashFn   pFnHash,
                                                       GblHashSetCmpFn    pFnCompare,
                                                       GblHashSetDtorFn   pFnDestruct)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_construct_4   (GBL_SELF,
                                                       size_t             entrySize,
                                                       GblHashSetHashFn   pFnHash,
                                                       GblHashSetCmpFn    pFnCompare)   GBL_NOEXCEPT;
#define                      GblHashSet_construct(...) \
        GBL_VA_OVERLOAD_CALL_ARGC(GblHashSet_construct,  __VA_ARGS__)

GBL_EXPORT GBL_RESULT        GblHashSet_clone         (GBL_SELF,
                                                       const GblHashSet* pRhs,
                                                       GblContext*       hCtx)          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT        GblHashSet_constructMove (GBL_SELF,
                                                       GblHashSet* pRhs,
                                                       GblContext* hCtx)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT        GblHashSet_constructClone(GBL_SELF, GblHashSet* pRhs)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_assignMove    (GBL_SELF, GblHashSet* pRhs)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_destruct      (GBL_SELF)                        GBL_NOEXCEPT;

GBL_EXPORT size_t            GblHashSet_size          (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_EXPORT size_t            GblHashSet_bucketCount   (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_EXPORT size_t            GblHashSet_bucketSize    (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_EXPORT GblContext*       GblHashSet_context       (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_EXPORT GblBool           GblHashSet_empty         (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_EXPORT void*             GblHashSet_userdata      (GBL_CSELF)                       GBL_NOEXCEPT;

GBL_EXPORT void*             GblHashSet_get           (GBL_CSELF, const void* pKey)     GBL_NOEXCEPT; //raw get, no error
GBL_EXPORT void*             GblHashSet_at            (GBL_CSELF, const void* pKey)     GBL_NOEXCEPT; //throws range error
GBL_EXPORT GblBool           GblHashSet_contains      (GBL_CSELF, const void* pKey)     GBL_NOEXCEPT; //true if entry exists
GBL_EXPORT size_t            GblHashSet_count         (GBL_CSELF, const void* pKey)     GBL_NOEXCEPT; //# of matching entries (0 or 1)
GBL_EXPORT GblHashSetIter    GblHashSet_find          (GBL_CSELF, const void* pKey)     GBL_NOEXCEPT;

GBL_EXPORT void*             GblHashSet_set           (GBL_SELF, const void* pEntry)    GBL_NOEXCEPT; //raw set, returns existing item w/o deleting
GBL_EXPORT GblBool           GblHashSet_insert        (GBL_SELF, const void* pEntry)    GBL_NOEXCEPT; //throws duplicate error
GBL_EXPORT void              GblHashSet_insertOrAssign(GBL_SELF, const void* pEntry)    GBL_NOEXCEPT; //deletes any overwritten value
GBL_EXPORT void*             GblHashSet_emplace       (GBL_SELF, const void* pKey)      GBL_NOEXCEPT; //throws duplicate error
GBL_EXPORT void*             GblHashSet_tryEmplace    (GBL_SELF, const void* pKey)      GBL_NOEXCEPT; //gracefully returns NULL if already exists

GBL_EXPORT GblBool           GblHashSet_erase         (GBL_SELF, const void* pKey)      GBL_NOEXCEPT; //deletes entry, not found is fine
GBL_EXPORT void*             GblHashSet_extract       (GBL_SELF, const void* pKey)      GBL_NOEXCEPT; //removes entry, no deletion, not found is fine
GBL_EXPORT void              GblHashSet_clear         (GBL_SELF)                        GBL_NOEXCEPT; //deletes entries

GBL_EXPORT GBL_RESULT        GblHashSet_shrinkToFit   (GBL_SELF)                        GBL_NOEXCEPT;

GBL_EXPORT void*             GblHashSet_probe         (GBL_CSELF, size_t  position)     GBL_NOEXCEPT; //returns entry at slot or NULL, sparse

GBL_EXPORT GblBool           GblHashSet_foreach       (GBL_CSELF,
                                                       GblHashSetIterFn iter,
                                                       void*            pUdata)         GBL_NOEXCEPT; //iterates over every non-null position

GBL_EXPORT GblHashSetIter    GblHashSet_next          (GBL_CSELF,
                                                       const GblHashSetIter* pPrev)     GBL_NOEXCEPT; //[Lua-style] Returns iterator after given
GBL_EXPORT const GblHashSet* GblHashSetIter_container (const GblHashSetIter* pSelf)     GBL_NOEXCEPT;
GBL_EXPORT GblBool           GblHashSetIter_valid     (const GblHashSetIter* pSelf)     GBL_NOEXCEPT;
GBL_EXPORT void*             GblHashSetIter_value     (const GblHashSetIter* pSelf)     GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_HASHSET_H
