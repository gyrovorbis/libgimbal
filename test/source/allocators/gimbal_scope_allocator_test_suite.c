#include "allocators/gimbal_scope_allocator_test_suite.h"
#include <gimbal/allocators/gimbal_scope_allocator.h>
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_doubly_linked_list.h>

#define GBL_SCOPE_ALLOCATOR_TEST_SUITE_(inst)   ((GblScopeAllocatorTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_SCOPE_ALLOCATOR_TEST_SUITE_TYPE))

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

typedef struct GblScopeAllocatorTestSuite_ {
    GblArenaAllocator   arena;
    GblScopeAllocator*  pScope;
    GblVariantListNode_ variantList;
} GblScopeAllocatorTestSuite_;



static GBL_RESULT GblScopeAllocatorTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblScopeAllocatorTestSuite_));
    GblArenaAllocator_construct(&pSelf_->arena, 32, 16, NULL, pCtx);
    GBL_CTX_END();
}

static GBL_RESULT GblScopeAllocatorTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);
    GblArenaAllocator_destruct(&pSelf_->arena);
    GBL_CTX_END();
}

static GBL_RESULT GblScopeAllocatorTestSuite_create_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);

    GBL_TEST_SKIP("TEMPORARILY REESTED");

    pSelf_->pScope = GblScopeAllocator_create(&pSelf_->arena);
    GBL_TEST_VERIFY(pSelf_->pScope);
    GBL_TEST_COMPARE(GblScopeAllocator_dtorCount(pSelf_->pScope), 0);


    GBL_CTX_END();
}

static GBL_RESULT GblScopeAllocatorTestSuite_destroy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);

    GBL_TEST_SKIP("TEMPORARILY REESTED");

    GBL_CTX_VERIFY_CALL(GblScopeAllocator_destroy(pSelf_->pScope));

    GBL_CTX_END();
}


GBL_EXPORT GblType GblScopeAllocatorTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "create",         GblScopeAllocatorTestSuite_create_  },
        { "destroy",        GblScopeAllocatorTestSuite_destroy_ },
        { NULL,             NULL            }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblScopeAllocatorTestSuite_init_,
        .pFnSuiteFinal  = GblScopeAllocatorTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblScopeAllocatorTestSuite"),
                                     &vTable,
                                     sizeof(GblScopeAllocatorTestSuite),
                                     sizeof(GblScopeAllocatorTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}
