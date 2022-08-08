#include <gimbal/strings/gimbal_quark.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/containers/gimbal_linked_list.h>
#ifdef _WIN32
#   define NOGDI
# else
#   define __USE_UNIX98
#endif
#include <tinycthread.h>

#define GBL_QUARK_PAGE_SIZE_DEFAULT_            1024
#define GBL_QUARK_REGISTRY_CAPACITY_DEFAULT_    32
#define GBL_QUARK_ENSURE_INITIALIZED_()                         \
    GBL_STMT_START {                                            \
        if(!inittedOnce_) call_once(&initOnce_, gblQuarkInit_); \
        else if(!initialized_) gblQuarkInit_();                 \
    } GBL_STMT_END

typedef struct QuarkAllocPage_ {
    GblLinkedListNode           listNode;
    GblSize                     pageSize;
    GblSize                     position;
    char                        bytes[1];
} QuarkAllocPage_;

static struct {
    QuarkAllocPage_ page;
    char            stackBytes[GBL_QUARK_PAGE_SIZE_DEFAULT_-1];
} pageStatic_ = {
    .page = {
        .listNode   = { .pNext = &pageStatic_.page.listNode },
        .pageSize   = GBL_QUARK_PAGE_SIZE_DEFAULT_,
        .position   = 0,
        .bytes      = { 0 }
    },
    .stackBytes     = { 0 }
};

static GblLinkedListNode    pageList_       = { .pNext = &pageStatic_.page.listNode };
static GblHashSet           registry_       = { 0 };
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

GBL_INLINE QuarkAllocPage_* pageCurrent_(void) {
    return GBL_LINKED_LIST_ENTRY(GblLinkedList_front(&pageList_), QuarkAllocPage_, listNode);
}

static void gblQuarkInit_(void) GBL_NOEXCEPT {
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
    GblLinkedList_init(&pageList_);
    GblLinkedList_pushFront(&pageList_, &pageStatic_.page.listNode);
    pageCurrent_()->position = 0;
    initialized_            = GBL_TRUE;
    inittedOnce_            = GBL_TRUE;
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    if(mtxLocked) mtx_unlock(&registryMtx_);
}

GBL_EXPORT GBL_RESULT GblQuark_final(void) GBL_NOEXCEPT {
    GblBool hasMutex = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblQuark]: Finalizing");
    GBL_API_VERIFY_EXPRESSION(initialized_);
    mtx_lock(&registryMtx_);
    hasMutex = GBL_TRUE;
    GBL_API_VERIFY_CALL(GblHashSet_destruct(&registry_));

    GblLinkedListNode* pIt = pageList_.pNext;
    while(pIt != &pageList_) {
        GblLinkedListNode* pNext = pIt->pNext;
        QuarkAllocPage_* pPage = GBL_LINKED_LIST_ENTRY(pIt, QuarkAllocPage_, listNode);
        if(pPage != &pageStatic_.page)
            GBL_API_FREE(pPage);
        pIt = pNext;
    }

    initialized_ = GBL_FALSE;
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    if(hasMutex) {
        mtx_unlock(&registryMtx_);
        if(!initialized_) mtx_destroy(&registryMtx_);
    }
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT GblQuark_init(GblContext* pCtx, GblSize pageSize, GblSize initialEntries) GBL_NOEXCEPT {
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


GBL_EXPORT GblSize GblQuark_pageCount(void) GBL_NOEXCEPT {
    GblSize count = 1;
    if(initialized_) {
        count = GblLinkedList_count(&pageList_);
    }
    return count;
}

GBL_EXPORT GblSize GblQuark_pageSize(void) GBL_NOEXCEPT {
    GblSize size = GBL_QUARK_PAGE_SIZE_DEFAULT_;
    if(initialized_) {
        size = pageCurrent_()->pageSize;
    }
    return size;
}

GBL_EXPORT GblSize GblQuark_bytesUsed(void) GBL_NOEXCEPT {
    GblSize bytes = 0;
    if(initialized_) {
        for(GblLinkedListNode* pIt = pageList_.pNext;
            pIt != &pageList_;
            pIt = pIt->pNext)
        {
            bytes += GBL_LINKED_LIST_ENTRY(pIt, QuarkAllocPage_, listNode)->position;
        }
    }
    return bytes;
}

GBL_EXPORT GblSize GblQuark_count(void) GBL_NOEXCEPT {
    GblSize count = 0;
    if(initialized_) {
        count = GblHashSet_size(&registry_);
    }
    return count;
}

GBL_EXPORT GblContext* GblQuark_context(void) GBL_NOEXCEPT {
    return pCtx_;
}

static inline GblQuark* GblQuark_tryString_(const char* pString) {
    GblQuark* pQuark  = NULL;
    mtx_lock(&registryMtx_);
    pQuark = (GblQuark*)GblHashSet_get(&registry_, &pString);
    mtx_unlock(&registryMtx_);
    return pQuark;
}

GBL_EXPORT GblQuark GblQuark_tryStringSized(const char* pString, GblSize length) GBL_NOEXCEPT {
    GblQuark quark = GBL_QUARK_INVALID;
    GBL_API_BEGIN(pCtx_);
    if(initialized_ && pString && length) {
        char* pCString = GBL_ALLOCA(length + 1);
        strncpy(pCString, pString, length);
        pCString[length] = '\0';
        GblQuark* pQuark = GblQuark_tryString_(pCString);
        if(pQuark) quark = *pQuark;
    }
    GBL_API_END_BLOCK();
    return quark;
}

GBL_EXPORT GblQuark GblQuark_tryString(const char* pString) GBL_NOEXCEPT {
    GblQuark quark = GBL_QUARK_INVALID;
    GBL_API_BEGIN(pCtx_);
    if(initialized_ && pString) {
        GblQuark* pQuark = GblQuark_tryString_(pString);
        if(pQuark) quark = *pQuark;
    }
    GBL_API_END_BLOCK();
    return quark;
}

const char* quarkStringAllocCopy_(const char* pString) {
    char* pNewString = NULL;
    GBL_API_BEGIN(pCtx_); {
        GBL_API_VERIFY_POINTER(pString);
        const GblSize size = strlen(pString) + 1;
        GBL_API_VERIFY(size <= GblQuark_pageSize(), GBL_RESULT_ERROR_OVERFLOW,
                       "Cannot allocate storage for Quark that is larger than page size! (%u vs %u)",
                       size, GblQuark_pageSize());
        if(pageCurrent_()->position + size > GblQuark_pageSize()) {
            QuarkAllocPage_* pPage = GBL_API_MALLOC(sizeof(QuarkAllocPage_)-1 + pageSize_);
            memset(pPage, 0, sizeof(QuarkAllocPage_)-1 + pageSize_);
            GblLinkedList_pushFront(&pageList_, &pPage->listNode);
            pageCurrent_()->pageSize = pageSize_;
        }
        pNewString = &pageCurrent_()->bytes[pageCurrent_()->position];
        strcpy(pNewString, pString);
        pageCurrent_()->position += size;
    } GBL_API_END_BLOCK();
    return pNewString;
}

GBL_EXPORT GblQuark quarkFromString_(const char* pString, GblBool alloc) GBL_NOEXCEPT {
    GblQuark quark = 0;
    GBL_API_BEGIN(pCtx_);
    if(pString) {
        GBL_QUARK_ENSURE_INITIALIZED_();
        quark = GblQuark_tryString(pString);
        if(!quark) {
            mtx_lock(&registryMtx_);
            if(alloc) pString = quarkStringAllocCopy_(pString);
            GBL_API_VERIFY_POINTER(pString);
            GblBool inserted = GblHashSet_insert(&registry_, &pString);
            GBL_API_VERBOSE("[GblQuark] Adding: %s", pString);
            mtx_unlock(&registryMtx_);
            GBL_API_VERIFY(inserted, GBL_RESULT_ERROR_INTERNAL,
                           "[GblQuark]: Failed to add string to hash map: %s", pString);
            quark = (GblQuark)pString;
        }
    }
    GBL_API_END_BLOCK();
    return quark;
}

GBL_EXPORT GblQuark GblQuark_fromString(const char* pString) GBL_NOEXCEPT {
    return quarkFromString_(pString, GBL_TRUE);
}

GBL_EXPORT GblQuark GblQuark_fromStringSized(const char* pString, GblSize length) GBL_NOEXCEPT {
    GblQuark quark = GBL_QUARK_INVALID;
    if(pString && length) { //maybe we're interning an empty string!?
        char* pCString = GBL_ALLOCA(length + 1);
        strncpy(pCString, pString, length);
        pCString[length] = '\0';
        quark = GblQuark_fromString(pCString);
    }
    return quark;
}

GBL_EXPORT GblQuark GblQuark_fromStringStatic(const char* pString) GBL_NOEXCEPT {
    return quarkFromString_(pString, GBL_FALSE);
}

GBL_EXPORT const char* GblQuark_toString(GblQuark quark) GBL_NOEXCEPT {
    return (const char*)quark;
}

GBL_EXPORT const char* GblQuark_internString(const char* pString) GBL_NOEXCEPT {
    return GblQuark_toString(GblQuark_fromString(pString));
}

GBL_EXPORT const char* GblQuark_internStringSized(const char* pString, GblSize length) GBL_NOEXCEPT {
    return GblQuark_toString(GblQuark_fromStringSized(pString, length));
}

GBL_EXPORT const char* GblQuark_internStringStatic(const char* pString) GBL_NOEXCEPT {
    return GblQuark_toString(GblQuark_fromStringStatic(pString));
}
