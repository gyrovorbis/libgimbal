#include "test_gimbal_type.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/meta/gimbal_type.h>
#include <gimbal/types/gimbal_quark.h>
#include <gimbal/meta/gimbal_class.h>
#include <gimbal/meta/gimbal_interface.h>

static GblType blankType_                           = GBL_INVALID_TYPE;
static GblType dependentType_                       = GBL_INVALID_TYPE;
static GblType derivable_                           = GBL_INVALID_TYPE;
static GblType derived_                             = GBL_INVALID_TYPE;
static GblType deepDerivable_                       = GBL_INVALID_TYPE;
static GblType middleDerived_                       = GBL_INVALID_TYPE;
static GblType deepDerived_                         = GBL_INVALID_TYPE;
static GblType classed_                             = GBL_INVALID_TYPE;
static GblType instanced_                           = GBL_INVALID_TYPE;
static GblType ifaced_                              = GBL_INVALID_TYPE;
static GblType ifaced2_                             = GBL_INVALID_TYPE;
static GblType ifaced3_                             = GBL_INVALID_TYPE;
static GblType classedDerived_                      = GBL_INVALID_TYPE;
static GblType ifacedDerived_                       = GBL_INVALID_TYPE;
static GblType ifacedDerived2_                      = GBL_INVALID_TYPE;
static GblType classedDeepDerivedIfaceMap_          = GBL_INVALID_TYPE;
static GblType classedDeepDerivedIfaceMapInherited_ = GBL_INVALID_TYPE;
static GblType ifacedIfaceMap_                      = GBL_INVALID_TYPE;
static GblType ifacedIfaceMapInherited_             = GBL_INVALID_TYPE;
static GblType classedMapIfacedIfaceMapInherited_   = GBL_INVALID_TYPE;

GBL_API GblType_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_CALL(GblType_final());
    GBL_API_END();
}


GBL_API GblType_test_testCaseFinal(GblContext* pCtx) {
    return GBL_RESULT_SUCCESS;
}

GBL_API GblType_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_CALL(GblType_init(pCtx, 0, 0));
    GBL_COMPARE(GblType_contextDefault(), pCtx);
    GBL_COMPARE(GblType_builtinCount(), GBL_TYPE_BUILTIN_COUNT);
    GBL_COMPARE(GblType_registeredCount(), GBL_TYPE_BUILTIN_COUNT);

    GblType prev = GBL_INVALID_TYPE;
    for(GblSize t = 0; t < GBL_TYPE_BUILTIN_COUNT; ++t) {
        GBL_VERIFY((prev = GblType_nextRegistered(prev))
                                  != GBL_INVALID_TYPE);
    }
    GBL_COMPARE(GblType_nextRegistered(prev), GBL_INVALID_TYPE);

    GBL_COMPARE(GblType_fromClass(NULL), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_fromBuiltinIndex(GBL_TYPE_BUILTIN_COUNT+1), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_fromName(NULL), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_fromName("Lolol"), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_fromNameQuark(GBL_QUARK_INVALID), GBL_INVALID_TYPE);
    GBL_API_END();
}

GBL_API GblType_test_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(strcmp("Invalid", GblType_name(GBL_INVALID_TYPE)), 0);
    GBL_COMPARE(GblType_nameQuark(GBL_INVALID_TYPE), GBL_QUARK_INVALID);
    GBL_COMPARE(GblType_parent(GBL_INVALID_TYPE), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_fundamental(GBL_INVALID_TYPE), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_ancestor(GBL_INVALID_TYPE, 2), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_depth(GBL_INVALID_TYPE), 0);
    GBL_COMPARE(GblType_flagsTest(GBL_INVALID_TYPE, 0xff), 0);
    GBL_COMPARE(GblType_verify(GBL_INVALID_TYPE), GBL_FALSE);
    GBL_VERIFY(GblType_check(GBL_INVALID_TYPE, GBL_INVALID_TYPE));
    GBL_VERIFY(!GblType_derives(GBL_INVALID_TYPE, GBL_INVALID_TYPE));
    GBL_VERIFY(!GblType_maps(GBL_INVALID_TYPE, GBL_INVALID_TYPE));
    GBL_COMPARE(GblType_info(GBL_INVALID_TYPE), NULL);
    GBL_COMPARE(GblType_plugin(GBL_INVALID_TYPE), NULL);
    GBL_COMPARE(GblType_classRefCount(GBL_INVALID_TYPE), 0);
    GBL_COMPARE(GblType_instanceRefCount(GBL_INVALID_TYPE),0);
    GBL_API_END();
}

GBL_API GblType_test_fundamental_blank(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
     blankType_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                         GblQuark_internStringStatic("Blank"),
                                          NULL,
                                          GBL_TYPE_FLAGS_NONE);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(blankType_ != GBL_INVALID_TYPE);
    GBL_VERIFY(!GBL_TYPE_IS_DEPENDENT(blankType_));
    GBL_VERIFY(!GBL_TYPE_IS_CLASSED(blankType_));
    GBL_VERIFY(!GBL_TYPE_IS_INSTANTIABLE(blankType_));
    GBL_VERIFY(!GBL_TYPE_IS_DERIVABLE(blankType_));
    GBL_VERIFY(!GBL_TYPE_IS_DEEP_DERIVABLE(blankType_));
    GBL_VERIFY(!GBL_TYPE_IS_BUILTIN(blankType_));
    GBL_VERIFY(!GBL_TYPE_IS_ABSTRACT(blankType_));
    GBL_VERIFY(!GBL_TYPE_IS_FINAL(blankType_));
    GBL_VERIFY(GBL_TYPE_IS_FUNDAMENTAL(blankType_));
    GBL_VERIFY(!GBL_TYPE_IS_INTERFACED(blankType_));
    GBL_VERIFY(GBL_TYPE_IS_ROOT(blankType_));
    GBL_VERIFY(GBL_TYPE_IS_VALID(blankType_));
    GBL_VERIFY(!GBL_TYPE_IS_VALUE(blankType_));

    GBL_COMPARE(GblType_registeredCount(), GBL_TYPE_BUILTIN_COUNT+1);
    GBL_COMPARE(GblType_fromName("Blank"), blankType_);
    GBL_COMPARE(GblType_fromNameQuark(GblQuark_fromString("Blank")), blankType_);
    GBL_COMPARE(GblType_classRefCount(blankType_), 0);
    GBL_COMPARE(GblType_instanceRefCount(blankType_), 0);

    GBL_VERIFY(strcmp(GblType_name(blankType_), "Blank") == 0);
    GBL_COMPARE(GblType_nameQuark(blankType_), GblQuark_fromStringStatic("Blank"));
    GBL_COMPARE(GblType_parent(blankType_), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_fundamental(blankType_), blankType_);
    GBL_COMPARE(GblType_base(blankType_, 0), blankType_);
    GBL_COMPARE(GblType_base(blankType_, 1), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_ancestor(blankType_, 0), blankType_);
    GBL_COMPARE(GblType_ancestor(blankType_, 1), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_depth(blankType_), 0);
    GBL_VERIFY(!GblType_flagsTest(blankType_, 0xffff));
    GBL_VERIFY(GblType_check(blankType_, blankType_));
    GBL_VERIFY(!GblType_check(blankType_, GBL_INVALID_TYPE));
    GBL_VERIFY(!GblType_derives(blankType_, blankType_));
    GBL_VERIFY(!GblType_derives(blankType_, GBL_INVALID_TYPE));
    GBL_VERIFY(!GblType_maps(blankType_, blankType_));
    GBL_COMPARE(GblType_plugin(blankType_), NULL);
    GBL_API_END();
}

GBL_API GblType_test_fundamental_duplicate(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType dupe = GblType_registerStatic(GBL_INVALID_TYPE,
                                        GblQuark_internStringStatic("Blank"),
                                         NULL,
                                         GBL_TYPE_FLAGS_NONE);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(dupe, GBL_INVALID_TYPE);
    GBL_API_END();
}

GBL_API GblType_test_fundamental_derived_register_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType invalid = GblType_registerStatic(blankType_,
                                            "DerivedInvalid",
                                             NULL,
                                             0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(invalid, GBL_INVALID_TYPE);
    GBL_API_END();
}

GBL_API GblType_test_fundamental_derived_register_valid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    derivable_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                               "Derivable",
                                               NULL,
                                               GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(derivable_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_DERIVABLE(derivable_));
    GBL_VERIFY(!GBL_TYPE_IS_DEEP_DERIVABLE(derivable_));

    derived_ = GblType_registerStatic(derivable_,
                                             "Derived",
                                             NULL,
                                             0);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(derived_ != GBL_INVALID_TYPE);
    GBL_API_END();
}

GBL_API GblType_test_fundamental_deep_derived_register_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType deepDerivedInvalid = GblType_registerStatic(derived_,
                                                        "DeepDerivedInvalid",
                                                        NULL,
                                                        0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(deepDerivedInvalid, GBL_INVALID_TYPE);
    GBL_API_END();
}

GBL_API GblType_test_fundamental_deep_derived_register_valid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    deepDerivable_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                            "DeepDerivable",
                                            NULL,
                                            GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(deepDerivable_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_DERIVABLE(deepDerivable_));
    GBL_VERIFY(GBL_TYPE_IS_DEEP_DERIVABLE(deepDerivable_));
    GBL_VERIFY(GBL_TYPE_IS_FUNDAMENTAL(deepDerivable_));
    GBL_COMPARE(GblType_parent(deepDerivable_), GBL_INVALID_TYPE);
    GBL_COMPARE(GblType_depth(deepDerivable_), 0);
    GBL_COMPARE(GblType_base(deepDerivable_, 0), deepDerivable_);
    GBL_COMPARE(GblType_ancestor(deepDerivable_, 0), deepDerivable_);
    GBL_VERIFY(GblType_flagsTest(deepDerivable_,
                                  GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE));
    GBL_VERIFY(GblType_verify(deepDerivable_));
    middleDerived_ = GblType_registerStatic(deepDerivable_,
                                                   "MiddleDerived",
                                                   NULL,
                                                   0);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(middleDerived_ != GBL_INVALID_TYPE);
    GBL_VERIFY(!GBL_TYPE_IS_FUNDAMENTAL(middleDerived_));
    GBL_COMPARE(GblType_depth(middleDerived_), 1);
    GBL_COMPARE(GblType_ancestor(middleDerived_, 0),middleDerived_);
    GBL_COMPARE(GblType_base(middleDerived_, 1), middleDerived_);
    GBL_VERIFY(GblType_derives(middleDerived_, deepDerivable_));
    GBL_VERIFY(GblType_check(middleDerived_, deepDerivable_));

    deepDerived_ = GblType_registerStatic(middleDerived_,
                                                 "DeepDerived",
                                                 NULL,
                                                 0);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(deepDerived_ != GBL_INVALID_TYPE);
    GBL_VERIFY(!GBL_TYPE_IS_FUNDAMENTAL(deepDerived_));
    GBL_COMPARE(GblType_depth(deepDerived_), 2);
    GBL_COMPARE(GblType_ancestor(deepDerived_, 0), deepDerived_);
    GBL_COMPARE(GblType_base(deepDerived_, 1), middleDerived_);
    GBL_VERIFY(GblType_derives(deepDerived_, deepDerivable_));
    GBL_VERIFY(GblType_check(deepDerived_, deepDerivable_));
    GBL_API_END();
}


GBL_API GblType_test_fundamental_classed_register_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType invalidClassed = GblType_registerStatic(GBL_INVALID_TYPE,
                                                    "InvalidClassed",
                                                    NULL,
                                                    GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(invalidClassed, GBL_INVALID_TYPE);
    GBL_API_END();
}
GBL_API GblType_test_fundamental_classed_register_valid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    classed_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                            "Classed",
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblClass)
                                            },
                                            GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED |
                                            GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE |
                                            GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(classed_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(classed_));
    GBL_API_END();
}

GBL_API GblType_test_fundamental_classed_inherit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    classedDerived_ = GblType_registerStatic(classed_,
                                            "ClassedDerived",
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblClass)
                                            },
                                            0);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(classedDerived_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(classedDerived_));
    GBL_VERIFY(GblType_derives(classedDerived_, classed_));
    GBL_VERIFY(GblType_check(classedDerived_, classed_));
    GBL_VERIFY(!GblType_maps(classedDerived_, classed_));
    GBL_API_END();
}

GBL_API GblType_test_fundamental_interfaced_register_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType invalidIFace = GblType_registerStatic(GBL_INVALID_TYPE,
                                                  "InvalidIFaced",
                                                  NULL,
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(invalidIFace, GBL_INVALID_TYPE);
    invalidIFace = GblType_registerStatic(GBL_INVALID_TYPE,
                                                  "InvalidIFaced",
                                                  &(const GblTypeInfo) {
                                                      .classSize = sizeof(GblInterface)
                                                  },
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED |
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(invalidIFace, GBL_INVALID_TYPE);
    GBL_VERIFY(!GBL_TYPE_IS_INTERFACED(invalidIFace));
    GBL_API_END();
}
GBL_API GblType_test_fundamental_interfaced_register_valid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    ifaced_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                            "IFaced",
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblInterface)
                                            },
                                            GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED |
                                            GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(ifaced_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(ifaced_));
    GBL_VERIFY(GBL_TYPE_IS_INTERFACED(ifaced_));
    ifaced2_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                            "IFaced2",
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblInterface)
                                            },
                                            GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED |
                                            GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(ifaced2_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(ifaced2_));
    GBL_VERIFY(GBL_TYPE_IS_INTERFACED(ifaced2_));
    ifaced3_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                            "IFaced3",
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblInterface)
                                            },
                                            GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED |
                                            GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(ifaced2_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(ifaced3_));
    GBL_VERIFY(GBL_TYPE_IS_INTERFACED(ifaced3_));
    GBL_API_END();
}

GBL_API GblType_test_fundamental_interfaced_inherit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    ifacedDerived_ = GblType_registerStatic(ifaced_,
                                            "IFacedDerived",
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblInterface)
                                            },
                                            0);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(ifaced_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(ifaced_));
    GBL_VERIFY(GBL_TYPE_IS_INTERFACED(ifaced_));
    GBL_COMPARE(GblType_parent(ifacedDerived_), ifaced_);
    GBL_VERIFY(GblType_derives(ifacedDerived_, ifaced_));
    GBL_COMPARE(GblType_ancestor(ifacedDerived_, 1), ifaced_);
    GBL_VERIFY(!GblType_maps(ifacedDerived_, ifaced_));
    GBL_VERIFY(GblType_verify(ifacedDerived_));
    GBL_VERIFY(GblType_check(ifacedDerived_, ifaced_));
    ifacedDerived2_ = GblType_registerStatic(ifaced2_,
                                            "IFacedDerived2",
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblInterface)
                                            },
                                            0);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(ifacedDerived2_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(ifacedDerived2_));
    GBL_VERIFY(GBL_TYPE_IS_INTERFACED(ifacedDerived2_));

    GBL_API_END();
}

GBL_API GblType_test_fundamental_classed_mapped_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
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
    GblType bullshitMapped = GblType_registerStatic(classedDerived_,
                                                    "ClassedDeepDerivedIfaceMapped",
                                                    &info,
                                                    0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(bullshitMapped, GBL_INVALID_TYPE);
    // Invalid type
    info.interfaceCount = 1;
    bullshitMapped = GblType_registerStatic(classedDerived_,
                                            "ClassedDeepDerivedIfaceMapped",
                                            &info,
                                            0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // Non-Inteface Type
    ifaceMap[0].interfaceType = classedDerived_;
    bullshitMapped = GblType_registerStatic(classedDerived_,
                                            "ClassDeepDerivedIfaceMapped",
                                            &info,
                                            0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // Invalid class size
    ifaceMap[0].interfaceType = ifaced_;
    bullshitMapped = GblType_registerStatic(classedDerived_,
                                            "ClassDeepDerivedIfaceMapped",
                                            &info,
                                            0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(bullshitMapped, GBL_INVALID_TYPE);


    // Invalid offset (in base)
    info.classSize = sizeof(GblClass) + sizeof(GblInterface);
    bullshitMapped = GblType_registerStatic(classedDerived_,
                                            "ClassDeepDerivedIfaceMapped",
                                            &info,
                                            0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // Offset too far
    ifaceMap[0].classOffset = sizeof(GblInterface);
    bullshitMapped = GblType_registerStatic(classedDerived_,
                                            "ClassDeepDerivedIfaceMapped",
                                            &info,
                                            0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // duplicated type
    info.classSize = sizeof(GblClass) + 2*sizeof(GblInterface) + 2;
    info.interfaceCount = 2;
    ifaceMap[0].classOffset = sizeof(GblClass);
    ifaceMap[1].interfaceType = ifaced_;
    ifaceMap[1].classOffset = sizeof(GblClass) + sizeof(GblInterface);
    bullshitMapped = GblType_registerStatic(classedDerived_,
                                            "ClassDeepDerivedIfaceMapped",
                                            &info,
                                            0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // ambiguous type
    ifaceMap[1].interfaceType = ifacedDerived_;
    bullshitMapped = GblType_registerStatic(classedDerived_,
                                            "ClassDeepDerivedIfaceMapped",
                                            &info,
                                            0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(bullshitMapped, GBL_INVALID_TYPE);

    // overlapping interfaces
    ifaceMap[1].interfaceType = ifaced2_;
    ifaceMap[1].classOffset -= 3;
    bullshitMapped = GblType_registerStatic(classedDerived_,
                                            "ClassDeepDerivedIfaceMapped",
                                            &info,
                                            0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(bullshitMapped, GBL_INVALID_TYPE);
    GBL_API_END();
}



GBL_API GblType_test_fundamental_classed_mapped_valid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    classedDeepDerivedIfaceMap_ = GblType_registerStatic(classedDerived_,
                                            "ClassedDeepDerivedIfaceMapped",
                                            &(const GblTypeInfo) {
                                                .classSize = sizeof(GblClass) + sizeof(GblInterface)*2,
                                                .interfaceCount = 2,
                                                .pInterfaceMap = (const GblTypeInterfaceMapEntry[]) {
                                                     {
                                                         .interfaceType = ifacedDerived_,
                                                         .classOffset = sizeof(GblClass)
                                                     },
                                                    {
                                                        .interfaceType = ifaced3_,
                                                        .classOffset = sizeof(GblClass) + sizeof(GblInterface)
                                                    }
                                                 }
                                            },
                                            0);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(classedDeepDerivedIfaceMap_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(classedDeepDerivedIfaceMap_));
    GBL_VERIFY(GblType_derives(classedDeepDerivedIfaceMap_, classedDerived_));
    GBL_VERIFY(GblType_derives(classedDeepDerivedIfaceMap_, classed_));
    GBL_VERIFY(!GblType_derives(classedDeepDerivedIfaceMap_, ifacedDerived_));
    GBL_VERIFY(GblType_check(classedDeepDerivedIfaceMap_, classed_));
    GBL_VERIFY(!GblType_maps(classedDeepDerivedIfaceMap_, classed_));
    GBL_VERIFY(GblType_maps(classedDeepDerivedIfaceMap_, ifacedDerived_));
    GBL_VERIFY(GblType_maps(classedDeepDerivedIfaceMap_, ifaced_));
    GBL_VERIFY(GblType_check(classedDeepDerivedIfaceMap_, ifacedDerived_));
    GBL_VERIFY(GblType_check(classedDeepDerivedIfaceMap_, ifaced_));
    GBL_VERIFY(GblType_maps(classedDeepDerivedIfaceMap_, ifaced3_));
    GBL_VERIFY(GblType_check(classedDeepDerivedIfaceMap_, ifaced3_));
    GBL_API_END();
}


GBL_API GblType_test_fundamental_classed_mapped_inherit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    classedDeepDerivedIfaceMapInherited_  = GblType_registerStatic(classedDeepDerivedIfaceMap_,
                                                                   "ClassedDeepDerivedIfaceMappedInherited",
                                                                   &(const GblTypeInfo) {
                                                                       .classSize = sizeof(GblClass) + sizeof(GblInterface) * 3,
                                                                       .interfaceCount = 1,
                                                                       .pInterfaceMap = (const GblTypeInterfaceMapEntry[]) {
                                                                            {
                                                                                .interfaceType = ifacedDerived2_,
                                                                                .classOffset = sizeof(GblClass) + sizeof(GblInterface) * 2
                                                                            },
                                                                        }
                                                                   },
                                                                   0);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(classedDeepDerivedIfaceMapInherited_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(classedDeepDerivedIfaceMapInherited_));
    GBL_VERIFY(GblType_derives(classedDeepDerivedIfaceMapInherited_, classedDeepDerivedIfaceMap_));
    GBL_VERIFY(GblType_derives(classedDeepDerivedIfaceMapInherited_, classedDerived_));
    GBL_VERIFY(GblType_derives(classedDeepDerivedIfaceMapInherited_, classed_));
    GBL_VERIFY(!GblType_derives(classedDeepDerivedIfaceMapInherited_, ifacedDerived_));
    GBL_VERIFY(!GblType_derives(classedDeepDerivedIfaceMapInherited_, ifacedDerived2_));
    GBL_VERIFY(GblType_check(classedDeepDerivedIfaceMapInherited_, classed_));
    GBL_VERIFY(!GblType_maps(classedDeepDerivedIfaceMapInherited_, classed_));
    GBL_VERIFY(GblType_maps(classedDeepDerivedIfaceMapInherited_, ifacedDerived_));
    GBL_VERIFY(GblType_maps(classedDeepDerivedIfaceMapInherited_, ifaced_));
    GBL_VERIFY(GblType_check(classedDeepDerivedIfaceMapInherited_, ifacedDerived_));
    GBL_VERIFY(GblType_check(classedDeepDerivedIfaceMapInherited_, ifaced_));
    GBL_VERIFY(GblType_maps(classedDeepDerivedIfaceMapInherited_, ifaced3_));
    GBL_VERIFY(GblType_check(classedDeepDerivedIfaceMapInherited_, ifaced3_));
    GBL_VERIFY(GblType_maps(classedDeepDerivedIfaceMapInherited_, ifacedDerived2_));
    GBL_VERIFY(GblType_check(classedDeepDerivedIfaceMapInherited_, ifacedDerived2_));
    GBL_VERIFY(GblType_maps(classedDeepDerivedIfaceMapInherited_, ifaced2_));
    GBL_VERIFY(GblType_check(classedDeepDerivedIfaceMapInherited_, ifaced2_));
    GBL_API_END();
}

GBL_API GblType_test_fundamental_interfaced_mapped(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    ifacedIfaceMap_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                             "IFaceMappedIFace",
                                             &(const GblTypeInfo) {
                                                 .classSize = sizeof(GblInterface)*3,
                                                 .interfaceCount = 2,
                                                 .pInterfaceMap = (const GblTypeInterfaceMapEntry[]) {
                                                      {
                                                          .interfaceType = ifacedDerived_,
                                                          .classOffset = sizeof(GblInterface)
                                                      },
                                                     {
                                                         .interfaceType = ifaced3_,
                                                         .classOffset = sizeof(GblInterface)*2
                                                     }
                                                  }
                                             },
                                             GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED |
                                             GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(ifacedIfaceMap_!= GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(ifacedIfaceMap_));
    GBL_VERIFY(GBL_TYPE_IS_INTERFACED(ifacedIfaceMap_));
    GBL_VERIFY(GblType_maps(ifacedIfaceMap_,  ifacedDerived_));
    GBL_VERIFY(GblType_maps(ifacedIfaceMap_,  ifaced_));
    GBL_VERIFY(GblType_check(ifacedIfaceMap_, ifacedDerived_));
    GBL_VERIFY(GblType_check(ifacedIfaceMap_, ifaced_));
    GBL_VERIFY(GblType_maps(ifacedIfaceMap_,  ifaced3_));
    GBL_VERIFY(GblType_check(ifacedIfaceMap_, ifaced3_));
    GBL_API_END();
}

GBL_API GblType_test_fundamental_interfaced_mapped_inherit_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblTypeInterfaceMapEntry entries[] = {
        { ifacedIfaceMap_, sizeof(GblInterface)*3 }
    };

    GblTypeInfo info = {
        .classSize = sizeof(GblInterface)*10,
        .interfaceCount = 1,
        .pInterfaceMap = entries
    };

    GblType invalidType = GblType_registerStatic(ifacedIfaceMap_,
                                                  "IfaceMappedIFaceInherited",
                                                  &info,
                                                  0);
    // Ambiguous type (inherited base is mapped)
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(invalidType, GBL_INVALID_TYPE);

    // Ambiguous type (inherited base common with mapped base)
    invalidType = GblType_registerStatic(ifacedDerived_,
                                         "IfaceMappedIFaceInherited1",
                                         &info,
                                         0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(invalidType, GBL_INVALID_TYPE);
    GBL_API_END();
}

GBL_API GblType_test_fundamental_interfaced_mapped_inherit_valid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    ifacedIfaceMapInherited_ = GblType_registerStatic(ifacedIfaceMap_,
                                                      "IfaceMappedIFaceInherited",
                                                      &(const GblTypeInfo) {
                                                          .classSize = sizeof(GblInterface)*4,
                                                          .interfaceCount = 1,
                                                          .pInterfaceMap = (const GblTypeInterfaceMapEntry[]) {
                                                               {
                                                                   .interfaceType = ifacedDerived2_,
                                                                   .classOffset = sizeof(GblInterface)*3
                                                               },
                                                           }
                                                      },
                                                      0);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(ifacedIfaceMapInherited_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_INTERFACED(ifacedIfaceMapInherited_));
    GBL_VERIFY(GblType_derives(ifacedIfaceMapInherited_, ifacedIfaceMap_));
    GBL_VERIFY(GblType_check(ifacedIfaceMapInherited_, ifacedIfaceMap_));
    GBL_VERIFY(!GblType_maps(ifacedIfaceMapInherited_, ifacedIfaceMap_));
    GBL_VERIFY(GblType_maps(ifacedIfaceMapInherited_, ifacedDerived_));
    GBL_VERIFY(GblType_maps(ifacedIfaceMapInherited_, ifaced_));
    GBL_VERIFY(GblType_check(ifacedIfaceMapInherited_, ifacedDerived_));
    GBL_VERIFY(GblType_check(ifacedIfaceMapInherited_, ifaced_));
    GBL_VERIFY(GblType_maps(ifacedIfaceMapInherited_, ifaced3_));
    GBL_VERIFY(GblType_check(ifacedIfaceMapInherited_, ifaced3_));
    GBL_VERIFY(GblType_maps(ifacedIfaceMapInherited_, ifacedDerived2_));
    GBL_VERIFY(GblType_check(ifacedIfaceMapInherited_, ifacedDerived2_));
    GBL_VERIFY(GblType_maps(ifacedIfaceMapInherited_, ifaced2_));
    GBL_VERIFY(GblType_check(ifacedIfaceMapInherited_, ifaced2_));
    GBL_API_END();
}


GBL_API GblType_test_fundamental_classed_mapped_interfaced_mapped_inherit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    classedMapIfacedIfaceMapInherited_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                                                "ClassedMapIfaceMappedIFaceInherited",
                                                                &(const GblTypeInfo) {
                                                                    .classSize = sizeof(GblClass) + sizeof(GblInterface) * 4,
                                                                    .interfaceCount = 1,
                                                                    .pInterfaceMap = (const GblTypeInterfaceMapEntry[]) {
                                                                         {
                                                                             .interfaceType = ifacedIfaceMapInherited_,
                                                                             .classOffset = sizeof(GblClass)
                                                                         },
                                                                     }
                                                                },
                                                                GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(classedMapIfacedIfaceMapInherited_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(classedMapIfacedIfaceMapInherited_));
    GBL_VERIFY(!GblType_derives(classedMapIfacedIfaceMapInherited_, ifacedIfaceMap_));
    GBL_VERIFY(GblType_check(classedMapIfacedIfaceMapInherited_, ifacedIfaceMap_));
    GBL_VERIFY(GblType_maps(classedMapIfacedIfaceMapInherited_, ifacedIfaceMap_));
    GBL_VERIFY(GblType_maps(classedMapIfacedIfaceMapInherited_, ifacedDerived_));
    GBL_VERIFY(GblType_maps(classedMapIfacedIfaceMapInherited_, ifaced_));
    GBL_VERIFY(GblType_check(classedMapIfacedIfaceMapInherited_, ifacedDerived_));
    GBL_VERIFY(GblType_check(classedMapIfacedIfaceMapInherited_, ifaced_));
    GBL_VERIFY(GblType_maps(classedMapIfacedIfaceMapInherited_, ifaced3_));
    GBL_VERIFY(GblType_check(classedMapIfacedIfaceMapInherited_, ifaced3_));
    GBL_VERIFY(GblType_maps(classedMapIfacedIfaceMapInherited_, ifacedDerived2_));
    GBL_VERIFY(GblType_check(classedMapIfacedIfaceMapInherited_, ifacedDerived2_));
    GBL_VERIFY(GblType_maps(classedMapIfacedIfaceMapInherited_, ifaced2_));
    GBL_VERIFY(GblType_check(classedMapIfacedIfaceMapInherited_, ifaced2_));
    GBL_API_END();
}

GBL_API GblType_test_fundamental_instantiable_register_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType invalid = GblType_registerStatic(GBL_INVALID_TYPE,
                                                     "Ininstanced",
                                                     &(const GblTypeInfo) {
                                                         .instanceSize = sizeof(GblInstance)
                                                     },
                                                     GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(invalid, GBL_INVALID_TYPE);

    invalid = GblType_registerStatic(GBL_INVALID_TYPE,
                                                     "Ininstanced",
                                                     &(const GblTypeInfo) {
                                                         .classSize = sizeof(GblInterface),
                                                         .instanceSize = sizeof(GblInstance)
                                                     },
                                                     GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE |
                                                     GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED   |
                                                     GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(invalid, GBL_INVALID_TYPE);
    GBL_API_END();
}
GBL_API GblType_test_fundamental_instantiable_register_valid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    instanced_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                         "Instanced",
                                         &(const GblTypeInfo) {
                                             .instanceSize = sizeof(GblInstance),
                                             .classSize = sizeof(GblClass)
                                         },
                                         GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_COMPARE(GblType_instanceRefCount(instanced_), 0);
    GBL_COMPARE(GblType_classRefCount(instanced_), 0);
    GBL_VERIFY(instanced_ != GBL_INVALID_TYPE);
    GBL_VERIFY(GBL_TYPE_IS_CLASSED(instanced_));
    GBL_VERIFY(GBL_TYPE_IS_INSTANTIABLE(instanced_));
    GBL_API_END();
}


GBL_API GblType_test_fundamental_dependent_register_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    dependentType_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                            "Dependent",
                                            &(const GblTypeInfo) {
                                                .dependencyCount = 1,
                                                .pDependencies = (const GblType[]) {
                                                    blankType_
                                                }
                                            },
                                            0);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_VERIFY(dependentType_ == GBL_INVALID_TYPE);
    GBL_VERIFY(!GBL_TYPE_IS_DEPENDENT(dependentType_));
    GBL_VERIFY(!GblType_depends(dependentType_, blankType_));
    GBL_VERIFY(!GblType_conforms(blankType_, dependentType_));

    dependentType_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                            "Dependent",
                                            &(const GblTypeInfo) {
                                                .dependencyCount = 2,
                                                .pDependencies = (const GblType[]) {
                                                    instanced_,
                                                    classedDerived_
                                                }
                                            }, GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT);
    //GBL_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_TYPE);
    GBL_COMPARE(dependentType_, GBL_INVALID_TYPE);

    GBL_API_END();
}

GBL_API GblType_test_fundamental_dependent_register_valid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
#if 0
    dependentType_ = GblType_registerStatic(GBL_INVALID_TYPE,
                                            "Dependent",
                                            &(const GblTypeInfo) {
                                                .dependencyCount = 1,

                                            })
#endif

    GBL_API_END();
}

GBL_API GblType_test_fundamental_dependent_derive_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblType type = GblType_registerStatic(dependentType_,
                                          "InvalidDependent",
                                          &(const GblTypeInfo) {
                                              .dependencyCount = 1,
                                              .pDependencies = (const GblType[]) {
                                                  blankType_
                                              }
                                          },
                                          0);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}
GBL_API GblType_test_fundamental_dependent_derive_valid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);


    GBL_API_END();
}

GBL_API GblType_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    //GBL_API_CALL(GblType_final());
    GBL_API_END();
}











