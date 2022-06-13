#ifndef GIMBAL_PRIMITIVES_H
#define GIMBAL_PRIMITIVES_H

#include "gimbal_type.h"
#include "../ifaces/gimbal_ivariant.h"

#define GBL_NIL_TYPE                        (GBL_BUILTIN_TYPE(NIL))
#define GBL_BOOL_TYPE                       (GBL_BUILTIN_TYPE(BOOL))
#define GBL_CHAR_TYPE                       (GBL_BUILTIN_TYPE(CHAR))
#define GBL_UINT8_TYPE                      (GBL_BUILTIN_TYPE(UINT8))
#define GBL_INT16_TYPE                      (GBL_BUILTIN_TYPE(INT16))
#define GBL_UINT16_TYPE                     (GBL_BUILTIN_TYPE(UINT16))
#define GBL_INT32_TYPE                      (GBL_BUILTIN_TYPE(INT32))
#define GBL_UINT32_TYPE                     (GBL_BUILTIN_TYPE(UINT32))
#define GBL_INT64_TYPE                      (GBL_BUILTIN_TYPE(INT64))
#define GBL_UINT64_TYPE                     (GBL_BUILTIN_TYPE(UINT64))
#define GBL_FLOAT_TYPE                      (GBL_BUILTIN_TYPE(FLOAT))
#define GBL_DOUBLE_TYPE                     (GBL_BUILTIN_TYPE(DOUBLE))
#define GBL_STRING_TYPE                     (GBL_BUILTIN_TYPE(STRING))
#define GBL_TYPE_TYPE                       (GBL_BUILTIN_TYPE(TYPE))

typedef struct GblPrimitiveClass {
    GblClass            base;
    GblIVariantIFace    iVariantIFace;
} GblPrimitiveClass;

GblType GblPrimitive_registerBuiltin(GblSize                        index,
                                     const char*                    pName,
                                     GblSize                        classSize,
                                     GblSize                        classPrivateSize,
                                     const GblIVariantIFaceVTable*  pIVTable,
                                     GblFlags                       typeFlags)       GBL_NOEXCEPT;


#endif // GIMBAL_PRIMITIVES_H
