#ifndef GIMBAL_RESULT_H
#define GIMBAL_RESULT_H

#include "gimbal_macros.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
1) autogenerate count
2) helpers for setting value
    - previous + someShit
    - previous | someShit
3) Automatically prefix C values with name
4) Handle user types (as + some shit)
5) Default value for C/++ or Lua?
*/
#define GBL_META_RESULT_TABLE (                                                                                                                     \
        ( GBL_RESULT, Result, "C API Return Status Code" ),                                                                                         \
        (                                                                                                                                           \
            (GBL_RESULT_UNKNOWN,                        0x0,                    Unknown,                "Unknown"),                                 \
            (GBL_RESULT_SUCCESS,                        0x1,                    Success,                "Success"),                                 \
            (GBL_RESULT_PARTIAL,                        0xa,                    Partial,                "Generic Partial Success"),                 \
            (GBL_RESULT_TRUNCATED,                      GBL_RESULT_PARTIAL + 1, Truncated,              "Truncated"),                               \
            (GBL_RESULT_LOSSY_CONVERSION,               GBL_RESULT_PARTIAL + 2, LossyConversion,        "Lossy Conversion"),                        \
            (GBL_RESULT_TIMEOUT,                        GBL_RESULT_PARTIAL + 3, Timeout,                "Timeout"),                                 \
            (GBL_RESULT_NOT_READY,                      GBL_RESULT_PARTIAL + 4, NotReady,               "Not Ready"),                               \
            (GBL_RESULT_NOT_FOUND,                      GBL_RESULT_PARTIAL + 5, NotFound,               "Not Found"),                               \
            (GBL_RESULT_INCOMPLETE,                     GBL_RESULT_PARTIAL + 6, Incomplete,             "Incomplete"),                              \
            (GBL_RESULT_UNIMPLEMENTED,                  GBL_RESULT_PARTIAL + 7, Unimplemented,          "Unimplemented"),                           \
            (GBL_RESULT_UNSUPPORTED,                    GBL_RESULT_PARTIAL + 8, Unsupported,            "Unsupported"),                             \
            (GBL_RESULT_VERSION_MISMATCH,               GBL_RESULT_PARTIAL + 9, VersionMismatch,        "Version Mismatch"),                        \
            (GBL_RESULT_ERROR,                          0xbad00000,             Error,                  "Generic Error"),                           \
            (GBL_RESULT_ERROR_INTERNAL,                 GBL_RESULT_ERROR | 1,   ErrorInternal,          "Unexpected Internal Error"),               \
            (GBL_RESULT_ERROR_TYPE_MISMATCH,            GBL_RESULT_ERROR | 2,   ErrorTypeMismatch,      "Type Mismatch"),                           \
            (GBL_RESULT_ERROR_UNDERFLOW,                GBL_RESULT_ERROR | 3,   ErrorUnderflow,         "Underflow"),                               \
            (GBL_RESULT_ERROR_OVERFLOW,                 GBL_RESULT_ERROR | 4,   ErrorOverflow,          "Overflow"),                                \
            (GBL_RESULT_ERROR_INVALID_HANDLE,           GBL_RESULT_ERROR | 10,  InvalidHandle,          "Invalid Handle"),                          \
            (GBL_RESULT_ERROR_INVALID_POINTER,          GBL_RESULT_ERROR | 11,  InvalidPointer,         "Invalid Pointer"),                         \
            (GBL_RESULT_ERROR_INVALID_ARG,              GBL_RESULT_ERROR | 12,  InvalidArg,             "Invalid Argument"),                        \
            (GBL_RESULT_ERROR_INVALID_EXPR,             GBL_RESULT_ERROR | 13,  InvalidExpr,            "Invalid Expression"),                      \
            (GBL_RESULT_ERROR_MEM_ALLOC,                GBL_RESULT_ERROR | 14,  MemAlloc,               "Memory Allocation Failed"),                \
            (GBL_RESULT_ERROR_MEM_REALLOC,              GBL_RESULT_ERROR | 15,  MemRealloc,             "Memory Reallocation Failed"),              \
            (GBL_RESULT_ERROR_MEM_FREE,                 GBL_RESULT_ERROR | 16,  MemFree,                "Memory Free Failed"),                      \
            (GBL_RESULT_ERROR_FILE_OPEN,                GBL_RESULT_ERROR | 17,  FileOpen,               "Failed to Open File Descriptor"),          \
            (GBL_RESULT_ERROR_FILE_CLOSE,               GBL_RESULT_ERROR | 18,  FileClose,              "Failed to Close File Descriptor"),         \
            (GBL_RESULT_ERROR_FILE_READ,                GBL_RESULT_ERROR | 19,  FileRead,               "Failed to Read from File Descriptor"),     \
            (GBL_RESULT_ERROR_FILE_WRITE,               GBL_RESULT_ERROR | 20,  FileWrite,              "Failed to Write to File Descriptor"),      \
            (GBL_RESULT_COUNT,                          GBL_RESULT_ERROR | 21,  Count,                   "# of Different Error Codes")              \
        )                                                                                                                                           \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_RESULT_TABLE);

#define GBL_RESULT_UNKNOWN(value) \
    (value == GBL_RESULT_UNKNOWN)

#define GBL_RESULT_SUCCESS(value) \
    (value == GBL_RESULT_SUCCESS)

#define GBL_RESULT_PARTIAL(value) \
    (!(GBL_RESULT_SUCCESS(value) || GBL_RESULT_ERROR(value) || GBL_RESULT_UNKNOWN(value)))

#define GBL_RESULT_UNAVAILABLE(value) \
    (value == GBL_RESULT_UNIMPLEMENTED || value == GBL_RESULT_UNSUPPORTED || value == GBL_RESULT_VERSION_MISMATCH)

#define GBL_RESULT_ERROR(value) \
    (((value) & 0xfff00000) == GBL_RESULT_ERROR)

GBL_MAYBE_UNUSED GBL_INLINE const char* gblResultString(GBL_RESULT code) {
    GBL_ENUM_TABLE_TO_STRING(GBL_META_RESULT_TABLE, code);
    return "NULL";
}



#ifdef __cplusplus
}
#endif

#endif // GIMBAL_RESULT_H
