#include "containers/gimbal_tree_set_test_suite.h"
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_tree_set.h>

// Do stress test
#define GBL_TREE_SET_TEST_SUITE_(inst)  ((GblTreeSetTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_TREE_SET_TEST_SUITE_TYPE))

typedef struct GblTreeSetTestSuite_ {
    GblTreeSet  treeSet1;
    uint64_t    hint;
    unsigned    dtorCount;
    uintptr_t   lastDtorValue;
} GblTreeSetTestSuite_;

typedef struct TreeEntry_ {
    const char* pKey;
    uintptr_t   value;
} TreeEntry_;

int GblTreeEntry_comparator_(const GblTreeSet* pSet, const void* pEntry1, const void* pEntry2) {
    GBL_UNUSED(pSet);
    return strcmp(((TreeEntry_*)pEntry1)->pKey, ((TreeEntry_*)pEntry2)->pKey);
}

void GblTreeEntry_destructor_(const GblTreeSet* pSet, void* pEntry) {
    GBL_UNUSED(pSet);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(GblTreeSet_userdata(pSet));
    pSelf_->lastDtorValue = ((TreeEntry_*)pEntry)->value;
    ++pSelf_->dtorCount;
}

static GBL_RESULT GblTreeSetTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblTreeSetTestSuite_));
    GBL_CTX_END();
}

static GBL_RESULT GblTreeSetTestSuite_construct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblTreeSet_construct(&pSelf_->treeSet1,
                                             sizeof(TreeEntry_),
                                             GblTreeEntry_comparator_,
                                             GblTreeEntry_destructor_,
                                             2,
                                             pCtx,
                                             pSelf));
    GBL_TEST_COMPARE(GblTreeSet_size(&pSelf_->treeSet1), 0);
    GBL_TEST_VERIFY(GblTreeSet_empty(&pSelf_->treeSet1));
    GBL_TEST_COMPARE(GblTreeSet_context(&pSelf_->treeSet1), pCtx);
    GBL_TEST_COMPARE(GblTreeSet_userdata(&pSelf_->treeSet1), pSelf);
    GBL_CTX_END();
}

static GBL_RESULT GblTreeSetTestSuite_get_empty_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblTreeSet_at(&pSelf_->treeSet1, "lol"), NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblTreeSetTestSuite_get_empty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblTreeSet_get(&pSelf_->treeSet1, "lol"), NULL);
    GBL_TEST_VERIFY(!GblTreeSet_contains(&pSelf_->treeSet1, "lol"));
    GBL_TEST_COMPARE(GblTreeSet_count(&pSelf_->treeSet1, "lol"), 0);
    GBL_CTX_END();
}

static GBL_RESULT GblTreeSet_verifyEntry_(GblTestSuite* pSelf,
                                          GblContext* pCtx,
                                          GblTreeSet* pSet,
                                          TreeEntry_* pEntry) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    TreeEntry_* pReturn = GblTreeSet_getHint(pSet, pEntry, &pSelf_->hint);
    GBL_TEST_VERIFY(pReturn);
    GBL_TEST_COMPARE(pReturn->value, pEntry->value);
    pReturn = GblTreeSet_atHint(pSet, pEntry, &pSelf_->hint);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pReturn);
    GBL_TEST_COMPARE(pReturn->value, pEntry->value);
    GBL_TEST_VERIFY(GblTreeSet_contains(pSet, pEntry));
    GBL_TEST_COMPARE(GblTreeSet_count(pSet, pEntry), 1);
    GBL_CTX_END();
}

static GBL_RESULT GblTreeSetTestSuite_set_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    TreeEntry_ entry = {
        "entry0",
        0
    };
    uint64_t hint = 0;
    GBL_TEST_COMPARE(GblTreeSet_setHint(&pSelf_->treeSet1, &entry, &hint), NULL);
    GBL_TEST_COMPARE(GblTreeSet_size(&pSelf_->treeSet1), 1);
    GBL_CTX_VERIFY_CALL(GblTreeSet_verifyEntry_(pSelf,
                                                pCtx,
                                                &pSelf_->treeSet1,
                                                &entry));
    entry.value++;
    TreeEntry_* pEntry = GblTreeSet_setHint(&pSelf_->treeSet1, &entry, &hint);
    GBL_TEST_COMPARE(GblTreeSet_size(&pSelf_->treeSet1), 1);
    GBL_TEST_VERIFY(pEntry);
    GBL_TEST_COMPARE(pEntry->value, 0);

    GBL_CTX_END();
}


static GBL_RESULT GblTreeSetTestSuite_set_hint_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    TreeEntry_ entry = {
        "entry1",
        1
    };
    GBL_TEST_COMPARE(GblTreeSet_setHint(&pSelf_->treeSet1, &entry, &pSelf_->hint), NULL);
    GBL_TEST_COMPARE(GblTreeSet_size(&pSelf_->treeSet1), 2);
    GBL_CTX_VERIFY_CALL(GblTreeSet_verifyEntry_(pSelf,
                                                pCtx,
                                                &pSelf_->treeSet1,
                                                &entry));
    entry.pKey = "entry0";
    entry.value = 1;
    GBL_CTX_VERIFY_CALL(GblTreeSet_verifyEntry_(pSelf,
                                                pCtx,
                                                &pSelf_->treeSet1,
                                                &entry));
    entry.pKey = "entry2";
    entry.value = 2;
    GBL_TEST_COMPARE(GblTreeSet_setHint(&pSelf_->treeSet1, &entry, &pSelf_->hint), NULL);
    GBL_TEST_COMPARE(GblTreeSet_size(&pSelf_->treeSet1), 3);
    GBL_CTX_VERIFY_CALL(GblTreeSet_verifyEntry_(pSelf,
                                                pCtx,
                                                &pSelf_->treeSet1,
                                                &entry));
    GBL_CTX_END();
}


static GBL_RESULT GblTreeSetTestSuite_extract_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    TreeEntry_ entry = {
        "entry1",
        1
    };
    TreeEntry_* pEntry = GblTreeSet_extract(&pSelf_->treeSet1, &entry);
    GBL_TEST_COMPARE(GblTreeSet_size(&pSelf_->treeSet1), 2);
    GBL_TEST_COMPARE(pSelf_->dtorCount, 0);
    GBL_TEST_COMPARE(pSelf_->lastDtorValue, 0);
    GBL_TEST_VERIFY(pEntry);
    GBL_TEST_COMPARE(strcmp(entry.pKey, pEntry->pKey), 0);
    GBL_TEST_COMPARE(entry.value, pEntry->value);
    GBL_CTX_END();
}


static GBL_RESULT GblTreeSetTestSuite_erase_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    TreeEntry_ entry = {
        "entry2",
        2
    };
    GBL_TEST_VERIFY(GblTreeSet_erase(&pSelf_->treeSet1, &entry));
    GBL_TEST_COMPARE(GblTreeSet_size(&pSelf_->treeSet1), 1);
    GBL_TEST_COMPARE(pSelf_->dtorCount, 1);
    GBL_TEST_COMPARE(pSelf_->lastDtorValue, 2);
    entry.pKey = "entry0";
    entry.value = 1;
    GBL_CTX_VERIFY_CALL(GblTreeSet_verifyEntry_(pSelf,
                                                pCtx,
                                                &pSelf_->treeSet1,
                                                &entry));
    GBL_CTX_END();
}

static GBL_RESULT GblTreeSetTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    GblTreeSet_clear(&pSelf_->treeSet1);
    GBL_TEST_VERIFY(GblTreeSet_empty(&pSelf_->treeSet1));
    GBL_TEST_COMPARE(GblTreeSet_size(&pSelf_->treeSet1), 0);
    GBL_TEST_COMPARE(pSelf_->dtorCount, 2);
    GBL_TEST_COMPARE(pSelf_->lastDtorValue, 1);
    GBL_CTX_END();
}


static GBL_RESULT GblTreeSetTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTreeSetTestSuite_* pSelf_ = GBL_TREE_SET_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblTreeSet_destruct(&pSelf_->treeSet1));
    GBL_CTX_END();
}

GBL_EXPORT GblType GblTreeSetTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "construct",          GblTreeSetTestSuite_construct_          },
        { "getEmptyInvalid",    GblTreeSetTestSuite_get_empty_invalid_  },
        { "getEmpty",           GblTreeSetTestSuite_get_empty_          },
        { "set",                GblTreeSetTestSuite_set_                },
        { "setHint",            GblTreeSetTestSuite_set_hint_           },
        { "extract",            GblTreeSetTestSuite_extract_            },
        { "erase",              GblTreeSetTestSuite_erase_              },
        { "clear",              GblTreeSetTestSuite_clear_              },
        { "destruct",           GblTreeSetTestSuite_destruct_           },
        { NULL,                 NULL                                    }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblTreeSetTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("TreeSetTestSuite"),
                                     &vTable,
                                     sizeof(GblTreeSetTestSuite),
                                     sizeof(GblTreeSetTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

