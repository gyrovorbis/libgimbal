#ifndef GIMBAL_TYPES_H
#define GIMBAL_TYPES_H

#include "gimbal_api.h"

#ifdef __cplusplus
extern "C" {
#endif

GBL_FORWARD_DECLARE_STRUCT(GblContext);

typedef GBL_BOOL                        GblBool;
typedef GBL_SIZE                        GblSize;
typedef GBL_REAL                        GblReal;
typedef GBL_INT                         GblInt;
typedef GBL_ENUM                        GblEnum;
typedef GBL_HASH                        GblHash;
typedef uint32_t                        GblVersion;

GBL_DECLARE_ENUM(GBL_RESULT) {
    GBL_RESULT_UNKNOWN                          = 0x0,
    GBL_RESULT_SUCCESS                          = 0x1,
    GBL_RESULT_PARTIAL_SUCCESS                  = 0x2,          // Some inner stuff failed, but outer is okay (getlasterror)
    GBL_RESULT_NOT_READY                        = 0x3,
    GBL_RESULT_NOT_FOUND                        = 0x4,          // Finding an item by name or index with NULL return
    GBL_RESULT_TIMEOUT                          = 0x5,
    GBL_RESULT_INCOMPLETE                       = 0x6,
    GBL_RESULT_TRUNCATED                        = 0x7,          // Buffer not large enough
    GBL_RESULT_LOSSY_CONVERSION                 = 0x8,
    GBL_RESULT_UNIMPLEMENTED                    = 0x9,          // Not implemented on this build yet (but planning)
    GBL_RESULT_UNSUPPORTED                      = 0xa,          // Either cannot or will not support it intentionally
    GBL_RESULT_VERSION_MISMATCH                 = 0xb,          // Requested version is greater than supported version
    GBL_RESULT_ERROR                            = 0xbad00000,
    GBL_RESULT_ERROR_INVALID_HANDLE             = GBL_RESULT_ERROR | 1,
    GBL_RESULT_ERROR_INVALID_ARG                = GBL_RESULT_ERROR | 2,
    GBL_RESULT_ERROR_MEM_MALLOC                 = GBL_RESULT_ERROR | 3,
    GBL_RESULT_ERROR_MEM_REALLOC                = GBL_RESULT_ERROR | 4,
    GBL_RESULT_ERROR_MEM_FREE                   = GBL_RESULT_ERROR | 5,
    GBL_RESULT_ERROR_FILE_OPEN                  = GBL_RESULT_ERROR | 6,
    GBL_RESULT_ERROR_FILE_READ                  = GBL_RESULT_ERROR | 7,
    GBL_RESULT_ERROR_FILE_WRITE                 = GBL_RESULT_ERROR | 8,
    GBL_RESULT_ERROR_FILE_CLOSE                 = GBL_RESULT_ERROR | 9,
    GBL_RESULT_ERROR_UNDERFLOW                  = GBL_RESULT_ERROR | 10, //integers, log depth, api cookie/context
    GBL_RESULT_ERROR_OVERFLOW                   = GBL_RESULT_ERROR | 11,
    GBL_RESULT_ERROR_VSNPRINTF                  = GBL_RESULT_ERROR | 12,
    GBL_RESULT_ERROR_INTERNAL_ASSERT            = GBL_RESULT_ERROR | 13,
    GBL_RESULT_ERROR_VARIANT_TYPE_INCOMPATIBLE  = GBL_RESULT_ERROR | 14,
    GBL_RESULT_COUNT                            = GBL_RESULT_ERROR_VARIANT_TYPE_INCOMPATIBLE + 1
};

#define GBL_RESULT_SUCCESS(value) \
    ((((value) & 0xfff00000) != GBL_RESULT_ERROR) && value != GBL_RESULT_UNKNOWN)

#define GBL_RESULT_PARTIAL_SUCCESS(value) \
    (GBL_RESULT_SUCCESS(value) && value != GBL_RESULT_SUCCESS)

#define GBL_RESULT_FAILURE(value) \
    !GBL_RESULT_SUCCESS(value)

GBL_DECLARE_ENUM(GBL_LOG_LEVEL) {
    GBL_LOG_LEVEL_DEBUG,
    GBL_LOG_LEVEL_VERBOSE,
    GBL_LOG_LEVEL_INFO,
    GBL_LOG_LEVEL_WARNING,
    GBL_LOG_LEVEL_ERROR,
    GBL_LOG_LEVEL_COUNT
};

typedef struct GblVersionInfo {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} GblVersionInfo;

GBL_API gblVersionString(const GblVersionInfo* pInfo, char* pBuffer, size_t size);

typedef struct GblSourceContext {
    const GblSourceLocation*    pSrcLoc;
    const char*                 pModuleName;
} GblSourceContext;

typedef struct GblError {
    char                msg[GBL_CONTEXT_LAST_ERROR_BUFFER_SIZE];
    GblSourceContext    srcCtx;
    GBL_RESULT          result;
    GBL_LOG_LEVEL       logLevel;
} GblError;

typedef struct GblApiCookie {
#ifdef GBL_RESULT_CALL_STACK_TRACKING
    GblSourceContext        sourceCtx;
#endif
    GblContext*             pContext;
    const void*             pNext;
    struct GblApiCookie*    pParent;
    GBL_RESULT              result;
    uint32_t                stackDepth;
} GblApiCookie;

typedef struct GblApiInfo {
    const char* pName;
    const char* pDesc;
    const char* pVersion;
} GblApiInfo;

#ifdef __cplusplus
}
#endif

#endif // GIMBAL_TYPES_H
