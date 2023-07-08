/*! \file
 *  \brief Basic primtive typedefs and forward declarations
 *  \ingroup core
 *
 *  \copyright 2023 Falco Girgis
 */
#ifndef GIMBAL_TYPEDEFS_H
#define GIMBAL_TYPEDEFS_H

#include <stdint.h>
#include <stdlib.h>

#define GBL_TRUE            1
#define GBL_FALSE           0
#define GBL_INDEX_INVALID   SIZE_MAX
#define GBL_NPOS            ((size_t )-1)

// ======== Primitive Types ==========

typedef uint8_t     GblBool;            //!< Basic boolean type, standardized to sizeof(char)
typedef char        GblChar;
typedef uint8_t     GblUint8;
typedef int8_t      GblInt8;
typedef uint16_t    GblUint16;
typedef int16_t     GblInt16;
typedef uint32_t    GblUint32;
typedef int32_t     GblInt32;
typedef uint64_t    GblUint64;
typedef int64_t     GblInt64;
typedef size_t      size_t;
typedef float       GblFloat;
typedef double      GblDouble;
typedef void*       GblPointer;
typedef void      (*GblFunction)();
typedef const char* GblString;
typedef uint32_t    GblEnum;        //!< Standard-sized enum type, 32-bits across platforms
typedef uint32_t    GblFlags;       //!< Standard-sized flags type, 32-bits across platforms

// ==========  Auxiliary Types ==========

typedef uint32_t    GblHash;        //!< Type representing a calculated numeric hash across the codebase
typedef uint16_t    GblRefCount;    //!< Type able to hold a reference counter across the codebase

typedef void (*GblFnPtr)(); //!< Type used for holding an untyped function pointer

//! Union for SAFELY holding untyped data or function pointers
typedef union GblPtr {
    void*    pData; //!< Data pointer
    GblFnPtr pFunc; //!< Function pointer
} GblPtr;


#endif // GIMBAL_TYPEDEFS_H

