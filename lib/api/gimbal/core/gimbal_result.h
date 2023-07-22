/*! \file
 *  \brief GBL_RESULT values, stringifiers, and utilities
 *  \ingroup core
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_RESULT_H
#define GIMBAL_RESULT_H

#include "gimbal_decls.h"

GBL_DECLS_BEGIN

#define GBL_META_RESULT_TABLE (                                                                                                                         \
        ( GBL_RESULT, Result, "C API Return Status Code", gblResultString),                                                                             \
        (                                                                                                                                               \
            (GBL_RESULT_UNKNOWN,                        0x0,                    Unknown,                    "Unknown"),                                 \
            (GBL_RESULT_SUCCESS,                        0x1,                    Success,                    "Success"),                                 \
            (GBL_RESULT_EVENT_IGNORED,                  0x2,                    EventIgnored,               "Event Ignored"),                           \
            (GBL_RESULT_EVENT_ACCEPTED,                 0x3,                    EventAccepted,              "Event Accepted"),                          \
            (GBL_RESULT_PARTIAL,                        0xa,                    Partial,                    "Generic Partial Success"),                 \
            (GBL_RESULT_TRUNCATED,                      GBL_RESULT_PARTIAL + 1, Truncated,                  "Truncated"),                               \
            (GBL_RESULT_LOSSY_CONVERSION,               GBL_RESULT_PARTIAL + 2, LossyConversion,            "Lossy Conversion"),                        \
            (GBL_RESULT_TIMEOUT,                        GBL_RESULT_PARTIAL + 3, Timeout,                    "Timeout"),                                 \
            (GBL_RESULT_NOT_READY,                      GBL_RESULT_PARTIAL + 4, NotReady,                   "Not Ready"),                               \
            (GBL_RESULT_NOT_FOUND,                      GBL_RESULT_PARTIAL + 5, NotFound,                   "Not Found"),                               \
            (GBL_RESULT_INCOMPLETE,                     GBL_RESULT_PARTIAL + 6, Incomplete,                 "Incomplete"),                              \
            (GBL_RESULT_UNIMPLEMENTED,                  GBL_RESULT_PARTIAL + 7, Unimplemented,              "Unimplemented"),                           \
            (GBL_RESULT_UNSUPPORTED,                    GBL_RESULT_PARTIAL + 8, Unsupported,                "Unsupported"),                             \
            (GBL_RESULT_VERSION_MISMATCH,               GBL_RESULT_PARTIAL + 9, VersionMismatch,            "Version Mismatch"),                        \
            (GBL_RESULT_SKIPPED,                        GBL_RESULT_PARTIAL + 10,Skipped,                    "Skipped"),                                 \
            (GBL_RESULT_ERROR,                          0xbad00000,             Error,                      "Generic Error"),                           \
            (GBL_RESULT_ERROR_UNHANDLED_EXCEPTION,      GBL_RESULT_ERROR | 1,   ErrorUnhandledException,    "Unhandled Exception"),                     \
            (GBL_RESULT_ERROR_INTERNAL,                 GBL_RESULT_ERROR | 2,   ErrorInternal,              "Internal Error"),                          \
            (GBL_RESULT_ERROR_TYPE_MISMATCH,            GBL_RESULT_ERROR | 3,   ErrorTypeMismatch,          "Type Mismatch"),                           \
            (GBL_RESULT_ERROR_INVALID_VARIANT_ACCESS,   GBL_RESULT_ERROR | 4,   ErrorInvalidVariantAccess,  "Invalid Variant Access"),                  \
            (GBL_RESULT_ERROR_UNDERFLOW,                GBL_RESULT_ERROR | 5,   ErrorUnderflow,             "Underflow"),                               \
            (GBL_RESULT_ERROR_OVERFLOW,                 GBL_RESULT_ERROR | 6,   ErrorOverflow,              "Overflow"),                                \
            (GBL_RESULT_ERROR_OUT_OF_RANGE,             GBL_RESULT_ERROR | 7,   ErrorOutOfRange,            "Out of Range"),                            \
            (GBL_RESULT_ERROR_INVALID_HANDLE,           GBL_RESULT_ERROR | 8,   ErrorInvalidHandle,         "Invalid Handle"),                          \
            (GBL_RESULT_ERROR_INVALID_POINTER,          GBL_RESULT_ERROR | 9,   ErrorInvalidPointer,        "Invalid Pointer"),                         \
            (GBL_RESULT_ERROR_INVALID_ARG,              GBL_RESULT_ERROR | 10,  ErrorInvalidArg,            "Invalid Argument"),                        \
            (GBL_RESULT_ERROR_INVALID_CMDLINE_ARG,      GBL_RESULT_ERROR | 11,  ErrorInvalidCmdLineArg,     "Invalid CmdLine Argument"),                \
            (GBL_RESULT_ERROR_INVALID_EXPRESSION,       GBL_RESULT_ERROR | 12,  ErrorInvalidExpression,     "Invalid Expression"),                      \
            (GBL_RESULT_ERROR_INVALID_OPERATION,        GBL_RESULT_ERROR | 13,  ErrorInvalidOperation,      "Invalid Operation"),                       \
            (GBL_RESULT_ERROR_INVALID_CONVERSION,       GBL_RESULT_ERROR | 14,  ErrorInvalidConversion,     "Invalid Conversion"),                      \
            (GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL,     GBL_RESULT_ERROR | 15,  ErrorInvalidVirtualCall,    "Invalid Virtual Function Called"),         \
            (GBL_RESULT_ERROR_INVALID_KEY,              GBL_RESULT_ERROR | 16,  ErrorInvalidKey,            "Invalid Key"),                             \
            (GBL_RESULT_ERROR_INVALID_PROPERTY,         GBL_RESULT_ERROR | 17,  ErrorInvalidProperty,       "Invalid Property"),                        \
            (GBL_RESULT_ERROR_INVALID_INSTANCE,         GBL_RESULT_ERROR | 18,  ErrorInvalidInstance,       "Invalid Instance"),                        \
            (GBL_RESULT_ERROR_INVALID_CLASS,            GBL_RESULT_ERROR | 19,  ErrorInvalidClass,          "Invalid Class"),                           \
            (GBL_RESULT_ERROR_INVALID_TYPE,             GBL_RESULT_ERROR | 20,  ErrorInvalidType,           "Invalid Type"),                            \
            (GBL_RESULT_ERROR_INVALID_DATE_TIME,        GBL_RESULT_ERROR | 21,  ErrorInvalidDateTime,       "Invalid Date/Time"),                       \
            (GBL_RESULT_ERROR_INVALID_THREAD,           GBL_RESULT_ERROR | 22,  ErrorInvalidThread,         "Invalid Thread"),                          \
            (GBL_RESULT_ERROR_INVALID_TOKEN,            GBL_RESULT_ERROR | 23,  ErrorInvalidToken,          "Invalid Token"),                           \
            (GBL_RESULT_ERROR_MEM_ALLOC,                GBL_RESULT_ERROR | 24,  ErrorMemAlloc,              "Memory Allocation Failed"),                \
            (GBL_RESULT_ERROR_MEM_REALLOC,              GBL_RESULT_ERROR | 25,  ErrorMemRealloc,            "Memory Reallocation Failed"),              \
            (GBL_RESULT_ERROR_MEM_FREE,                 GBL_RESULT_ERROR | 26,  ErrorMemFree,               "Memory Free Failed"),                      \
            (GBL_RESULT_ERROR_FILE_OPEN,                GBL_RESULT_ERROR | 27,  ErrorFileOpen,              "Failed to Open File Descriptor"),          \
            (GBL_RESULT_ERROR_FILE_CLOSE,               GBL_RESULT_ERROR | 28,  ErrorFileClose,             "Failed to Close File Descriptor"),         \
            (GBL_RESULT_ERROR_FILE_READ,                GBL_RESULT_ERROR | 29,  ErrorFileRead,              "Failed to Read from File Descriptor"),     \
            (GBL_RESULT_ERROR_FILE_WRITE,               GBL_RESULT_ERROR | 30,  ErrorFileWrite,             "Failed to Write to File Descriptor"),      \
            (GBL_RESULT_COUNT,                          GBL_RESULT_ERROR | 31,  Count,                      "# of Different Error Codes")               \
        )                                                                                                                                               \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_RESULT_TABLE)

#define GBL_RESULT_UNKNOWN(value) \
    ((GBL_RESULT)value == GBL_RESULT_UNKNOWN)

#define GBL_RESULT_SUCCESS(value) \
    ((GBL_RESULT)value == GBL_RESULT_SUCCESS || (GBL_RESULT)value == GBL_RESULT_EVENT_IGNORED || (GBL_RESULT)value == GBL_RESULT_EVENT_ACCEPTED)

#define GBL_RESULT_PARTIAL(value) \
    (!(GBL_RESULT_SUCCESS(value) || GBL_RESULT_ERROR(value) || GBL_RESULT_UNKNOWN(value)))

#define GBL_RESULT_UNAVAILABLE(value) \
    ((GBL_RESULT)value == GBL_RESULT_UNIMPLEMENTED || (GBL_RESULT)value == GBL_RESULT_UNSUPPORTED || (GBL_RESULT)value == GBL_RESULT_VERSION_MISMATCH)

#define GBL_RESULT_ERROR(value) \
    ((GBL_RESULT)((value) & 0xfff00000) == GBL_RESULT_ERROR)

#define GBL_RESULT_ISSUE(value) \
    (!GBL_RESULT_SUCCESS(value))

GBL_DECLS_END

#endif // GIMBAL_RESULT_H
