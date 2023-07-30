/*! \file
 *  \brief GblPrimitiveClass abstract primitive base plus builtin types
 *  \ingroup meta
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_PRIMITIVES_H
#define GIMBAL_PRIMITIVES_H

#include "../types/gimbal_type.h"
#include "../ifaces/gimbal_ivariant.h"
#include "../../preprocessor/gimbal_compiler.h"

#define GBL_NIL_TYPE        (GBL_BUILTIN_TYPE(NIL))     //!< Builtin ID for nil GblVariant type
#define GBL_BOOL_TYPE       (GBL_BUILTIN_TYPE(BOOL))    //!< Builtin ID for boolean GblVariant type
#define GBL_CHAR_TYPE       (GBL_BUILTIN_TYPE(CHAR))    //!< Builtin ID for char GblVariant type
#define GBL_UINT8_TYPE      (GBL_BUILTIN_TYPE(UINT8))   //!< Builtin ID for uint8_t GblVariant type
#define GBL_INT16_TYPE      (GBL_BUILTIN_TYPE(INT16))   //!< Builtin ID for int16_t GblVariant type
#define GBL_UINT16_TYPE     (GBL_BUILTIN_TYPE(UINT16))  //!< Builtin ID for uint16_t GblVariant type
#define GBL_INT32_TYPE      (GBL_BUILTIN_TYPE(INT32))   //!< Builtin ID for int32_t GblVariant type
#define GBL_UINT32_TYPE     (GBL_BUILTIN_TYPE(UINT32))  //!< Builtin ID for uint32_t GblVariant type
#define GBL_INT64_TYPE      (GBL_BUILTIN_TYPE(INT64))   //!< Builtin ID for int64_t GblVariant type
#define GBL_UINT64_TYPE     (GBL_BUILTIN_TYPE(UINT64))  //!< Builtin ID for uint64_t GblVariant type
#define GBL_FLOAT_TYPE      (GBL_BUILTIN_TYPE(FLOAT))   //!< Builtin ID for float GblVariant type
#define GBL_DOUBLE_TYPE     (GBL_BUILTIN_TYPE(DOUBLE))  //!< Builtin ID for double GblVariant type
#define GBL_STRING_TYPE     (GBL_BUILTIN_TYPE(STRING))  //!< Builtin ID for string GblVariant type
#define GBL_TYPE_TYPE       (GBL_TYPEID(GblType))       //!< Builtin ID for ::GblType GblVariant type
#define GBL_BITMASK_TYPE    (GBL_TYPEID(GblBitMask))

#ifdef GBL_64BIT
#   define GBL_SIZE_TYPE GBL_UINT64_TYPE
#else
#   define GBL_SIZE_TYPE GBL_UINT32_TYPE
#endif

#define GblNil_type()       GBL_NIL_TYPE
#define GblBool_type()      GBL_BOOL_TYPE
#define GblChar_type()      GBL_CHAR_TYPE
#define GblUint8_type()     GBL_UINT8_TYPE
#define GblInt16_type()     GBL_INT16_TYPE
#define GblUint16_type()    GBL_UINT16_TYPE
#define GblInt32_type()     GBL_INT32_TYPE
#define GblUint32_type()    GBL_UINT32_TYPE
#define GblInt64_type()     GBL_INT64_TYPE
#define GblUint64_type()    GBL_UINT64_TYPE
#define GblFloat_type()     GBL_FLOAT_TYPE
#define GblDouble_type()    GBL_DOUBLE_TYPE
#define GblString_type()    GBL_STRING_TYPE
#define GblSize_type()      GBL_SIZE_TYPE

/*! \struct GblPrimitiveClass
 *  \implements GblIVariantClass
 *  \brief Static GblClass structure for any primitive tyes
 *
 *  No public members.
 */
GBL_CLASS_BASE_EMPTY(GblPrimitive, GblIVariant)

typedef uint64_t GblBitmask;

GBL_EXPORT GblType GblType_type    (void) GBL_NOEXCEPT;
GBL_EXPORT GblType GblBitmask_type (void) GBL_NOEXCEPT;

GBL_EXPORT GblType GblPrimitive_register (const char*               pName,
                                          size_t                    classSize,
                                          size_t                    classPrivateSize,
                                          const GblIVariantVTable*  pIVTable,
                                          GblFlags                  typeFlags) GBL_NOEXCEPT;

GBL_EXPORT GblType GblPrimitive_registerBuiltin
                                         (size_t                    index,
                                          const char*               pName,
                                          size_t                    classSize,
                                          size_t                    classPrivateSize,
                                          const GblIVariantVTable*  pIVTable,
                                          GblFlags                  typeFlags) GBL_NOEXCEPT;


#endif // GIMBAL_PRIMITIVES_H
