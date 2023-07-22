#include "core/gimbal_module_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_module.h>

#define TEST_MODULE1_TYPE_            (GBL_TYPEOF(TestModule1))
#define TEST_MODULE1_(self)           (GBL_INSTANCE_CAST(self, TestModule1))
#define TEST_MODULE1_CLASS_(klass)    (GBL_CLASS_CAST(klass, TestModule1))
#define TEST_MODULE1_GET_CLASS_(self) (GBL_INSTANCE_GET_CLASS(self, TestModule1))


#define TEST_MODULE2_TYPE_            (GBL_TYPEOF(TestModule2))
#define TEST_MODULE2_(self)           (GBL_INSTANCE_CAST(self, TestModule2))
#define TEST_MODULE2_CLASS_(klass)    (GBL_CLASS_CAST(klass, TestModule2))
#define TEST_MODULE2_GET_CLASS_(self) (GBL_INSTANCE_GET_CLASS(self, TestModule2))

#define GBL_SELF_TYPE GblModuleTestSuite

GBL_CLASS_DERIVE(TestModule1, GblModule)
    const char* pStatic;
GBL_CLASS_END

GBL_INSTANCE_DERIVE(TestModule1, GblModule)
    float       value1;
    int         value2;
    const char* pValue3;
GBL_INSTANCE_END

GBL_CLASS_DERIVE_EMPTY(TestModule2, GblModule)

GBL_INSTANCE_DERIVE(TestModule2, GblModule)
    GblBool     value1;
    double      value2;
    const char* pValue3;
GBL_INSTANCE_END

GBL_TEST_FIXTURE {
    TestModule1* pModule1;
    TestModule2* pModule2;
    GblModule*   pModule3;
    GblRefCount  classRefCount;
    GblRefCount  instances;
    GblRefCount  refCount;
    size_t       typeCount;
    unsigned     foreachCount;
};

static GblType TestModule1_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        type = GblType_registerStatic(GblQuark_internStringStatic("TestModule1"),
                                      GBL_MODULE_TYPE,
                                      &(const GblTypeInfo) {
                                          .classSize    = sizeof(TestModule1Class),
                                          .instanceSize = sizeof(TestModule1)
                                      },
                                      GBL_TYPE_FLAGS_NONE);

    }

    return type;
}

static GblType TestModule2_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        type = GblType_registerStatic(GblQuark_internStringStatic("TestModule2"),
                                      GBL_MODULE_TYPE,
                                      &(const GblTypeInfo) {
                                          .classSize    = sizeof(TestModule2Class),
                                          .instanceSize = sizeof(TestModule2)
                                      },
                                      GBL_TYPE_FLAGS_NONE);

    }

    return type;
}

GBL_TEST_INIT() {
    pFixture->classRefCount = GblType_classRefCount(GBL_MODULE_TYPE);
    pFixture->instances     = GblType_instanceRefCount(GBL_MODULE_TYPE);
    pFixture->refCount      = GblRef_activeCount();
    pFixture->typeCount     = GblType_registeredCount();
    GBL_TEST_CASE_END;
}

GBL_TEST_FINAL() {
    GblType_unregister(TEST_MODULE1_TYPE_);
    GblType_unregister(TEST_MODULE2_TYPE_);

    GBL_TEST_COMPARE(GblRef_activeCount(), pFixture->refCount);
    GBL_TEST_COMPARE(GblType_instanceRefCount(GBL_MODULE_TYPE), pFixture->instances);
    GBL_TEST_COMPARE(GblType_classRefCount(GBL_MODULE_TYPE), pFixture->classRefCount);
    GBL_TEST_COMPARE(GblType_registeredCount(), pFixture->typeCount);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(findFail) {
    GBL_TEST_COMPARE(GblModule_find(NULL),
                     NULL);

    GBL_TEST_COMPARE(GblModule_find("trololo"),
                     NULL);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(findQuarkFail) {
    GBL_TEST_COMPARE(GblModule_findQuark(GBL_QUARK_INVALID),
                     NULL);

    GBL_TEST_COMPARE(GblModule_findQuark(GblQuark_fromStringStatic("trololo")),
                     NULL);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(atInvalid) {

    GBL_TEST_COMPARE(GblModule_at(233), NULL);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(countEmpty) {

    GBL_TEST_COMPARE(GblModule_count(), 0);

    GBL_TEST_CASE_END;
}

static GblBool nullTestIter_(GblModule* pIt, void* pClosure) {
    GBL_UNUSED(pIt);
    ++((GblModuleTestSuite_*)pClosure)->foreachCount;
    return GBL_FALSE;
}

GBL_TEST_CASE(foreachEmpty) {

    GBL_TEST_COMPARE(GblModule_foreach(nullTestIter_,
                                       pFixture),
                     GBL_FALSE);

    GBL_TEST_COMPARE(pFixture->foreachCount, 0);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(createBase) {
    GblModule* pModule = GblModule_create(GBL_MODULE_TYPE,
                                          "TestModule3",
                                          GblVersion_make(1, 2, 3),
                                          "David Bowie",
                                          "Ziggy Stardust's Module",
                                          "ziggy");


    pFixture->pModule3 = pModule;

    GBL_TEST_VERIFY(pModule);

    GBL_TEST_COMPARE(GblObject_name(GBL_OBJECT(pModule)),
                                            "TestModule3");
    GBL_TEST_COMPARE(pModule->pPrefix,      "ziggy");
    GBL_TEST_COMPARE(pModule->version,      GblVersion_make(1, 2, 3));
    GBL_TEST_COMPARE(pModule->pAuthor,      "David Bowie");
    GBL_TEST_COMPARE(pModule->pDescription, "Ziggy Stardust's Module");

    GBL_TEST_COMPARE(GblModule_useCount(pModule), 0);
    GBL_TEST_VERIFY(!GblModule_isLoaded(pModule));

    GBL_TEST_CASE_END;
}

static GblBool findIter_(GblModule* pIt, void* pClosure) {
    if(--((GblModuleTestSuite_*)pClosure)->foreachCount == 0)
        return GBL_TRUE;

    return GBL_FALSE;
}

GBL_RESULT registerVerify_(GblModule* pModule, GblModuleTestSuite_* pFixture) {
    GBL_CTX_BEGIN(NULL);

    const char*  pName    = GblObject_name(GBL_OBJECT(pModule));
    const size_t newCount = GblModule_count() + 1;

    GBL_TEST_CALL(GblModule_register(pModule));
    GBL_TEST_COMPARE(GblModule_unref(pModule), 1);

    GBL_TEST_COMPARE(GblModule_count(), newCount);
    GBL_TEST_COMPARE(GblModule_find(pName), pModule);
    GBL_TEST_COMPARE(GblModule_findQuark(GblQuark_fromString(pName)), pModule);

    GblBool found = GBL_FALSE;
    for(size_t  m = 0; m < GblModule_count(); ++m)
        if(GblModule_at(m) == pModule) {
            found = GBL_TRUE;
            break;
        }
    GBL_TEST_VERIFY(found);

    pFixture->foreachCount = newCount;
    GBL_TEST_COMPARE(GblModule_foreach(findIter_, pFixture), GBL_TRUE);
    GBL_TEST_COMPARE(pFixture->foreachCount, 0);

    GBL_CTX_END();
}

GBL_TEST_CASE(registerBase) {
    GBL_TEST_CALL(registerVerify_(pFixture->pModule3, pFixture));
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(registerDerived) {

    pFixture->pModule1 = GBL_OBJECT_NEW(TestModule1);
    pFixture->pModule2 = GBL_OBJECT_NEW(TestModule2);

    GBL_TEST_CALL(registerVerify_(GBL_MODULE(pFixture->pModule1), pFixture));
    GBL_TEST_CALL(registerVerify_(GBL_MODULE(pFixture->pModule2), pFixture));
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(requireVersion) {
    GblModule* pModule = GBL_REQUIRE(GblModule, "TestModule3", "1.2.3");
    GBL_TEST_COMPARE(pModule, pFixture->pModule3);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(requireName) {
    TestModule2* pModule = GBL_REQUIRE(TestModule2, "TestModule2");
    GBL_TEST_COMPARE(pModule, pFixture->pModule2);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(require) {
    TestModule1* pModule = GBL_REQUIRE(TestModule1);
    GBL_TEST_COMPARE(pModule, pFixture->pModule1);
    GBL_TEST_CASE_END;
}

// option group checking

GBL_TEST_CASE(unregister) {
    GBL_TEST_CALL(GblModule_unregister(GBL_MODULE(pFixture->pModule1)));
    GBL_TEST_CALL(GblModule_unregister(GBL_MODULE(pFixture->pModule2)));
    GBL_TEST_CALL(GblModule_unregister(pFixture->pModule3));
    GBL_TEST_CASE_END;
}

GBL_TEST_REGISTER(findFail,
                  findQuarkFail,
                  atInvalid,
                  countEmpty,
                  foreachEmpty,
                  createBase,
                  registerBase,
                  registerDerived,
                  requireVersion,
                  requireName,
                  require,
                  unregister)
