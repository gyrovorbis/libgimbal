#ifndef GIMBAL_TYPEDEFS_H
#define GIMBAL_TYPEDEFS_H

#include "../core/gimbal_api_generators.h"
#include "gimbal_result.h"
#include "gimbal_version.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// move random ass shit like memory allocatoin and call stack and event types the fuck out of here!!!

#define GBL_INDEX_INVALID       GBL_SIZE_MAX

// ======== basic types ==========

typedef GBL_BOOL                        GblBool;
typedef GBL_SIZE                        GblSize;
typedef GBL_FLOAT                       GblFloat;
typedef GBL_INT                         GblInt;
typedef GBL_UINT                        GblUint;
typedef GBL_ENUM                        GblEnum;
typedef GBL_FLAGS                       GblFlags;
typedef GBL_HASH                        GblHash;
typedef GblHash                         GblGuid;
typedef size_t                          GblType;
typedef GBL_CMP_RESULT                  GblCmpResult;
typedef uint32_t                        GblVersion;
typedef uint64_t                        GblEventType;
typedef void                            (*GblFnPtr)();

typedef union GblPtr {
    GblFnPtr    pFunc;
    void*       pData;
} GblPtr;

// ====== Handle types ==========
GBL_DECLARE_OPAQUE(GblObject);
GBL_DECLARE_OPAQUE(GblHandle);
GBL_DECLARE_OPAQUE(GblContext);
GBL_FORWARD_DECLARE_STRUCT(GblVariant);
GBL_FORWARD_DECLARE_STRUCT(GblStackFrame);
GBL_FORWARD_DECLARE_STRUCT(GblCallRecord);
GBL_FORWARD_DECLARE_STRUCT(GblMemAllocInfo);
GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblClass);
GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblInstance);
GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblInterface);

// ======== Log Level ============







#ifdef __cplusplus
}
#endif

#endif // GIMBAL_TYPEDEFS_H