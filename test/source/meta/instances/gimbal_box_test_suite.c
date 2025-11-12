#include "meta/instances/gimbal_box_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_ref.h>
#include <gimbal/meta/signals/gimbal_closure.h>

#define GBL_SELF_TYPE GblBoxTestSuite

GBL_TEST_FIXTURE {
    size_t       refCount;
    size_t       instanceCount;
    size_t       classRefCount;
    GblQuark     testQuark;
    GblBool      classUserDtorCalled;
    GblBool      userDtorCalled;
    size_t       fieldDtorsCalled;
    GblBoxClass* pBoxClass;
    GblBox*      pBox;
    GblVariant*  pVariant;
    unsigned     finalizeSignalCounter1;
    unsigned     finalizeSignalCounter2;
};

GBL_TEST_INIT()
    pFixture->testQuark     = GblQuark_fromStatic("test");
    pFixture->refCount      = GblRef_activeCount();
    pFixture->instanceCount = GblType_instanceCount(GBL_BOX_TYPE);
    pFixture->classRefCount = GblType_classRefCount(GBL_BOX_TYPE);
    pFixture->pVariant      = GBL_CTX_MALLOC(sizeof(GblVariant));
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_CTX_FREE(pFixture->pVariant);
    GBL_TEST_COMPARE(GblType_classRefCount(GBL_BOX_TYPE), pFixture->classRefCount);
    GBL_TEST_COMPARE(GblType_instanceCount(GBL_BOX_TYPE), pFixture->instanceCount);
    GBL_TEST_COMPARE(GblRef_activeCount(),                pFixture->refCount);
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

    pFixture->classUserDtorCalled = GBL_TRUE;

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
    GBL_TEST_VERIFY(pFixture->classUserDtorCalled);
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

    pFixture->classUserDtorCalled = GBL_FALSE;
    GBL_TEST_CALL(GblClass_destructFloating(GBL_CLASS(&klass)));
    GBL_TEST_VERIFY(pFixture->classUserDtorCalled);
GBL_TEST_CASE_END

static GBL_RESULT fieldDtor_(const GblArrayMap* pMap, uintptr_t key, void* pEntry) {
    GBL_CTX_BEGIN(NULL);

    // This is a horrible thing to do, but this is a test, so whatever.
    GblTestFixture* pFixture = (GblTestFixture*)
                               GblArrayMap_getValue((GblArrayMap**)&pMap,
                                                    GblQuark_fromStatic("_ud"));

    GBL_TEST_VERIFY(pFixture && key == pFixture->testQuark);

    ++pFixture->fieldDtorsCalled;
    GblStringRef_unref(pEntry);

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
                                       fieldDtor_));

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
                                       fieldDtor_));

    GBL_TEST_CALL(GblBoxClass_setField(pFixture->pBoxClass,
                                       pFixture->testQuark,
                                       (uintptr_t)GblStringRef_create("Treamcast2"),
                                       fieldDtor_));

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
                                   fieldDtor_));

    GBL_TEST_CALL(GblClass_destroyFloating(GBL_CLASS(pFixture->pBoxClass)));

    GBL_TEST_COMPARE(pFixture->fieldDtorsCalled, 3);
GBL_TEST_CASE_END

GBL_TEST_CASE(createInvalid)
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_VERIFY(!GblBox_create(GBL_INSTANCE_TYPE, sizeof(GblBox)));
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_TYPE_MISMATCH);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_VERIFY(!GblBox_create(GBL_BOX_TYPE, sizeof(GblInstance)));
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(constructInvalid)
    GblBox box;
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblBox_construct(&box, GBL_INSTANCE_TYPE),
                     GBL_RESULT_ERROR_TYPE_MISMATCH);

    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

static GBL_RESULT instanceUserDtor_(const GblArrayMap* pMap, uintptr_t key, void* pEntry) {
    GblBox*         pBox     = pEntry;
    GblTestFixture* pFixture = GblBox_userdata(pBox);

    if(GblBox_isDestructing(pBox))
        pFixture->userDtorCalled = GBL_TRUE;

    return GBL_RESULT_SUCCESS;
}

GBL_TEST_CASE(createDestroy)
    GblBox* pBox = GblBox_create(GBL_BOX_TYPE,
                                 sizeof(GblBox),
                                 pFixture,
                                 instanceUserDtor_);
    GBL_TEST_VERIFY(pBox);
    GBL_TEST_VERIFY(!GblBox_isDestructing(pBox));

    pFixture->userDtorCalled = GBL_FALSE;

    GBL_TEST_COMPARE(GblBox_unref(pBox), 0);
    GBL_TEST_VERIFY(pFixture->userDtorCalled);
GBL_TEST_CASE_END

GBL_TEST_CASE(createDestroyWithFloatingClass)
    GblBox* pBox = GblBox_create(GBL_BOX_TYPE,
                                 sizeof(GblBox),
                                 pFixture,
                                 instanceUserDtor_,
                                 GblBoxClass_createFloating(GBL_BOX_TYPE,
                                                            sizeof(GblBoxClass),
                                                            pFixture,
                                                            classFloatingUserDtor_));
    GBL_TEST_VERIFY(pBox);

    GBL_TEST_CALL(GblInstance_sinkClass(GBL_INSTANCE(pBox)));

    pFixture->userDtorCalled      = GBL_FALSE;
    pFixture->classUserDtorCalled = GBL_FALSE;

    GBL_TEST_COMPARE(GblBox_unref(pBox), 0);
    GBL_TEST_VERIFY(pFixture->userDtorCalled);
    GBL_TEST_VERIFY(pFixture->classUserDtorCalled);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructDestruct)
    GblBox box;

    GBL_TEST_CALL(GblBox_construct(&box,
                                   GBL_BOX_TYPE,
                                   pFixture,
                                   instanceUserDtor_));

    pFixture->userDtorCalled = GBL_FALSE;

    GBL_TEST_COMPARE(GblBox_unref(&box), 0);

    GBL_TEST_VERIFY(pFixture->userDtorCalled);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructDestructWithFloatingClass)
    GblBoxClass boxClass;
    struct {
        GblBox  box;
        char    extra[64];
    } extBox;

    GBL_TEST_CALL(GblBoxClass_constructFloating(&boxClass,
                                                GBL_BOX_TYPE,
                                                pFixture,
                                                classFloatingUserDtor_));

    GBL_TEST_CALL(GblBox_construct(&extBox.box,
                                   GBL_BOX_TYPE,
                                   pFixture,
                                   instanceUserDtor_,
                                   &boxClass));

    pFixture->classUserDtorCalled = GBL_FALSE;
    pFixture->userDtorCalled      = GBL_FALSE;

    GBL_TEST_COMPARE(GblBox_unref(&extBox.box), 0);
    GBL_TEST_VERIFY(!pFixture->classUserDtorCalled);
    GBL_TEST_VERIFY(pFixture->userDtorCalled);

    GBL_TEST_CALL(GblClass_destructFloating(GBL_CLASS(&boxClass)));
    GBL_TEST_VERIFY(pFixture->classUserDtorCalled);
GBL_TEST_CASE_END


GBL_TEST_CASE(addField)
    pFixture->fieldDtorsCalled = 0;

    pFixture->pBox = GblBox_create(GBL_BOX_TYPE,
                                   sizeof(GblBox),
                                   pFixture);

    GBL_TEST_VERIFY(!GblBox_hasField(pFixture->pBox,
                                     pFixture->testQuark));

    GBL_TEST_CALL(GblBox_setField(pFixture->pBox,
                                  pFixture->testQuark,
                                  (uintptr_t)GblStringRef_create("Trololo"),
                                  fieldDtor_));

    GBL_TEST_VERIFY(GblBox_hasField(pFixture->pBox,
                                    pFixture->testQuark));

    GBL_TEST_COMPARE((const char*)GblBox_field(pFixture->pBox,
                                               pFixture->testQuark),
                     "Trololo");
GBL_TEST_CASE_END

GBL_TEST_CASE(takeField)
    GblStringRef* pRef = (GblStringRef*)
        GblBox_takeField(pFixture->pBox,
                         pFixture->testQuark);

    GBL_TEST_COMPARE(pRef, "Trololo");
    GblStringRef_unref(pRef);

    GBL_TEST_VERIFY(!pFixture->fieldDtorsCalled);

    GBL_TEST_VERIFY(!GblBox_hasField(pFixture->pBox,
                                     pFixture->testQuark));

    GBL_TEST_VERIFY(!GblBox_field(pFixture->pBox,
                                  pFixture->testQuark));
GBL_TEST_CASE_END

GBL_TEST_CASE(replaceField)
    GBL_TEST_CALL(GblBox_setField(pFixture->pBox,
                                  pFixture->testQuark,
                                  (uintptr_t)GblStringRef_create("Treamcast"),
                                  fieldDtor_));

    GBL_TEST_CALL(GblBox_setField(pFixture->pBox,
                                  pFixture->testQuark,
                                  (uintptr_t)GblStringRef_create("Treamcast2"),
                                  fieldDtor_));

    GBL_TEST_COMPARE(pFixture->fieldDtorsCalled, 1);

    GBL_TEST_COMPARE((const char*)GblBox_field(pFixture->pBox,
                                               pFixture->testQuark),
                     "Treamcast2");
GBL_TEST_CASE_END

GBL_TEST_CASE(clearField)
    GBL_TEST_VERIFY(GblBox_clearField(pFixture->pBox,
                                      pFixture->testQuark));

    GBL_TEST_COMPARE(pFixture->fieldDtorsCalled, 2);

    GBL_TEST_VERIFY(!GblBox_hasField(pFixture->pBox,
                                     pFixture->testQuark));

    GBL_TEST_VERIFY(!GblBox_field(pFixture->pBox,
                                  pFixture->testQuark));
GBL_TEST_CASE_END

GBL_TEST_CASE(destructField)
    GBL_TEST_CALL(GblBox_setField(pFixture->pBox,
                                  pFixture->testQuark,
                                  (uintptr_t)GblStringRef_create("Screamcast"),
                                  fieldDtor_));

    GBL_TEST_COMPARE(GblBox_unref(pFixture->pBox), 0);

    GBL_TEST_COMPARE(pFixture->fieldDtorsCalled, 3);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantConstructDefault)
    GBL_TEST_CALL(GblVariant_constructDefault(pFixture->pVariant, GBL_BOX_TYPE));
GBL_TEST_CASE_END

GBL_TEST_CASE(variantValuePeek)
    GblBox* pBox = GblVariant_boxPeek(pFixture->pVariant);

    GBL_TEST_VERIFY(pBox);
    GBL_TEST_VERIFY(GBL_TYPECHECK(GblBox, pBox));
    GBL_TEST_COMPARE(GblBox_refCount(pBox), 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantValueCopy)
    GblBox* pBox = GblVariant_boxCopy(pFixture->pVariant);

    GBL_TEST_VERIFY(pBox);
    GBL_TEST_VERIFY(GBL_TYPECHECK(GblBox, pBox));
    GBL_TEST_COMPARE(GblBox_refCount(pBox), 2);

    GblBox_unref(pBox);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantValueMove)
    pFixture->pBox = GblVariant_boxMove(pFixture->pVariant);

    GBL_TEST_VERIFY(pFixture->pBox);
    GBL_TEST_VERIFY(GBL_TYPECHECK(GblBox, pFixture->pBox));
    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 1);
    GBL_TEST_VERIFY(!GblVariant_isValid(pFixture->pVariant));

    GBL_TEST_CALL(GblVariant_destruct(pFixture->pVariant));
GBL_TEST_CASE_END

GBL_TEST_CASE(variantConstructValueCopy)
    GBL_TEST_CALL(GblVariant_constructValueCopy(pFixture->pVariant,
                                                GBL_BOX_TYPE,
                                                pFixture->pBox));
    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 2);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantDestruct)
    GBL_TEST_CALL(GblVariant_destruct(pFixture->pVariant));
    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantConstructValueMove)
    GBL_TEST_CALL(GblVariant_constructValueMove(pFixture->pVariant,
                                                GBL_BOX_TYPE,
                                                pFixture->pBox));
    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantSetValueCopy)
    pFixture->pBox = GblBox_create(GBL_BOX_TYPE);

    GBL_TEST_CALL(GblVariant_setValueCopy(pFixture->pVariant,
                                          GBL_BOX_TYPE,
                                          pFixture->pBox));

    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 2);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantSetValueMove)
    GBL_TEST_CALL(GblVariant_setValueMove(pFixture->pVariant,
                                          GBL_BOX_TYPE,
                                          pFixture->pBox));

    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantConstructCopy)
    GblVariant v;

    GBL_TEST_CALL(GblVariant_constructCopy(&v, pFixture->pVariant));
    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 2);
    GBL_TEST_CALL(GblVariant_destruct(&v));
    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantConstructMove)
    GblVariant v;

    GblBox_ref(pFixture->pBox);
    GBL_TEST_CALL(GblVariant_constructMove(&v, pFixture->pVariant));
    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 2);
    GBL_TEST_CALL(GblVariant_destruct(&v));
    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(variantComparison)
    GblVariant v;

    GBL_TEST_CALL(GblVariant_constructValueCopy(&v, GBL_BOX_TYPE, pFixture->pBox));
    GBL_TEST_CALL(GblVariant_setValueMove(pFixture->pVariant, GBL_BOX_TYPE, pFixture->pBox));
    GBL_TEST_VERIFY(GblVariant_equals(&v, pFixture->pVariant));
    GBL_TEST_CALL(GblVariant_destruct(&v));
    GBL_TEST_COMPARE(GblBox_refCount(pFixture->pBox), 1);
    GBL_TEST_CALL(GblVariant_destruct(pFixture->pVariant));
GBL_TEST_CASE_END

static void onFinalize_(GblBox* pBox) {
    if(GblBox_isFinalizing(pBox))
        (*(unsigned*)GblBox_userdata(pBox))++;
}

GBL_TEST_CASE(finalizeSignal)
    GblBox box1;
    GblBox_construct(&box1, GBL_BOX_TYPE, &pFixture->finalizeSignalCounter1);
    GBL_CONNECT(&box1, "finalize", onFinalize_);
    GBL_TEST_COMPARE(pFixture->finalizeSignalCounter1, 0);

    GblBox* pBox2 = GblBox_create(GBL_BOX_TYPE, sizeof(GblBox), &pFixture->finalizeSignalCounter2);
    GBL_CONNECT(pBox2, "finalize", onFinalize_);
    GBL_TEST_COMPARE(pFixture->finalizeSignalCounter2, 0);

    GBL_REF(&box1);
    GBL_TEST_COMPARE(pFixture->finalizeSignalCounter1, 0);
    GBL_UNREF(&box1);
    GBL_TEST_COMPARE(pFixture->finalizeSignalCounter1, 0);
    GBL_TEST_VERIFY(!GblBox_isFinalizing(&box1));
    GBL_UNREF(&box1);
    GBL_TEST_COMPARE(pFixture->finalizeSignalCounter1, 1);

    GBL_REF(pBox2);
    GBL_TEST_COMPARE(pFixture->finalizeSignalCounter2, 0);
    GBL_UNREF(pBox2);
    GBL_TEST_COMPARE(pFixture->finalizeSignalCounter2, 0);
    GBL_UNREF(pBox2);
    GBL_TEST_COMPARE(pFixture->finalizeSignalCounter2, 1);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(classCreateFloatingInvalid,
                  classConstructFloatingInvalid,
                  classCreateDestroyFloating,
                  classConstructDestructFloating,
                  classAddField,
                  classTakeField,
                  classReplaceField,
                  classClearField,
                  classDestructField,
                  createInvalid,
                  constructInvalid,
                  createDestroy,
                  createDestroyWithFloatingClass,
                  constructDestruct,
                  constructDestructWithFloatingClass,
                  addField,
                  takeField,
                  replaceField,
                  clearField,
                  destructField,
                  variantConstructDefault,
                  variantValuePeek,
                  variantValueCopy,
                  variantValueMove,
                  variantConstructValueCopy,
                  variantDestruct,
                  variantConstructValueMove,
                  variantSetValueCopy,
                  variantSetValueMove,
                  variantConstructCopy,
                  variantConstructMove,
                  variantComparison,
                  finalizeSignal)
