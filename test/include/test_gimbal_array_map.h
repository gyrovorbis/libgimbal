#ifndef TEST_GIMBAL_ARRAY_MAP_H
#define TEST_GIMBAL_ARRAY_MAP_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/utils/gimbal_test.h>

#define GBL_ARRAY_MAP_TEST_SET(X)                   \
    X(GblArrayMap_test_testCaseInit)                \
    X(GblArrayMap_test_testCaseFinal)               \
    X(GblArrayMap_test_init)                        \
    X(GblArrayMap_test_null_invalid)                \
    X(GblArrayMap_test_null)                        \
    X(GblArrayMap_test_empty)                       \
    X(GblArrayMap_test_setUserdata)                 \
    X(GblArrayMap_test_setUserdata_lazy)            \
    X(GblArrayMap_test_insertUserdata_duplicate)    \
    X(GblArrayMap_test_insertUserdata)              \
    X(GblArrayMap_test_setVariant_overUserdata)     \
    X(GblArrayMap_test_insertVariant)               \
    X(GblArrayMap_test_eraseUserdata)               \
    X(GblArrayMap_test_extractValue)                \
    X(GblArrayMap_test_extractVariant)              \
    X(GblArrayMap_test_clear)                       \
    X(GblArrayMap_test_destroy)                     \
    X(GblArrayMap_test_final)

GBL_DECLS_BEGIN
GBL_ARRAY_MAP_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_ARRAY_MAP_H
