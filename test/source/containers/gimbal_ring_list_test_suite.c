#include "containers/gimbal_ring_list_test_suite.h"
#include <gimbal/containers/gimbal_ring_list.h>
#include <gimbal/test/gimbal_test_macros.h>

#define GBL_RING_LIST_TEST_SUITE_(instance)     ((GblRingListTestSuite_*)GBL_INSTANCE_PRIVATE(instance, GBL_RING_LIST_TEST_SUITE_TYPE))

typedef struct GblRingListTestSuite_ {
    GblRingList* ringLists[5];
} GblRingListTestSuite_;

static GBL_RESULT GblRingListTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblRingListTestSuite_));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_verify_(GblContext* pCtx, const GblRingList* pList, ...) {
    va_list varArgs;
    va_start(varArgs, pList);
    GBL_CTX_BEGIN(pCtx);

    size_t  count = 0;
    const char* pStr = NULL;

    while((pStr = va_arg(varArgs, const char*))) {
        GBL_TEST_VERIFY(count < GblRingList_size(pList));

        if(!count) {
            const char* pFront = GblRingList_front(pList);
            GBL_TEST_VERIFY(pFront);
            GBL_TEST_COMPARE(pFront, pStr);
        }

        const char* pAt = GblRingList_at(pList, count);
        GBL_TEST_VERIFY(pAt);
        GBL_TEST_COMPARE(pAt, pStr);

        if(++count == GblRingList_size(pList)) {
            const char* pBack = GblRingList_back(pList);
            GBL_TEST_VERIFY(pBack);
            GBL_TEST_COMPARE(pBack, pStr);
        }
    }

    GBL_TEST_COMPARE(GblRingList_size(pList), count);
    GBL_TEST_COMPARE(GblRingList_empty(pList), count? GBL_FALSE : GBL_TRUE);

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblRingListTestSuite_createEmpty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    pSelf_->ringLists[0] = GblRingList_createEmpty();
    GBL_TEST_VERIFY(pSelf_->ringLists[0]);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[0], NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_createSingle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    pSelf_->ringLists[1] = GblRingList_create("Xbox");
    GBL_TEST_VERIFY(pSelf_->ringLists[1]);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1], "Xbox", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_createMulti_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    pSelf_->ringLists[2] = GblRingList_create("Dreamcast", "GameCube");
    GBL_TEST_VERIFY(pSelf_->ringLists[2]);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[2], "Dreamcast", "GameCube", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_copyShallow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    pSelf_->ringLists[3] = GblRingList_copy(pSelf_->ringLists[2]);
    GBL_TEST_VERIFY(pSelf_->ringLists[3]);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[2], "Dreamcast", "GameCube", NULL));
    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[3], "Dreamcast", "GameCube", NULL));

    GBL_CTX_END();
}

static void* deepCopyStr_(const void* pValue, void* pClosure) {
    const size_t  totalLen = strlen((const char*)pValue) + (pClosure? strlen((const char*)pClosure) : 0) + 1;
    char* pNewStr = NULL;
    GBL_CTX_BEGIN(NULL);
    pNewStr = GBL_CTX_MALLOC(totalLen);
    strcpy(pNewStr, (const char*)pValue);
    if(pClosure) strcat(pNewStr, (const char*)pClosure);
    GBL_CTX_END_BLOCK();
    return pNewStr;
}

static GBL_RESULT GblRingListTestSuite_copyDeep_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    pSelf_->ringLists[4] = GblRingList_copy(pSelf_->ringLists[3], deepCopyStr_, "er");
    GBL_TEST_VERIFY(pSelf_->ringLists[4]);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[4], "Dreamcaster", "GameCubeer", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_pushBackSingle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_pushBack(pSelf_->ringLists[1], "Playstation 2"));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1], "Xbox", "Playstation 2", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_pushBackMulti_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_pushBack(pSelf_->ringLists[1], "Nintendo 64", "Sega Saturn"));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "Xbox", "Playstation 2", "Nintendo 64", "Sega Saturn", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_pushFrontSingle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_pushFront(pSelf_->ringLists[1], "GameBoy Color"));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "GameBoy Color", "Xbox", "Playstation 2", "Nintendo 64", "Sega Saturn", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_pushFrontMulti_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_pushFront(pSelf_->ringLists[1], "Atari Jaguar", "Neo Geo CD"));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "Atari Jaguar", "Neo Geo CD", "GameBoy Color", "Xbox", "Playstation 2",
                        "Nintendo 64", "Sega Saturn", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_insertInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblRingList_insert(pSelf_->ringLists[1], 90, "PS2"),
                     GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}


static GBL_RESULT GblRingListTestSuite_insertSingleFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_insert(pSelf_->ringLists[3], 0, "PS2"));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[3],
                        "PS2", "Dreamcast", "GameCube", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_insertSingleMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_insert(pSelf_->ringLists[1], 2, "Dreamcast"));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "Atari Jaguar", "Neo Geo CD", "Dreamcast", "GameBoy Color", "Xbox", "Playstation 2",
                        "Nintendo 64", "Sega Saturn", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_insertMultiBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_insert(pSelf_->ringLists[1], 8, "GameCube", "Wonderswan"));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "Atari Jaguar", "Neo Geo CD", "Dreamcast", "GameBoy Color", "Xbox", "Playstation 2",
                        "Nintendo 64", "Sega Saturn", "GameCube", "Wonderswan", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_insertNegativeInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblRingList_insert(pSelf_->ringLists[1], -12, "PS2"),
                     GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_insertMultiFrontNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_insert(pSelf_->ringLists[1], -11, "NES", "Genesis"));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "NES", "Genesis", "Atari Jaguar", "Neo Geo CD", "Dreamcast", "GameBoy Color", "Xbox", "Playstation 2",
                        "Nintendo 64", "Sega Saturn", "GameCube", "Wonderswan", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_insertMultiMiddleNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_insert(pSelf_->ringLists[1], -2, "Super Nintendo", "Switch"));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "NES", "Genesis", "Atari Jaguar", "Neo Geo CD", "Dreamcast", "GameBoy Color", "Xbox", "Playstation 2",
                        "Nintendo 64", "Sega Saturn", "GameCube", "Super Nintendo", "Switch", "Wonderswan", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_insertMultiBackNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_insert(pSelf_->ringLists[1], -1, "Colecovision", "Atari 2600"));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "NES", "Genesis", "Atari Jaguar", "Neo Geo CD", "Dreamcast",
                        "GameBoy Color", "Xbox", "Playstation 2", "Nintendo 64",
                        "Sega Saturn", "GameCube", "Super Nintendo", "Switch",
                        "Wonderswan", "Colecovision", "Atari 2600", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_replaceMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE((const char*)GblRingList_replace(pSelf_->ringLists[1], 2, "Atari Lynx"),
                     "Atari Jaguar");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "NES", "Genesis", "Atari Lynx", "Neo Geo CD", "Dreamcast",
                        "GameBoy Color", "Xbox", "Playstation 2", "Nintendo 64",
                        "Sega Saturn", "GameCube", "Super Nintendo", "Switch",
                        "Wonderswan", "Colecovision", "Atari 2600", NULL));

    GBL_CTX_END();
}

static int sortComparator_(const void* pValue1, const void* pValue2, void* pClosure) {
    GBL_UNUSED(pClosure);
    return strcmp((const char*)pValue1, (const char*)pValue2);
}

static GBL_RESULT GblRingListTestSuite_insertSorted_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_insertSorted(pSelf_->ringLists[0],
                                                 "Bad",
                                                 sortComparator_));

    GBL_CTX_VERIFY_CALL(GblRingList_insertSorted(pSelf_->ringLists[0],
                                                 "Apple",
                                                 sortComparator_));

    GBL_CTX_VERIFY_CALL(GblRingList_insertSorted(pSelf_->ringLists[0],
                                                 "Zebra",
                                                 sortComparator_));

    GBL_CTX_VERIFY_CALL(GblRingList_insertSorted(pSelf_->ringLists[0],
                                                 "Dolphin",
                                                 sortComparator_));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[0],
                        "Apple", "Bad", "Dolphin", "Zebra", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_spliceMiddleNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_VERIFY(GblRingList_splice(pSelf_->ringLists[0], pSelf_->ringLists[3], -2));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[0],
                        "Apple", "Bad", "Dolphin", "PS2", "Dreamcast", "GameCube", "Zebra", NULL));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[3], NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_popBackSingle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE((const char*)GblRingList_popBack(pSelf_->ringLists[1]),
                     "Atari 2600");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "NES", "Genesis", "Atari Lynx", "Neo Geo CD", "Dreamcast",
                        "GameBoy Color", "Xbox", "Playstation 2", "Nintendo 64",
                        "Sega Saturn", "GameCube", "Super Nintendo", "Switch",
                        "Wonderswan", "Colecovision", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_popBackMulti_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE((const char*)GblRingList_popBack(pSelf_->ringLists[1], 3),
                     "Switch");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "NES", "Genesis", "Atari Lynx", "Neo Geo CD", "Dreamcast",
                        "GameBoy Color", "Xbox", "Playstation 2", "Nintendo 64",
                        "Sega Saturn", "GameCube", "Super Nintendo", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_popFrontSingle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE((const char*)GblRingList_popFront(pSelf_->ringLists[1]),
                     "NES");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "Genesis", "Atari Lynx", "Neo Geo CD", "Dreamcast",
                        "GameBoy Color", "Xbox", "Playstation 2", "Nintendo 64",
                        "Sega Saturn", "GameCube", "Super Nintendo", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_popFrontMulti_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE((const char*)GblRingList_popFront(pSelf_->ringLists[1], 4),
                     "Dreamcast");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "GameBoy Color", "Xbox", "Playstation 2", "Nintendo 64",
                        "Sega Saturn", "GameCube", "Super Nintendo", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_removeInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblRingList_remove(pSelf_->ringLists[1], 3, 5),
                     NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_removeFrontSingle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE((const char*)GblRingList_remove(pSelf_->ringLists[1], 0),
                     "GameBoy Color");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "Xbox", "Playstation 2", "Nintendo 64",
                        "Sega Saturn", "GameCube", "Super Nintendo", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_removeMiddleMulti_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE((const char*)GblRingList_remove(pSelf_->ringLists[1], 1, 2),
                     "Nintendo 64");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "Xbox", "Sega Saturn", "GameCube", "Super Nintendo", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_removeEndMulti_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE((const char*)GblRingList_remove(pSelf_->ringLists[1], 2, 2),
                     "Super Nintendo");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "Xbox", "Sega Saturn", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_removeNegativeInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblRingList_remove(pSelf_->ringLists[1], -3),
                     NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblRingList_remove(pSelf_->ringLists[1], -1, 3),
                     NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_removeBackMultiNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE((const char*)GblRingList_remove(pSelf_->ringLists[1], -1, 2),
                     "Xbox");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1], NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_removeMiddleMultiNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_pushBack(pSelf_->ringLists[1],
                        "Dreamcast", "Xbox", "GameCube", "Playstation 2"));

    GBL_TEST_COMPARE((const char*)GblRingList_remove(pSelf_->ringLists[1], -2, 2),
                     "Xbox");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1],
                        "Dreamcast", "Playstation 2", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_removeFrontMultiNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_pushBack(pSelf_->ringLists[1],
                        "Xbox", "GameCube"));

    GBL_TEST_COMPARE((const char*)GblRingList_remove(pSelf_->ringLists[1], -3, 2),
                     "Dreamcast");

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1], "Xbox", "GameCube", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_clear(pSelf_->ringLists[0]));
    GBL_CTX_VERIFY_CALL(GblRingList_clear(pSelf_->ringLists[1]));

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[0], NULL));
    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[1], NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_sort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_pushBack(pSelf_->ringLists[0], "Xbox", "Playstation 2", "Dreamcast", "Gamecube"));
    GblRingList_sort(pSelf_->ringLists[0], sortComparator_);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[0], "Dreamcast",
                                                                                 "Gamecube",
                                                                                 "Playstation 2",
                                                                                 "Xbox", NULL));
    GblRingList_clear(pSelf_->ringLists[0]);

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_rotate_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GblRingList_clear(pSelf_->ringLists[0]);

    GBL_CTX_VERIFY_CALL(GblRingList_pushBack(pSelf_->ringLists[0],
                                             "Dreamcast", "Playstation 2", "Xbox", "GameCube"));

    GblRingList_rotate(pSelf_->ringLists[0], 3);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[0],
                        "GameCube", "Dreamcast", "Playstation 2", "Xbox", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_rotateNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GblRingList_rotate(pSelf_->ringLists[0], -3);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[0],
                        "Dreamcast", "Playstation 2", "Xbox", "GameCube", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_reverse_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GblRingList_reverse(pSelf_->ringLists[0]);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[0],
                        "GameCube", "Xbox", "Playstation 2", "Dreamcast", NULL));
    GBL_CTX_END();
}

static GblBool ringListIterator_(void* pValue, void* pClosure) {
    int* pState = (int*)pClosure;
    GblBool retValue = GBL_FALSE;
    switch(*pState) {
    case 0:
        if(strcmp((const char*)pValue, "GameCube") == 0)
            ++(*pState);
        break;
    case 1:
        if(strcmp((const char*)pValue, "Xbox") == 0)
            ++(*pState);
        break;
    case 2:
        if(strcmp((const char*)pValue, "Playstation 2") == 0)
            ++(*pState);
        break;
    case 3:
        if(strcmp((const char*)pValue, "Dreamcast") == 0) {
            ++(*pState);
            retValue = GBL_TRUE;
        }
        break;
    default: break;
    }
    return retValue;
}

static GBL_RESULT GblRingListTestSuite_foreach_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    int state = 0;
    GBL_TEST_VERIFY(GblRingList_foreach(pSelf_->ringLists[0], ringListIterator_, &state));
    GBL_TEST_COMPARE(state, 4);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[0],
                        "GameCube", "Xbox", "Playstation 2", "Dreamcast", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_find_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblRingList_find(pSelf_->ringLists[0], "GameCube", sortComparator_), 0);
    GBL_TEST_COMPARE(GblRingList_find(pSelf_->ringLists[0], "Xbox", sortComparator_), 1);
    GBL_TEST_COMPARE(GblRingList_find(pSelf_->ringLists[0], "Playstation 2", sortComparator_), 2);
    GBL_TEST_COMPARE(GblRingList_find(pSelf_->ringLists[0], "Dreamcast", sortComparator_), 3);

    GBL_CTX_VERIFY_CALL(GblRingListTestSuite_verify_(pCtx, pSelf_->ringLists[0], "GameCube", "Xbox", "Playstation 2", "Dreamcast", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_extract_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    const size_t  size = GblRingList_size(pSelf_->ringLists[0]);

    const char* pString = GblRingList_extract(pSelf_->ringLists[0],
                                              pSelf_->ringLists[0]->ringNode.pNext);
    GBL_TEST_COMPARE(pString, "GameCube");
    GBL_TEST_COMPARE(size - 1, GblRingList_size(pSelf_->ringLists[0]));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_destroyShallow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    for(size_t  l = 0; l < 4; ++l)
        GBL_CTX_VERIFY_CALL(GblRingList_destroy(pSelf_->ringLists[l]));

    GBL_CTX_END();
}

static GBL_RESULT listDtor_(void* pValue, void* pClosure) {
    GBL_UNUSED(pClosure);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_FREE(pValue);
    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_destroyDeep_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingList_destroy(pSelf_->ringLists[4], listDtor_));

    GBL_CTX_END();
}

static GBL_RESULT GblRingListTestSuite_stressTest_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingListTestSuite_* pSelf_ = GBL_RING_LIST_TEST_SUITE_(pSelf);

    for(size_t  i = 0; i < 2000; ++i) {
        GblRingList* pList = GblRingList_create("a", "b", "c", "d", "e", "f", "g");
        GblRingList_remove(pList, 0, 3);
        GblRingList_destroy(pList);
    }

    GBL_CTX_END();
}

GBL_EXPORT GblType GblRingListTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "createEmpty",                GblRingListTestSuite_createEmpty_               },
        { "createSingle",               GblRingListTestSuite_createSingle_              },
        { "createMulti",                GblRingListTestSuite_createMulti_               },
        { "copyShallow",                GblRingListTestSuite_copyShallow_               },
        { "copyDeep",                   GblRingListTestSuite_copyDeep_                  },
        { "pushBackSingle",             GblRingListTestSuite_pushBackSingle_            },
        { "pushBackMulti",              GblRingListTestSuite_pushBackMulti_             },
        { "pushFrontSingle",            GblRingListTestSuite_pushFrontSingle_           },
        { "pushFrontMulti",             GblRingListTestSuite_pushFrontMulti_            },
        { "insertInvalid",              GblRingListTestSuite_insertInvalid_             },
        { "insertSingleFront",          GblRingListTestSuite_insertSingleFront_         },
        { "insertSingleMiddle",         GblRingListTestSuite_insertSingleMiddle_        },
        { "insertMultiBack",            GblRingListTestSuite_insertMultiBack_           },
        { "insertNegativeInvalid",      GblRingListTestSuite_insertNegativeInvalid_     },
        { "insertMultiFrontNegative",   GblRingListTestSuite_insertMultiFrontNegative_  },
        { "insertMultiMiddleNegative",  GblRingListTestSuite_insertMultiMiddleNegative_ },
        { "insertMultiBackNegative",    GblRingListTestSuite_insertMultiBackNegative_   },
        { "replaceMiddle",              GblRingListTestSuite_replaceMiddle_             },
        { "insertSorted",               GblRingListTestSuite_insertSorted_              },
        { "spliceMiddleNegative",       GblRingListTestSuite_spliceMiddleNegative_      },
        { "popBackSingle",              GblRingListTestSuite_popBackSingle_             },
        { "popBackMulti",               GblRingListTestSuite_popBackMulti_              },
        { "popFrontSingle",             GblRingListTestSuite_popFrontSingle_            },
        { "popFrontMulti",              GblRingListTestSuite_popFrontMulti_             },
        { "removeInvalid",              GblRingListTestSuite_removeInvalid_             },
        { "removeFrontSingle",          GblRingListTestSuite_removeFrontSingle_         },
        { "removeMiddleMulti",          GblRingListTestSuite_removeMiddleMulti_         },
        { "removeEndMulti",             GblRingListTestSuite_removeEndMulti_            },
        { "removeNegativeInvalid",      GblRingListTestSuite_removeNegativeInvalid_     },
        { "removeBackMultiNegative",    GblRingListTestSuite_removeBackMultiNegative_   },
        { "removeMiddleMultiNegative",  GblRingListTestSuite_removeMiddleMultiNegative_ },
        { "removeFrontMultiNegative",   GblRingListTestSuite_removeFrontMultiNegative_  },
        { "clear",                      GblRingListTestSuite_clear_                     },
        { "sort",                       GblRingListTestSuite_sort_                      },
        { "rotate",                     GblRingListTestSuite_rotate_                    },
        { "rotateNegative",             GblRingListTestSuite_rotateNegative_            },
        { "reverse",                    GblRingListTestSuite_reverse_                   },
        { "foreach",                    GblRingListTestSuite_foreach_                   },
        { "find",                       GblRingListTestSuite_find_                      },
        { "extract",                    GblRingListTestSuite_extract_                   },
        { "destroyShallow",             GblRingListTestSuite_destroyShallow_            },
        { "destroyDeep",                GblRingListTestSuite_destroyDeep_               },
        { "stressTest",                 GblRingListTestSuite_stressTest_                },
        { NULL,                         NULL                                            }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblRingListTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblRingListTestSuite"),
                                     &vTable,
                                     sizeof(GblRingListTestSuite),
                                     sizeof(GblRingListTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}
