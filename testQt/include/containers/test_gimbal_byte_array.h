#ifndef TEST_GIMBAL_BYTE_ARRAY_H
#define TEST_GIMBAL_BYTE_ARRAY_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gimbal/test/gimbal_test.h>

#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#define GBL_BYTE_ARRAY_TEST_SET(X)                \
        X(GblByteArray_test_testCaseInit)         \
        X(GblByteArray_test_testCaseFinal)        \
        X(GblByteArray_test_init)                 \
        X(GblByteArray_test_create_empty)         \
        X(GblByteArray_test_create_pointer)       \
        X(GblByteArray_test_copy)                 \
        X(GblByteArray_test_set)                  \
        X(GblByteArray_test_move)                 \
        X(GblByteArray_test_ref)                  \
        X(GblByteArray_test_unref)                \
        X(GblByteArray_test_write_read)           \
        X(GblByteArray_test_write_read_invalid)   \
        X(GblByteArray_test_acquire_release)      \
        X(GblByteArray_test_resize)               \
        X(GblByteArray_test_grow)                 \
        X(GblByteArray_test_shrink)               \
        X(GblByteArray_test_erase)                \
        X(GblByteArray_test_insert)               \
        X(GblByteArray_test_append)               \
        X(GblByteArray_test_prepend)              \
        X(GblByteArray_test_clear)                \
        X(GblByteArray_test_final)

GBL_DECLS_BEGIN
GBL_BYTE_ARRAY_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_BYTE_ARRAY_H
