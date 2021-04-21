#ifndef GIMBAL_RESULT_H
#define GIMBAL_RESULT_H

#include "gimbal_macros.h"

#ifdef __cplusplus
extern "C" {
#endif


#define GBL_RESULT_TYPE_MASK 0xfff00000

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
    GBL_RESULT_ERROR_INVALID_POINTER            = GBL_RESULT_ERROR | 2,
    GBL_RESULT_ERROR_INVALID_ARG                = GBL_RESULT_ERROR | 3,
    GBL_RESULT_ERROR_MEM_ALLOC                  = GBL_RESULT_ERROR | 4,
    GBL_RESULT_ERROR_MEM_REALLOC                = GBL_RESULT_ERROR | 5,
    GBL_RESULT_ERROR_MEM_FREE                   = GBL_RESULT_ERROR | 6,
    GBL_RESULT_ERROR_FILE_OPEN                  = GBL_RESULT_ERROR | 7,
    GBL_RESULT_ERROR_FILE_READ                  = GBL_RESULT_ERROR | 8,
    GBL_RESULT_ERROR_FILE_WRITE                 = GBL_RESULT_ERROR | 9,
    GBL_RESULT_ERROR_FILE_CLOSE                 = GBL_RESULT_ERROR | 10,
    GBL_RESULT_ERROR_UNDERFLOW                  = GBL_RESULT_ERROR | 11, //integers, log depth, api cookie/context
    GBL_RESULT_ERROR_OVERFLOW                   = GBL_RESULT_ERROR | 12,
    GBL_RESULT_ERROR_VSNPRINTF                  = GBL_RESULT_ERROR | 13,
    GBL_RESULT_ERROR_INTERNAL_ASSERT            = GBL_RESULT_ERROR | 14,
    GBL_RESULT_ERROR_VARIANT_TYPE_INCOMPATIBLE  = GBL_RESULT_ERROR | 15,
    GBL_RESULT_COUNT                            = GBL_RESULT_ERROR_VARIANT_TYPE_INCOMPATIBLE + 1
};

#define GBL_RESULT_SUCCESS(value) \
    (value == GBL_RESULT_SUCCESS)

#define GBL_RESULT_PARTIAL(value) \
    (!GBL_RESULT_SUCCESS(value) && !GBL_RESULT_ERROR(value) && value != GBL_RESULT_UNKNOWN)

#define GBL_RESULT_ERROR(value) \
    (((value) & 0xfff00000) == GBL_RESULT_ERROR)

static inline const char* gblResultString(GBL_RESULT code) {
    return "LULZ";
}



#ifdef __cplusplus
}
#endif

#endif // GIMBAL_RESULT_H
