#include <gimbal/strings/gimbal_quark.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/utils/gimbal_arena_allocator.h>
#ifdef _WIN32
#   define NOGDI
# else
#   define __USE_UNIX98
#endif
#include <tinycthread.h>

#define GBL_QUARK_PAGE_SIZE_DEFAULT_            1024
#define GBL_QUARK_REGISTRY_CAPACITY_DEFAULT_    64
#define GBL_QUARK_ENSURE_INITIALIZED_()                         \
    GBL_STMT_START {                                            \
        if(!inittedOnce_) call_once(&initOnce_, gblQuarkInit_); \
        else if(!initialized_) gblQuarkInit_();                 \
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

static GblArenaAllocator    arena_;
static GblHashSet           registry_;
static GblBool              initialized_    = GBL_FALSE;
static GblBool              inittedOnce_    = GBL_FALSE;
static once_flag            initOnce_       = ONCE_FLAG_INIT;
static mtx_t                registryMtx_;

static GblContext*          pCtx_           = NULL;
static GblSize              pageSize_       = GBL_QUARK_PAGE_SIZE_DEFAULT_;
static GblSize              registryCap_    = GBL_QUARK_REGISTRY_CAPACITY_DEFAULT_;

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

static const char* quarkStringAllocCopy_(const char* pString) {
    char* pNewString = NULL;
    GBL_ASSERT(pString);

    const GblSize size = strlen(pString) + 1;
    pNewString = GblArenaAllocator_allocAligned(&arena_, size, 1);
    if(pNewString) strcpy(pNewString, pString);

    return pNewString;
}

static void gblQuarkInit_(void) {
    GblBool mtxLocked = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblQuark]: Initializing");
    GBL_API_VERIFY_EXPRESSION(!initialized_);
    mtx_init(&registryMtx_, mtx_plain);
    mtx_lock(&registryMtx_);
    mtxLocked = GBL_TRUE;
    GBL_API_CALL(GblHashSet_construct(&registry_,
                                      sizeof(const char*),
                                      (GblHashSetHashFn)registryHasher_,
                                      (GblHashSetCmpFn)registryComparator_,
                                      NULL,
                                      registryCap_,
                                      pCtx_));

    GBL_API_CALL(GblArenaAllocator_construct(&arena_,
                                             pageSize_,
                                             0,
                                             &pageStatic_.page,
                                             pCtx_));
    initialized_            = GBL_TRUE;
    inittedOnce_            = GBL_TRUE;
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
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
                GBL_ASSERT(inserted);
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
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblQuark]: Finalizing");
    GBL_API_VERIFY_EXPRESSION(initialized_);
    mtx_lock(&registryMtx_);
    hasMutex = GBL_TRUE;
    GBL_API_VERIFY_CALL(GblHashSet_destruct(&registry_));

    GBL_API_VERIFY_CALL(GblArenaAllocator_destruct(&arena_));

    initialized_ = GBL_FALSE;
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    if(hasMutex) {
        mtx_unlock(&registryMtx_);
        if(!initialized_) mtx_destroy(&registryMtx_);
    }
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT GblQuark_init(GblContext* pCtx, GblSize pageSize, GblSize initialEntries) {
    GBL_API_BEGIN(NULL);
    if(!initialized_) {
        pCtx_           = pCtx;
        pageSize_       = pageSize;
        registryCap_    = initialEntries;
        GBL_QUARK_ENSURE_INITIALIZED_();
    } else {
        GBL_API_PUSH_VERBOSE("[GblQuark]: Reinititalizing");
        GBL_API_CALL(GblQuark_final());
        pCtx_           = pCtx;
        pageSize_       = pageSize;
        registryCap_    = initialEntries;
        gblQuarkInit_();
    }
    GBL_API_END();
}

GBL_EXPORT GblSize GblQuark_pageCount(void) {
    GblSize count = 1;
    if(initialized_) {
        count = GblArenaAllocator_pageCount(&arena_);
    }
    return count;
}

GBL_EXPORT GblSize GblQuark_pageSize(void) {
    GblSize size = GBL_QUARK_PAGE_SIZE_DEFAULT_;
    if(initialized_) {
        size = arena_.pageSize;
    }
    return size;
}

GBL_EXPORT GblSize GblQuark_bytesUsed(void) {
    GblSize bytes = 0;
    if(initialized_) {
        bytes = GblArenaAllocator_bytesUsed(&arena_);
    }
    return bytes;
}

GBL_EXPORT GblSize GblQuark_bytesAvailable(void) {
    GblSize bytes = 0;
    if(initialized_) {
        bytes = GblArenaAllocator_bytesAvailable(&arena_);
    }
    return bytes;
}

GBL_EXPORT GblSize GblQuark_totalCapacity(void) {
    GblSize bytes = 0;
    if(initialized_) {
        bytes = GblArenaAllocator_totalCapacity(&arena_);
    }
    return bytes;
}

GBL_EXPORT GblSize GblQuark_fragmentedBytes(void) {
    GblSize bytes = 0;
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

GBL_EXPORT GblSize GblQuark_count(void) {
    GblSize count = 0;
    if(initialized_) {
        count = GblHashSet_size(&registry_);
    }
    return count;
}

GBL_EXPORT GblContext* GblQuark_context(void) {
    return pCtx_;
}

GBL_EXPORT GblQuark GblQuark_tryStringSized(const char* pString, GblSize length) {
    GblQuark quark = GBL_QUARK_INVALID;
    if(initialized_ && pString && length) {
        char* pCString = GBL_ALLOCA(length + 1);
        strncpy(pCString, pString, length);
        pCString[length] = '\0';
        GblQuark* pQuark = GblQuark_tryString_(pCString);
        if(pQuark) quark = *pQuark;
    }
    return quark;
}

GBL_EXPORT GblQuark GblQuark_tryString(const char* pString) {
    GblQuark quark = GBL_QUARK_INVALID;
    if(initialized_ && pString) {
        GblQuark* pQuark = GblQuark_tryString_(pString);
        if(pQuark) quark = *pQuark;
    }
    return quark;
}

GBL_EXPORT GblQuark GblQuark_fromString(const char* pString) {
    return quarkFromString_(pString, GBL_TRUE);
}

GBL_EXPORT GblQuark GblQuark_fromStringSized(const char* pString, GblSize length) {
    GblQuark quark = GBL_QUARK_INVALID;
    if(pString && length) { //maybe we're interning an empty string!?
        char* pCString = GBL_ALLOCA(length + 1);
        strncpy(pCString, pString, length);
        pCString[length] = '\0';
        quark = GblQuark_fromString(pCString);
    }
    return quark;
}

GBL_EXPORT GblQuark GblQuark_fromStringStatic(const char* pString) {
    return quarkFromString_(pString, GBL_FALSE);
}

GBL_EXPORT const char* GblQuark_toString(GblQuark quark) {
    return (const char*)quark;
}

GBL_EXPORT const char* GblQuark_internString(const char* pString) {
    return GblQuark_toString(GblQuark_fromString(pString));
}

GBL_EXPORT const char* GblQuark_internStringSized(const char* pString, GblSize length) {
    return GblQuark_toString(GblQuark_fromStringSized(pString, length));
}

GBL_EXPORT const char* GblQuark_internStringStatic(const char* pString) {
    return GblQuark_toString(GblQuark_fromStringStatic(pString));
}
