/*! \file
 *  \brief Basic primtive typedefs and forward declarations
 *  \ingroup dataTypes
 */
#ifndef GIMBAL_TYPEDEFS_H
#define GIMBAL_TYPEDEFS_H

#include "../core/gimbal_api_generators.h"
#include "../preprocessor/gimbal_atomics.h"
#include "gimbal_result.h"
#include "gimbal_version.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

GBL_DECLS_BEGIN

#define GBL_INDEX_INVALID       GBL_SIZE_MAX

// ======== Primitive Types ==========

typedef GBL_BOOL                        GblBool;
typedef GBL_SIZE                        GblSize;
typedef GBL_INT                         GblInt;
typedef GBL_UINT                        GblUint;
typedef GBL_ENUM                        GblEnum;
typedef GBL_FLAGS                       GblFlags;
typedef GBL_HASH                        GblHash;
typedef uint16_t                        GblRefCount;
typedef uint32_t                        GblVersion;
typedef void                            (*GblFnPtr)();
typedef union GblPtr {
    GblFnPtr        pFunc;
    void*           pData;
} GblPtr;

// ====== Struct Types ==========
GBL_FORWARD_DECLARE_STRUCT(GblVariant);
GBL_FORWARD_DECLARE_STRUCT(GblStringBuffer);
GBL_FORWARD_DECLARE_STRUCT(GblStackFrame);
GBL_FORWARD_DECLARE_STRUCT(GblCallRecord);
GBL_FORWARD_DECLARE_STRUCT(GblClass);
GBL_FORWARD_DECLARE_STRUCT(GblInstance);
GBL_FORWARD_DECLARE_STRUCT(GblInterface);
GBL_FORWARD_DECLARE_STRUCT(GblIPlugin);
GBL_FORWARD_DECLARE_STRUCT(GblObject);
GBL_FORWARD_DECLARE_STRUCT(GblContext);
GBL_FORWARD_DECLARE_STRUCT(GblEvent);

GBL_DECLS_END

#endif // GIMBAL_TYPEDEFS_H
