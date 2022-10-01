#include "utils/gimbal_pool_allocator_test_suite.h"
#include <gimbal/utils/gimbal_pool_allocator.h>
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_array_list.h>
#include <gimbal/containers/gimbal_doubly_linked_list.h>
#include <gimbal/algorithms/gimbal_numeric.h>

#define GBL_POOL_ALLOCATOR_TEST_SUITE_(inst)   ((GblPoolAllocatorTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_POOL_ALLOCATOR_TEST_SUITE_TYPE))


typedef union GblVariantListNode_ {
    struct {
        struct GblVariantList_* pNext;
        struct GblVariantList_* pPrev;
    };
    GblDoublyLinkedListNode     list;
} GblVariantListNode_;


typedef struct GblVariantList_ {
    GblVariantListNode_ node;
    GblVariant          variant;
} GblVariantList_;

typedef struct GblPoolAllocatorTestSuite_ {
    GblPoolAllocator pool;
    struct {
        GblArenaAllocatorPage  page;
        char                   bytes[63];
    };
    GblVariantListNode_ variantList;
} GblPoolAllocatorTestSuite_;



static GBL_RESULT GblPoolAllocatorTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblPoolAllocatorTestSuite_* pSelf_ = GBL_POOL_ALLOCATOR_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblPoolAllocatorTestSuite_));

    GblDoublyLinkedList_init(&pSelf_->variantList.list);

    pSelf_->page.capacity    = 64;
    pSelf_->page.staticAlloc = GBL_TRUE;

    GBL_API_END();
}


static GBL_RESULT GblPoolAllocatorTestSuite_construct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblPoolAllocatorTestSuite_* pSelf_ = GBL_POOL_ALLOCATOR_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblPoolAllocator_construct(&pSelf_->pool,
                                                   sizeof(GblVariantList_),
                                                   5,
                                                   32,
                                                   &pSelf_->page,
                                                   pCtx));

    GBL_TEST_COMPARE(GblPoolAllocator_freeListSize(&pSelf_->pool), 0);

    GBL_API_END();
}

static GBL_RESULT GblPoolAllocatorTestSuite_new_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblPoolAllocatorTestSuite_* pSelf_ = GBL_POOL_ALLOCATOR_TEST_SUITE_(pSelf);

    GblVariantList_* pVariant = GblPoolAllocator_new(&pSelf_->pool);
    GBL_TEST_VERIFY(pVariant);
    GBL_TEST_VERIFY(!((uintptr_t)pVariant & 0x1f));

    GblVariant_constructUint32(&pVariant->variant, 0);

    GblDoublyLinkedList_pushBack(&pSelf_->variantList.list,
                                 &pVariant->node.list);

    GBL_TEST_COMPARE(pSelf_->pool.activeEntries, 1);
    GBL_TEST_COMPARE(GblArenaAllocator_pageCount(&pSelf_->pool.arena), 1);
    GBL_TEST_COMPARE(GblArenaAllocator_bytesUsed(&pSelf_->pool.arena),
                     gblAlignedAllocSize(sizeof(GblVariantList_), 32));

    GBL_API_END();
}

static GBL_RESULT GblPoolAllocatorTestSuite_delete_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblPoolAllocatorTestSuite_* pSelf_ = GBL_POOL_ALLOCATOR_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblPoolAllocator_delete(&pSelf_->pool,
                                                GblDoublyLinkedList_popFront(&pSelf_->variantList.list)));

    GBL_TEST_COMPARE(GblPoolAllocator_freeListSize(&pSelf_->pool), 1);
    GBL_TEST_COMPARE(pSelf_->pool.activeEntries, 0);

    GBL_API_END();
}

static GBL_RESULT GblPoolAllocatorTestSuite_renew_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblPoolAllocatorTestSuite_* pSelf_ = GBL_POOL_ALLOCATOR_TEST_SUITE_(pSelf);

    GblVariantList_* pVariant = GblPoolAllocator_new(&pSelf_->pool);
    GBL_TEST_VERIFY(pVariant);
    GBL_TEST_VERIFY(!((uintptr_t)pVariant & 0x1f));

    GblVariant_constructUint32(&pVariant->variant, 0);

    GblDoublyLinkedList_pushBack(&pSelf_->variantList.list,
                                 &pVariant->node.list);

    GBL_TEST_COMPARE(pSelf_->pool.arena.allocCount, 1);
    GBL_TEST_COMPARE(pSelf_->pool.activeEntries, 1);
    GBL_TEST_COMPARE(GblArenaAllocator_pageCount(&pSelf_->pool.arena), 1);
    GBL_TEST_COMPARE(GblArenaAllocator_bytesUsed(&pSelf_->pool.arena),
                     gblAlignedAllocSize(sizeof(GblVariantList_), 32));

    GBL_API_END();
}

static GBL_RESULT GblPoolAllocatorTestSuite_fillPage_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblPoolAllocatorTestSuite_* pSelf_ = GBL_POOL_ALLOCATOR_TEST_SUITE_(pSelf);


    GblVariantList_* pVariant = GblPoolAllocator_new(&pSelf_->pool);
    GBL_TEST_VERIFY(pVariant);
    GBL_TEST_VERIFY(!((uintptr_t)pVariant & 0x1f));

    GblVariant_constructUint32(&pVariant->variant, 2);

    GblDoublyLinkedList_pushBack(&pSelf_->variantList.list,
                                 &pVariant->node.list);


    GBL_TEST_COMPARE(pSelf_->pool.arena.allocCount, 2);
    GBL_TEST_COMPARE(pSelf_->pool.activeEntries, 2);
    GBL_TEST_COMPARE(GblArenaAllocator_bytesUsed(&pSelf_->pool.arena),
                     gblAlignedAllocSize(sizeof(GblVariantList_), 32) * 2);
    GBL_TEST_COMPARE(GblArenaAllocator_pageCount(&pSelf_->pool.arena), 1);
    GBL_TEST_COMPARE(GblArenaAllocator_bytesAvailable(&pSelf_->pool.arena), 0);

    GBL_API_END();
}

static GBL_RESULT GblPoolAllocatorTestSuite_overflowPage_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblPoolAllocatorTestSuite_* pSelf_ = GBL_POOL_ALLOCATOR_TEST_SUITE_(pSelf);

    for(GblSize i = 0; i < 10; ++i) {
        GblVariantList_* pVariant = GblPoolAllocator_new(&pSelf_->pool);
        GBL_TEST_VERIFY(pVariant);
        GBL_TEST_VERIFY(!((uintptr_t)pVariant & 0x1f));

        GblVariant_constructUint32(&pVariant->variant, 2);

        GblDoublyLinkedList_pushBack(&pSelf_->variantList.list,
                                     &pVariant->node.list);
    }

    GBL_TEST_COMPARE(pSelf_->pool.arena.allocCount, 12);
    GBL_TEST_COMPARE(pSelf_->pool.activeEntries, 12);
    GBL_TEST_COMPARE(GblArenaAllocator_bytesUsed(&pSelf_->pool.arena),
                     gblAlignedAllocSize(sizeof(GblVariantList_), 32) * 12);
    GBL_TEST_COMPARE(GblArenaAllocator_pageCount(&pSelf_->pool.arena), 3);
    GBL_TEST_COMPARE(GblArenaAllocator_bytesAvailable(&pSelf_->pool.arena), 0);

    GBL_API_END();
}

static GBL_RESULT GblPoolAllocatorTestSuite_renewAllOverflow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblPoolAllocatorTestSuite_* pSelf_ = GBL_POOL_ALLOCATOR_TEST_SUITE_(pSelf);

    GblDoublyLinkedListNode* pNode = NULL;
    GblSize freeCount = 0;
    while((pNode = GblDoublyLinkedList_popFront(&pSelf_->variantList.list))) {
        GBL_API_VERIFY_CALL(GblPoolAllocator_delete(&pSelf_->pool, pNode));
        ++freeCount;
    }

    GBL_TEST_COMPARE(GblPoolAllocator_freeListSize(&pSelf_->pool), freeCount);

    for(GblSize i = 0; i < 13; ++i) {
        GblVariantList_* pVariant = GblPoolAllocator_new(&pSelf_->pool);
        GBL_TEST_VERIFY(pVariant);
        GBL_TEST_VERIFY(!((uintptr_t)pVariant & 0x1f));

        GblVariant_constructUint32(&pVariant->variant, i);

        GblDoublyLinkedList_pushBack(&pSelf_->variantList.list,
                                     &pVariant->node.list);
    }

    GBL_TEST_COMPARE(pSelf_->pool.arena.allocCount, 13);
    GBL_TEST_COMPARE(pSelf_->pool.activeEntries, 13);
    GBL_TEST_COMPARE(GblArenaAllocator_bytesUsed(&pSelf_->pool.arena),
                     gblAlignedAllocSize(sizeof(GblVariantList_), 32) * 13);
    GBL_TEST_COMPARE(GblArenaAllocator_pageCount(&pSelf_->pool.arena), 4);
    GBL_TEST_VERIFY(GblArenaAllocator_bytesAvailable(&pSelf_->pool.arena) > 0);

    GBL_API_END();
}

static GBL_RESULT GblPoolAllocatorTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblPoolAllocatorTestSuite_* pSelf_ = GBL_POOL_ALLOCATOR_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblPoolAllocator_destruct(&pSelf_->pool));

    GBL_API_END();
}


GBL_EXPORT GblType GblPoolAllocatorTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "construct",         GblPoolAllocatorTestSuite_construct_         },
        { "new",               GblPoolAllocatorTestSuite_new_               },
        { "delete",            GblPoolAllocatorTestSuite_delete_            },
        { "renew",             GblPoolAllocatorTestSuite_renew_             },
        { "fillPage",          GblPoolAllocatorTestSuite_fillPage_          },
        { "overflowPage",      GblPoolAllocatorTestSuite_overflowPage_      },
        { "renewAllOverflow",  GblPoolAllocatorTestSuite_renewAllOverflow_  },
        { "destruct",          GblPoolAllocatorTestSuite_destruct_          },
        { NULL,             NULL                                            }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblPoolAllocatorTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblPoolAllocatorTestSuite"),
                                     &vTable,
                                     sizeof(GblPoolAllocatorTestSuite),
                                     sizeof(GblPoolAllocatorTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }

    return type;
}
