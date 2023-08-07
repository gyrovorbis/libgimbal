#include "allocators/gimbal_arena_allocator_test_suite.h"
#include <gimbal/allocators/gimbal_arena_allocator.h>
#include <gimbal/test/gimbal_test_macros.h>

#define GBL_ARENA_ALLOCATOR_TEST_SUITE_(inst)   (GBL_PRIVATE(GblArenaAllocatorTestSuite, inst))

typedef struct GblArenaAllocatorTestSuite_ {
    GblArenaAllocator arena;
    struct {
        GblArenaAllocatorPage page;
        char                  bytes[127];
    };
} GblArenaAllocatorTestSuite_;

static GBL_RESULT GblArenaAllocatorTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArenaAllocatorTestSuite_* pSelf_ = GBL_ARENA_ALLOCATOR_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblArenaAllocatorTestSuite_));

    pSelf_->page.capacity    = 128;
    pSelf_->page.staticAlloc = GBL_TRUE;

    GBL_CTX_END();
}

static GBL_RESULT GblArenaAllocatorTestSuite_construct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArenaAllocatorTestSuite_* pSelf_ = GBL_ARENA_ALLOCATOR_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblArenaAllocator_construct(&pSelf_->arena, 128, 1, &pSelf_->page, pCtx));

    GBL_TEST_COMPARE(GblArenaAllocator_pageCount(&pSelf_->arena), 1);
    GBL_TEST_COMPARE(GblArenaAllocator_bytesUsed(&pSelf_->arena), 0);
    GBL_TEST_COMPARE(GblArenaAllocator_totalCapacity(&pSelf_->arena), 128);
    GBL_TEST_COMPARE(GblArenaAllocator_utilization(&pSelf_->arena), 0.0f);

    GBL_CTX_END();
}

static GBL_RESULT GblArenaAllocatorTestSuite_alloc_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArenaAllocatorTestSuite_* pSelf_ = GBL_ARENA_ALLOCATOR_TEST_SUITE_(pSelf);
    char* pStr = GblArenaAllocator_alloc(&pSelf_->arena, 12);
    GBL_TEST_VERIFY(pStr);

    strcpy(pStr, "1234567890");

    GBL_TEST_COMPARE(GblArenaAllocator_pageCount(&pSelf_->arena), 1);
    GBL_TEST_VERIFY(GblArenaAllocator_bytesUsed(&pSelf_->arena) >= 12);
    GBL_TEST_COMPARE(GblArenaAllocator_totalCapacity(&pSelf_->arena), 128);
    GBL_TEST_VERIFY(GblArenaAllocator_utilization(&pSelf_->arena) >= 12.0f / 128.0f);

    GBL_CTX_END();
}

static GBL_RESULT GblArenaAllocatorTestSuite_allocNewPage_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArenaAllocatorTestSuite_* pSelf_ = GBL_ARENA_ALLOCATOR_TEST_SUITE_(pSelf);

    size_t  bytesAvail = GblArenaAllocator_bytesAvailable(&pSelf_->arena);
    GBL_TEST_VERIFY(bytesAvail <= 128 - 12);

    char* pStr = GblArenaAllocator_alloc(&pSelf_->arena, bytesAvail+1);
    GBL_TEST_VERIFY(pStr);

    strcpy(pStr, "1234567890");

    GBL_TEST_COMPARE(GblArenaAllocator_pageCount(&pSelf_->arena), 2);
    GBL_TEST_COMPARE(GblArenaAllocator_totalCapacity(&pSelf_->arena), 256);
    GBL_TEST_VERIFY(GblArenaAllocator_fragmentedBytes(&pSelf_->arena) == bytesAvail);

    GBL_CTX_END();
}

static GBL_RESULT GblArenaAllocatorTestSuite_allocAligned_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArenaAllocatorTestSuite_* pSelf_ = GBL_ARENA_ALLOCATOR_TEST_SUITE_(pSelf);

    void* pPtr = GblArenaAllocator_alloc(&pSelf_->arena, 56, 64);
    GBL_TEST_VERIFY(!((uintptr_t)pPtr & 0x3f));

    GBL_CTX_END();
}

static GBL_RESULT GblArenaAllocatorTestSuite_allocFail_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArenaAllocatorTestSuite_* pSelf_ = GBL_ARENA_ALLOCATOR_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    void* pPtr = GblArenaAllocator_alloc(&pSelf_->arena, 256, 64);
    GBL_TEST_VERIFY(!pPtr);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OVERFLOW);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblArenaAllocatorTestSuite_saveLoadState_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArenaAllocatorTestSuite_* pSelf_ = GBL_ARENA_ALLOCATOR_TEST_SUITE_(pSelf);

    char* pTestString = GblArenaAllocator_alloc(&pSelf_->arena, 20, 1);
    strcpy(pTestString, "Restoration Point");

    const size_t  bytesUsed = GblArenaAllocator_bytesUsed(&pSelf_->arena);
    const size_t  pageCount = GblArenaAllocator_pageCount(&pSelf_->arena);

    GblArenaAllocatorState state;
    GblArenaAllocator_saveState(&pSelf_->arena, &state);

    for(size_t  i = 0; i < 5; ++i)
        GblArenaAllocator_alloc(&pSelf_->arena, 100);

    GBL_CTX_VERIFY_CALL(GblArenaAllocator_loadState(&pSelf_->arena, &state));

    GBL_TEST_COMPARE(pTestString, "Restoration Point");
    GBL_TEST_COMPARE(GblArenaAllocator_bytesUsed(&pSelf_->arena), bytesUsed);
    GBL_TEST_COMPARE(GblArenaAllocator_pageCount(&pSelf_->arena), pageCount);

    GBL_CTX_END();
}

static GBL_RESULT GblArenaAllocatorTestSuite_freeAll_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArenaAllocatorTestSuite_* pSelf_ = GBL_ARENA_ALLOCATOR_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArenaAllocator_freeAll(&pSelf_->arena));

    GBL_TEST_COMPARE(GblArenaAllocator_pageCount(&pSelf_->arena), 1);
    GBL_TEST_COMPARE(GblArenaAllocator_bytesUsed(&pSelf_->arena), 0);
    GBL_TEST_COMPARE(GblArenaAllocator_totalCapacity(&pSelf_->arena), 128);
    GBL_TEST_COMPARE(GblArenaAllocator_utilization(&pSelf_->arena), 0.0f);

    GBL_CTX_END();
}

static GBL_RESULT GblArenaAllocatorTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArenaAllocatorTestSuite_* pSelf_ = GBL_ARENA_ALLOCATOR_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArenaAllocator_destruct(&pSelf_->arena));

    GBL_CTX_END();
}

GBL_EXPORT GblType GblArenaAllocatorTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "construct",      GblArenaAllocatorTestSuite_construct_    },
        { "alloc",          GblArenaAllocatorTestSuite_alloc_        },
        { "allocNewPage",   GblArenaAllocatorTestSuite_allocNewPage_ },
        { "allocAligned",   GblArenaAllocatorTestSuite_allocAligned_ },
        { "allocFail",      GblArenaAllocatorTestSuite_allocFail_    },
        { "saveLoadState",  GblArenaAllocatorTestSuite_saveLoadState_},
        { "freeAll",        GblArenaAllocatorTestSuite_freeAll_      },
        { "destruct",       GblArenaAllocatorTestSuite_destruct_     },
        { NULL,             NULL                                     }
    };

    const static GblTestSuiteVTable vTable = {
        .pFnSuiteInit   = GblArenaAllocatorTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStatic("GblArenaAllocatorTestSuite"),
                                     &vTable,
                                     sizeof(GblArenaAllocatorTestSuite),
                                     sizeof(GblArenaAllocatorTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}
