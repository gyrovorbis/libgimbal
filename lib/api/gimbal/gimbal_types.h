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

// ======== basic types ==========

typedef GBL_BOOL                        GblBool;
typedef GBL_SIZE                        GblSize;
typedef GBL_REAL                        GblReal;
typedef GBL_INT                         GblInt;
typedef GBL_ENUM                        GblEnum;
typedef GBL_HASH                        GblHash;
typedef uint32_t                        GblVersion;

// ======== Log Level ============

#define GBL_META_LOG_LEVEL_TABLE (                                              \
        ( GBL_LOG_LEVEL, LogLevel, "Log Message Severity Level"),               \
        (                                                                       \
            (GBL_LOG_LEVEL_DEBUG,       0x0,    Debug,      "Debug"),           \
            (GBL_LOG_LEVEL_VERBOSE,     0x1,    Verbose,    "Verbose"),         \
            (GBL_LOG_LEVEL_INFO,        0x2,    Info,       "Info"),            \
            (GBL_LOG_LEVEL_WARNING,     0x3,    Warning,    "Warning"),         \
            (GBL_LOG_LEVEL_ERROR,       0x4,    Error,      "Error"),           \
            (GBL_LOG_LEVEL_COUNT,       0x5,    Count,      "# of Levels")      \
        )                                                                       \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_LOG_LEVEL_TABLE);

// =============== API RESULT ==================

GBL_DECLARE_OPAQUE(GblHandle);

typedef struct GblApiResult {
    char                message[GBL_API_RESULT_MSG_BUFFER_SIZE];
    GblSourceLocation   srcLocation;
    GblHandle           hHandle;
    GBL_RESULT          resultCode;
} GblApiResult;

GBL_MAYBE_UNUSED GBL_INLINE void GBL_API_RESULT_CONSTRUCT(GblApiResult* pResult, GblHandle hHandle, GBL_RESULT resultCode, GblSourceLocation source, const char* pFmt, ...) {
    va_list varArgs;
    va_start(varArgs, pFmt);
    memset(pResult, 0, sizeof(GblApiResult));
    if(pFmt) vsnprintf(pResult->message, sizeof(pResult->message), pFmt, varArgs);
    pResult->srcLocation    = source;
    pResult->hHandle        = hHandle;
    pResult->resultCode     = resultCode;
}

// ============ VERSION ================

#define GBL_VERSION_FIELD_MAJOR_BIT                 24u
#define GBL_VERSION_FIELD_MAJOR_MASK                0xf

#define GBL_VERSION_FIELD_MINOR_BIT                 8u
#define GBL_VERSION_FIELD_MINOR_MASK                0xff

#define GBL_VERSION_FIELD_PATCH_BIT                 0u
#define GBL_VERSION_FIELD_PATCH_MASK                0xf

#define GBL_VERSION_MAKE(major, minor, patch)                                               \
    (uint32_t)((major & GBL_VERSION_FIELD_MAJOR_MASK) << GBL_VERSION_FIELD_MAJOR_BIT)   |   \
    (uint32_t)((minor & GBL_VERSION_FIELD_MINOR_MASK) << GBL_VERSION_FIELD_MINOR_BIT)   |   \
    (uint32_t)((patch & GBL_VERSION_FIELD_PATCH_MASK) << GBL_VERSION_FIELD_PATCH_BIT)

#define GBL_VERSION_EXTRACT_MAJOR(version)                                                  \
    (uint8_t)((uint32_t)((version >> GBL_VERSION_FIELD_MAJOR_BIT) & GBL_VERSION_FIELD_MAJOR_MASK))

#define GBL_VERSION_EXTRACT_MINOR(version)                                                  \
    (uint16_t)((uint32_t)((version >> GBL_VERSION_FIELD_MINOR_BIT) & GBL_VERSION_FIELD_MINOR_MASK))

#define GBL_VERSION_EXTRACT_PATCH(version)                                                  \
    (uint8_t)((uint32_t)((version >> GBL_VERSION_FIELD_PATCH_BIT) & GBL_VERSION_FIELD_PATCH_MASK))

#define GBL_VERSION_STRINGIFY(major, minor, patch) \
    GBL_STRINGIFY(major) "."                       \
    GBL_STRINGIFY(minor) "."                       \
    GBL_STRINGIFY(patch)

//#define GBL_BIT_MASK(...) \
  //    GBL_VA_OVERLOAD_SUFFIXER_ARGC(GBL_BIT_COUNT_MASK, GBL_BIT_FIELD_MASK, __VA_ARGS__)

typedef struct GblVersionInfo {
    uint8_t major;
    uint16_t minor;
    uint8_t patch;
} GblVersionInfo;

GBL_MAYBE_UNUSED GBL_INLINE GblVersionInfo GBL_VERSION_EXTRACT(GblVersion version) {
    const GblVersionInfo info = {
        GBL_VERSION_EXTRACT_MAJOR(version),
        GBL_VERSION_EXTRACT_MINOR(version),
        GBL_VERSION_EXTRACT_PATCH(version)
    };
    return info;
}


#define GBL_VERSION_STRING_SIZE_MAX     (3 * 3 + 2 + 1) // 3 chars * 3 subTypes + 2 dots + 1 null char
GBL_API gblVersionInfoString(const GblVersionInfo* pInfo, char* pBuffer, size_t size);


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_TYPES_H
