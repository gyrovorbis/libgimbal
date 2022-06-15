#include "containers/test_gimbal_array_map.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/containers/gimbal_array_map.h>
#include <gimbal/meta/gimbal_primitives.h>

static GblArrayMap* pMap1_      = NULL;
static GblArrayMap* pMap2_      = NULL;
static unsigned     dtorCalls_  = 0;

GblBool GblArrayMap_comparator_(uintptr_t key1,
                                uintptr_t key2)
{
    return strcmp((const char*)key1, (const char*)key2) == 0;
}

GBL_RESULT GblArrayMap_destructor_(void* pValue) {
    ++dtorCalls_;
    return GBL_RESULT_SUCCESS;
}

GBL_API GblArrayMap_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblArrayMap_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblArrayMap_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblArrayMap_test_null_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GBL_COMPARE(GblArrayMap_atValue(&pMap1_, 0), 0);
    GBL_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_COMPARE(GblArrayMap_atVariant(&pMap1_, 0), NULL);
    GBL_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}

GBL_API GblArrayMap_test_null(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    uintptr_t value;
    GblVariant variant;
    GblArrayMap* pOther = NULL;
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 0);
    GBL_VERIFY(GblArrayMap_empty(&pMap1_));
    GBL_COMPARE(GblArrayMap_context(&pMap1_), NULL);
    GBL_VERIFY(!GblArrayMap_contains(&pMap1_, 0));
    GBL_VERIFY(!GblArrayMap_containsUserdata(&pMap1_, 0));
    GBL_VERIFY(!GblArrayMap_containsVariant(&pMap1_, 0));
    GBL_COMPARE(GblArrayMap_getValue(&pMap1_, 0), 0);
    GBL_COMPARE(GblArrayMap_getVariant(&pMap1_, 0), NULL);
    GBL_VERIFY(!GblArrayMap_erase(&pMap1_, 0));
    GBL_VERIFY(!GblArrayMap_extractValue(&pMap1_, 0, &value));
    GBL_VERIFY(!GblArrayMap_extractVariant(&pMap1_, 0, &variant));
    GBL_COMPARE(GblArrayMap_find(&pMap1_, 0), GBL_ARRAY_MAP_NPOS);
    GBL_COMPARE(GblArrayMap_probeKey(&pMap1_, 0), 0);
    GBL_COMPARE(GblArrayMap_probeValue(&pMap1_, 0), 0);
    GBL_COMPARE(GblArrayMap_probeVariant(&pMap1_, 0), NULL);
    //pOther = GblArrayMap_clone(&pMap1_);
    //GBL_COMPARE(pOther, NULL);
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&pMap1_), GBL_RESULT_SUCCESS);
    GblArrayMap_clear(&pMap1_);
    GBL_API_END();
}

GBL_API GblArrayMap_test_empty(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    uintptr_t value;
    GblVariant variant;
    GblArrayMap* pOther = NULL;
    pMap1_ = GblArrayMap_create(GblArrayMap_comparator_, pCtx);
    GBL_VERIFY(pMap1_);
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 0);
    GBL_VERIFY(GblArrayMap_empty(&pMap1_));
    GBL_COMPARE(GblArrayMap_context(&pMap1_), pCtx);
    GBL_VERIFY(!GblArrayMap_contains(&pMap1_, 0));
    GBL_VERIFY(!GblArrayMap_containsUserdata(&pMap1_, 0));
    GBL_VERIFY(!GblArrayMap_containsVariant(&pMap1_, 0));
    GBL_COMPARE(GblArrayMap_getValue(&pMap1_, 0), 0);
    GBL_COMPARE(GblArrayMap_getVariant(&pMap1_, 0), NULL);
    GBL_VERIFY(!GblArrayMap_erase(&pMap1_, 0));
    GBL_VERIFY(!GblArrayMap_extractValue(&pMap1_, 0, &value));
    GBL_VERIFY(!GblArrayMap_extractVariant(&pMap1_, 0, &variant));
    GBL_COMPARE(GblArrayMap_find(&pMap1_, 0), GBL_ARRAY_MAP_NPOS);
    GBL_COMPARE(GblArrayMap_probeKey(&pMap1_, 0), 0);
    GBL_COMPARE(GblArrayMap_probeValue(&pMap1_, 0), 0);
    GBL_COMPARE(GblArrayMap_probeVariant(&pMap1_, 0), NULL);
    //pOther = GblArrayMap_clone(&pMap1_);
    //GBL_VERIFY(pOther);
    //GBL_COMPARE(GblArrayMap_context(&pOther), pCtx);
    //GBL_VERIFY(GblArrayMap_empty(&pOther));
    //GblArrayMap_destroy(&pOther);
    //GBL_VERIFY(!pOther);
    GBL_API_END();
}

GBL_API GblArrayMap_test_setUserdata(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblArrayMap_setUserdata(&pMap1_,
                                                (uintptr_t)"key1",
                                                (uintptr_t)1,
                                                GblArrayMap_destructor_));
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 1);
    GBL_VERIFY(!GblArrayMap_empty(&pMap1_));
    GBL_VERIFY(GblArrayMap_contains(&pMap1_, (uintptr_t)"key1"));
    GBL_VERIFY(GblArrayMap_containsUserdata(&pMap1_, (uintptr_t)"key1"));
    GBL_VERIFY(!GblArrayMap_containsVariant(&pMap1_, (uintptr_t)"key1"));
    GBL_COMPARE(GblArrayMap_getValue(&pMap1_, (uintptr_t)"key1"), 1);
    GBL_COMPARE(GblArrayMap_getVariant(&pMap1_, (uintptr_t)"key1"), NULL);
    GBL_COMPARE(GblArrayMap_atValue(&pMap1_, (uintptr_t)"key1"), 1);
    GBL_COMPARE(GblArrayMap_atVariant(&pMap1_, (uintptr_t)"key1"), NULL);
    GBL_COMPARE(GblArrayMap_find(&pMap1_, (uintptr_t)"key1"), 0);
    const char* pKey = (const char*)GblArrayMap_probeKey(&pMap1_, 0);
    GBL_COMPARE(strcmp(pKey, "key1"), 0);
    GBL_COMPARE(GblArrayMap_probeValue(&pMap1_, 0), 1);
    GBL_COMPARE(GblArrayMap_probeVariant(&pMap1_, 0), NULL);
    GBL_API_END();
}

GBL_API GblArrayMap_test_setUserdata_lazy(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblArrayMap_setUserdata(&pMap2_,
                                                (uintptr_t)GblQuark_fromString("key2"),
                                                (uintptr_t)2,
                                                GblArrayMap_destructor_));
    GBL_COMPARE(GblArrayMap_size(&pMap2_), 1);
    GBL_VERIFY(!GblArrayMap_empty(&pMap2_));
    GBL_VERIFY(GblArrayMap_contains(&pMap2_, (uintptr_t)GblQuark_fromString("key2")));
    GBL_VERIFY(GblArrayMap_containsUserdata(&pMap2_, (uintptr_t)GblQuark_fromString("key2")));
    GBL_VERIFY(!GblArrayMap_containsVariant(&pMap2_, (uintptr_t)GblQuark_fromString("key2")));
    GBL_COMPARE(GblArrayMap_getValue(&pMap2_, (uintptr_t)GblQuark_fromString("key2")), 2);
    GBL_COMPARE(GblArrayMap_getVariant(&pMap2_, (uintptr_t)GblQuark_fromString("key2")), NULL);
    GBL_COMPARE(GblArrayMap_atValue(&pMap2_, (uintptr_t)GblQuark_fromString("key2")), 2);
    GBL_COMPARE(GblArrayMap_atVariant(&pMap2_, (uintptr_t)GblQuark_fromString("key2")), NULL);
    GBL_COMPARE(GblArrayMap_find(&pMap2_, (uintptr_t)GblQuark_fromString("key2")), 0);
    const char* pKey = (const char*)GblArrayMap_probeKey(&pMap2_, 0);
    GBL_COMPARE(strcmp(pKey, "key2"), 0);
    GBL_COMPARE(GblArrayMap_probeValue(&pMap2_, 0), 2);
    GBL_COMPARE(GblArrayMap_probeVariant(&pMap2_, 0), NULL);
    GBL_API_END();
}

GBL_API GblArrayMap_test_insertUserdata_duplicate(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblArrayMap_insertUserdata(&pMap1_, (uintptr_t)"key1", 5, NULL), 0);
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 1);
    GBL_API_END();
}

GBL_API GblArrayMap_test_validate_userdata_(GblContext* pCtx, GblArrayMap* pMap, uintptr_t key, uintptr_t value) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(GblArrayMap_contains(&pMap, key));
    GBL_VERIFY(GblArrayMap_containsUserdata(&pMap, key));
    GBL_COMPARE(GblArrayMap_getValue(&pMap, key), value);
    GBL_COMPARE(GblArrayMap_atValue(&pMap, key), value);
    GblSize loc = GblArrayMap_find(&pMap, key);
    GBL_VERIFY(loc != GBL_ARRAY_MAP_NPOS);
    const char* pKey = (const char*)GblArrayMap_probeKey(&pMap1_, loc);
    GBL_VERIFY(pKey);
    GBL_COMPARE(strcmp(pKey, (const char*)key), 0);
    GBL_COMPARE(GblArrayMap_probeValue(&pMap1_, loc), value);
    GBL_API_END();
}

GBL_API GblArrayMap_test_validate_variant_(GblContext* pCtx, GblArrayMap* pMap, uintptr_t key, uintptr_t value) {
    GBL_API_BEGIN(pCtx);
    GblVariant* pVariant = NULL;
    GBL_VERIFY(GblArrayMap_contains(&pMap, key));
    GBL_VERIFY(!GblArrayMap_containsUserdata(&pMap, key));
    GBL_VERIFY(GblArrayMap_containsVariant(&pMap, key));
    GBL_COMPARE(GblArrayMap_getValue(&pMap, key), value);
    pVariant = GblArrayMap_getVariant(&pMap, key);
    GBL_VERIFY(pVariant);
    uint32_t val = 0;
    GBL_COMPARE(GblVariant_getValuePeek(pVariant, &val), GBL_RESULT_SUCCESS);
    GBL_COMPARE(val, value);
    GBL_COMPARE(GblArrayMap_atValue(&pMap, key), value);
    GBL_COMPARE(GblArrayMap_atVariant(&pMap, key), pVariant);
    GblSize loc = GblArrayMap_find(&pMap, key);
    GBL_VERIFY(loc != GBL_ARRAY_MAP_NPOS);
    const char* pKey = (const char*)GblArrayMap_probeKey(&pMap1_, loc);
    GBL_VERIFY(pKey);
    GBL_COMPARE(strcmp(pKey, (const char*)key), 0);
    GBL_COMPARE(GblArrayMap_probeValue(&pMap1_, loc), value);
    GBL_COMPARE(GblArrayMap_probeVariant(&pMap1_, loc), pVariant);
    GBL_API_END();
}

GBL_API GblArrayMap_test_insertUserdata(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblArrayMap_insertUserdata(&pMap1_, (uintptr_t)"key3", 3, GblArrayMap_destructor_), 1);
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 2);
    GBL_API_VERIFY_CALL(GblArrayMap_test_validate_userdata_(pCtx,
                                                       pMap1_,
                                                       (uintptr_t)"key3",
                                                       3));
    GBL_API_END();
}

GBL_API GblArrayMap_test_setVariant_overUserdata(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVariant variant;
    GblVariant_constructValueCopy(&variant, GBL_UINT32_TYPE, 4);
    GBL_API_VERIFY_CALL(GblArrayMap_setVariant(&pMap1_, (uintptr_t)"key3", &variant));
    GBL_COMPARE(dtorCalls_, 1);
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 2);
    GBL_API_VERIFY_CALL(GblArrayMap_test_validate_variant_(pCtx,
                                                           pMap1_,
                                                           (uintptr_t)"key3",
                                                           4));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));
    GBL_API_END();
}

GBL_API GblArrayMap_test_insertVariant(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVariant variant;
    GblVariant_constructValueCopy(&variant, GBL_POINTER_TYPE, 0xdeadbeef);
    GBL_COMPARE(GblArrayMap_insertVariant(&pMap1_, (uintptr_t)"key4", &variant), 2);
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 3);
    GBL_API_VERIFY_CALL(GblArrayMap_test_validate_variant_(pCtx,
                                                           pMap1_,
                                                           (uintptr_t)"key4",
                                                           0xdeadbeef));
    GBL_API_VERIFY_CALL(GblArrayMap_test_validate_userdata_(pCtx,
                                                           pMap1_,
                                                           (uintptr_t)"key1",
                                                           1));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));
    GBL_API_END();
}

GBL_API GblArrayMap_test_eraseUserdata(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(GblArrayMap_erase(&pMap1_, (uintptr_t)"key1"));
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 2);
    GBL_COMPARE(GblArrayMap_find(&pMap1_, (uintptr_t)"key1"), GBL_ARRAY_MAP_NPOS);
    GBL_COMPARE(dtorCalls_, 2);
    GBL_API_END();
}

GBL_API GblArrayMap_test_extractValue(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblArrayMap_setUserdata(&pMap1_, (uintptr_t)"key7", 7, GblArrayMap_destructor_));
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 3);
    GBL_API_VERIFY_CALL(GblArrayMap_test_validate_userdata_(pCtx,
                                                            pMap1_,
                                                            (uintptr_t)"key7",
                                                            7));
    uintptr_t value = 0;
    GBL_API_VERIFY_CALL(GblArrayMap_extractValue(&pMap1_, (uintptr_t)"key7", &value));
    GBL_COMPARE(value, 7);
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 2);
    GBL_COMPARE(dtorCalls_, 2);
    GBL_API_END();
}


GBL_API GblArrayMap_test_extractVariant(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVariant variant = GBL_VARIANT_INIT;
    GBL_API_VERIFY_CALL(GblArrayMap_extractVariant(&pMap1_, (uintptr_t)"key4", &variant));
    uintptr_t value = 0;
    GblVariant_getValuePeek(&variant, &value);
    GBL_COMPARE(value, 0xdeadbeef);
    GBL_COMPARE(GblArrayMap_size(&pMap1_), 1);
    GBL_COMPARE(dtorCalls_, 2);
    GBL_API_VERIFY_CALL(GblVariant_destruct(&variant));
    GBL_API_END();
}

GBL_API GblArrayMap_test_clear(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblArrayMap_setUserdata(&pMap1_, (uintptr_t)"key7", 7, GblArrayMap_destructor_));
    GblArrayMap_clear(&pMap1_);
    GBL_VERIFY(GblArrayMap_empty(&pMap1_));
    GBL_COMPARE(dtorCalls_, 3);

    GblArrayMap_clear(&pMap2_);
    GBL_VERIFY(GblArrayMap_empty(&pMap2_));
    GBL_API_END();
}


GBL_API GblArrayMap_test_destroy(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&pMap1_));
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&pMap2_));
    GBL_API_END();
}


GBL_API GblArrayMap_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}
