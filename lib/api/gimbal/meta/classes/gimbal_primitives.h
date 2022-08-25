/*! \file
 *  \brief GblPrimitiveClass abstract primitive base plus builtin types
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_PRIMITIVES_H
#define GIMBAL_PRIMITIVES_H

#include "../types/gimbal_type.h"
#include "../ifaces/gimbal_ivariant.h"


/// \addtogroup metaBuiltinTypes
///@{
#define GBL_NIL_TYPE                        (GBL_BUILTIN_TYPE(NIL))     ///< \details Builtin ID for nil GblVariant type
#define GBL_BOOL_TYPE                       (GBL_BUILTIN_TYPE(BOOL))    ///< \details Builtin ID for boolean GblVariant type
#define GBL_CHAR_TYPE                       (GBL_BUILTIN_TYPE(CHAR))    ///< \details Builtin ID for char GblVariant type
#define GBL_UINT8_TYPE                      (GBL_BUILTIN_TYPE(UINT8))   ///< \details Builtin ID for uint8_t GblVariant type
#define GBL_INT16_TYPE                      (GBL_BUILTIN_TYPE(INT16))   ///< \details Builtin ID for int16_t GblVariant type
#define GBL_UINT16_TYPE                     (GBL_BUILTIN_TYPE(UINT16))  ///< \details Builtin ID for uint16_t GblVariant type
#define GBL_INT32_TYPE                      (GBL_BUILTIN_TYPE(INT32))   ///< \details Builtin ID for int32_t GblVariant type
#define GBL_UINT32_TYPE                     (GBL_BUILTIN_TYPE(UINT32))  ///< \details Builtin ID for uint32_t GblVariant type
#define GBL_INT64_TYPE                      (GBL_BUILTIN_TYPE(INT64))   ///< \details Builtin ID for int64_t GblVariant type
#define GBL_UINT64_TYPE                     (GBL_BUILTIN_TYPE(UINT64))  ///< \details Builtin ID for uint64_t GblVariant type
#define GBL_FLOAT_TYPE                      (GBL_BUILTIN_TYPE(FLOAT))   ///< \details Builtin ID for float GblVariant type
#define GBL_DOUBLE_TYPE                     (GBL_BUILTIN_TYPE(DOUBLE))  ///< \details Builtin ID for double GblVariant type
#define GBL_STRING_TYPE                     (GBL_BUILTIN_TYPE(STRING))  ///< \details Builtin ID for string GblVariant type
#define GBL_TYPE_TYPE                       (GBL_BUILTIN_TYPE(TYPE))    ///< \details Builtin ID for ::GblType GblVariant type
///@}


GBL_CLASS_BASE_EMPTY(GblPrimitive, GblIVariant)

GblType GblPrimitive_registerBuiltin(GblSize                        index,
                                     const char*                    pName,
                                     GblSize                        classSize,
                                     GblSize                        classPrivateSize,
                                     const GblIVariantIFaceVTable*  pIVTable,
                                     GblFlags                       typeFlags)       GBL_NOEXCEPT;


#endif // GIMBAL_PRIMITIVES_H
