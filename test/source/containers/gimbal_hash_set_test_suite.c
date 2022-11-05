#include "containers/gimbal_hash_set_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_hash_set.h>

#define GBL_HASH_SET_TEST_SUITE_STRESS_TEST_ENTRY_COUNT_     2047

#define GBL_HASH_SET_TEST_SUITE_(inst)     ((GblHashSetTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_HASH_SET_TEST_SUITE_TYPE))

typedef struct GblHashSetTestSuite_ {
    GblHashSet  hashSet;
    GblSize     initialAllocCount;
    GblSize     dtorCount;
    const char* pDtorLastKey;
} GblHashSetTestSuite_;

typedef struct HashSetEntry_ {
    const char* pKey;
    uintptr_t   value;
} HashSetEntry_;

static GblHash hasher_(const GblHashSet* pSet, const void* pEntry) {
    GBL_UNUSED(pSet);
    const HashSetEntry_* pActual = (const HashSetEntry_*)pEntry;
    return gblHash(pActual->pKey, strlen(pActual->pKey));
}

static GblBool comparator_(const GblHashSet* pSet, const void* pEntryA, const void* pEntryB) {
    GBL_UNUSED(pSet);
    const HashSetEntry_* pActualA = (const HashSetEntry_*)pEntryA;
    const HashSetEntry_* pActualB = (const HashSetEntry_*)pEntryB;
    return strcmp(pActualA->pKey, pActualB->pKey) == 0;
}

static void destructor_(const GblHashSet* pSet, void* pEntry) {
    GblHashSetTestSuite_* pSelf_ = GblHashSet_userdata(pSet);
    const HashSetEntry_* pActual = (const HashSetEntry_*)pEntry;
    pSelf_->pDtorLastKey = pActual->pKey;
    ++pSelf_->dtorCount;
}

static GblBool foreach_(const GblHashSet* pSet, void* pEntry, void* pUd) {
    GBL_UNUSED(pSet);
    HashSetEntry_*  pActual = pEntry;
    HashSetEntry_*  pComp   = pUd;
    return (strcmp(pActual->pKey, pComp->pKey) == 0 &&
            pActual->value == pComp->value)? GBL_FALSE : GBL_TRUE;
}

static GBL_RESULT verify_(const GblHashSet* pSet, GblSize entryCount, const HashSetEntry_* pEntries, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblHashSet_size(pSet), entryCount);
    if(!entryCount) GBL_TEST_VERIFY(GblHashSet_empty(pSet));

    // get
    for(GblSize e = 0; e < entryCount; ++e) {
        HashSetEntry_* pEntry = GblHashSet_get(pSet, &pEntries[e]);
        GBL_TEST_VERIFY(pEntry);
        GBL_TEST_COMPARE(pEntry->pKey, pEntries[e].pKey);
        GBL_TEST_COMPARE(pEntry->value, pEntries[e].value);
    }

    // at
    for(GblSize e = 0; e < entryCount; ++e) {
        HashSetEntry_* pEntry = GblHashSet_at(pSet, &pEntries[e]);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_TEST_VERIFY(pEntry);
        GBL_TEST_COMPARE(pEntry->pKey, pEntries[e].pKey);
        GBL_TEST_COMPARE(pEntry->value, pEntries[e].value);
    }

    // contains
    for(GblSize e = 0; e < entryCount; ++e) {
        GBL_TEST_VERIFY(GblHashSet_contains(pSet, &pEntries[e]));
    }

    // count
    for(GblSize e = 0; e < entryCount; ++e) {
        GBL_TEST_COMPARE(GblHashSet_count(pSet, &pEntries[e]), 1);
    }

    //find
    for(GblSize e = 0; e < entryCount; ++e) {
        GblHashSetIter it = GblHashSet_find(pSet, &pEntries[e]);
        GBL_TEST_VERIFY(GblHashSetIter_valid(&it));
        HashSetEntry_* pEntry = GblHashSetIter_value(&it);
        GBL_TEST_VERIFY(pEntry);
        GBL_TEST_COMPARE(pEntry->pKey, pEntries[e].pKey);
        GBL_TEST_COMPARE(pEntry->value, pEntries[e].value);
    }

    // probe
    for(GblSize e = 0; e < entryCount; ++e) {
        GblBool found = GBL_FALSE;
        for(GblSize b = 0; b < GblHashSet_bucketCount(pSet); ++b) {
            HashSetEntry_* pEntry = GblHashSet_probe(pSet, b);
            if(pEntry) {
                if(strcmp(pEntry->pKey, pEntries[e].pKey) == 0 &&
                        pEntry->value == pEntries[e].value)
                {
                    found = GBL_TRUE;
                    break;
                }
            }
        }
        GBL_TEST_VERIFY(found);
    }

    // foreach
    for(GblSize e = 0; e < entryCount; ++e) {
        GBL_TEST_VERIFY(!GblHashSet_foreach(pSet, foreach_, (void*)&pEntries[e]));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblHashSetTestSuite_));
    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_construct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblHashSet_construct(&pSelf_->hashSet,
                                             sizeof(HashSetEntry_),
                                             hasher_,
                                             comparator_,
                                             destructor_,
                                             2,
                                             pCtx,
                                             pSelf_));
    GBL_TEST_COMPARE(GblHashSet_context(&pSelf_->hashSet), pCtx);
    GBL_TEST_COMPARE(GblHashSet_userdata(&pSelf_->hashSet), pSelf_);
    GBL_TEST_VERIFY(GblHashSet_empty(&pSelf_->hashSet));
    GBL_CTX_END();
}


static GBL_RESULT GblHashSetTestSuite_getInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    const char* pKey = "Entry1";

    void* pEntry = GblHashSet_get(&pSelf_->hashSet, &pKey);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(pEntry, NULL);
    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_atInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    const char* pKey = "Entry1";

    GBL_TEST_EXPECT_ERROR();
    void* pEntry = GblHashSet_at(&pSelf_->hashSet, &pKey);
    GBL_TEST_COMPARE(pEntry, NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_containsInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    const char* pKey = "Entry1";

    GBL_TEST_COMPARE(GblHashSet_contains(&pSelf_->hashSet, &pKey), GBL_FALSE);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_findInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    const char* pKey = "Entry1";

    GblHashSetIter it = GblHashSet_find(&pSelf_->hashSet, &pKey);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(!GblHashSetIter_valid(&it));

    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_setInsert_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    HashSetEntry_ entry = {
        .pKey = "Entry1",
        .value = 2
    };

    GBL_TEST_COMPARE(GblHashSet_set(&pSelf_->hashSet, &entry), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                1,
                                &entry,
                                pCtx));


    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_setOverride_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    HashSetEntry_ entry = {
        .pKey = "Entry1",
        .value = 1
    };

    HashSetEntry_* pEntry = GblHashSet_set(&pSelf_->hashSet, &entry);
    GBL_TEST_VERIFY(pEntry);
    GBL_TEST_COMPARE(pEntry->value, 2);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                1,
                                &entry,
                                pCtx));


    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_insertInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    HashSetEntry_ entry = {
        .pKey = "Entry1",
        .value = 1
    };

    GBL_TEST_VERIFY(!GblHashSet_insert(&pSelf_->hashSet, &entry));

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                1,
                                &entry,
                                pCtx));


    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_insert_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    HashSetEntry_ entries[] = {
        {
            .pKey = "Entry1",
            .value = 1
        }, {
            .pKey = "Entry2",
            .value = 2
        }
    };

    GBL_TEST_VERIFY(GblHashSet_insert(&pSelf_->hashSet, &entries[1]));

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                2,
                                entries,
                                pCtx));

    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_insertOrAssignInsert_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    HashSetEntry_ entries[] = {
        {
            .pKey = "Entry1",
            .value = 1
        }, {
            .pKey = "Entry2",
            .value = 2
        }, {
            .pKey = "Entry3",
            .value = 4
        }
    };

    GblHashSet_insertOrAssign(&pSelf_->hashSet, &entries[2]);

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                3,
                                entries,
                                pCtx));

    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_insertOrAssignAssign_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);

    HashSetEntry_ entries[] = {
        {
            .pKey = "Entry1",
            .value = 1
        }, {
            .pKey = "Entry2",
            .value = 2
        }, {
            .pKey = "Entry3",
            .value = 3
        }
    };

    GblHashSet_insertOrAssign(&pSelf_->hashSet, &entries[2]);

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                3,
                                entries,
                                pCtx));

    GBL_TEST_COMPARE(pSelf_->dtorCount, 1);
    GBL_TEST_COMPARE(pSelf_->pDtorLastKey, "Entry3");

    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_emplaceOverExisting_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);


    HashSetEntry_ entries[] = {
        {
            .pKey = "Entry1",
            .value = 0
        }, {
            .pKey = "Entry2",
            .value = 2
        }, {
            .pKey = "Entry3",
            .value = 3
        }
    };
    const char* pKey = "Entry1";

    HashSetEntry_* pEntry = GblHashSet_emplace(&pSelf_->hashSet, &pKey);
    pEntry->value = 0;

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                3,
                                entries,
                                pCtx));


    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_emplaceInsert_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);

    HashSetEntry_ entries[] = {
        {
            .pKey = "Entry1",
            .value = 0
        }, {
            .pKey = "Entry2",
            .value = 2
        }, {
            .pKey = "Entry3",
            .value = 3
        }, {
            .pKey = "Entry4",
            .value = 4
        }
    };

    HashSetEntry_* pKey = GBL_ALLOCA(sizeof(HashSetEntry_));
    pKey->pKey = "Entry4";

    HashSetEntry_* pEntry = GblHashSet_emplace(&pSelf_->hashSet, pKey);
    GBL_TEST_VERIFY(pEntry);
    //pEntry->pKey = pKey;
    pEntry->value = 4;

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                4,
                                entries,
                                pCtx));

    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_tryEmplaceInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    HashSetEntry_ entries[] = {
        {
            .pKey = "Entry1",
            .value = 0
        }, {
            .pKey = "Entry2",
            .value = 2
        }, {
            .pKey = "Entry3",
            .value = 3
        }, {
            .pKey = "Entry4",
            .value = 4
        }
    };
    const char* pKey = "Entry4";

    HashSetEntry_* pEntry = GblHashSet_tryEmplace(&pSelf_->hashSet, &pKey);
    GBL_TEST_COMPARE(pEntry, NULL);

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                4,
                                entries,
                                pCtx));


    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_tryEmplace_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    HashSetEntry_ entries[] = {
        {
           .pKey = "Entry0",
            .value = 0,
        }, {
            .pKey = "Entry1",
            .value = 0
        }, {
            .pKey = "Entry2",
            .value = 2
        }, {
            .pKey = "Entry3",
            .value = 3
        }, {
            .pKey = "Entry4",
            .value = 4
        }
    };
    HashSetEntry_* pKey = GBL_ALLOCA(sizeof(HashSetEntry_));
    pKey->pKey = "Entry0";

    HashSetEntry_* pEntry = GblHashSet_tryEmplace(&pSelf_->hashSet, pKey);
    GBL_TEST_VERIFY(pEntry);
//    pEntry->pKey = "Entry0";
    pEntry->value = 0;

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                5,
                                entries,
                                pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_eraseInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    HashSetEntry_ entries[] = {
        {
           .pKey = "Entry0",
            .value = 0,
        }, {
            .pKey = "Entry1",
            .value = 0
        }, {
            .pKey = "Entry2",
            .value = 2
        }, {
            .pKey = "Entry3",
            .value = 3
        }, {
            .pKey = "Entry4",
            .value = 4
        }
    };
    const char* pKey = "Entry5";

    GBL_TEST_VERIFY(!GblHashSet_erase(&pSelf_->hashSet, &pKey));

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                5,
                                entries,
                                pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_erase_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);

    HashSetEntry_ entries[] = {
        {
           .pKey = "Entry0",
            .value = 0,
        }, {
            .pKey = "Entry2",
            .value = 2
        }, {
            .pKey = "Entry3",
            .value = 3
        }, {
            .pKey = "Entry4",
            .value = 4
        }
    };
    const char* pKey = "Entry1";

    GBL_TEST_VERIFY(GblHashSet_erase(&pSelf_->hashSet, &pKey));

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                4,
                                entries,
                                pCtx));

    GBL_TEST_COMPARE(pSelf_->pDtorLastKey, "Entry1");
    GBL_TEST_COMPARE(pSelf_->dtorCount, 2);
    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_extractInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);

    HashSetEntry_ entries[] = {
        {
           .pKey = "Entry0",
            .value = 0,
        }, {
            .pKey = "Entry2",
            .value = 2
        }, {
            .pKey = "Entry3",
            .value = 3
        }, {
            .pKey = "Entry4",
            .value = 4
        }
    };
    const char* pKey = "Entry1";

    GBL_TEST_COMPARE(GblHashSet_extract(&pSelf_->hashSet, &pKey), NULL);

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                4,
                                entries,
                                pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_extract_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    HashSetEntry_ entries[] = {
        {
            .pKey = "Entry2",
            .value = 2
        }, {
            .pKey = "Entry3",
            .value = 3
        }, {
            .pKey = "Entry4",
            .value = 4
        }
    };
    const char* pKey = "Entry0";

    HashSetEntry_* pEntry = GblHashSet_extract(&pSelf_->hashSet, &pKey);
    GBL_TEST_VERIFY(pEntry);
    GBL_TEST_COMPARE(pEntry->pKey, "Entry0");
    GBL_TEST_COMPARE(pEntry->value, 0);

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                3,
                                entries,
                                pCtx));


    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);

    GblHashSet_clear(&pSelf_->hashSet);

    GBL_CTX_VERIFY_CALL(verify_(&pSelf_->hashSet,
                                0,
                                NULL,
                                pCtx));

    GBL_CTX_END();
}

static GBL_RESULT GblHashSetTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblHashSet_destruct(&pSelf_->hashSet));
    GBL_CTX_END();
}

static GblHash collidingHasher_(const GblHashSet* pSelf, const void* pEntry) {
    GBL_UNUSED(pSelf && pEntry);
    return 0;
}

static GBL_RESULT GblHashSetTestSuite_collidingConstruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);

    pSelf_->dtorCount = 0;

    GBL_CTX_VERIFY_CALL(GblHashSet_construct(&pSelf_->hashSet,
                                             sizeof(HashSetEntry_),
                                             collidingHasher_,
                                             comparator_,
                                             destructor_,
                                             2,
                                             pCtx,
                                             pSelf_));

    GBL_TEST_COMPARE(GblHashSet_context(&pSelf_->hashSet), pCtx);
    GBL_TEST_COMPARE(GblHashSet_userdata(&pSelf_->hashSet), pSelf_);
    GBL_TEST_VERIFY(GblHashSet_empty(&pSelf_->hashSet));
    GBL_CTX_END();
}

GBL_EXPORT GblType GblHashSetTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "construct",                       GblHashSetTestSuite_construct_                 },
        { "getInvalid",                      GblHashSetTestSuite_getInvalid_                },
        { "atInvalid",                       GblHashSetTestSuite_atInvalid_                 },
        { "containsInvalid",                 GblHashSetTestSuite_containsInvalid_           },
        { "findInvalid",                     GblHashSetTestSuite_findInvalid_               },
        { "setInsert",                       GblHashSetTestSuite_setInsert_                 },
        { "setOverride",                     GblHashSetTestSuite_setOverride_               },
        { "insertInvalid",                   GblHashSetTestSuite_insertInvalid_             },
        { "insert",                          GblHashSetTestSuite_insert_                    },
        { "insertOrAssignInsert",            GblHashSetTestSuite_insertOrAssignInsert_      },
        { "insertOrAssignAssign",            GblHashSetTestSuite_insertOrAssignAssign_      },
        { "emplaceOverExisting",             GblHashSetTestSuite_emplaceOverExisting_       },
        { "emplaceInsert",                   GblHashSetTestSuite_emplaceInsert_             },
        { "tryEmplaceInvalid",               GblHashSetTestSuite_tryEmplaceInvalid_         },
        { "tryEmplace",                      GblHashSetTestSuite_tryEmplace_                },
        { "eraseInvalid",                    GblHashSetTestSuite_eraseInvalid_              },
        { "erase",                           GblHashSetTestSuite_erase_                     },
        { "extractInvalid",                  GblHashSetTestSuite_extractInvalid_            },
        { "extract",                         GblHashSetTestSuite_extract_                   },
        { "clear",                           GblHashSetTestSuite_clear_                     },
        { "destruct",                        GblHashSetTestSuite_destruct_                  },
        { "collidingConstruct",              GblHashSetTestSuite_collidingConstruct_        },
        { "collidingSetInsert",              GblHashSetTestSuite_setInsert_                 },
        { "collidingSetOverride",            GblHashSetTestSuite_setOverride_               },
        { "collidingInsert",                 GblHashSetTestSuite_insert_                    },
        { "collidingInsertOrAssignInsert",   GblHashSetTestSuite_insertOrAssignInsert_      },
        { "collidingInsertOrAssignAssign",   GblHashSetTestSuite_insertOrAssignAssign_      },
        { "collidingEmplaceOverExisting",    GblHashSetTestSuite_emplaceOverExisting_       },
        { "collidingEmplaceInsert",          GblHashSetTestSuite_emplaceInsert_             },
        { "collidingTryEmplace",             GblHashSetTestSuite_tryEmplace_                },
        { "collidingErase",                  GblHashSetTestSuite_erase_                     },
        { "collidingExtract",                GblHashSetTestSuite_extract_                   },
        { "collidingDestruct",               GblHashSetTestSuite_destruct_                  },
        { NULL,                              NULL                                           }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblHashSetTestSuite_init_,
        .pFnSuiteFinal  = GblHashSetTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblHashSetTestSuite"),
                                     &vTable,
                                     sizeof(GblHashSetTestSuite),
                                     sizeof(GblHashSetTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}
