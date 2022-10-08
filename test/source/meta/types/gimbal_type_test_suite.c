#include "meta/types/gimbal_type_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/meta/types/gimbal_type.h>
#include <gimbal/strings/gimbal_quark.h>
#include <gimbal/meta/classes/gimbal_class.h>
#include <gimbal/meta/ifaces/gimbal_interface.h>

#define GBL_TYPE_TEST_SUITE_(inst)  ((GblTypeTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_TYPE_TEST_SUITE_TYPE))

typedef struct GblTypeTestSuite_ {
    GblType blankType;
    GblType derivable;
    GblType derived;
    GblType deepDerivable;
    GblType middleDerived;
    GblType deepDerived;
    GblType classed;
    GblType instanced;
    GblType ifaced;
    GblType ifaced2;
    GblType ifaced3;
    GblType classedDerived;
    GblType ifacedDerived;
    GblType ifacedDerived2;
    GblType classedDeepDerivedIfaceMap;
    GblType classedDeepDerivedIfaceMapInherited;
    GblType ifacedIfaceMap;
    GblType ifacedIfaceMapInherited;
    GblType classedMapIfacedIfaceMapInherited;
    GblType dependent;
    GblType dependentDependent;
    GblType dependentDependentDerived;
} GblTypeTestSuite_;


static GBL_RESULT GblTypeTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);
    memset(pSelf_, 0, sizeof(GblTypeTestSuite_));
    //GBL_CTX_CALL(GblType_final());
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);

    GblType_unregister(pSelf_->blankType);
    GblType_unregister(pSelf_->derivable);
    GblType_unregister(pSelf_->derived);
    GblType_unregister(pSelf_->deepDerivable);
    GblType_unregister(pSelf_->middleDerived);
    GblType_unregister(pSelf_->deepDerived);
    GblType_unregister(pSelf_->classed);
    GblType_unregister(pSelf_->instanced);
    GblType_unregister(pSelf_->ifaced);
    GblType_unregister(pSelf_->ifaced2);
    GblType_unregister(pSelf_->ifaced3);
    GblType_unregister(pSelf_->classedDerived);
    GblType_unregister(pSelf_->ifacedDerived);
    GblType_unregister(pSelf_->ifacedDerived2);
    GblType_unregister(pSelf_->classedDeepDerivedIfaceMap);
    GblType_unregister(pSelf_->classedDeepDerivedIfaceMapInherited);
    GblType_unregister(pSelf_->ifacedIfaceMap);
    GblType_unregister(pSelf_->ifacedIfaceMapInherited);
    GblType_unregister(pSelf_->classedMapIfacedIfaceMapInherited);
    GblType_unregister(pSelf_->dependent);
    GblType_unregister(pSelf_->dependentDependent);
    GblType_unregister(pSelf_->dependentDependentDerived);
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_initDefaults_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    //GBL_CTX_CALL(GblType_init(pCtx, 0, 0));
    //GBL_TEST_COMPARE(GblType_contextDefault(), pCtx);
    GBL_TEST_COMPARE(GblType_builtinCount(), GBL_TYPE_BUILTIN_COUNT);
    //GBL_TEST_COMPARE(GblType_registeredCount(), GBL_TYPE_BUILTIN_COUNT);

    GblType prev = GBL_INVALID_TYPE;
    for(GblSize t = 0; t < GBL_TYPE_BUILTIN_COUNT; ++t) {
        GBL_TEST_VERIFY((prev = GblType_nextRegistered(prev))
                                  != GBL_INVALID_TYPE);
    }
    //GBL_TEST_COMPARE(GblType_nextRegistered(prev), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_fromBuiltinIndex(GBL_TYPE_BUILTIN_COUNT+1), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_fromName(NULL), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_fromName("Lolol"), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_fromNameQuark(GBL_QUARK_INVALID), GBL_INVALID_TYPE);
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(strcmp("Invalid", GblType_name(GBL_INVALID_TYPE)), 0);
    GBL_TEST_COMPARE(GblType_nameQuark(GBL_INVALID_TYPE), GBL_QUARK_INVALID);
    GBL_TEST_COMPARE(GblType_parent(GBL_INVALID_TYPE), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_root(GBL_INVALID_TYPE), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_ancestor(GBL_INVALID_TYPE, 2), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_depth(GBL_INVALID_TYPE), 0);
    GBL_TEST_COMPARE(GblType_flagsCheck(GBL_INVALID_TYPE, 0xff), 0);
    GBL_TEST_COMPARE(GblType_verify(GBL_INVALID_TYPE), GBL_FALSE);
    GBL_TEST_VERIFY(GblType_check(GBL_INVALID_TYPE, GBL_INVALID_TYPE));
    GBL_TEST_VERIFY(!GblType_derives(GBL_INVALID_TYPE, GBL_INVALID_TYPE));
    GBL_TEST_VERIFY(!GblType_maps(GBL_INVALID_TYPE, GBL_INVALID_TYPE));
    GBL_TEST_COMPARE(GblType_info(GBL_INVALID_TYPE), NULL);
    GBL_TEST_COMPARE(GblType_plugin(GBL_INVALID_TYPE), NULL);
    GBL_TEST_COMPARE(GblType_classRefCount(GBL_INVALID_TYPE), 0);
    GBL_TEST_COMPARE(GblType_instanceRefCount(GBL_INVALID_TYPE),0);
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_blank_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);

     pSelf_->blankType = GblType_registerStatic(GblQuark_internStringStatic("Blank"),
                                          GBL_INVALID_TYPE,
                                          NULL,
                                          GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->blankType != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(!GBL_TYPE_DEPENDENT_CHECK(pSelf_->blankType));
    GBL_TEST_VERIFY(!GBL_TYPE_CLASSED_CHECK(pSelf_->blankType));
    GBL_TEST_VERIFY(!GBL_TYPE_INSTANTIABLE_CHECK(pSelf_->blankType));
    GBL_TEST_VERIFY(!GBL_TYPE_DERIVABLE_CHECK(pSelf_->blankType));
    GBL_TEST_VERIFY(!GBL_TYPE_DEEP_DERIVABLE_CHECK(pSelf_->blankType));
    GBL_TEST_VERIFY(!GBL_TYPE_BUILTIN_CHECK(pSelf_->blankType));
    GBL_TEST_VERIFY(!GBL_TYPE_ABSTRACT_CHECK(pSelf_->blankType));
    GBL_TEST_VERIFY(!GBL_TYPE_FINAL_CHECK(pSelf_->blankType));
    GBL_TEST_VERIFY(GBL_TYPE_ROOT_CHECK(pSelf_->blankType));
    GBL_TEST_VERIFY(!GBL_TYPE_INTERFACED_CHECK(pSelf_->blankType));
    GBL_TEST_VERIFY(GblType_verify(pSelf_->blankType));

    //GBL_TEST_COMPARE(GblType_registeredCount(), GBL_TYPE_BUILTIN_COUNT+1);
    GBL_TEST_COMPARE(GblType_fromName("Blank"), pSelf_->blankType);
    GBL_TEST_COMPARE(GblType_fromNameQuark(GblQuark_fromString("Blank")), pSelf_->blankType);
    GBL_TEST_COMPARE(GblType_classRefCount(pSelf_->blankType), 0);
    GBL_TEST_COMPARE(GblType_instanceRefCount(pSelf_->blankType), 0);

    GBL_TEST_COMPARE(GblType_name(pSelf_->blankType), "Blank");
    GBL_TEST_COMPARE(GblType_nameQuark(pSelf_->blankType), GblQuark_fromStringStatic("Blank"));
    GBL_TEST_COMPARE(GblType_parent(pSelf_->blankType), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_root(pSelf_->blankType), pSelf_->blankType);
    GBL_TEST_COMPARE(GblType_base(pSelf_->blankType, 0), pSelf_->blankType);
    GBL_TEST_COMPARE(GblType_base(pSelf_->blankType, 1), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_ancestor(pSelf_->blankType, 0), pSelf_->blankType);
    GBL_TEST_COMPARE(GblType_ancestor(pSelf_->blankType, 1), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_depth(pSelf_->blankType), 0);
    GBL_TEST_VERIFY(!GblType_flagsCheck(pSelf_->blankType, 0xffff));
    GBL_TEST_VERIFY(GblType_check(pSelf_->blankType, pSelf_->blankType));
    GBL_TEST_VERIFY(!GblType_check(pSelf_->blankType, GBL_INVALID_TYPE));
    GBL_TEST_VERIFY(!GblType_derives(pSelf_->blankType, pSelf_->blankType));
    GBL_TEST_VERIFY(!GblType_derives(pSelf_->blankType, GBL_INVALID_TYPE));
    GBL_TEST_VERIFY(!GblType_maps(pSelf_->blankType, pSelf_->blankType));
    GBL_TEST_COMPARE(GblType_plugin(pSelf_->blankType), NULL);
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_duplicate_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType dupe = GblType_registerStatic(GblQuark_internStringStatic("Blank"),
                                          GBL_INVALID_TYPE,
                                          NULL,
                                          GBL_TYPE_FLAGS_NONE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(dupe, GBL_INVALID_TYPE);
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_derived_register_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    GblType invalid = GblType_registerStatic("DerivedInvalid",
                                             pSelf_->blankType,
                                             NULL,
                                             0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(invalid, GBL_INVALID_TYPE);
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_derived_register_valid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    pSelf_->derivable = GblType_registerStatic("Derivable",
                                               GBL_INVALID_TYPE,
                                               NULL,
                                               GBL_TYPE_ROOT_FLAG_DERIVABLE);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->derivable != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_DERIVABLE_CHECK(pSelf_->derivable));
    GBL_TEST_VERIFY(!GBL_TYPE_DEEP_DERIVABLE_CHECK(pSelf_->derivable));

    pSelf_->derived = GblType_registerStatic("Derived",
                                            pSelf_->derivable,
                                             NULL,
                                             0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->derived != GBL_INVALID_TYPE);
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_deep_derived_register_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    GblType deepDerivedInvalid = GblType_registerStatic("DeepDerivedInvalid",
                                                        pSelf_->derived,
                                                        NULL,
                                                        0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(deepDerivedInvalid, GBL_INVALID_TYPE);
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_deep_derived_register_valid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    pSelf_->deepDerivable = GblType_registerStatic("DeepDerivable",
                                               GBL_INVALID_TYPE,
                                               NULL,
                                               GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->deepDerivable != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_DERIVABLE_CHECK(pSelf_->deepDerivable));
    GBL_TEST_VERIFY(GBL_TYPE_DEEP_DERIVABLE_CHECK(pSelf_->deepDerivable));
    GBL_TEST_VERIFY(GBL_TYPE_ROOT_CHECK(pSelf_->deepDerivable));
    GBL_TEST_COMPARE(GblType_parent(pSelf_->deepDerivable), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GblType_depth(pSelf_->deepDerivable), 0);
    GBL_TEST_COMPARE(GblType_base(pSelf_->deepDerivable, 0), pSelf_->deepDerivable);
    GBL_TEST_COMPARE(GblType_ancestor(pSelf_->deepDerivable, 0), pSelf_->deepDerivable);
    GBL_TEST_VERIFY(GblType_flagsCheck(pSelf_->deepDerivable,
                                  GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE));
    GBL_TEST_VERIFY(GblType_verify(pSelf_->derivable));
    pSelf_->middleDerived = GblType_registerStatic("MiddleDerived",
                                                   pSelf_->deepDerivable,
                                                   NULL,
                                                   0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->middleDerived != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(!GBL_TYPE_ROOT_CHECK(pSelf_->middleDerived));
    GBL_TEST_COMPARE(GblType_depth(pSelf_->middleDerived), 1);
    GBL_TEST_COMPARE(GblType_ancestor(pSelf_->middleDerived, 0),pSelf_->middleDerived);
    GBL_TEST_COMPARE(GblType_base(pSelf_->middleDerived, 1), pSelf_->middleDerived);
    GBL_TEST_VERIFY(GblType_derives(pSelf_->middleDerived, pSelf_->deepDerivable));
    GBL_TEST_VERIFY(GblType_check(pSelf_->middleDerived, pSelf_->deepDerivable));

    pSelf_->deepDerived = GblType_registerStatic("DeepDerived",
                                             pSelf_->middleDerived,
                                             NULL,
                                             0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->deepDerived != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(!GBL_TYPE_ROOT_CHECK(pSelf_->deepDerived));
    GBL_TEST_COMPARE(GblType_depth(pSelf_->deepDerived), 2);
    GBL_TEST_COMPARE(GblType_ancestor(pSelf_->deepDerived, 0), pSelf_->deepDerived);
    GBL_TEST_COMPARE(GblType_base(pSelf_->deepDerived, 1), pSelf_->middleDerived);
    GBL_TEST_VERIFY(GblType_derives(pSelf_->deepDerived, pSelf_->deepDerivable));
    GBL_TEST_VERIFY(GblType_check(pSelf_->deepDerived, pSelf_->deepDerivable));
    GBL_CTX_END();
}


static GBL_RESULT GblTypeTestSuite_fundamental_classed_register_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType invalidClassed = GblType_registerStatic("InvalidClassed",
                                                    GBL_INVALID_TYPE,
                                                    NULL,
                                                    GBL_TYPE_ROOT_FLAG_CLASSED);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(invalidClassed, GBL_INVALID_TYPE);
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_classed_register_valid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    pSelf_->classed = GblType_registerStatic("Classed",
                                             GBL_INVALID_TYPE,
                                             &(const GblTypeInfo) {
                                                 .classSize = sizeof(GblClass)
                                             },
                                             GBL_TYPE_ROOT_FLAG_CLASSED |
                                             GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->classed != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->classed));
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_classed_inherit_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    pSelf_->classedDerived = GblType_registerStatic("ClassedDerived",
                                                    pSelf_->classed,
                                                    &(const GblTypeInfo) {
                                                        .classSize = sizeof(GblClass)
                                                    },
                                                    0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->classedDerived != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->classedDerived));
    GBL_TEST_VERIFY(GblType_derives(pSelf_->classedDerived, pSelf_->classed));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDerived, pSelf_->classed));
    GBL_TEST_VERIFY(!GblType_maps(pSelf_->classedDerived, pSelf_->classed));
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_interfaced_register_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType invalidIFace = GblType_registerStatic("InvalidIFaced",
                                                  GBL_INVALID_TYPE,
                                                  NULL,
                                                  GBL_TYPE_ROOT_FLAG_INTERFACED);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(invalidIFace, GBL_INVALID_TYPE);
    invalidIFace = GblType_registerStatic("InvalidIFaced",
                                          GBL_INVALID_TYPE,
                                          &(const GblTypeInfo) {
                                              .classSize = sizeof(GblInterface)
                                          },
                                          GBL_TYPE_ROOT_FLAG_INTERFACED |
                                          GBL_TYPE_ROOT_FLAG_INSTANTIABLE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(invalidIFace, GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(!GBL_TYPE_INTERFACED_CHECK(invalidIFace));
    GBL_CTX_END();
}
static GBL_RESULT GblTypeTestSuite_fundamental_interfaced_register_valid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    pSelf_->ifaced = GblType_registerStatic("IFaced",
                                            GBL_INVALID_TYPE,
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblInterface)
                                            },
                                            GBL_TYPE_ROOT_FLAG_INTERFACED |
                                            GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->ifaced != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->ifaced));
    GBL_TEST_VERIFY(GBL_TYPE_INTERFACED_CHECK(pSelf_->ifaced));
    pSelf_->ifaced2 = GblType_registerStatic("IFaced2",
                                            GBL_INVALID_TYPE,
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblInterface)
                                            },
                                            GBL_TYPE_ROOT_FLAG_INTERFACED |
                                            GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->ifaced2 != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->ifaced2));
    GBL_TEST_VERIFY(GBL_TYPE_INTERFACED_CHECK(pSelf_->ifaced2));
    pSelf_->ifaced3 = GblType_registerStatic("IFaced3",
                                             GBL_INVALID_TYPE,
                                             &(const GblTypeInfo) {
                                                 .classSize = sizeof(GblInterface)
                                             },
                                             GBL_TYPE_ROOT_FLAG_INTERFACED |
                                             GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->ifaced2 != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->ifaced3));
    GBL_TEST_VERIFY(GBL_TYPE_INTERFACED_CHECK(pSelf_->ifaced3));
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_interfaced_inherit_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    pSelf_->ifacedDerived = GblType_registerStatic("IFacedDerived",
                                                   pSelf_->ifaced,
                                                   &(const GblTypeInfo) {
                                                       .classSize = sizeof(GblInterface)
                                                   },
                                                   0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->ifaced != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->ifaced));
    GBL_TEST_VERIFY(GBL_TYPE_INTERFACED_CHECK(pSelf_->ifaced));
    GBL_TEST_COMPARE(GblType_parent(pSelf_->ifacedDerived), pSelf_->ifaced);
    GBL_TEST_VERIFY(GblType_derives(pSelf_->ifacedDerived, pSelf_->ifaced));
    GBL_TEST_COMPARE(GblType_ancestor(pSelf_->ifacedDerived, 1), pSelf_->ifaced);
    GBL_TEST_VERIFY(!GblType_maps(pSelf_->ifacedDerived, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_verify(pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_check(pSelf_->ifacedDerived, pSelf_->ifaced));
    pSelf_->ifacedDerived2 = GblType_registerStatic("IFacedDerived2",
                                            pSelf_->ifaced2,
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblInterface)
                                            },
                                            0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->ifacedDerived2 != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->ifacedDerived2));
    GBL_TEST_VERIFY(GBL_TYPE_INTERFACED_CHECK(pSelf_->ifacedDerived2));

    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_classed_mapped_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();

    GblTypeInterfaceMapEntry ifaceMap[] = {
        { GBL_INVALID_TYPE, 0 },
        { GBL_INVALID_TYPE, 0}
    };

    GblTypeInfo info = {
        .classSize = sizeof(GblClass),
        .interfaceCount = 0,
        .pInterfaceMap = ifaceMap
    };

    // Non-null map with 0 interfaces
    GblType bullshitMapped = GblType_registerStatic("ClassedDeepDerivedIfaceMapped",
                                                    pSelf_->classedDerived,
                                                    &info,
                                                    0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(bullshitMapped, GBL_INVALID_TYPE);
    // Invalid type
    info.interfaceCount = 1;
    bullshitMapped = GblType_registerStatic("ClassedDeepDerivedIfaceMapped",
                                            pSelf_->classedDerived,
                                            &info,
                                            0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // Non-Inteface Type
    ifaceMap[0].interfaceType = pSelf_->classedDerived;
    bullshitMapped = GblType_registerStatic("ClassDeepDerivedIfaceMapped",
                                            pSelf_->classedDerived,
                                            &info,
                                            0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // Invalid class size
    ifaceMap[0].interfaceType = pSelf_->ifaced;
    bullshitMapped = GblType_registerStatic("ClassDeepDerivedIfaceMapped",
                                            pSelf_->classedDerived,
                                            &info,
                                            0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(bullshitMapped, GBL_INVALID_TYPE);


    // Invalid offset (in base)
    info.classSize = sizeof(GblClass) + sizeof(GblInterface);
    bullshitMapped = GblType_registerStatic("ClassDeepDerivedIfaceMapped",
                                            pSelf_->classedDerived,
                                            &info,
                                            0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // Offset too far
    ifaceMap[0].classOffset = sizeof(GblInterface);
    bullshitMapped = GblType_registerStatic("ClassDeepDerivedIfaceMapped",
                                            pSelf_->classedDerived,
                                            &info,
                                            0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // duplicated type
    info.classSize = sizeof(GblClass) + 2*sizeof(GblInterface) + 2;
    info.interfaceCount = 2;
    ifaceMap[0].classOffset = sizeof(GblClass);
    ifaceMap[1].interfaceType = pSelf_->ifaced;
    ifaceMap[1].classOffset = sizeof(GblClass) + sizeof(GblInterface);
    bullshitMapped = GblType_registerStatic("ClassDeepDerivedIfaceMapped",
                                            pSelf_->classedDerived,
                                            &info,
                                            0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // ambiguous type
    ifaceMap[1].interfaceType = pSelf_->ifacedDerived;
    bullshitMapped = GblType_registerStatic("ClassDeepDerivedIfaceMapped",
                                            pSelf_->classedDerived,
                                            &info,
                                            0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // overlapping interfaces
    ifaceMap[1].interfaceType = pSelf_->ifaced2;
    ifaceMap[1].classOffset -= 3;
    bullshitMapped = GblType_registerStatic("ClassDeepDerivedIfaceMapped",
                                            pSelf_->classedDerived,
                                            &info,
                                            0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(bullshitMapped, GBL_INVALID_TYPE);
    GBL_CTX_END();
}



static GBL_RESULT GblTypeTestSuite_fundamental_classed_mapped_valid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    pSelf_->classedDeepDerivedIfaceMap = GblType_registerStatic("ClassedDeepDerivedIfaceMapped",
                                            pSelf_->classedDerived,
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblClass) + sizeof(GblInterface)*2,
                                                .interfaceCount = 2,
                                                .pInterfaceMap = (const GblTypeInterfaceMapEntry[]) {
                                                     {
                                                         .interfaceType = pSelf_->ifacedDerived,
                                                         .classOffset = sizeof(GblClass)
                                                     },
                                                    {
                                                        .interfaceType = pSelf_->ifaced3,
                                                        .classOffset = sizeof(GblClass) + sizeof(GblInterface)
                                                    }
                                                 }
                                            },
                                            0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->classedDeepDerivedIfaceMap != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->classedDeepDerivedIfaceMap));
    GBL_TEST_VERIFY(GblType_derives(pSelf_->classedDeepDerivedIfaceMap, pSelf_->classedDerived));
    GBL_TEST_VERIFY(GblType_derives(pSelf_->classedDeepDerivedIfaceMap, pSelf_->classed));
    GBL_TEST_VERIFY(!GblType_derives(pSelf_->classedDeepDerivedIfaceMap, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDeepDerivedIfaceMap, pSelf_->classed));
    GBL_TEST_VERIFY(!GblType_maps(pSelf_->classedDeepDerivedIfaceMap, pSelf_->classed));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedDeepDerivedIfaceMap, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedDeepDerivedIfaceMap, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDeepDerivedIfaceMap, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDeepDerivedIfaceMap, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedDeepDerivedIfaceMap, pSelf_->ifaced3));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDeepDerivedIfaceMap, pSelf_->ifaced3));
    GBL_CTX_END();
}


static GBL_RESULT GblTypeTestSuite_fundamental_classed_mapped_inherit_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    pSelf_->classedDeepDerivedIfaceMapInherited  = GblType_registerStatic("ClassedDeepDerivedIfaceMappedInherited",
                                                                    pSelf_->classedDeepDerivedIfaceMap,
                                                                   &(const GblTypeInfo) {
                                                                       .classSize = sizeof(GblClass) + sizeof(GblInterface) * 3,
                                                                       .interfaceCount = 1,
                                                                       .pInterfaceMap = (const GblTypeInterfaceMapEntry[]) {
                                                                            {
                                                                                .interfaceType = pSelf_->ifacedDerived2,
                                                                                .classOffset = sizeof(GblClass) + sizeof(GblInterface) * 2
                                                                            },
                                                                        }
                                                                   },
                                                                   0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->classedDeepDerivedIfaceMapInherited != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->classedDeepDerivedIfaceMapInherited));
    GBL_TEST_VERIFY(GblType_derives(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->classedDeepDerivedIfaceMap));
    GBL_TEST_VERIFY(GblType_derives(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->classedDerived));
    GBL_TEST_VERIFY(GblType_derives(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->classed));
    GBL_TEST_VERIFY(!GblType_derives(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(!GblType_derives(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifacedDerived2));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->classed));
    GBL_TEST_VERIFY(!GblType_maps(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->classed));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifaced3));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifaced3));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifacedDerived2));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifacedDerived2));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifaced2));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->ifaced2));
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_interfaced_mapped_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    pSelf_->ifacedIfaceMap = GblType_registerStatic("IFaceMappedIFace",
                                            GBL_INVALID_TYPE,
                                             &(const GblTypeInfo) {
                                                 .classSize = sizeof(GblInterface)*3,
                                                 .interfaceCount = 2,
                                                 .pInterfaceMap = (const GblTypeInterfaceMapEntry[]) {
                                                      {
                                                          .interfaceType = pSelf_->ifacedDerived,
                                                          .classOffset = sizeof(GblInterface)
                                                      },
                                                     {
                                                         .interfaceType = pSelf_->ifaced3,
                                                         .classOffset = sizeof(GblInterface)*2
                                                     }
                                                  }
                                             },
                                             GBL_TYPE_ROOT_FLAG_INTERFACED |
                                             GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->ifacedIfaceMap!= GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->ifacedIfaceMap));
    GBL_TEST_VERIFY(GBL_TYPE_INTERFACED_CHECK(pSelf_->ifacedIfaceMap));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->ifacedIfaceMap,  pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->ifacedIfaceMap,  pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_check(pSelf_->ifacedIfaceMap, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_check(pSelf_->ifacedIfaceMap, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->ifacedIfaceMap,  pSelf_->ifaced3));
    GBL_TEST_VERIFY(GblType_check(pSelf_->ifacedIfaceMap, pSelf_->ifaced3));
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_interfaced_mapped_inherit_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    GblTypeInterfaceMapEntry entries[] = {
        { pSelf_->ifacedIfaceMap, sizeof(GblInterface)*3 }
    };

    GblTypeInfo info = {
        .classSize = sizeof(GblInterface)*10,
        .interfaceCount = 1,
        .pInterfaceMap = entries
    };

    GblType invalidType = GblType_registerStatic("IfaceMappedIFaceInherited",
                                                pSelf_->ifacedIfaceMap,
                                                  &info,
                                                  0);
    // Ambiguous type (inherited base is mapped)
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(invalidType, GBL_INVALID_TYPE);

    // Ambiguous type (inherited base common with mapped base)
    invalidType = GblType_registerStatic("IfaceMappedIFaceInherited1",
                                        pSelf_->ifacedDerived,
                                         &info,
                                         0);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(invalidType, GBL_INVALID_TYPE);
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_interfaced_mapped_inherit_valid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);

    pSelf_->ifacedIfaceMapInherited = GblType_registerStatic("IfaceMappedIFaceInherited",
                                                      pSelf_->ifacedIfaceMap,
                                                      &(const GblTypeInfo) {
                                                          .classSize = sizeof(GblInterface)*4,
                                                          .interfaceCount = 1,
                                                          .pInterfaceMap = (const GblTypeInterfaceMapEntry[]) {
                                                               {
                                                                   .interfaceType = pSelf_->ifacedDerived2,
                                                                   .classOffset = sizeof(GblInterface)*3
                                                               },
                                                           }
                                                      },
                                                      0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->ifacedIfaceMapInherited != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_INTERFACED_CHECK(pSelf_->ifacedIfaceMapInherited));
    GBL_TEST_VERIFY(GblType_derives(pSelf_->ifacedIfaceMapInherited, pSelf_->ifacedIfaceMap));
    GBL_TEST_VERIFY(GblType_check(pSelf_->ifacedIfaceMapInherited, pSelf_->ifacedIfaceMap));
    GBL_TEST_VERIFY(!GblType_maps(pSelf_->ifacedIfaceMapInherited, pSelf_->ifacedIfaceMap));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->ifacedIfaceMapInherited, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->ifacedIfaceMapInherited, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_check(pSelf_->ifacedIfaceMapInherited, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_check(pSelf_->ifacedIfaceMapInherited, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->ifacedIfaceMapInherited, pSelf_->ifaced3));
    GBL_TEST_VERIFY(GblType_check(pSelf_->ifacedIfaceMapInherited, pSelf_->ifaced3));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->ifacedIfaceMapInherited, pSelf_->ifacedDerived2));
    GBL_TEST_VERIFY(GblType_check(pSelf_->ifacedIfaceMapInherited, pSelf_->ifacedDerived2));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->ifacedIfaceMapInherited, pSelf_->ifaced2));
    GBL_TEST_VERIFY(GblType_check(pSelf_->ifacedIfaceMapInherited, pSelf_->ifaced2));
    GBL_CTX_END();
}


static GBL_RESULT GblTypeTestSuite_fundamental_classed_mapped_interfaced_mapped_inherit_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);

    pSelf_->classedMapIfacedIfaceMapInherited = GblType_registerStatic("ClassedMapIfaceMappedIFaceInherited",
                                                                GBL_INVALID_TYPE,
                                                                &(const GblTypeInfo) {
                                                                    .classSize = sizeof(GblClass) + sizeof(GblInterface) * 4,
                                                                    .interfaceCount = 1,
                                                                    .pInterfaceMap = (const GblTypeInterfaceMapEntry[]) {
                                                                         {
                                                                             .interfaceType = pSelf_->ifacedIfaceMapInherited,
                                                                             .classOffset = sizeof(GblClass)
                                                                         },
                                                                     }
                                                                },
                                                                GBL_TYPE_ROOT_FLAG_CLASSED);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->classedMapIfacedIfaceMapInherited != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->classedMapIfacedIfaceMapInherited));
    GBL_TEST_VERIFY(!GblType_derives(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifacedIfaceMap));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifacedIfaceMap));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifacedIfaceMap));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifacedDerived));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifaced3));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifaced3));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifacedDerived2));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifacedDerived2));
    GBL_TEST_VERIFY(GblType_maps(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifaced2));
    GBL_TEST_VERIFY(GblType_check(pSelf_->classedMapIfacedIfaceMapInherited, pSelf_->ifaced2));
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_instantiable_register_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType invalid = GblType_registerStatic("Ininstanced",
                                                GBL_INVALID_TYPE,
                                                     &(const GblTypeInfo) {
                                                         .instanceSize = sizeof(GblInstance)
                                                     },
                                                     GBL_TYPE_ROOT_FLAG_INSTANTIABLE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(invalid, GBL_INVALID_TYPE);

    invalid = GblType_registerStatic("Ininstanced",
                                    GBL_INVALID_TYPE,
                                     &(const GblTypeInfo) {
                                         .classSize = sizeof(GblInterface),
                                         .instanceSize = sizeof(GblInstance)
                                     },
                                     GBL_TYPE_ROOT_FLAG_INSTANTIABLE |
                                     GBL_TYPE_ROOT_FLAG_CLASSED   |
                                     GBL_TYPE_ROOT_FLAG_INTERFACED);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(invalid, GBL_INVALID_TYPE);
    GBL_CTX_END();
}
static GBL_RESULT GblTypeTestSuite_fundamental_instantiable_register_valid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    pSelf_->instanced = GblType_registerStatic("Instanced",
                                        GBL_INVALID_TYPE,
                                         &(const GblTypeInfo) {
                                             .instanceSize = sizeof(GblInstance),
                                             .classSize = sizeof(GblClass)
                                         },
                                         GBL_TYPE_ROOT_FLAG_INSTANTIABLE);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblType_instanceRefCount(pSelf_->instanced), 0);
    GBL_TEST_COMPARE(GblType_classRefCount(pSelf_->instanced), 0);
    GBL_TEST_VERIFY(pSelf_->instanced != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_CLASSED_CHECK(pSelf_->instanced));
    GBL_TEST_VERIFY(GBL_TYPE_INSTANTIABLE_CHECK(pSelf_->instanced));
    GBL_CTX_END();
}


static GBL_RESULT GblTypeTestSuite_fundamental_dependent_register_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();

    // Multiple primary/non-dependent dependencies:
    pSelf_->dependent = GblType_registerStatic("Dependent",
                                                   GBL_INVALID_TYPE,
                                                    &(const GblTypeInfo) {
                                                        .dependencyCount = 2,
                                                        .pDependencies = (const GblType[]) {
                                                            pSelf_->instanced,
                                                            pSelf_->classedDerived
                                                        }
                                                    }, GBL_TYPE_ROOT_FLAG_DEPENDENT);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_TYPE);
    GBL_TEST_COMPARE(pSelf_->dependent, GBL_INVALID_TYPE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_dependent_register_valid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);

    // Dependencies on non-dependent types
    pSelf_->dependent = GblType_registerStatic("ClassedDependent",
                                               GBL_INVALID_TYPE,
                                               &(const GblTypeInfo) {
                                                   .dependencyCount = 1,
                                                   .pDependencies = (const GblType[]) {
                                                       pSelf_->classed
                                                   }
                                               },
                                               0);

    GBL_TEST_VERIFY(pSelf_->dependent != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_DEPENDENT_CHECK(pSelf_->dependent));
    GBL_TEST_VERIFY(GblType_depends(pSelf_->dependent, pSelf_->classed));
    GBL_TEST_VERIFY(GblType_conforms(pSelf_->classed, pSelf_->dependent));
    GBL_CTX_END();
}

static GBL_RESULT GblTypeTestSuite_fundamental_dependent_depends_dependent_valid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);

    pSelf_->dependentDependent = GblType_registerStatic("ClassedIFacedDependent",
                                          GBL_INVALID_TYPE,
                                          &(const GblTypeInfo) {
                                              .dependencyCount = 2,
                                              .pDependencies = (const GblType[]) {
                                                  pSelf_->dependent,
                                                  pSelf_->ifaced
                                              }
                                          },
                                          GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE);
    GBL_TEST_VERIFY(pSelf_->dependentDependent != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_DEPENDENT_CHECK(pSelf_->dependentDependent));
    GBL_TEST_VERIFY(GblType_depends(pSelf_->dependentDependent, pSelf_->classed));
    GBL_TEST_VERIFY(GblType_depends(pSelf_->dependentDependent, pSelf_->ifaced));
    GBL_TEST_VERIFY(!GblType_conforms(pSelf_->blankType, pSelf_->dependentDependent));
    GBL_TEST_VERIFY(!GblType_conforms(pSelf_->classed, pSelf_->dependentDependent));
    GBL_TEST_VERIFY(!GblType_conforms(pSelf_->ifaced, pSelf_->dependentDependent));
    GBL_TEST_VERIFY(GblType_conforms(pSelf_->classedDeepDerivedIfaceMap, pSelf_->dependentDependent));

    GBL_CTX_END();
}
static GBL_RESULT GblTypeTestSuite_fundamental_dependent_derive_depends_dependent_valid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblTypeTestSuite_* pSelf_ = GBL_TYPE_TEST_SUITE_(pSelf);

    pSelf_->dependentDependentDerived = GblType_registerStatic("ClassedIFacedDependentDerived",
                                          pSelf_->dependentDependent,
                                          &(const GblTypeInfo) {
                                              .dependencyCount = 2,
                                              .pDependencies = (const GblType[]) {
                                                  pSelf_->classedDerived,
                                                  pSelf_->ifaced2
                                              }
                                          },
                                          GBL_TYPE_FLAGS_NONE);

    GBL_TEST_VERIFY(pSelf_->dependentDependentDerived != GBL_INVALID_TYPE);
    GBL_TEST_VERIFY(GBL_TYPE_DEPENDENT_CHECK(pSelf_->dependentDependentDerived));
    GBL_TEST_VERIFY(GblType_depends(pSelf_->dependentDependentDerived, pSelf_->classed));
    GBL_TEST_VERIFY(GblType_depends(pSelf_->dependentDependentDerived, pSelf_->ifaced));
    GBL_TEST_VERIFY(GblType_depends(pSelf_->dependentDependentDerived, pSelf_->ifaced2));
    GBL_TEST_VERIFY(GblType_depends(pSelf_->dependentDependentDerived, pSelf_->classedDerived));
    GBL_TEST_VERIFY(!GblType_conforms(pSelf_->blankType, pSelf_->dependentDependentDerived));
    GBL_TEST_VERIFY(!GblType_conforms(pSelf_->classed, pSelf_->dependentDependentDerived));
    GBL_TEST_VERIFY(!GblType_conforms(pSelf_->ifaced, pSelf_->dependentDependentDerived));
    GBL_TEST_VERIFY(!GblType_conforms(pSelf_->ifaced2, pSelf_->dependentDependentDerived));
    GBL_TEST_VERIFY(!GblType_conforms(pSelf_->classedDerived, pSelf_->dependentDependentDerived));
    GBL_TEST_VERIFY(!GblType_conforms(pSelf_->classedDeepDerivedIfaceMap, pSelf_->dependentDependentDerived));
    GBL_TEST_VERIFY(GblType_conforms(pSelf_->classedDeepDerivedIfaceMapInherited, pSelf_->dependentDependentDerived));

    GBL_CTX_END();
}


GBL_EXPORT GblType GblTypeTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "initDefaults",                           GblTypeTestSuite_initDefaults_                                          },
        { "invalid",                                GblTypeTestSuite_invalid_                                               },
        { "blank",                                  GblTypeTestSuite_fundamental_blank_                                     },
        { "duplicate",                              GblTypeTestSuite_fundamental_duplicate_                                 },
        { "derivedInvalid",                         GblTypeTestSuite_fundamental_derived_register_invalid_                  },
        { "derived",                                GblTypeTestSuite_fundamental_derived_register_valid_                    },
        { "deepDerivedInvalid",                     GblTypeTestSuite_fundamental_deep_derived_register_invalid_             },
        { "deepDerived",                            GblTypeTestSuite_fundamental_deep_derived_register_valid_               },
        { "classedInvalid",                         GblTypeTestSuite_fundamental_classed_register_invalid_                  },
        { "classed",                                GblTypeTestSuite_fundamental_classed_register_valid_                    },
        { "classedInherit",                         GblTypeTestSuite_fundamental_classed_inherit_                           },
        { "interfacedInvalid",                      GblTypeTestSuite_fundamental_interfaced_register_invalid_               },
        { "interfaced",                             GblTypeTestSuite_fundamental_interfaced_register_valid_                 },
        { "interfacedInherit",                      GblTypeTestSuite_fundamental_interfaced_inherit_                        },
        { "classedMappedInvalid",                   GblTypeTestSuite_fundamental_classed_mapped_invalid_                    },
        { "classedMapped",                          GblTypeTestSuite_fundamental_classed_mapped_valid_                      },
        { "classedMappedInherit",                   GblTypeTestSuite_fundamental_classed_mapped_inherit_                    },
        { "interfacedMapped",                       GblTypeTestSuite_fundamental_interfaced_mapped_                         },
        { "interfacedMappedInheritInvalid",         GblTypeTestSuite_fundamental_interfaced_mapped_inherit_invalid_         },
        { "interfacedMappedInherit",                GblTypeTestSuite_fundamental_interfaced_mapped_inherit_valid_           },
        { "classedMappedInterfacedMappedInherit",   GblTypeTestSuite_fundamental_classed_mapped_interfaced_mapped_inherit_  },
        { "instantiableInvalid",                    GblTypeTestSuite_fundamental_instantiable_register_invalid_             },
        { "instantiableValid",                      GblTypeTestSuite_fundamental_instantiable_register_valid_               },
        { "dependentInvalid",                       GblTypeTestSuite_fundamental_dependent_register_invalid_                },
        { "dependent",                              GblTypeTestSuite_fundamental_dependent_register_valid_                  },
        { "dependentDependsDependent",              GblTypeTestSuite_fundamental_dependent_depends_dependent_valid_         },
        { "dependentDeriveDependsDependent",        GblTypeTestSuite_fundamental_dependent_derive_depends_dependent_valid_  },
        { NULL,                                     NULL                                                                    }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblTypeTestSuite_init_,
        .pFnSuiteFinal  = GblTypeTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("TypeTestSuite"),
                                     &vTable,
                                     sizeof(GblTypeTestSuite),
                                     sizeof(GblTypeTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}











