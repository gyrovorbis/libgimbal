/*! \file
 *  \brief Basic primtive typedefs and forward declarations
 *  \ingroup core
 *
 *  \author Falco Girgis
 */
#ifndef GIMBAL_TYPEDEFS_H
#define GIMBAL_TYPEDEFS_H

#include <stdint.h>
#include <stdlib.h>

#define GBL_TRUE                1
#define GBL_FALSE               0
#define GBL_INDEX_INVALID       SIZE_MAX
#define GBL_NPOS                ((size_t )-1)

// ======== Primitive Types ==========

typedef uint8_t     GblBool;
typedef char        GblChar;
typedef uint8_t     GblUint8;
typedef int8_t      GblInt8;
typedef uint16_t    GblUint16;
typedef int16_t     GblInt16;
typedef uint32_t    GblUint32;
typedef int32_t     GblInt32;
typedef uint64_t    GblUint64;
typedef int64_t     GblInt64;
typedef size_t      GblSize;
typedef float       GblFloat;
typedef double      GblDouble;
typedef void*       GblPointer;
typedef void      (*GblFunction)();
typedef const char* GblString;
typedef uint32_t    GblEnum;
typedef uint32_t    GblFlags;

// ==========  Auxiliary Types ==========

typedef uint32_t    GblHash;
typedef uint16_t    GblRefCount;

typedef void (*GblFnPtr)();

typedef union GblPtr {
    void*    pData;
    GblFnPtr pFunc;
} GblPtr;


#endif // GIMBAL_TYPEDEFS_H

