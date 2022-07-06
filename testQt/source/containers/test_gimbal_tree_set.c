#include "containers/test_gimbal_tree_set.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/containers/gimbal_tree_set.h>

static GblTreeSet   treeSet1_;
static uint64_t     hint_           = 0;
static unsigned     dtorCount_      = 0;
static uintptr_t    lastDtorValue_  = 0;

typedef struct TreeEntry_ {
    const char* pKey;
    uintptr_t   value;
} TreeEntry_;

GblInt GblTreeEntry_comparator_(const GblTreeSet* pSet, const void* pEntry1, const void* pEntry2) {
    GBL_UNUSED(pSet);
    return strcmp(((TreeEntry_*)pEntry1)->pKey, ((TreeEntry_*)pEntry2)->pKey);
}

void GblTreeEntry_destructor_(const GblTreeSet* pSet, void* pEntry) {
    GBL_UNUSED(pSet);
    lastDtorValue_ = ((TreeEntry_*)pEntry)->value;
    ++dtorCount_;
}

GBL_API GblTreeSet_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblTreeSet_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblTreeSet_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblTreeSet_construct(&treeSet1_,
                                             sizeof(TreeEntry_),
                                             GblTreeEntry_comparator_,
                                             GblTreeEntry_destructor_,
                                             2,
                                             pCtx,
                                             (void*)0xdeadbeef));
    GBL_TEST_COMPARE(GblTreeSet_size(&treeSet1_), 0);
    GBL_TEST_VERIFY(GblTreeSet_empty(&treeSet1_));
    GBL_TEST_COMPARE(GblTreeSet_context(&treeSet1_), pCtx);
    GBL_TEST_COMPARE(GblTreeSet_userdata(&treeSet1_), (void*)0xdeadbeef);
    GBL_API_END();
}

GBL_API GblTreeSet_test_get_empty_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblTreeSet_at(&treeSet1_, "lol"), NULL);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}

GBL_API GblTreeSet_test_get_empty(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblTreeSet_get(&treeSet1_, "lol"), NULL);
    GBL_TEST_VERIFY(!GblTreeSet_contains(&treeSet1_, "lol"));
    GBL_TEST_COMPARE(GblTreeSet_count(&treeSet1_, "lol"), 0);
    GBL_API_END();
}

GBL_API GblTreeSet_verifyEntry_(GblContext* pCtx,
                                GblTreeSet* pSet,
                                TreeEntry_* pEntry) {
    GBL_API_BEGIN(pCtx);
    TreeEntry_* pReturn = GblTreeSet_getHint(pSet, pEntry, &hint_);
    GBL_TEST_VERIFY(pReturn);
    GBL_TEST_COMPARE(pReturn->value, pEntry->value);
    pReturn = GblTreeSet_atHint(pSet, pEntry, &hint_);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pReturn);
    GBL_TEST_COMPARE(pReturn->value, pEntry->value);
    GBL_TEST_VERIFY(GblTreeSet_contains(pSet, pEntry));
    GBL_TEST_COMPARE(GblTreeSet_count(pSet, pEntry), 1);
    GBL_API_END();
}

GBL_API GblTreeSet_test_set(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    TreeEntry_ entry = {
        "entry0",
        0
    };
    uint64_t hint = 0;
    GBL_TEST_COMPARE(GblTreeSet_setHint(&treeSet1_, &entry, &hint), NULL);
    GBL_TEST_COMPARE(GblTreeSet_size(&treeSet1_), 1);
    GBL_API_VERIFY_CALL(GblTreeSet_verifyEntry_(pCtx,
                                                &treeSet1_,
                                                &entry));
    entry.value++;
    TreeEntry_* pEntry = GblTreeSet_setHint(&treeSet1_, &entry, &hint);
    GBL_TEST_COMPARE(GblTreeSet_size(&treeSet1_), 1);
    GBL_TEST_VERIFY(pEntry);
    GBL_TEST_COMPARE(pEntry->value, 0);

    GBL_API_END();
}


GBL_API GblTreeSet_test_set_hint(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    TreeEntry_ entry = {
        "entry1",
        1
    };
    GBL_TEST_COMPARE(GblTreeSet_setHint(&treeSet1_, &entry, &hint_), NULL);
    GBL_TEST_COMPARE(GblTreeSet_size(&treeSet1_), 2);
    GBL_API_VERIFY_CALL(GblTreeSet_verifyEntry_(pCtx,
                                                &treeSet1_,
                                                &entry));
    entry.pKey = "entry0";
    entry.value = 1;
    GBL_API_VERIFY_CALL(GblTreeSet_verifyEntry_(pCtx,
                                                &treeSet1_,
                                                &entry));
    entry.pKey = "entry2";
    entry.value = 2;
    GBL_TEST_COMPARE(GblTreeSet_setHint(&treeSet1_, &entry, &hint_), NULL);
    GBL_TEST_COMPARE(GblTreeSet_size(&treeSet1_), 3);
    GBL_API_VERIFY_CALL(GblTreeSet_verifyEntry_(pCtx,
                                                &treeSet1_,
                                                &entry));
    GBL_API_END();
}


GBL_API GblTreeSet_test_extract(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    TreeEntry_ entry = {
        "entry1",
        1
    };
    TreeEntry_* pEntry = GblTreeSet_extract(&treeSet1_, &entry);
    GBL_TEST_COMPARE(GblTreeSet_size(&treeSet1_), 2);
    GBL_TEST_COMPARE(dtorCount_, 0);
    GBL_TEST_COMPARE(lastDtorValue_, 0);
    GBL_TEST_VERIFY(pEntry);
    GBL_TEST_COMPARE(strcmp(entry.pKey, pEntry->pKey), 0);
    GBL_TEST_COMPARE(entry.value, pEntry->value);
    GBL_API_END();
}


GBL_API GblTreeSet_test_erase(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    TreeEntry_ entry = {
        "entry2",
        2
    };
    GBL_TEST_VERIFY(GblTreeSet_erase(&treeSet1_, &entry));
    GBL_TEST_COMPARE(GblTreeSet_size(&treeSet1_), 1);
    GBL_TEST_COMPARE(dtorCount_, 1);
    GBL_TEST_COMPARE(lastDtorValue_, 2);
    entry.pKey = "entry0";
    entry.value = 1;
    GBL_API_VERIFY_CALL(GblTreeSet_verifyEntry_(pCtx,
                                                &treeSet1_,
                                                &entry));
    GBL_API_END();
}

GBL_API GblTreeSet_test_clear(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblTreeSet_clear(&treeSet1_);
    GBL_TEST_VERIFY(GblTreeSet_empty(&treeSet1_));
    GBL_TEST_COMPARE(GblTreeSet_size(&treeSet1_), 0);
    GBL_TEST_COMPARE(dtorCount_, 2);
    GBL_TEST_COMPARE(lastDtorValue_, 1);
    GBL_API_END();
}


GBL_API GblTreeSet_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblTreeSet_destruct(&treeSet1_));
    GBL_API_END();
}
