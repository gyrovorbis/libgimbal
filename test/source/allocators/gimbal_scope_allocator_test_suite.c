#include "allocators/gimbal_scope_allocator_test_suite.h"
#include <gimbal/allocators/gimbal_scope_allocator.h>
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_doubly_linked_list.h>

#define GBL_SCOPE_ALLOCATOR_TEST_SUITE_(inst)   ((GblScopeAllocatorTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_SCOPE_ALLOCATOR_TEST_SUITE_TYPE))

typedef struct GblScopeAllocatorTestSuite_ {
    GblArenaAllocator   arena;
    GblScopeAllocator*  pScope;
    char*               pInitialString;
    char*               pString;
    char*               pAlignedString;
    GblVariant*         pDtorVariant;
    unsigned            dtorCount;
} GblScopeAllocatorTestSuite_;

static GBL_RESULT GblScopeAllocatorTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblScopeAllocatorTestSuite_));
    GblArenaAllocator_construct(&pSelf_->arena, 32, 32, NULL, pCtx);
    pSelf_->pInitialString = GblArenaAllocator_alloc(&pSelf_->arena, 14);
    strcpy(pSelf_->pInitialString, "InitialString");

    GBL_CTX_END();
}

static GBL_RESULT GblScopeAllocatorTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(pSelf_->pInitialString, "InitialString");
    GblArenaAllocator_destruct(&pSelf_->arena);
    GBL_CTX_END();
}

static GBL_RESULT GblScopeAllocatorTestSuite_create_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);

    pSelf_->pScope = GblScopeAllocator_create(&pSelf_->arena);
    GBL_TEST_VERIFY(pSelf_->pScope);
    GBL_TEST_COMPARE(GblScopeAllocator_dtorCount(pSelf_->pScope), 0);


    GBL_CTX_END();
}

static GBL_RESULT GblScopeAllocatorTestSuite_alloc_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);

    pSelf_->pString = GblScopeAllocator_alloc(pSelf_->pScope, 20);
    strcpy(pSelf_->pString, "TestString");

    GBL_CTX_END();
}

static GBL_RESULT GblScopeAllocatorTestSuite_allocAligned_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);

    pSelf_->pAlignedString = GblScopeAllocator_alloc(pSelf_->pScope, 32, 32);
    GBL_TEST_VERIFY(!((uintptr_t)pSelf_->pAlignedString & 0x1f));
    strcpy(pSelf_->pAlignedString, "AlignedString");

    GBL_TEST_COMPARE(pSelf_->pString, "TestString");

    GBL_CTX_END();
}


static GBL_RESULT GblScopeAllocatorTestSuite_allocDtor_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);

    pSelf_->pDtorVariant = GblScopeAllocator_alloc(pSelf_->pScope,
                                                   sizeof(GblVariant),
                                                   16,
                                                   (GblScopeAllocatorDtorFn)GblVariant_destruct);

    //GBL_TEST_VERIFY(!((uintptr_t)pSelf_->pDtorVariant & 0xf));
    GblVariant_construct(pSelf_->pDtorVariant, (const char*)"DtorVariant");

    GBL_TEST_COMPARE(GblScopeAllocator_dtorCount(pSelf_->pScope), 1);

    GBL_TEST_COMPARE(pSelf_->pString,        "TestString");
    GBL_TEST_COMPARE(pSelf_->pAlignedString, "AlignedString");
    GBL_TEST_COMPARE(GblVariant_toString(pSelf_->pDtorVariant), "DtorVariant");

    GBL_CTX_END();
}

static GBL_RESULT dtor_(void* pData) {
    GblScopeAllocatorTestSuite_* pSelf_ = pData;
    ++pSelf_->dtorCount;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblScopeAllocatorTestSuite_pushDtor_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);

    for(GblSize i = 0; i < 3; ++i)
        GBL_CTX_VERIFY_CALL(GblScopeAllocator_pushDtor(pSelf_->pScope,
                                                       dtor_,
                                                       pSelf_));

    GBL_TEST_COMPARE(GblScopeAllocator_dtorCount(pSelf_->pScope), 4);

    GBL_TEST_COMPARE(pSelf_->pString,        "TestString");
    GBL_TEST_COMPARE(pSelf_->pAlignedString, "AlignedString");
    GBL_TEST_COMPARE(GblVariant_toString(pSelf_->pDtorVariant), "DtorVariant");

    GBL_CTX_END();
}

static GBL_RESULT GblScopeAllocatorTestSuite_destroy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblScopeAllocatorTestSuite_* pSelf_ = GBL_SCOPE_ALLOCATOR_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblScopeAllocator_destroy(pSelf_->pScope));

    GBL_TEST_COMPARE(pSelf_->dtorCount, 3);

    GBL_CTX_END();
}


GBL_EXPORT GblType GblScopeAllocatorTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "create",         GblScopeAllocatorTestSuite_create_          },
        { "alloc",          GblScopeAllocatorTestSuite_alloc_           },
        { "allocAligned",   GblScopeAllocatorTestSuite_allocAligned_    },
        { "allocDtor",      GblScopeAllocatorTestSuite_allocDtor_       },
        { "pushDtor",       GblScopeAllocatorTestSuite_pushDtor_        },
        { "destroy",        GblScopeAllocatorTestSuite_destroy_         },
        { NULL,             NULL                                        }
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
