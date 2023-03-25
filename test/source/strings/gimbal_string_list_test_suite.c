#include "strings/gimbal_string_list_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/strings/gimbal_string_list.h>

#define GBL_STRING_LIST_TEST_SUITE_(inst)    ((GblStringListTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_STRING_LIST_TEST_SUITE_TYPE))

typedef struct GblStringListTestSuite_ {
    GblRefCount     beginActiveRefCount;
    GblStringList*  lists[12];
} GblStringListTestSuite_;


static GBL_RESULT GblStringListTestSuite_verify_(GblTestSuite* pSelf, size_t  index, ...) {
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    va_list varArgs;
    va_start(varArgs, index);
    GBL_CTX_BEGIN(pSelf);

    size_t  size = 0;
    const char* pCurStr = NULL;
    GblStringList* pList = pSelf_->lists[index];

    while((pCurStr = va_arg(varArgs, const char*))) {
        GBL_TEST_COMPARE(GblStringList_at(pList, size), pCurStr);

        if(!size) GBL_TEST_COMPARE(pCurStr, GblStringList_front(pList));
        GBL_TEST_VERIFY(GblStringList_find(pList, pCurStr) != GBL_STRING_LIST_NPOS);
        GBL_TEST_VERIFY(GblStringList_contains(pList, pCurStr));

        if(++size == GblStringList_size(pList)) {
             GBL_TEST_COMPARE(pCurStr, GblStringList_back(pList));
        }
    }

    if(size) GBL_TEST_VERIFY(!GblStringList_empty(pList));
    else GBL_TEST_VERIFY(GblStringList_empty(pList));
    GBL_TEST_COMPARE(GblStringList_size(pList), size);

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblStringListTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);
    pSelf_->beginActiveRefCount = GblRef_activeCount();
    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->beginActiveRefCount);
    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_createEmpty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);
    pSelf_->lists[0] = GblStringList_createEmpty();
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 0, NULL));
    GBL_CTX_END();
}


static GBL_RESULT GblStringListTestSuite_create_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    pSelf_->lists[1] = GblStringList_create("one");
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 1, "one", NULL));

    pSelf_->lists[2] = GblStringList_create("one", "two", "three", "four");
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "one", "two", "three", "four", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_createWithRefs_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    const size_t  refs = GblRef_activeCount();

    GblStringRef* pRef1 = GblStringRef_create("one");
    GblStringRef* pRef2 = GblStringRef_create("two");

    pSelf_->lists[3] = GblStringList_createWithRefs(pRef1, pRef2);

    GblStringRef_release(pRef1);
    GblStringRef_release(pRef2);

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 3, "one", "two", NULL));

    GBL_TEST_COMPARE(refs + 2, GblRef_activeCount());

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_createSplit_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    pSelf_->lists[4] = GblStringList_createSplit("/home/usr/local/bin/lol.png//..", "/.");

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 4, "home", "usr", "local", "bin", "lol", "png", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_createFromArray_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    const char* pStringList[] = {
        "lolol",
        "Dreamcast",
        "is",
        "best",
        NULL
    };

    pSelf_->lists[5] = GblStringList_createFromArray(pStringList, GBL_COUNT_OF(pStringList));
    pSelf_->lists[6] = GblStringList_createFromArray(pStringList);

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 5, "lolol", "Dreamcast", "is", "best", NULL));
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 6, "lolol", "Dreamcast", "is", "best", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_createSubList_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    pSelf_->lists[7] = GblStringList_createSubList(pSelf_->lists[6], 1, 3);

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 7, "Dreamcast", "is", "best", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_createFilter_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    pSelf_->lists[8] = GblStringList_createFilter(pSelf_->lists[7], "^Dream");

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 8, "Dreamcast", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_copy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    pSelf_->lists[9] = GblStringList_copy(pSelf_->lists[7]);

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 9, "Dreamcast", "is", "best", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_count_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    pSelf_->lists[11] = GblStringList_create("1", "2", "2", "3", "3", "3", "a", "A");

    GBL_TEST_COMPARE(GblStringList_count(pSelf_->lists[11], "0"), 0);
    GBL_TEST_COMPARE(GblStringList_count(pSelf_->lists[11], "1"), 1);
    GBL_TEST_COMPARE(GblStringList_count(pSelf_->lists[11], "2"), 2);
    GBL_TEST_COMPARE(GblStringList_count(pSelf_->lists[11], "3"), 3);
    GBL_TEST_COMPARE(GblStringList_count(pSelf_->lists[11], "a"), 1);
    GBL_TEST_COMPARE(GblStringList_count(pSelf_->lists[11], "a", GBL_FALSE), 2);
    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_equals_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_VERIFY(GblStringList_equals(pSelf_->lists[5], pSelf_->lists[6]));
    GBL_TEST_VERIFY(GblStringList_equals(pSelf_->lists[7], pSelf_->lists[9]));

    GBL_TEST_VERIFY(!GblStringList_equals(pSelf_->lists[7], pSelf_->lists[8]));

    const char* pStringList[] = {
        "loLoL",
        "DrEAMcasT",
        "IS",
        "beSt",
        NULL
    };

    pSelf_->lists[10] = GblStringList_createFromArray(pStringList);

    GBL_TEST_VERIFY(!GblStringList_equals(pSelf_->lists[6], pSelf_->lists[10]));
    GBL_TEST_VERIFY(GblStringList_equals(pSelf_->lists[6], pSelf_->lists[10], GBL_FALSE));

    GBL_CTX_END();
}


static GBL_RESULT GblStringListTestSuite_frontInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringList_front(pSelf_->lists[0]), NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_backInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);


    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringList_back(pSelf_->lists[0]), NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_atInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringList_at(pSelf_->lists[0], 0), NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringList_at(pSelf_->lists[1], 999), NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_find_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblStringList_find(pSelf_->lists[0], ""), GBL_STRING_LIST_NPOS);
    GBL_TEST_COMPARE(GblStringList_find(pSelf_->lists[5], "DReamCAST"), GBL_STRING_LIST_NPOS);
    GBL_TEST_COMPARE(GblStringList_find(pSelf_->lists[5], "DReamCAST", GBL_FALSE), 1);

    GBL_CTX_END();
}


static GBL_RESULT GblStringListTestSuite_rfind_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblStringList_rfind(pSelf_->lists[11], ""), GBL_STRING_LIST_NPOS);
    GBL_TEST_COMPARE(GblStringList_rfind(pSelf_->lists[11], "3"), 5);
    GBL_TEST_COMPARE(GblStringList_rfind(pSelf_->lists[11], "a", GBL_FALSE), 7);

    GBL_CTX_END();
}


static GBL_RESULT GblStringListTestSuite_contains_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_VERIFY(!GblStringList_contains(pSelf_->lists[0], ""));
    GBL_TEST_VERIFY(!GblStringList_contains(pSelf_->lists[5], "DReamCAST"));
    GBL_TEST_VERIFY(GblStringList_contains(pSelf_->lists[5], "DReamCAST", GBL_FALSE));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_pushBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblStringList_pushBack(pSelf_->lists[1], "two"));
    GBL_CTX_VERIFY_CALL(GblStringList_pushBack(pSelf_->lists[1], "three", "four"));
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 1, "one", "two", "three", "four", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_pushBackRefs_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringRef* pRef1 = GblStringRef_create("five");
    GblStringRef* pRef2 = GblStringRef_create("six");
    GblStringRef* pRef3 = GblStringRef_create("seven");

    GBL_CTX_VERIFY_CALL(GblStringList_pushBackRefs(pSelf_->lists[1], pRef1));
    GBL_CTX_VERIFY_CALL(GblStringList_pushBackRefs(pSelf_->lists[1], pRef2, pRef3));
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 1, "one", "two", "three", "four",
                                                                 "five", "six", "seven", NULL));

    GblStringRef_release(pRef1);
    GblStringRef_release(pRef2);
    GblStringRef_release(pRef3);

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_pushFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblStringList_pushFront(pSelf_->lists[1], "zero"));
    GBL_CTX_VERIFY_CALL(GblStringList_pushFront(pSelf_->lists[1], "neg2", "neg1"));
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 1, "neg2", "neg1", "zero", "one",
                                                                 "two", "three", "four",
                                                                 "five", "six", "seven", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_pushFrontRefs_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringRef* pRef1 = GblStringRef_create("neg3");
    GblStringRef* pRef2 = GblStringRef_create("neg4");
    GblStringRef* pRef3 = GblStringRef_create("neg5");

    GBL_CTX_VERIFY_CALL(GblStringList_pushFrontRefs(pSelf_->lists[1], pRef1));
    GBL_CTX_VERIFY_CALL(GblStringList_pushFrontRefs(pSelf_->lists[1], pRef3, pRef2));

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 1, "neg5", "neg4", "neg3",
                                                                 "neg2", "neg1", "zero", "one",
                                                                 "two", "three", "four",
                                                                 "five", "six", "seven", NULL));

    GblStringRef_release(pRef1);
    GblStringRef_release(pRef2);
    GblStringRef_release(pRef3);

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_insertInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringList_insert(pSelf_->lists[0], 1, "test", "fail"),
                     GBL_RESULT_ERROR_OUT_OF_RANGE);

    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_insert_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblStringList_insert(pSelf_->lists[2], 0, "zero"));
    GBL_CTX_VERIFY_CALL(GblStringList_insert(pSelf_->lists[2], -1, "five", "six"));

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "zero", "one", "two", "three",
                                                                 "four", "five", "six", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_insertRefsInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GblStringRef* pRef = GblStringRef_create("Lamecube");

    const GBL_RESULT result = GblStringList_insertRefs(pSelf_->lists[0], 1, pRef);
    GBL_TEST_COMPARE(result,
                     GBL_RESULT_ERROR_OUT_OF_RANGE);

    GBL_CTX_CLEAR_LAST_RECORD();

    GblStringRef_release(pRef);

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_insertRefs_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringRef* pRef1 = GblStringRef_create("negOne");
    GblStringRef* pRef2 = GblStringRef_create("seven");
    GblStringRef* pRef3 = GblStringRef_create("eight");

    GBL_CTX_VERIFY_CALL(GblStringList_insertRefs(pSelf_->lists[2], 0, pRef1));
    GBL_CTX_VERIFY_CALL(GblStringList_insert(pSelf_->lists[2], -1, pRef2, pRef3));

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "negOne", "zero", "one", "two", "three",
                                                                 "four", "five", "six", "seven", "eight", NULL));

    GblStringRef_release(pRef1);
    GblStringRef_release(pRef2);
    GblStringRef_release(pRef3);

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_setInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringList_set(pSelf_->lists[0], 999, "lolol"),
                     GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_set_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblStringList_set(pSelf_->lists[2], 0, "negTwo"));
    GBL_CTX_VERIFY_CALL(GblStringList_set(pSelf_->lists[2], -1, "nine"));

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "negTwo", "zero", "one", "two", "three",
                                                                 "four", "five", "six", "seven", "nine", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_setRefInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GblStringRef* pRef = GblStringRef_create("trolo");
    GBL_TEST_COMPARE(GblStringList_setRef(pSelf_->lists[2], -333, pRef),
                     GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GblStringRef_release(pRef);

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_setRef_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringRef* pRef = GblStringRef_create("trolo");
    GBL_CTX_VERIFY_CALL(GblStringList_setRef(pSelf_->lists[2], -3, pRef));
    GblStringRef_release(pRef);

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "negTwo", "zero", "one", "two", "three",
                                                                 "four", "five", "trolo", "seven", "nine", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_replace_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    // noexistent
    GBL_TEST_VERIFY(!GblStringList_replace(pSelf_->lists[2], "nope", "lolol"));

    // exists single value
    GBL_TEST_COMPARE(GblStringList_replace(pSelf_->lists[2], "seven", "omicron"), 1);

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "negTwo", "zero", "one", "two", "three",
                                                                 "four", "five", "trolo", "omicron", "nine", NULL));

    // exists multi default values
    GBL_CTX_VERIFY_CALL(GblStringList_pushBack(pSelf_->lists[2], "nine"));
    GBL_TEST_COMPARE(GblStringList_replace(pSelf_->lists[2], "nine", "omiCron"), 2);


    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "negTwo", "zero", "one", "two", "three",
                                                                 "four", "five", "trolo", "omicron", "omiCron", "omiCron", NULL));

    // exists case sensitive
    GBL_TEST_COMPARE(GblStringList_replace(pSelf_->lists[2], "omicron", "Soul", GBL_FALSE), 3);


    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "negTwo", "zero", "one", "two", "three",
                                                                 "four", "five", "trolo", "Soul", "Soul", "Soul", NULL));

    // exists with limit
    GBL_TEST_COMPARE(GblStringList_replace(pSelf_->lists[2], "Soul", "Nomad", GBL_TRUE, 2), 2);

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "negTwo", "zero", "one", "two", "three",
                                                                 "four", "five", "trolo", "Nomad", "Nomad", "Soul", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_replaceWithRef_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringRef* pRef1 = GblStringRef_create("1");
    GblStringRef* pRef2 = GblStringRef_create("2");
    GblStringRef* pRef3 = GblStringRef_create("3");

    GBL_TEST_COMPARE(GblStringList_replaceWithRef(pSelf_->lists[2], "nope", pRef1), 0);

    GBL_TEST_COMPARE(GblStringList_replaceWithRef(pSelf_->lists[2], "Nomad", pRef1), 2);

    GBL_TEST_COMPARE(GblStringList_replaceWithRef(pSelf_->lists[2], "TROLO", pRef2, GBL_FALSE), 1);

    GBL_TEST_COMPARE(GblStringList_replace(pSelf_->lists[2], "five", "SoUl"), 1);
    GBL_TEST_COMPARE(GblStringList_replaceWithRef(pSelf_->lists[2], "SOuL", pRef3, GBL_FALSE, 1), 1);

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "negTwo", "zero", "one", "two", "three",
                                                                 "four", "3", "2", "1", "1", "Soul", NULL));
    GblStringRef_release(pRef1);
    GblStringRef_release(pRef2);
    GblStringRef_release(pRef3);

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_spliceInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    //GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblStringList_splice(pSelf_->lists[0], pSelf_->lists[1], 22), GBL_FALSE);
    //GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    //GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_splice_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_VERIFY(GblStringList_splice(pSelf_->lists[3], pSelf_->lists[4], -2));
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 3, "one", "home", "usr", "local",
                                                                 "bin", "lol", "png", "two", NULL));

    GBL_CTX_VERIFY_CALL(GblStringList_pushBack(pSelf_->lists[4], "endzer"));
    GBL_TEST_VERIFY(GblStringList_splice(pSelf_->lists[3], pSelf_->lists[4]));
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 3, "one", "home", "usr", "local",
                                                                 "bin", "lol", "png", "two", "endzer", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_join_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringRef* pRef = GblStringList_join(pSelf_->lists[3], "-");
    GBL_TEST_COMPARE(pRef, "one-home-usr-local-bin-lol-png-two-endzer");
    GblStringRef_release(pRef);

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_popBackInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringList_popBack(pSelf_->lists[0]), NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_popBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringRef* pRef = GblStringList_popBack(pSelf_->lists[3]);

    GBL_TEST_COMPARE(pRef, "endzer");

    GblStringRef_release(pRef);

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_popFrontInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringList_popFront(pSelf_->lists[0]), NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_popFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringRef* pRef = GblStringList_popFront(pSelf_->lists[3]);

    GBL_TEST_COMPARE(pRef, "one");

    GblStringRef_release(pRef);

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_eraseInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringList_erase(pSelf_->lists[0], 0, 1), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringList_erase(pSelf_->lists[1], -1, 99), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_erase_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblStringList_erase(pSelf_->lists[2], 0));
    GBL_CTX_VERIFY_CALL(GblStringList_erase(pSelf_->lists[2], 2, 2));
    GBL_CTX_VERIFY_CALL(GblStringList_erase(pSelf_->lists[2], -1, 2));

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "zero", "one",
                                                                 "four", "3", "2", "1", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_remove_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringList_pushBack(pSelf_->lists[2], "1", "2", "2", "zErO");

    GBL_TEST_COMPARE(GblStringList_remove(pSelf_->lists[2], "lolnope"), 0);
    GBL_TEST_COMPARE(GblStringList_remove(pSelf_->lists[2], "2"), 3);
    GBL_TEST_COMPARE(GblStringList_remove(pSelf_->lists[2], "zero", GBL_FALSE), 2);


    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "one", "four", "3",
                                                                 "1", "1", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_deduplicate_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringList_pushBack(pSelf_->lists[2], "4", "4", "4", "twelve", "twelVe");

    GBL_CTX_VERIFY_CALL(GblStringList_deduplicate(pSelf_->lists[2]));

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "one", "four", "3",
                                                                 "1", "4", "twelve", "twelVe", NULL));

    GBL_CTX_VERIFY_CALL(GblStringList_deduplicate(pSelf_->lists[2], GBL_FALSE));


    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 2, "one", "four", "3",
                                                                 "1", "4", "twelve", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_sort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringList_pushBack(pSelf_->lists[4], "A", "D", "C", "E", "B", "F");

    GblStringList_sort(pSelf_->lists[4]);
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 4, "A", "B", "C", "D", "E", "F", NULL));

    GblStringList_sort(pSelf_->lists[4], GBL_FALSE);
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 4, "F", "E", "D", "C", "B", "A", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_rotate_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringList_rotate(pSelf_->lists[4], -2);
    GblStringList_rotate(pSelf_->lists[4], 2);

    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 4, "F", "E", "D", "C", "B", "A", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_reverse_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringList_reverse(pSelf_->lists[4]);
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 4, "A", "B", "C", "D", "E", "F", NULL));

    GBL_CTX_END();
}

static GblBool GblStringListTestSuite_iterator_(GblStringRef* pRef, void* pClosure) {
    int* pCount = pClosure;

    if(!strcmp(pRef, "A") && *pCount == 0)
        (*pCount)++;
    else if(!strcmp(pRef, "B") && *pCount == 1)
        (*pCount)++;
    else if(!strcmp(pRef, "C") && *pCount == 2)
        (*pCount)++;
    else if(!strcmp(pRef, "D") && *pCount == 3)
        (*pCount)++;
    else if(!strcmp(pRef, "E") && *pCount == 4)
        (*pCount)++;
    else if(!strcmp(pRef, "F") && *pCount == 5)
        (*pCount)++;
    else return GBL_TRUE;

    return GBL_FALSE;
}

static GBL_RESULT GblStringListTestSuite_foreach_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    int count = 0;
    GBL_TEST_VERIFY(!GblStringList_foreach(pSelf_->lists[4], GblStringListTestSuite_iterator_, &count));
    GBL_TEST_COMPARE(count, 6);

    GBL_CTX_END();
}


static GBL_RESULT GblStringListTestSuite_extract_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GblStringRef* pRef = GblStringList_extract(pSelf_->lists[4],
                                               pSelf_->lists[4]->ringNode.pNext);

    GBL_TEST_COMPARE(GblStringList_size(pSelf_->lists[4]), 5);
    GBL_TEST_COMPARE(pRef, "A");
    GblStringRef_release(pRef);


    GBL_CTX_END();
}


static GBL_RESULT GblStringListTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblStringList_clear(pSelf_->lists[0]));
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 0, NULL));

    GBL_CTX_VERIFY_CALL(GblStringList_clear(pSelf_->lists[4]));
    GBL_CTX_VERIFY_CALL(GblStringListTestSuite_verify_(pSelf, 4, NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblStringListTestSuite_destroy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringListTestSuite_* pSelf_ = GBL_STRING_LIST_TEST_SUITE_(pSelf);

    for(size_t  l = 0; l < GBL_COUNT_OF(pSelf_->lists); ++l) {
        GBL_CTX_VERIFY_CALL(GblStringList_destroy(pSelf_->lists[l]));
    }

    GBL_CTX_END();
}

GBL_EXPORT GblType GblStringListTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "createEmpty",           GblStringListTestSuite_createEmpty_       },
        { "create",                GblStringListTestSuite_create_            },
        { "createWithRefs",        GblStringListTestSuite_createWithRefs_    },
        { "createSplit",           GblStringListTestSuite_createSplit_       },
        { "createFromArray",       GblStringListTestSuite_createFromArray_   },
        { "createSubList",         GblStringListTestSuite_createSubList_     },
        { "createFilter",          GblStringListTestSuite_createFilter_      },
        { "copy",                  GblStringListTestSuite_copy_              },
        { "count",                 GblStringListTestSuite_count_             },
        { "equals",                GblStringListTestSuite_equals_            },
        { "frontInvalid",          GblStringListTestSuite_frontInvalid_      },
        { "backInvalid",           GblStringListTestSuite_backInvalid_       },
        { "atInvalid",             GblStringListTestSuite_atInvalid_         },
        { "find",                  GblStringListTestSuite_find_              },
        { "rfind",                 GblStringListTestSuite_rfind_             },
        { "contains",              GblStringListTestSuite_contains_          },
        { "pushBack",              GblStringListTestSuite_pushBack_          },
        { "pushBackRefs",          GblStringListTestSuite_pushBackRefs_      },
        { "pushFront",             GblStringListTestSuite_pushFront_         },
        { "pushFrontRefs",         GblStringListTestSuite_pushFrontRefs_     },
        { "insertInvalid",         GblStringListTestSuite_insertInvalid_     },
        { "insert",                GblStringListTestSuite_insert_            },
        { "insertRefsInvalid",     GblStringListTestSuite_insertRefsInvalid_ },
        { "insertRefs",            GblStringListTestSuite_insertRefs_        },
        { "setInvalid",            GblStringListTestSuite_setInvalid_        },
        { "set",                   GblStringListTestSuite_set_               },
        { "setRefInvalid",         GblStringListTestSuite_setRefInvalid_     },
        { "setRef",                GblStringListTestSuite_setRef_            },
        { "replace",               GblStringListTestSuite_replace_           },
        { "replaceWithRef",        GblStringListTestSuite_replaceWithRef_    },
        { "spliceInvalid",         GblStringListTestSuite_spliceInvalid_     },
        { "splice",                GblStringListTestSuite_splice_            },
        { "join",                  GblStringListTestSuite_join_              },
        { "popBackInvalid",        GblStringListTestSuite_popBackInvalid_    },
        { "popBack",               GblStringListTestSuite_popBack_           },
        { "popFrontInvalid",       GblStringListTestSuite_popFrontInvalid_   },
        { "popFront",              GblStringListTestSuite_popFront_          },
        { "eraseInvalid",          GblStringListTestSuite_eraseInvalid_      },
        { "erase",                 GblStringListTestSuite_erase_             },
        { "remove",                GblStringListTestSuite_remove_            },
        { "deduplicate",           GblStringListTestSuite_deduplicate_       },
        { "sort",                  GblStringListTestSuite_sort_              },
        { "rotate",                GblStringListTestSuite_rotate_            },
        { "reverse",               GblStringListTestSuite_reverse_           },
        { "foreach",               GblStringListTestSuite_foreach_           },
        { "extract",               GblStringListTestSuite_extract_           },
        { "clear",                 GblStringListTestSuite_clear_             },
        { "destroy",               GblStringListTestSuite_destroy_           },
        { NULL,                    NULL                                      }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblStringListTestSuite_init_,
        .pFnSuiteFinal  = GblStringListTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("StringListTestSuite"),
                                     &vTable,
                                     sizeof(GblStringListTestSuite),
                                     sizeof(GblStringListTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

