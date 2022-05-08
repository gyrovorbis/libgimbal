#include <gimbal/types/gimbal_quark.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <pthread.h>
#include <threads.h>

#define GBL_QUARK_PAGE_SIZE_DEFAULT_            1024
#define GBL_QUARK_REGISTRY_CAPACITY_DEFAULT_    32
#define GBL_QUARK_ENSURE_INITIALIZED_()         if(!initialized_) pthread_once(&initOnce_, gblQuarkInit_)

typedef struct QuarkAllocPage_ {
    struct QuarkAllocPage_*     pNext;
    GblSize                     pageSize;
    GblSize                     position;
    char                        bytes[1];
} QuarkAllocPage_;

static struct {
    QuarkAllocPage_ page;
    char            stackBytes[GBL_QUARK_PAGE_SIZE_DEFAULT_-1];
} pageStatic_ = {
    .page = {
        .pNext      = NULL,
        .pageSize   = GBL_QUARK_PAGE_SIZE_DEFAULT_,
        .position   = 0,
        .bytes      = { 0 }
    },
    .stackBytes     = { 0 }
};


static QuarkAllocPage_* pPageCurrent_  = &pageStatic_.page;
static GblHashSet       registry_       = { 0 };
static GblBool          initialized_    = GBL_FALSE;
static pthread_once_t   initOnce_       = PTHREAD_ONCE_INIT;
static pthread_mutex_t  registryMtx_;

static GblContext*      pCtx_           = NULL;
static GblSize          pageSize_       = GBL_QUARK_PAGE_SIZE_DEFAULT_;
static GblSize          registryCap_    = GBL_QUARK_REGISTRY_CAPACITY_DEFAULT_;

static int registryComparator_(const GblHashSet* pSet, const char** pA, const char** pB) {
    GBL_UNUSED(pSet);
    if(!pA || !*pA) return INT_MIN;
    else if(!pB || !*pB) return INT_MAX;
    else return (strcmp(*pA, *pB) == 0);
}

static GblHash registryHasher_(const GblHashSet* pSet, const char** pStr) {
    GBL_UNUSED(pSet);
    return gblHashMurmur(*pStr, strlen(*pStr));
}

static void gblQuarkInit_(void) GBL_NOEXCEPT {
    GblBool mtxLocked = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblQuark]: Initializing");
    GBL_API_VERIFY_EXPRESSION(!initialized_);
    pthread_mutex_init(&registryMtx_, NULL);
    pthread_mutex_lock(&registryMtx_);
    mtxLocked = GBL_TRUE;
    GBL_API_CALL(GblHashSet_construct(&registry_,
                                      sizeof(const char*),
                                      (GblHashSetEntryHashFn)registryHasher_,
                                      (GblHashSetEntryCompareFn)registryComparator_,
                                      NULL,
                                      registryCap_,
                                      pCtx_));
    pPageCurrent_           = &pageStatic_.page;
    pPageCurrent_->pNext    = NULL;
    pPageCurrent_->position = 0;
    initialized_            = GBL_TRUE;
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    if(mtxLocked) pthread_mutex_unlock(&registryMtx_);
}

static GBL_RESULT gblQuarkFinal_(void) GBL_NOEXCEPT {
    GblBool hasMutex = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblQuark]: Finalizing");
    GBL_API_VERIFY_EXPRESSION(initialized_);
    pthread_mutex_lock(&registryMtx_);
    GblHashSet_destruct(&registry_);
    for(QuarkAllocPage_* pPageIt = pageStatic_.page.pNext;
        pPageIt != NULL;
        pPageIt = pPageIt->pNext)
    {
        GBL_API_FREE(pPageIt);
    }
    initialized_ = GBL_FALSE;
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    if(hasMutex) {
        pthread_mutex_unlock(&registryMtx_);
        if(!initialized_) pthread_mutex_destroy(&registryMtx_);
    }
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT gblQuarkReinit(GblContext* pCtx, GblSize pageSize, GblSize initialEntries) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_PUSH_VERBOSE("[GblQuark]: Reinititalizing");
    if(!initialized_) {
        pCtx_           = pCtx;
        pageSize_       = pageSize;
        registryCap_    = initialEntries;
        GBL_QUARK_ENSURE_INITIALIZED_();
    } else {
        GBL_API_CALL(gblQuarkFinal_());
        pCtx_           = pCtx;
        pageSize_       = pageSize;
        registryCap_    = initialEntries;
        gblQuarkInit_();
    }
    GBL_API_END();
}


GblSize gblQuarkPageCount(void) GBL_NOEXCEPT {
    GblSize count = 1;
    if(initialized_) {
        for(QuarkAllocPage_* pIt = pageStatic_.page.pNext;
            pIt != NULL;
            pIt = pIt->pNext)
        {
            ++count;
        }
    }
    return count;
}

GblSize gblQuarkBytesUsed(void) GBL_NOEXCEPT {
    GblSize bytes = 0;
    if(initialized_) {
        for(QuarkAllocPage_* pIt = &pageStatic_.page;
            pIt != NULL;
            pIt = pIt->pNext)
        {
           bytes += pIt->position;
        }
    }
    return bytes;
}

GblSize gblQuarkCount(void) GBL_NOEXCEPT {
    GblSize count = 0;
    if(initialized_) {
        count = GblHashSet_size(&registry_);
    }
    return count;
}

GBL_EXPORT GblQuark gblQuarkTryString(const char* pString) GBL_NOEXCEPT {
    GblQuark quark = 0;
    GBL_API_BEGIN(pCtx_);
    if(initialized_ && pString) {
        GblQuark* pQuark = NULL;
        pthread_mutex_lock(&registryMtx_);
        pQuark = (GblQuark*)GblHashSet_get(&registry_, &pString);
        if(pQuark) quark = *pQuark;
        pthread_mutex_unlock(&registryMtx_);
        GBL_API_END_BLOCK();
    }
    return quark;
}

const char* quarkStringAllocCopy_(const char* pString) {
    const char* pNewString = NULL;
    GBL_API_BEGIN(pCtx_); {
        GBL_API_VERIFY_POINTER(pString);
        GblSize size = strlen(pString) + 1;
        GBL_API_VERIFY(size <= pageSize_, GBL_RESULT_ERROR_OVERFLOW,
                       "Cannot allocate storage for Quark that is larger than page size! (%d vs %d)",
                       size, pageSize_);
        if(pPageCurrent_->position + size >= pPageCurrent_->pageSize) {
            QuarkAllocPage_* pPage = GBL_API_MALLOC(sizeof(QuarkAllocPage_)-1 + pageSize_);
            pPageCurrent_->pNext = pPage;
            pPageCurrent_ = pPage;
            pPageCurrent_->position = 0;
        }
        pNewString = &pPageCurrent_->bytes[pPageCurrent_->position];
        strcpy(pNewString, pString);
        pPageCurrent_->position += size;
    } GBL_API_END_BLOCK();
    return pNewString;
}

GBL_EXPORT GblQuark quarkFromString_(const char* pString, GblBool alloc) GBL_NOEXCEPT {
    GblQuark quark = 0;
    GBL_API_BEGIN(pCtx_);
    if(pString) {
        GBL_QUARK_ENSURE_INITIALIZED_();
        quark = gblQuarkTryString(pString);
        if(!quark) {
            pthread_mutex_lock(&registryMtx_);
            if(alloc) pString = quarkStringAllocCopy_(pString);
            GBL_API_VERIFY_EXPRESSION(pString);
            GblBool inserted = GblHashSet_insert(&registry_, &pString);
            GBL_API_VERBOSE("[GblQuark] Adding: %s", pString);
            pthread_mutex_unlock(&registryMtx_);
            GBL_API_VERIFY(inserted, GBL_RESULT_ERROR_INTERNAL,
                           "[GblQuark]: Failed to add string to hash map: %s", pString);
            quark = (GblQuark)pString;
        }
        GBL_API_END_BLOCK();
    }
    return quark;
}


GBL_EXPORT GblQuark gblQuarkFromString(const char* pString) GBL_NOEXCEPT {
    return quarkFromString_(pString, GBL_TRUE);
}

GBL_EXPORT GblQuark gblQuarkFromStringStatic(const char* pString) GBL_NOEXCEPT {
    return quarkFromString_(pString, GBL_FALSE);
}

GBL_EXPORT const char* gblQuarkToString(GblQuark quark) GBL_NOEXCEPT {
    return (const char*)quark;
}

GBL_EXPORT const char* gblQuarkInternString(const char* pString) GBL_NOEXCEPT {
    return gblQuarkToString(gblQuarkFromString(pString));
}

GBL_EXPORT const char* gblQuarkInternStringStatic(const char* pString) GBL_NOEXCEPT {
    return gblQuarkToString(gblQuarkFromStringStatic(pString));
}
