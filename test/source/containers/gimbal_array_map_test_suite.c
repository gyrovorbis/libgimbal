#include "containers/gimbal_array_map_test_suite.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_array_map.h>
#include <gimbal/meta/gimbal_primitives.h>

#define GBL_ARRAY_MAP_TEST_SUITE_BINARY_SEARCHABLE_ENTRIES_ 1000
#define GBL_ARRAY_MAP_TEST_SUITE_(inst)     ((GblArrayMapTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_ARRAY_MAP_TEST_SUITE_TYPE))

typedef struct GblArrayMapTestSuite_ {
    GblArrayMap*    pMap1;
    GblArrayMap*    pMap2;
} GblArrayMapTestSuite_;

// god fucking damnit, static/global implicit state!
static unsigned dtorCalls_  = 0;
static uintptr_t dtorLastKey_ = 0;

int GblArrayMap_comparator_(const GblArrayMap* pMap, uintptr_t key1,
                            uintptr_t key2)
{
    GBL_UNUSED(pMap);
    return strcmp((const char*)key1, (const char*)key2);
}

GBL_RESULT GblArrayMap_destructor_(const GblArrayMap* pMap, uintptr_t key, void* pData) {
    GBL_UNUSED(pData, pMap);
    ++dtorCalls_;
    dtorLastKey_ = key;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblArrayMapTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblArrayMapTestSuite_));
    dtorCalls_ = 0;
    dtorLastKey_ = 0;
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_null_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblArrayMap_atValue(&pSelf_->pMap1, 0), 0);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblArrayMap_atVariant(&pSelf_->pMap1, 0), NULL);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_null_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    uintptr_t value;
    GblVariant variant;
    //GblArrayMap* pOther = NULL;
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 0);
    GBL_TEST_VERIFY(GblArrayMap_empty(&pSelf_->pMap1));
    GBL_TEST_COMPARE(GblArrayMap_context(&pSelf_->pMap1), NULL);
    GBL_TEST_VERIFY(!GblArrayMap_contains(&pSelf_->pMap1, 0));
    GBL_TEST_VERIFY(!GblArrayMap_containsUserdata(&pSelf_->pMap1, 0));
    GBL_TEST_VERIFY(!GblArrayMap_containsVariant(&pSelf_->pMap1, 0));
    GBL_TEST_COMPARE(GblArrayMap_getValue(&pSelf_->pMap1, 0), 0);
    GBL_TEST_COMPARE(GblArrayMap_getVariant(&pSelf_->pMap1, 0), NULL);
    GBL_TEST_VERIFY(!GblArrayMap_erase(&pSelf_->pMap1, 0));
    GBL_TEST_VERIFY(!GblArrayMap_extractValue(&pSelf_->pMap1, 0, &value));
    GBL_TEST_VERIFY(!GblArrayMap_extractVariant(&pSelf_->pMap1, 0, &variant));
    GBL_TEST_COMPARE(GblArrayMap_find(&pSelf_->pMap1, 0), GBL_ARRAY_MAP_NPOS);
    GBL_TEST_COMPARE(GblArrayMap_probeKey(&pSelf_->pMap1, 0), 0);
    GBL_TEST_COMPARE(GblArrayMap_probeValue(&pSelf_->pMap1, 0), 0);
    GBL_TEST_COMPARE(GblArrayMap_probeVariant(&pSelf_->pMap1, 0), NULL);
    //pOther = GblArrayMap_clone(&pSelf_->pMap1);
    //GBL_TEST_COMPARE(pOther, NULL);
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&pSelf_->pMap1), GBL_RESULT_SUCCESS);
    GblArrayMap_clear(&pSelf_->pMap1);
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_empty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    uintptr_t value;
    GblVariant variant;
    //GblArrayMap* pOther = NULL;
    pSelf_->pMap1 = GblArrayMap_create(GblArrayMap_comparator_, GBL_FALSE, pCtx);
    GBL_TEST_VERIFY(pSelf_->pMap1);
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 0);
    GBL_TEST_VERIFY(GblArrayMap_empty(&pSelf_->pMap1));
    GBL_TEST_COMPARE(GblArrayMap_context(&pSelf_->pMap1), pCtx);
    GBL_TEST_VERIFY(!GblArrayMap_contains(&pSelf_->pMap1, 0));
    GBL_TEST_VERIFY(!GblArrayMap_containsUserdata(&pSelf_->pMap1, 0));
    GBL_TEST_VERIFY(!GblArrayMap_containsVariant(&pSelf_->pMap1, 0));
    GBL_TEST_COMPARE(GblArrayMap_getValue(&pSelf_->pMap1, 0), 0);
    GBL_TEST_COMPARE(GblArrayMap_getVariant(&pSelf_->pMap1, 0), NULL);
    GBL_TEST_VERIFY(!GblArrayMap_erase(&pSelf_->pMap1, 0));
    GBL_TEST_VERIFY(!GblArrayMap_extractValue(&pSelf_->pMap1, 0, &value));
    GBL_TEST_VERIFY(!GblArrayMap_extractVariant(&pSelf_->pMap1, 0, &variant));
    GBL_TEST_COMPARE(GblArrayMap_find(&pSelf_->pMap1, 0), GBL_ARRAY_MAP_NPOS);
    GBL_TEST_COMPARE(GblArrayMap_probeKey(&pSelf_->pMap1, 0), 0);
    GBL_TEST_COMPARE(GblArrayMap_probeValue(&pSelf_->pMap1, 0), 0);
    GBL_TEST_COMPARE(GblArrayMap_probeVariant(&pSelf_->pMap1, 0), NULL);
    //pOther = GblArrayMap_clone(&pSelf_->pMap1);
    //GBL_TEST_VERIFY(pOther);
    //GBL_TEST_COMPARE(GblArrayMap_context(&pOther), pCtx);
    //GBL_TEST_VERIFY(GblArrayMap_empty(&pOther));
    //GblArrayMap_destroy(&pOther);
    //GBL_TEST_VERIFY(!pOther);
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_setUserdata_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayMap_setUserdata(&pSelf_->pMap1,
                                                (uintptr_t)"key1",
                                                (uintptr_t)1,
                                                GblArrayMap_destructor_));
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 1);
    GBL_TEST_VERIFY(!GblArrayMap_empty(&pSelf_->pMap1));
    GBL_TEST_VERIFY(GblArrayMap_contains(&pSelf_->pMap1, (uintptr_t)"key1"));
    GBL_TEST_VERIFY(GblArrayMap_containsUserdata(&pSelf_->pMap1, (uintptr_t)"key1"));
    GBL_TEST_VERIFY(!GblArrayMap_containsVariant(&pSelf_->pMap1, (uintptr_t)"key1"));
    GBL_TEST_COMPARE(GblArrayMap_getValue(&pSelf_->pMap1, (uintptr_t)"key1"), 1);
    GBL_TEST_COMPARE(GblArrayMap_getVariant(&pSelf_->pMap1, (uintptr_t)"key1"), NULL);
    GBL_TEST_COMPARE(GblArrayMap_atValue(&pSelf_->pMap1, (uintptr_t)"key1"), 1);
    GBL_TEST_COMPARE(GblArrayMap_atVariant(&pSelf_->pMap1, (uintptr_t)"key1"), NULL);
    GBL_TEST_COMPARE(GblArrayMap_find(&pSelf_->pMap1, (uintptr_t)"key1"), 0);
    const char* pKey = (const char*)GblArrayMap_probeKey(&pSelf_->pMap1, 0);
    GBL_TEST_COMPARE(strcmp(pKey, "key1"), 0);
    GBL_TEST_COMPARE(GblArrayMap_probeValue(&pSelf_->pMap1, 0), 1);
    GBL_TEST_COMPARE(GblArrayMap_probeVariant(&pSelf_->pMap1, 0), NULL);
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_setUserdata_lazy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayMap_setUserdata(&pSelf_->pMap2,
                                                (uintptr_t)GblQuark_fromString("key2"),
                                                (uintptr_t)2,
                                                GblArrayMap_destructor_));
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap2), 1);
    GBL_TEST_VERIFY(!GblArrayMap_empty(&pSelf_->pMap2));
    GBL_TEST_VERIFY(GblArrayMap_contains(&pSelf_->pMap2, (uintptr_t)GblQuark_fromString("key2")));
    GBL_TEST_VERIFY(GblArrayMap_containsUserdata(&pSelf_->pMap2, (uintptr_t)GblQuark_fromString("key2")));
    GBL_TEST_VERIFY(!GblArrayMap_containsVariant(&pSelf_->pMap2, (uintptr_t)GblQuark_fromString("key2")));
    GBL_TEST_COMPARE(GblArrayMap_getValue(&pSelf_->pMap2, (uintptr_t)GblQuark_fromString("key2")), 2);
    GBL_TEST_COMPARE(GblArrayMap_getVariant(&pSelf_->pMap2, (uintptr_t)GblQuark_fromString("key2")), NULL);
    GBL_TEST_COMPARE(GblArrayMap_atValue(&pSelf_->pMap2, (uintptr_t)GblQuark_fromString("key2")), 2);
    GBL_TEST_COMPARE(GblArrayMap_atVariant(&pSelf_->pMap2, (uintptr_t)GblQuark_fromString("key2")), NULL);
    GBL_TEST_COMPARE(GblArrayMap_find(&pSelf_->pMap2, (uintptr_t)GblQuark_fromString("key2")), 0);
    const char* pKey = (const char*)GblArrayMap_probeKey(&pSelf_->pMap2, 0);
    GBL_TEST_COMPARE(strcmp(pKey, "key2"), 0);
    GBL_TEST_COMPARE(GblArrayMap_probeValue(&pSelf_->pMap2, 0), 2);
    GBL_TEST_COMPARE(GblArrayMap_probeVariant(&pSelf_->pMap2, 0), NULL);
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_insertUserdata_duplicate_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblArrayMap_insertUserdata(&pSelf_->pMap1, (uintptr_t)"key1", 5, NULL), 0);
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 1);
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_validate_userdata_(GblTestSuite* pSelf, GblContext* pCtx, GblArrayMap* pMap, uintptr_t key, uintptr_t value) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_TEST_VERIFY(GblArrayMap_contains(&pMap, key));
    GBL_TEST_VERIFY(GblArrayMap_containsUserdata(&pMap, key));
    GBL_TEST_COMPARE(GblArrayMap_getValue(&pMap, key), value);
    GBL_TEST_COMPARE(GblArrayMap_atValue(&pMap, key), value);
    GblSize loc = GblArrayMap_find(&pMap, key);
    GBL_TEST_VERIFY(loc != GBL_ARRAY_MAP_NPOS);
    const char* pKey = (const char*)GblArrayMap_probeKey(&pSelf_->pMap1, loc);
    GBL_TEST_VERIFY(pKey);
    GBL_TEST_COMPARE(strcmp(pKey, (const char*)key), 0);
    GBL_TEST_COMPARE(GblArrayMap_probeValue(&pSelf_->pMap1, loc), value);
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_validate_variant_(GblTestSuite* pSelf, GblContext* pCtx, GblArrayMap* pMap, uintptr_t key, uintptr_t value) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GblVariant* pVariant = NULL;
    GBL_TEST_VERIFY(GblArrayMap_contains(&pMap, key));
    GBL_TEST_VERIFY(!GblArrayMap_containsUserdata(&pMap, key));
    GBL_TEST_VERIFY(GblArrayMap_containsVariant(&pMap, key));
    GBL_TEST_COMPARE(GblArrayMap_getValue(&pMap, key), value);
    pVariant = GblArrayMap_getVariant(&pMap, key);
    GBL_TEST_VERIFY(pVariant);
    uintptr_t val = 0;
    GBL_TEST_COMPARE(GblVariant_getValuePeek(pVariant, &val), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(val, value);
    GBL_TEST_COMPARE(GblArrayMap_atValue(&pMap, key), value);
    GBL_TEST_COMPARE(GblArrayMap_atVariant(&pMap, key), pVariant);
    GblSize loc = GblArrayMap_find(&pMap, key);
    GBL_TEST_VERIFY(loc != GBL_ARRAY_MAP_NPOS);
    const char* pKey = (const char*)GblArrayMap_probeKey(&pSelf_->pMap1, loc);
    GBL_TEST_VERIFY(pKey);
    GBL_TEST_COMPARE(strcmp(pKey, (const char*)key), 0);
    GBL_TEST_COMPARE(GblArrayMap_probeValue(&pSelf_->pMap1, loc), value);
    GBL_TEST_COMPARE(GblArrayMap_probeVariant(&pSelf_->pMap1, loc), pVariant);
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_insertUserdata_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblArrayMap_insertUserdata(&pSelf_->pMap1, (uintptr_t)"key3", 3, GblArrayMap_destructor_), 1);
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 2);
    GBL_API_VERIFY_CALL(GblArrayMapTestSuite_validate_userdata_(pSelf,
                                                                pCtx,
                                                                pSelf_->pMap1,
                                                                (uintptr_t)"key3",
                                                                3));
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_setVariant_overUserdata_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GblVariant variant;
    GblVariant_constructValueCopy(&variant, GBL_UINT32_TYPE, 4);
    GBL_API_VERIFY_CALL(GblArrayMap_setVariant(&pSelf_->pMap1, (uintptr_t)"key3", &variant));
    GBL_TEST_COMPARE(dtorCalls_, 1);
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 2);
    GBL_API_VERIFY_CALL(GblArrayMapTestSuite_validate_variant_(pSelf,
                                                               pCtx,
                                                               pSelf_->pMap1,
                                                               (uintptr_t)"key3",
                                                               4));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_insertVariant_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GblVariant variant;
    GblVariant_constructValueCopy(&variant, GBL_POINTER_TYPE, 0xdeadbeef);
    GBL_TEST_COMPARE(GblArrayMap_insertVariant(&pSelf_->pMap1, (uintptr_t)"key4", &variant), 2);
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 3);
    GBL_API_VERIFY_CALL(GblArrayMapTestSuite_validate_variant_(pSelf,
                                                               pCtx,
                                                               pSelf_->pMap1,
                                                               (uintptr_t)"key4",
                                                                0xdeadbeef));
    GBL_API_VERIFY_CALL(GblArrayMapTestSuite_validate_userdata_(pSelf,
                                                                pCtx,
                                                                pSelf_->pMap1,
                                                                (uintptr_t)"key1",
                                                                1));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_eraseUserdata_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_TEST_VERIFY(GblArrayMap_erase(&pSelf_->pMap1, (uintptr_t)"key1"));
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 2);
    GBL_TEST_COMPARE(GblArrayMap_find(&pSelf_->pMap1, (uintptr_t)"key1"), GBL_ARRAY_MAP_NPOS);
    GBL_TEST_COMPARE(dtorCalls_, 2);
    GBL_TEST_COMPARE((const char*)dtorLastKey_, "key1");
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_extractValue_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayMap_setUserdata(&pSelf_->pMap1, (uintptr_t)"key7", 7, GblArrayMap_destructor_));
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 3);
    GBL_API_VERIFY_CALL(GblArrayMapTestSuite_validate_userdata_(pSelf,
                                                                pCtx,
                                                                pSelf_->pMap1,
                                                                (uintptr_t)"key7",
                                                                7));
    uintptr_t value = 0;
    GBL_API_VERIFY_CALL(GblArrayMap_extractValue(&pSelf_->pMap1, (uintptr_t)"key7", &value));
    GBL_TEST_COMPARE(value, 7);
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 2);
    GBL_TEST_COMPARE(dtorCalls_, 2);
    GBL_API_END();
}


static GBL_RESULT GblArrayMapTestSuite_extractVariant_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GblVariant variant = GBL_VARIANT_INIT;
    GBL_API_VERIFY_CALL(GblArrayMap_extractVariant(&pSelf_->pMap1, (uintptr_t)"key4", &variant));
    uintptr_t value = 0;
    GblVariant_getValuePeek(&variant, &value);
    GBL_TEST_COMPARE(value, 0xdeadbeef);
    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), 1);
    GBL_TEST_COMPARE(dtorCalls_, 2);
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayMap_setUserdata(&pSelf_->pMap1, (uintptr_t)"key7", 7, GblArrayMap_destructor_));
    GblArrayMap_clear(&pSelf_->pMap1);
    GBL_TEST_VERIFY(GblArrayMap_empty(&pSelf_->pMap1));
    GBL_TEST_COMPARE(dtorCalls_, 3);

    GblArrayMap_clear(&pSelf_->pMap2);
    GBL_TEST_VERIFY(GblArrayMap_empty(&pSelf_->pMap2));
    GBL_API_END();
}


static GBL_RESULT GblArrayMapTestSuite_destroy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&pSelf_->pMap1));
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&pSelf_->pMap2));
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_createBinarySearchable_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    pSelf_->pMap1 = GblArrayMap_create(NULL, GBL_TRUE, pCtx);
    GBL_TEST_VERIFY(GblArrayMap_binarySearches(&pSelf_->pMap1));
    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_verifyInsertionSort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);

    for(GblSize i = 0; i < GBL_ARRAY_MAP_TEST_SUITE_BINARY_SEARCHABLE_ENTRIES_; ++i) {
        uintptr_t key = gblRand();
        GblSize prevSize = GblArrayMap_size(&pSelf_->pMap1);
        while(GblArrayMap_size(&pSelf_->pMap1) <= prevSize) // check for collisions
            GBL_API_VERIFY_CALL(GblArrayMap_setUserdata(&pSelf_->pMap1, key, key, NULL));
    }

    GBL_TEST_COMPARE(GblArrayMap_size(&pSelf_->pMap1), GBL_ARRAY_MAP_TEST_SUITE_BINARY_SEARCHABLE_ENTRIES_);

    uintptr_t prevKey = 0;
    for(GblSize i = 0; i < GBL_ARRAY_MAP_TEST_SUITE_BINARY_SEARCHABLE_ENTRIES_; ++i) {
        const uintptr_t key = GblArrayMap_probeKey(&pSelf_->pMap1, i);
        GBL_TEST_VERIFY(prevKey <= key);
        prevKey = key;
    }

    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_findBinarySearch_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);

    for(GblSize i = 0; i < GBL_ARRAY_MAP_TEST_SUITE_BINARY_SEARCHABLE_ENTRIES_; ++i) {
        const uintptr_t key = GblArrayMap_probeKey(&pSelf_->pMap1, i);
        GBL_TEST_COMPARE(GblArrayMap_getValue(&pSelf_->pMap1, key), key);
    }

    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_findLinearSearch_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    pSelf_->pMap1->binarySearches = GBL_FALSE;

    for(GblSize i = 0; i < GBL_ARRAY_MAP_TEST_SUITE_BINARY_SEARCHABLE_ENTRIES_; ++i) {
        const uintptr_t key = GblArrayMap_probeKey(&pSelf_->pMap1, i);
        GBL_TEST_COMPARE(GblArrayMap_getValue(&pSelf_->pMap1, key), key);
    }

    GBL_API_END();
}

static GBL_RESULT GblArrayMapTestSuite_destroyBinarySearchable_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayMapTestSuite_* pSelf_ = GBL_ARRAY_MAP_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&pSelf_->pMap1));
    GBL_API_END();
}

GBL_EXPORT GblType GblArrayMapTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "nullInvalid",                GblArrayMapTestSuite_null_invalid_              },
        { "null",                       GblArrayMapTestSuite_null_                      },
        { "empty",                      GblArrayMapTestSuite_empty_                     },
        { "setUserdata",                GblArrayMapTestSuite_setUserdata_               },
        { "setUserdataLazy",            GblArrayMapTestSuite_setUserdata_lazy_          },
        { "insertUserdataDuplicate",    GblArrayMapTestSuite_insertUserdata_duplicate_  },
        { "insertUserdata",             GblArrayMapTestSuite_insertUserdata_            },
        { "setVariantOverUserdata",     GblArrayMapTestSuite_setVariant_overUserdata_   },
        { "insertVariant",              GblArrayMapTestSuite_insertVariant_             },
        { "eraseUserdata",              GblArrayMapTestSuite_eraseUserdata_             },
        { "extractValue",               GblArrayMapTestSuite_extractValue_              },
        { "extractVariant",             GblArrayMapTestSuite_extractVariant_            },
        { "clear",                      GblArrayMapTestSuite_clear_                     },
        { "destroy",                    GblArrayMapTestSuite_destroy_                   },
        { "createBinarySearchable",     GblArrayMapTestSuite_createBinarySearchable_    },
        { "verifyInsertionSort",        GblArrayMapTestSuite_verifyInsertionSort_       },
        { "findBinarySearch",           GblArrayMapTestSuite_findBinarySearch_          },
        { "findLinearSearch",           GblArrayMapTestSuite_findLinearSearch_          },
        { "destroyBinarySearchable",    GblArrayMapTestSuite_destroyBinarySearchable_   },
        { NULL,                         NULL                                            }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblArrayMapTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("ArrayMapTestSuite"),
                                     &vTable,
                                     sizeof(GblArrayMapTestSuite),
                                     sizeof(GblArrayMapTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}
