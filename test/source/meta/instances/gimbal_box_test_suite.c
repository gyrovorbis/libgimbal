#include "meta/instances/gimbal_box_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>

#define GBL_SELF_TYPE GblBoxTestSuite

GBL_TEST_FIXTURE {
    size_t       refCount;
    GblQuark     testQuark;
    GblBool      userDtorCalled;
    size_t       fieldDtorsCalled;
    GblBoxClass* pBoxClass;
};

GBL_TEST_INIT()
    pFixture->testQuark = GblQuark_fromStringStatic("test");
    pFixture->refCount  = GblRef_activeCount();
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_TEST_COMPARE(pFixture->refCount, GblRef_activeCount());
GBL_TEST_CASE_END

GBL_TEST_CASE(classCreateFloatingInvalid)
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_VERIFY(!GblBoxClass_createFloating(GBL_INSTANCE_TYPE,
                                                sizeof(GblBoxClass),
                                                NULL,
                                                NULL));
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_TYPE_MISMATCH);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_VERIFY(!GblBoxClass_createFloating(GBL_BOX_TYPE,
                                                sizeof(GblClass),
                                                NULL,
                                                NULL));
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(classConstructFloatingInvalid)
    GBL_TEST_EXPECT_ERROR();
    GblBoxClass klass;

    GBL_TEST_COMPARE(GblBoxClass_constructFloating(&klass,
                                                   GBL_INSTANCE_TYPE,
                                                   NULL,
                                                   NULL),
                     GBL_RESULT_ERROR_TYPE_MISMATCH);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

static GBL_RESULT classFloatingUserDtor_(const GblArrayMap* pMap, uintptr_t key, void* pEntry) {
    GblBoxClass*    pClass   = pEntry;
    GblTestFixture* pFixture = GblBoxClass_userdata(pClass);

    pFixture->userDtorCalled = GBL_TRUE;

    return GBL_RESULT_SUCCESS;
}

typedef struct ExtBoxClass_ {
    GblBoxClass   base;
    char          ext[64];
} ExtBoxClass_;

GBL_TEST_CASE(classCreateDestroyFloating)
    GblBoxClass* pClass =
        GblBoxClass_createFloating(GBL_BOX_TYPE,
                                   sizeof(ExtBoxClass_),
                                   pFixture,
                                   classFloatingUserDtor_);

    GBL_TEST_VERIFY(pClass);

    ExtBoxClass_* pExt = (ExtBoxClass_*)pClass;
    memset(pExt->ext, 0xff, sizeof(pExt->ext));

    GBL_TEST_COMPARE(GBL_CLASS_TYPEOF(pClass), GBL_BOX_TYPE);
    GBL_TEST_VERIFY(GblClass_isFloating(GBL_CLASS(pClass)));
    GBL_TEST_VERIFY(!GblClass_isOwned(GBL_CLASS(pClass)));
    GBL_TEST_VERIFY(!GblClass_isDefault(GBL_CLASS(pClass)));
    GBL_TEST_VERIFY(!GblClass_isInPlace(GBL_CLASS(pClass)));

    GBL_TEST_COMPARE(GblBoxClass_userdata(pClass), pFixture);

    GBL_TEST_CALL(GblClass_destroyFloating(GBL_CLASS(pClass)));
    GBL_TEST_VERIFY(pFixture->userDtorCalled);
GBL_TEST_CASE_END

GBL_TEST_CASE(classConstructDestructFloating)
    GblBoxClass klass;

    GBL_TEST_CALL(GblBoxClass_constructFloating(&klass, GBL_BOX_TYPE));

    GBL_TEST_COMPARE(GBL_CLASS_TYPEOF(&klass), GBL_BOX_TYPE);
    GBL_TEST_VERIFY(GblClass_isFloating(GBL_CLASS(&klass)));
    GBL_TEST_VERIFY(!GblClass_isOwned(GBL_CLASS(&klass)));
    GBL_TEST_VERIFY(!GblClass_isDefault(GBL_CLASS(&klass)));
    GBL_TEST_VERIFY(GblClass_isInPlace(GBL_CLASS(&klass)));

    GblBoxClass_setUserdata(&klass, pFixture);
    GBL_TEST_COMPARE(GblBoxClass_userdata(&klass), pFixture);

    GblBoxClass_setUserDestructor(&klass, classFloatingUserDtor_);

    pFixture->userDtorCalled = GBL_FALSE;
    GBL_TEST_CALL(GblClass_destructFloating(GBL_CLASS(&klass)));
    GBL_TEST_VERIFY(pFixture->userDtorCalled);
GBL_TEST_CASE_END

static GBL_RESULT classFieldDtor_(const GblArrayMap* pMap, uintptr_t key, void* pEntry) {
    GBL_CTX_BEGIN(NULL);

    // This is a horrible thing to do, but this is a test, so whatever.
    GblTestFixture* pFixture = (GblTestFixture*)
                               GblArrayMap_getValue((GblArrayMap**)&pMap,
                                                    GblQuark_fromStringStatic("_ud"));

    GBL_TEST_VERIFY(pFixture && key == pFixture->testQuark);

    ++pFixture->fieldDtorsCalled;
    GblStringRef_release(pEntry);

    GBL_CTX_END();
}

GBL_TEST_CASE(classAddField)
    pFixture->pBoxClass =
        GblBoxClass_createFloating(GBL_BOX_TYPE,
                                   sizeof(GblBoxClass),
                                   pFixture);

    GBL_TEST_VERIFY(!GblBoxClass_hasField(pFixture->pBoxClass,
                                          pFixture->testQuark));

    GBL_TEST_CALL(GblBoxClass_setField(pFixture->pBoxClass,
                                       pFixture->testQuark,
                                       (uintptr_t)GblStringRef_create("Trololo"),
                                       classFieldDtor_));

    GBL_TEST_VERIFY(GblBoxClass_hasField(pFixture->pBoxClass,
                                         pFixture->testQuark));

    GBL_TEST_COMPARE((const char*)GblBoxClass_field(pFixture->pBoxClass,
                                                       pFixture->testQuark),
                     "Trololo");
GBL_TEST_CASE_END

GBL_TEST_CASE(classTakeField)
    GblStringRef* pRef = (GblStringRef*)
        GblBoxClass_takeField(pFixture->pBoxClass,
                              pFixture->testQuark);

    GBL_TEST_COMPARE(pRef, "Trololo");
    GblStringRef_unref(pRef);

    GBL_TEST_VERIFY(!pFixture->fieldDtorsCalled);

    GBL_TEST_VERIFY(!GblBoxClass_hasField(pFixture->pBoxClass,
                                          pFixture->testQuark));

    GBL_TEST_VERIFY(!GblBoxClass_field(pFixture->pBoxClass,
                                          pFixture->testQuark));
GBL_TEST_CASE_END

GBL_TEST_CASE(classReplaceField)
    GBL_TEST_CALL(GblBoxClass_setField(pFixture->pBoxClass,
                                       pFixture->testQuark,
                                       (uintptr_t)GblStringRef_create("Treamcast"),
                                       classFieldDtor_));

    GBL_TEST_CALL(GblBoxClass_setField(pFixture->pBoxClass,
                                           pFixture->testQuark,
                                           (uintptr_t)GblStringRef_create("Treamcast2"),
                                           classFieldDtor_));

    GBL_TEST_COMPARE(pFixture->fieldDtorsCalled, 1);

    GBL_TEST_COMPARE((const char*)GblBoxClass_field(pFixture->pBoxClass,
                                                       pFixture->testQuark),
                     "Treamcast2");
GBL_TEST_CASE_END

GBL_TEST_CASE(classClearField)
    GBL_TEST_VERIFY(GblBoxClass_clearField(pFixture->pBoxClass,
                                           pFixture->testQuark));

    GBL_TEST_COMPARE(pFixture->fieldDtorsCalled, 2);

    GBL_TEST_VERIFY(!GblBoxClass_hasField(pFixture->pBoxClass,
                                          pFixture->testQuark));

    GBL_TEST_VERIFY(!GblBoxClass_field(pFixture->pBoxClass,
                                          pFixture->testQuark));
GBL_TEST_CASE_END

GBL_TEST_CASE(classDestructField)
    GBL_TEST_CALL(GblBoxClass_setField(pFixture->pBoxClass,
                                   pFixture->testQuark,
                                   (uintptr_t)GblStringRef_create("Screamcast"),
                                   classFieldDtor_));

    GBL_TEST_CALL(GblClass_destroyFloating(GBL_CLASS(pFixture->pBoxClass)));

    GBL_TEST_COMPARE(pFixture->fieldDtorsCalled, 3);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(classCreateFloatingInvalid,
                  classConstructFloatingInvalid,
                  classCreateDestroyFloating,
                  classConstructDestructFloating,
                  classAddField,
                  classTakeField,
                  classReplaceField,
                  classClearField,
                  classDestructField)
