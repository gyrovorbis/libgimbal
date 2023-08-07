#include <gimbal/strings/gimbal_quark.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/allocators/gimbal_arena_allocator.h>
#include <limits.h>

#include <tinycthread.h>

#define GBL_QUARK_PAGE_SIZE_DEFAULT_            1024
#define GBL_QUARK_REGISTRY_CAPACITY_DEFAULT_    64
#define GBL_QUARK_ENSURE_INITIALIZED_() \
    GBL_STMT_START {   \
        if(!initializing_) { \
            if(!inittedOnce_) call_once(&initOnce_, gblQuarkInit_); \
            else if(!initialized_) gblQuarkInit_(); \
        } \
    } GBL_STMT_END

static struct {
    GblArenaAllocatorPage page;
    char                  staticBytes[GBL_QUARK_PAGE_SIZE_DEFAULT_-1];
} pageStatic_ = {
    .page = {
        .capacity    = GBL_QUARK_PAGE_SIZE_DEFAULT_,
        .staticAlloc = GBL_TRUE
    }
};

static GblArenaAllocator arena_;
static GblHashSet        registry_;
static GblBool           initialized_    = GBL_FALSE;
static GblBool           inittedOnce_    = GBL_FALSE;
static once_flag         initOnce_       = ONCE_FLAG_INIT;
static mtx_t             registryMtx_;
static
GBL_THREAD_LOCAL GblBool initializing_   = GBL_FALSE;

static GblContext*       pCtx_           = NULL;
static size_t            pageSize_       = GBL_QUARK_PAGE_SIZE_DEFAULT_;
static size_t            registryCap_    = GBL_QUARK_REGISTRY_CAPACITY_DEFAULT_;

static int registryComparator_(const GblHashSet* pSet, const char** pA, const char** pB) {
    GBL_UNUSED(pSet);
    if(!pA || !*pA) return INT_MIN;
    else if(!pB || !*pB) return INT_MAX;
    else return (strcmp(*pA, *pB) == 0);
}

static GblHash registryHasher_(const GblHashSet* pSet, const char** pStr) {
    GBL_UNUSED(pSet);
    return gblHash(*pStr, strlen(*pStr));
}

static const char* quarkStringAllocCopy_(const char* pString) {
    char* pNewString = NULL;
    GBL_ASSERT(pString);

    const size_t  size = strlen(pString) + 1;
    pNewString = GblArenaAllocator_alloc(&arena_, size, 1);
    if(pNewString) strcpy(pNewString, pString);

    return pNewString;
}

static void GblQuark_final_(void) {
    GblQuark_final();
}

static void gblQuarkInit_(void) {
    initializing_ = GBL_TRUE;
    GblBool mtxLocked = GBL_FALSE;
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_PUSH_VERBOSE("[GblQuark]: Initializing");
    GBL_CTX_VERIFY_EXPRESSION(!initialized_);
    mtx_init(&registryMtx_, mtx_recursive);
    mtx_lock(&registryMtx_);
    mtxLocked = GBL_TRUE;
    GBL_CTX_CALL(GblHashSet_construct(&registry_,
                                      sizeof(const char*),
                                      (GblHashSetHashFn)registryHasher_,
                                      (GblHashSetCmpFn)registryComparator_,
                                      NULL,
                                      registryCap_,
                                      pCtx_));

    GBL_CTX_CALL(GblArenaAllocator_construct(&arena_,
                                             pageSize_,
                                             0,
                                             &pageStatic_.page,
                                             pCtx_));
    initialized_            = GBL_TRUE;
    inittedOnce_            = GBL_TRUE;
    atexit(GblQuark_final_);
    GBL_CTX_POP(1);
    GBL_CTX_END_BLOCK();
    initializing_ = GBL_FALSE;
    if(mtxLocked) mtx_unlock(&registryMtx_);
}

static  GblQuark quarkFromString_(const char* pString, GblBool alloc) {
    GblQuark quark = 0;
    if(pString) {
        GBL_QUARK_ENSURE_INITIALIZED_();
        quark = GblQuark_tryString(pString);
        if(!quark) {
            mtx_lock(&registryMtx_);
            if(alloc) pString = quarkStringAllocCopy_(pString);
            GBL_ASSERT(pString);
            if(pString) {
                GblBool inserted = GblHashSet_insert(&registry_, &pString);
                mtx_unlock(&registryMtx_);
                GBL_UNUSED(inserted);
                //GBL_ASSERT(inserted);
            }
            quark = (GblQuark)pString;
        }
    }
    return quark;
}

static inline GblQuark* GblQuark_tryString_(const char* pString) {
    GblQuark* pQuark  = NULL;
    mtx_lock(&registryMtx_);
    pQuark = (GblQuark*)GblHashSet_get(&registry_, &pString);
    mtx_unlock(&registryMtx_);
    return pQuark;
}

GBL_EXPORT GBL_RESULT GblQuark_final(void) {
    GblBool hasMutex = GBL_FALSE;
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_PUSH_VERBOSE("[GblQuark]: Finalizing");
    GBL_CTX_VERIFY_EXPRESSION(initialized_);
    mtx_lock(&registryMtx_);
    hasMutex = GBL_TRUE;
    GBL_CTX_VERIFY_CALL(GblHashSet_destruct(&registry_));

    GBL_CTX_VERIFY_CALL(GblArenaAllocator_destruct(&arena_));

    initialized_ = GBL_FALSE;
    GBL_CTX_POP(1);
    GBL_CTX_END_BLOCK();
    if(hasMutex) {
        mtx_unlock(&registryMtx_);
        if(!initialized_) mtx_destroy(&registryMtx_);
    }
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblQuark_init(GblContext* pCtx, size_t  pageSize, size_t  initialEntries) {
    GBL_CTX_BEGIN(NULL);
    if(!initialized_) {
        pCtx_           = pCtx;
        pageSize_       = pageSize;
        registryCap_    = initialEntries;
        GBL_QUARK_ENSURE_INITIALIZED_();
    } else {
        GBL_CTX_PUSH_VERBOSE("[GblQuark]: Reinititalizing");
        GBL_CTX_CALL(GblQuark_final());
        pCtx_           = pCtx;
        pageSize_       = pageSize;
        registryCap_    = initialEntries;
        gblQuarkInit_();
    }
    GBL_CTX_END();
}

GBL_EXPORT size_t GblQuark_pageCount(void) {
    size_t count = 1;
    if(initialized_) {
        count = GblArenaAllocator_pageCount(&arena_);
    }
    return count;
}

GBL_EXPORT size_t GblQuark_pageSize(void) {
    size_t size = GBL_QUARK_PAGE_SIZE_DEFAULT_;
    if(initialized_) {
        size = arena_.pageSize;
    }
    return size;
}

GBL_EXPORT size_t GblQuark_bytesUsed(void) {
    size_t  bytes = 0;
    if(initialized_) {
        bytes = GblArenaAllocator_bytesUsed(&arena_);
    }
    return bytes;
}

GBL_EXPORT size_t GblQuark_bytesAvailable(void) {
    size_t bytes = 0;
    if(initialized_) {
        bytes = GblArenaAllocator_bytesAvailable(&arena_);
    }
    return bytes;
}

GBL_EXPORT size_t GblQuark_totalCapacity(void) {
    size_t bytes = 0;
    if(initialized_) {
        bytes = GblArenaAllocator_totalCapacity(&arena_);
    }
    return bytes;
}

GBL_EXPORT size_t GblQuark_fragmentedBytes(void) {
    size_t bytes = 0;
    if(initialized_) {
        bytes = GblArenaAllocator_fragmentedBytes(&arena_);
    }
    return bytes;
}

GBL_EXPORT float GblQuark_utilization(void) {
    float value = 0.0f;
    if(initialized_) {
        value = GblArenaAllocator_utilization(&arena_);
    }
    return value;
}

GBL_EXPORT size_t  GblQuark_count(void) {
    size_t  count = 0;
    if(initialized_) {
        count = GblHashSet_size(&registry_);
    }
    return count;
}

GBL_EXPORT GblContext* GblQuark_context(void) {
    return pCtx_;
}

GBL_EXPORT GblQuark (GblQuark_tryString)(const char* pString, size_t length) {
    GblQuark quark = GBL_QUARK_INVALID;

    if(initialized_ && pString) {
        if(length) {
            char* pTempStr = GBL_ALLOCA(length + 1);
            strncpy(pTempStr, pString, length);
            pTempStr[length] = '\0';
            pString = pTempStr;
        }

        GblQuark* pQuark = GblQuark_tryString_(pString);
        if(pQuark) quark = *pQuark;
    }
    return quark;
}

GBL_EXPORT GblQuark (GblQuark_fromString)(const char* pString, size_t length) {
    GblQuark quark = GBL_QUARK_INVALID;

    if(pString) {
        if(length) {
            char* pTempStr = GBL_ALLOCA(length + 1);
            strncpy(pTempStr, pString, length);
            pTempStr[length] = '\0';
            pString = pTempStr;
        }

        quark = quarkFromString_(pString, GBL_TRUE);
    }
    return quark;
}

GBL_EXPORT GblQuark GblQuark_fromStatic(const char* pString) {
    return quarkFromString_(pString, GBL_FALSE);
}

GBL_EXPORT const char* GblQuark_toString(GblQuark quark) {
    return (const char*)quark;
}

GBL_EXPORT const char* GblQuark_internString(const char* pString, size_t len) {
    return GblQuark_toString(GblQuark_fromString(pString, len));
}

GBL_EXPORT const char* GblQuark_internStatic(const char* pString) {
    return GblQuark_toString(GblQuark_fromStatic(pString));
}
