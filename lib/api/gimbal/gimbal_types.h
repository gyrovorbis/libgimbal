#ifndef GIMBAL_TYPES_H
#define GIMBAL_TYPES_H

#include <gimbal/gimbal_config.h>
#include "gimbal_macros.h"
#include "gimbal_result.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef GBL_BOOL                        GblBool;
typedef GBL_SIZE                        GblSize;
typedef GBL_REAL                        GblReal;
typedef GBL_INT                         GblInt;
typedef GBL_ENUM                        GblEnum;
typedef GBL_HASH                        GblHash;
typedef uint32_t                        GblVersion;

GBL_DECLARE_ENUM(GBL_LOG_LEVEL) {
    GBL_LOG_LEVEL_DEBUG,
    GBL_LOG_LEVEL_VERBOSE,
    GBL_LOG_LEVEL_INFO,
    GBL_LOG_LEVEL_WARNING,
    GBL_LOG_LEVEL_ERROR,
    GBL_LOG_LEVEL_COUNT
};

GBL_DECLARE_OPAQUE(GblHandle);

typedef struct GblApiResult {
    char                message[GBL_API_RESULT_MSG_BUFFER_SIZE];
    GblSourceLocation   srcLocation;
    GblHandle           hHandle;
    GBL_RESULT          resultCode;
} GblApiResult;

static inline void GBL_API_RESULT_CONSTRUCT(GblApiResult* pResult, GblHandle hHandle, GBL_RESULT resultCode, GblSourceLocation source, const char* pFmt, ...) {
    va_list varArgs;
    va_start(varArgs, pFmt);
    memset(pResult, 0, sizeof(GblApiResult));
    if(pFmt) vsnprintf(pResult->message, sizeof(pResult->message), pFmt, varArgs);
    pResult->srcLocation    = source;
    pResult->hHandle        = hHandle;
    pResult->resultCode     = resultCode;
}

typedef struct GblVersionInfo {
    uint8_t major;
    uint16_t minor;
    uint8_t patch;
} GblVersionInfo;

static inline GblVersionInfo GBL_VERSION_EXTRACT(GblVersion version) {
    const GblVersionInfo info = {
        GBL_VERSION_EXTRACT_MAJOR(version),
        GBL_VERSION_EXTRACT_MINOR(version),
        GBL_VERSION_EXTRACT_PATCH(version)
    };
    return info;
}


#define GBL_VERSION_STRING_SIZE_MAX     (3 * 3 + 2 + 1) // 3 chars * 3 subTypes + 2 dots + 1 null char
GBL_API gblVersionInfoString(const GblVersionInfo* pInfo, char* pBuffer, size_t size);



typedef struct GblApiInfo {
    const char* pName;
    const char* pDesc;
    const char* pVersion;
} GblApiInfo;

#ifdef __cplusplus
}
#endif

#endif // GIMBAL_TYPES_H
