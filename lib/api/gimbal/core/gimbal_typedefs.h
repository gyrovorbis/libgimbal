/*! \file
 *  \brief Basic primtive typedefs and forward declarations
 *  \ingroup dataTypes
 */
#ifndef GIMBAL_TYPEDEFS_H
#define GIMBAL_TYPEDEFS_H

#include <stdint.h>
#include <stdlib.h>

#define GBL_TRUE                1
#define GBL_FALSE               0
#define GBL_INDEX_INVALID       SIZE_MAX
#define GBL_NPOS                ((GblSize)-1)

#ifndef __DREAMCAST__
#   define GBL_SIZE_FMT         "zu"
#else
#   define GBL_SIZE_FMT         "u"
#endif

// ======== Primitive Types ==========

typedef uint8_t     GblBool;
typedef size_t      GblSize;
typedef uint32_t    GblEnum;
typedef uint32_t    GblFlags;
typedef uint32_t    GblHash;
typedef uint16_t    GblRefCount;
typedef uint32_t    GblVersion;
typedef void        (*GblFnPtr)();
typedef union GblPtr {
    GblFnPtr        pFunc;
    void*           pData;
} GblPtr;


#endif // GIMBAL_TYPEDEFS_H
