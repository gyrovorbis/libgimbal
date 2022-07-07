#ifndef TEST_GIMBAL_TYPE_H
#define TEST_GIMBAL_TYPE_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/test/gimbal_test.h>

#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#define GBL_TYPE_TEST_SET(X)                                                            \
        X(GblType_test_testCaseInit)                                                    \
        X(GblType_test_testCaseFinal)                                                   \
        X(GblType_test_init)                                                            \
        X(GblType_test_invalid)                                                         \
        X(GblType_test_fundamental_blank)                                               \
        X(GblType_test_fundamental_duplicate)                                           \
        X(GblType_test_fundamental_derived_register_invalid)                            \
        X(GblType_test_fundamental_derived_register_valid)                              \
        X(GblType_test_fundamental_deep_derived_register_invalid)                       \
        X(GblType_test_fundamental_deep_derived_register_valid)                         \
        X(GblType_test_fundamental_classed_register_invalid)                            \
        X(GblType_test_fundamental_classed_register_valid)                              \
        X(GblType_test_fundamental_classed_inherit)                                     \
        X(GblType_test_fundamental_interfaced_register_invalid)                         \
        X(GblType_test_fundamental_interfaced_register_valid)                           \
        X(GblType_test_fundamental_interfaced_inherit)                                  \
        X(GblType_test_fundamental_classed_mapped_invalid)                              \
        X(GblType_test_fundamental_classed_mapped_valid)                                \
        X(GblType_test_fundamental_classed_mapped_inherit)                              \
        X(GblType_test_fundamental_interfaced_mapped)                                   \
        X(GblType_test_fundamental_interfaced_mapped_inherit_invalid)                   \
        X(GblType_test_fundamental_interfaced_mapped_inherit_valid)                     \
        X(GblType_test_fundamental_classed_mapped_interfaced_mapped_inherit)            \
        X(GblType_test_fundamental_instantiable_register_invalid)                       \
        X(GblType_test_fundamental_instantiable_register_valid)                         \
        X(GblType_test_fundamental_dependent_register_invalid)                          \
        X(GblType_test_fundamental_dependent_register_valid)                            \
        X(GblType_test_fundamental_dependent_derive_invalid)                            \
        X(GblType_test_fundamental_dependent_derive_valid)                              \
        X(GblType_test_final)                                                           \

GBL_DECLS_BEGIN
GBL_TYPE_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_TYPE_H
