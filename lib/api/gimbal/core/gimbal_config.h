#ifndef GIMBAL_CONFIG_H
#define GIMBAL_CONFIG_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <gimbal/preprocessor/gimbal_compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GBL_ASSERT_1
#   define GBL_ASSERT_1(expr)                   assert((expr))
#endif

#ifndef GBL_ASSERT_2
#   define GBL_ASSERT_2(expr, cause)            GBL_ASSERT_1((expr) && (cause))
#endif

#ifndef GBL_CTX_FRAME_NAME
#   define GBL_CTX_FRAME_NAME                   gblApiFrame_
#endif

#ifndef GBL_CTX_END_LABEL
#   define GBL_CTX_END_LABEL                    gbl_api_end_
#endif

#ifndef GBL_VA_SNPRINTF_BUFFER_SIZE
#   define GBL_VA_SNPRINTF_BUFFER_SIZE          512
#endif

#ifndef GBL_CTX_RESULT_MSG_BUFFER_SIZE
#   define GBL_CTX_RESULT_MSG_BUFFER_SIZE       128
#endif

#ifndef GBL_STRING_FLOAT_DECIMAL_PLACES
#   define GBL_STRING_FLOAT_DECIMAL_PLACES      3
#endif

#ifdef __cplusplus
}
#endif


#endif // GBL_CONFIG_H

