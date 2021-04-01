#ifndef GIMBAL_CONFIG_H
#define GIMBAL_CONFIG_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef GBL_ASSERT
#  define GBL_ASSERT(b, s)                      assert(b && s)
#endif

#ifndef GBL_BOOL
#   define GBL_BOOL                             uint32_t
#endif

#ifndef GBL_REAL
#   define GBL_REAL                             float
#endif

#ifndef GBL_INT
#   define GBL_INT                              int32_t
#endif

#ifndef GBL_ENUM
#   define GBL_ENUM                             uint32_t
#endif

#ifndef GBL_SIZE
#   define GBL_SIZE                             size_t
#   define GBL_SIZE_MAX                         SIZE_MAX
#endif

#ifndef GBL_HASH
#   define GBL_HASH                             uint32_t
#endif

#ifndef GBL_TRUE
#   define GBL_TRUE                             1
#endif

#ifndef GBL_FALSE
#   define GBL_FALSE                            0
#endif

#ifndef GBL_SOURCE_FILE
#   define GBL_SOURCE_FILE                      __FILE__
#endif
#ifndef GBL_SOURCE_FUNC
#   define GBL_SOURCE_FUNC                      __func__
#endif
#ifndef GBL_SOURCE_LINE
#   define GBL_SOURCE_LINE                      __LINE__
#endif
#ifndef GBL_SOURCE_COLUMN
#   define GBL_SOURCE_COLUMN                    0
#endif

#ifndef GBL_API_COOKIE_NAME_DEFAULT
#   define GBL_API_COOKIE_NAME_DEFAULT          gblApiCookie
#endif

#ifndef GBL_API_END_LABEL
#   define GBL_API_END_LABEL                    gbl_api_end
#endif

#ifndef GBL_VA_SNPRINTF_BUFFER_SIZE
#   define GBL_VA_SNPRINTF_BUFFER_SIZE          512
#endif

#ifndef GBL_STRING_BUFFER_STACK_SIZE
#   define GBL_STRING_BUFFER_STACK_SIZE         64
#endif

#ifndef GBL_CONTEXT_ERROR_MESSAGE_BUFFER_SIZE
#   define GBL_CONTEXT_ERROR_MESSAGE_BUFFER_SIZE   128
#endif

#define GBL_SOURCE_LOCATION_TRACKING 1

#if GBL_SOURCE_LOCATION_TRACKING
#   ifndef GBL_SOURCE_LOCATION
        typedef struct GblSourceLocation {
            const char* pFile;
            const char* pFunc;
            GBL_SIZE     line;
            GBL_SIZE     column;
        } GblSourceLocation;

inline GblSourceLocation GBL_SOURCE_LOCATION(const char* pFile, const char* pFunc, GBL_SIZE line, GBL_SIZE column) {
    const GblSourceLocation location = {
        pFile, pFunc, line, column
    };
    return location;
}

#       define GBL_SOURCE_LOCATION_CURRENT()     \
            GblSourceLocation {          \
                GBL_SOURCE_FILE,         \
                GBL_SOURCE_FUNC,         \
                GBL_SOURCE_LINE,         \
                GBL_SOURCE_COLUMN        \
            }
#   endif

#else

#   ifndef GBL_SOURCE_LOCATION
        typedef struct GblSourceLocation {
            char cppDummyVar;
        } GblSourceLocation;

#       define GBL_SOURCE_LOCATION()     \
            GblSourceLocation { 0 }

#   endif

#endif

// Assert upon encountering an error result?
#ifndef GBL_RESULT_ERROR_ASSERT
#   define GBL_RESULT_ERROR_ASSERT      0
#endif

// Assertion operation upon encountering an error.
#ifndef GBL_RESULT_ASSERT
#   if GBL_RESULT_ERROR_ASSERT
#       define GBL_RESULT_ASSERT(b, s)       GBL_ASSERT(b, s)
#   else
#       define GBL_RESULT_ASSERT(b, s)
#   endif
#endif

// Write to log when after encountering an error result?
#ifndef GBL_RESULT_LOG_ERRORS
#   define GBL_RESULT_LOG_ERRORS         1
#endif

// Log severity of error result message
#ifndef GBL_RESULT_ERROR_LOG_LEVEL
#   define GBL_RESULT_LOG_ERROR_LEVEL    GBL_LOG_LEVEL_ERROR
#endif

// Write to log when after encountering a partial succcess result?
#ifndef GBL_RESULT_LOG_PARTIAL_SUCCESSES
#   define GBL_RESULT_LOG_PARTIAL_SUCCESSES         1
#endif

// Log severity of partial success result message
#ifndef GBL_RESULT_LOG_PARTIAL_SUCCESS_LEVEL
#   define GBL_RESULT_LOG_PARTIAL_SUCCESS_LEVEL    GBL_LOG_LEVEL_WARNING
#endif

// Write to log when after encountering a succcess result?
#ifndef GBL_RESULT_LOG_SUCCESSES
#   define GBL_RESULT_LOG_SUCCESSES         0
#endif

// Log severity of success result message
#ifndef GBL_RESULT_LOG_SUCCESS_LEVEL
#   define GBL_RESULT_LOG_SUCCESS_LEVEL    GBL_LOG_LEVEL_DEBUG
#endif

// .....fuck
#ifndef GBL_RESULT_LOG
#   ifdef GBL_RESULT_ERROR_LOG
#       define GBL_RESULT_LOG(pCtx, file, func, line, exp, ...)     \
            do {                                                     \
                if(exp) GBLLogWrite_(pCtx,                          \
                               file, func, line,                     \
                               GBL_LOG_LEVEL_ERROR, __VA_ARGS__);   \
            } while(0)
#   else
#       define GBL_RESULT_LOG(pCtx, file, func, line, exp, ...) void
#   endif
#endif


// Log last error config



#define GBL_RESULT_CTX_FOLD(pCtx, HANDLER, ...) \
    GBL_RESULT_##HANDLER(pCtx, NULL, __VA_ARGS__)




#ifndef GBL_RESULT_SET_LAST_ERROR
#   ifdef GBL_RESULT_CONTEXT_TRACK_LAST_ERROR
#       define GBL_RESULT_SET_LAST_ERROR(pCtx, pDevice, result, level, pFile, pFunc, line, ...) \
            GBLContextUpdateLastError_(pCtx, pDevice, result, level, pFile, pFunc, line, __VA_ARGS__)
#   else
#       define GBL_RESULT_SET_LAST_ERROR(pCtx, pDevice, result, level, pFile, pFunc, line, pFormat, ...)
#   endif
#endif


#ifdef __cplusplus
}
#endif


#endif // GBL_CONFIG_H
