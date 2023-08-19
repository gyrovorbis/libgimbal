/*! \file
 *  \brief GblVariant structure and related functions
 *  \ingroup meta
 *
 *  This file contains the GblVariant type and its associated API.
 *  GblVariant is the core, fundamental dynamically-typed structure
 *  capable of representing any type that is known to the type system
 *  uniformly.
 *
 *  The convenience wrappers provide a wide variety of methods around
 *  a set of fundamental, low-level calls, which perform standard
 *  GblVariant lifetime and value management in a type-generic way:
 *
 *      GblVariant v;
 *
 *      // Always construct a variant before using it
 *      GblVariant_constructValueCopy(&v GBL_FLOAT_TYPE, -17.0f);
 *
 *      // You can now fetch its value as a float
 *      float f = 0.0f;
 *      GblVariant_valueCopy(&v, GBL_FLOAT_TYPE, &f); // or GblVariant_float()
 *      GBL_ASSERT(f == -17.0f);
 *
 *      // You can now freely assign and change its value
 *      // Lets do so by "moving" a reference to a GblObject into it
 *      GblVariant_setValueMove(&v, GBL_OBJECT_TYPE, GBL_NEW(GblObject));
 *
 *      // ALWAYS call the destructor when it leaves scope to free resources
 *      // this will release the otherwise leaked reference to the new GblObject
 *      GblVariant_destruct(&v);
 *
 *  \todo
 *      - function call operator()? Or call into a Closure?
 *      - serializing/deserializing
 *      - Ensure unregistering a type also removes its converters
 *      - when you have an instance/box/object type, have to propagate inner type to box's outer type
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_VARIANT_H
#define GIMBAL_VARIANT_H

#include "../../core/gimbal_typedefs.h"
#include "../../core/gimbal_ctx.h"
#include "../../strings/gimbal_string_ref.h"
#include "gimbal_pointer.h"
#include "../classes/gimbal_primitives.h"
#include "../classes/gimbal_enum.h"
#include "../classes/gimbal_flags.h"
#include "../classes/gimbal_opaque.h"
#include "../instances/gimbal_instance.h"

//! Convience macro for GblVariant value initialization
#define GBL_VARIANT_INIT                        { .type = GBL_INVALID_TYPE }
//! Convience macro for declaring and initalizing a GblVariant
#define GBL_VARIANT(name)                       GblVariant name = GBL_VARIANT_INIT

//! Convenience macro providing a generically-typed constructor method
#define GblVariant_construct(/*pVariant,*/ ...) GblVariant_construct_(__VA_ARGS__)
 //! Convenience macro providing a generically-typed assignment method
#define GblVariant_set(pVariant, ...)           GblVariant_set_(pVariant, __VA_ARGS__)

#define GBL_SELF_TYPE GblVariant

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblBox)
GBL_FORWARD_DECLARE_STRUCT(GblDateTime)

//! Function signature for a type converter to be used with GblVariant_registerConverter()
typedef GBL_RESULT (*GblVariantConverterFn)(GBL_CSELF, GblVariant* pOther);

/*! \brief Contains a single generic, dynamically typed value
 *  \ingroup meta
 *
 *  GblVariant is a type-tagged union capable of representing
 *  any type within the type system which implements the \ref
 *  GblIVariantClass interface.
 *
 *  For the table-like accessors such as GblVariant_index() and
 *  GblVariant_setIndex(), the type must additionally implement
 *  the \ref GblITableVariantClass interface.
 *
 *  \warning
 *  All fields within the structure are PRIVATE. You should
 *  ALWAYS use the API, which carefully manages lifetimes,
 *  conversions, validation, range-checking, and more rather
 *  than reaching directly for its members. The exception to
 *  this rule is when writing an actual implementation of
 *  GblIVariantClass for a type.
 *
 *  \sa GblIVariantClass, GblITableVariantClass
 */
typedef struct GblVariant {
    GblType             type;        //!< GblType UUID
    union {
        char            character;   //!< char value
        GblBool         boolean;     //!< boolean value
        uint8_t         u8;          //!< uint8_t value
        uint16_t        u16;         //!< uint16_t value
        int16_t         i16;         //!< int16_t value
        uint32_t        u32;         //!< uint32_t value
        int32_t         i32;         //!< int32_t value
        uint64_t        u64;         //!< uint64_t value
        int64_t         i64;         //!< int64_t value
        GblEnum         enumeration; //!< enum value
        GblFlags        flags;       //!< GblFlags value
        float           f32;         //!< float avlue
        double          f64;         //!< double value
        void*           pVoid;       //!< void* value
        GBL_RESULT      result;      //!< GBL_RESULT value
        GblFnPtr        pFnPtr;      //!< function pointer value
        GblStringRef*   pString;     //!< GblStringRef value
        GblType         typeValue;   //!< GblType value
        GblBitmask      bitmask;     //!< GblBitmask value
        GblDateTime*    pDateTime;   //!< GblDateTime value
        GblInstance*    pInstance;   //!< GblInstance* value
        GblBox*         pBox;        //!< GblBox* value
        GblObject*      pObject;     //!< GblObject* value
    };
} GblVariant;

//! Checks whether the given value can be represented by a GblVariant (implements GblIVariant)
GBL_EXPORT GblBool GblVariant_checkTypeCompatible (GblType type) GBL_NOEXCEPT;

/*! \name Constructors
 *  \brief Methods for various types of construction
 *  \relatesalso GblVariant
 *  @{
 */
//! Invokes the default constructor for the given type
GBL_EXPORT GBL_RESULT GblVariant_constructDefault     (GBL_SELF, GblType type)        GBL_NOEXCEPT;
//! Invokes the copy constructor, constructing then copying the \p pOther variant
GBL_EXPORT GBL_RESULT GblVariant_constructCopy        (GBL_SELF,
                                                       const GblVariant* pOther)      GBL_NOEXCEPT;
//! Invokes the move constructor, constructing then moving the \p pOther variant
GBL_EXPORT GBL_RESULT GblVariant_constructMove        (GBL_SELF,
                                                       GblVariant* pOther)            GBL_NOEXCEPT;
//! Invokes the value copy constructor, constructing then copying the given value
GBL_EXPORT GBL_RESULT GblVariant_constructValueCopy   (GBL_SELF, GblType type, ...)   GBL_NOEXCEPT;
//! va_list* variation of GblVariant_constructValueCopy()
GBL_EXPORT GBL_RESULT GblVariant_constructValueCopyVa (GBL_SELF,
                                                       GblType type,
                                                       va_list* pList)                GBL_NOEXCEPT;
//! Invokes the value move constructor, constructing then  moving the given value
GBL_EXPORT GBL_RESULT GblVariant_constructValueMove   (GBL_SELF, GblType type, ...)   GBL_NOEXCEPT;
//! va_list* variation of GblVariant_constructValueMove()
GBL_EXPORT GBL_RESULT GblVariant_constructValueMoveVa (GBL_SELF,
                                                       GblType type,
                                                       va_list* pList)                GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with a NIL value
GBL_EXPORT GBL_RESULT GblVariant_constructNil         (GBL_SELF)                      GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with a bool value
GBL_EXPORT GBL_RESULT GblVariant_constructBool        (GBL_SELF, GblBool value)       GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with a char value
GBL_EXPORT GBL_RESULT GblVariant_constructChar        (GBL_SELF, char value)          GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with a uint8_t value
GBL_EXPORT GBL_RESULT GblVariant_constructUint8       (GBL_SELF, uint8_t value)       GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with a uint16 value
GBL_EXPORT GBL_RESULT GblVariant_constructUint16      (GBL_SELF, uint16_t value)      GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with an int16 value
GBL_EXPORT GBL_RESULT GblVariant_constructInt16       (GBL_SELF, int16_t value)       GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with a uint32 value
GBL_EXPORT GBL_RESULT GblVariant_constructUint32      (GBL_SELF, uint32_t value)      GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with an int32 value
GBL_EXPORT GBL_RESULT GblVariant_constructInt32       (GBL_SELF, int32_t value)       GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with a uint64 value
GBL_EXPORT GBL_RESULT GblVariant_constructUint64      (GBL_SELF, uint64_t value)      GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with an int64 value
GBL_EXPORT GBL_RESULT GblVariant_constructInt64       (GBL_SELF, int64_t value)       GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with a float value
GBL_EXPORT GBL_RESULT GblVariant_constructFloat       (GBL_SELF, float value)         GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant with a double value
GBL_EXPORT GBL_RESULT GblVariant_constructDouble      (GBL_SELF, double value)        GBL_NOEXCEPT;
//! Convenience wrapper for value copy constructing a variant with a const char*
GBL_EXPORT GBL_RESULT GblVariant_constructString      (GBL_SELF, const char* pValue)  GBL_NOEXCEPT;
//! Convenience wrapper for value move constructing a variant with a GblStringRef (transferring ownership)
GBL_EXPORT GBL_RESULT GblVariant_constructStringRef   (GBL_SELF,
                                                       const GblStringRef* pStrRef)   GBL_NOEXCEPT;
//! Convenience wrapper for value copy constructing a string variant from a string view
GBL_EXPORT GBL_RESULT GblVariant_constructStringView  (GBL_SELF, GblStringView value) GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant from a GblType
GBL_EXPORT GBL_RESULT GblVariant_constructTypeValue   (GBL_SELF, GblType type)        GBL_NOEXCEPT;
//! Convenience wrapper for value copy constructing a variant with a size_t
GBL_EXPORT GBL_RESULT GblVariant_constructSize        (GBL_SELF, size_t  value)       GBL_NOEXCEPT;
//! Convenience wrapper for value copy constructing a variant with a GblDateTime
GBL_EXPORT GBL_RESULT GblVariant_constructDateTime    (GBL_SELF,
                                                       const GblDateTime* pDateTime)  GBL_NOEXCEPT;
//! Convenience wrapper for value constructing an enum derived variant with its value
GBL_EXPORT GBL_RESULT GblVariant_constructEnum        (GBL_SELF,
                                                       GblType type,
                                                       GblEnum value)                 GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a flags-derived variant with its value
GBL_EXPORT GBL_RESULT GblVariant_constructFlags       (GBL_SELF,
                                                       GblType  type,
                                                       GblFlags value)                GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a pointer-derived variant with its value
GBL_EXPORT GBL_RESULT GblVariant_constructPointer     (GBL_SELF,
                                                       GblType ptrType,
                                                       void*   pValue)                GBL_NOEXCEPT;
//! Convenience wrapper for value copy constructing a variant from an opaque-derived type pointer
GBL_EXPORT GBL_RESULT GblVariant_constructOpaqueCopy  (GBL_SELF,
                                                       GblType opaqueType,
                                                       void*   pValue)                GBL_NOEXCEPT;
//! Convenience wrapper for value move constructing a variant from an opaque-derived type pointer
GBL_EXPORT GBL_RESULT GblVariant_constructOpaqueMove  (GBL_SELF,
                                                       GblType opaqueType,
                                                       void*   pValue)                GBL_NOEXCEPT;
//! Convenience wrapper for value constructing a variant from a GblInstance pointer
GBL_EXPORT GBL_RESULT GblVariant_constructInstance    (GBL_SELF, GblInstance* pValue) GBL_NOEXCEPT;
//! Convenience wrapper for value copy constructing a variant from a GblBox, increasing its refCount
GBL_EXPORT GBL_RESULT GblVariant_constructBoxCopy     (GBL_SELF, GblBox* pValue)      GBL_NOEXCEPT;
//! Convenience wrapper for value move constructing a variant from a GblBox, taking ownership of the reference
GBL_EXPORT GBL_RESULT GblVariant_constructBoxMove     (GBL_SELF, GblBox* pValue)      GBL_NOEXCEPT;
//! Convenience wrapper for value copy constructing a variant from a GblObject, increasing its refCount
GBL_EXPORT GBL_RESULT GblVariant_constructObjectCopy  (GBL_SELF, GblObject* pObj)     GBL_NOEXCEPT;
//! Convenience wrapper for value m ove constructing a variant from a GblObject, taking ownership of the reference
GBL_EXPORT GBL_RESULT GblVariant_constructObjectMove  (GBL_SELF, GblObject* pObj)     GBL_NOEXCEPT;
//! Destroys the given variant, invoking its destructor and resetting its type and value
GBL_EXPORT GBL_RESULT GblVariant_destruct             (GBL_SELF)                      GBL_NOEXCEPT;
//! @}

/*! \name Setters
 *  \brief Methods for modifying the value of a constructed GblVariant
 *  \relatesalso GblVariant
 *  @{
 */
//! Performs a copy assignment operation, setting \p pOther to the given variant's value
GBL_EXPORT GBL_RESULT GblVariant_setCopy        (GBL_SELF,
                                                 const GblVariant* pOther)      GBL_NOEXCEPT;
//! Performs a move assignment operation, moving the given variant's value into \p pOther
GBL_EXPORT GBL_RESULT GblVariant_setMove        (GBL_SELF,  GblVariant* pOther) GBL_NOEXCEPT;
//! Performs a generic value copy assignment operation, copying the variadic argument into the variant
GBL_EXPORT GBL_RESULT GblVariant_setValueCopy   (GBL_SELF,  GblType type, ...)  GBL_NOEXCEPT;
//! va_list* variation of GblVariant_setValueCopy(), where the value is sourdced from a va_list*
GBL_EXPORT GBL_RESULT GblVariant_setValueCopyVa (GBL_SELF,
                                                 GblType  type,
                                                 va_list* pVarArgs)              GBL_NOEXCEPT;
//! Performs a generic value move assignment operation, moving the variadic argument into the variant
GBL_EXPORT GBL_RESULT GblVariant_setValueMove    (GBL_SELF,  GblType type, ...)  GBL_NOEXCEPT;
//! va_list* variation of GblVariant_setValueMove(), where the value is sourced from a va_list*
GBL_EXPORT GBL_RESULT GblVariant_setValueMoveVa  (GBL_SELF,
                                                  GblType  type,
                                                  va_list* pVarArgs)             GBL_NOEXCEPT;
//! Clears and resets the given variants type and value. ONLY call after being the source of a move operation!
GBL_EXPORT GBL_RESULT GblVariant_invalidate      (GBL_SELF)                      GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to NIL
GBL_EXPORT GBL_RESULT GblVariant_setNil          (GBL_SELF)                      GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a boolean
GBL_EXPORT GBL_RESULT GblVariant_setBool         (GBL_SELF, GblBool value)       GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a character
GBL_EXPORT GBL_RESULT GblVariant_setChar         (GBL_SELF, char value)          GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a uint8
GBL_EXPORT GBL_RESULT GblVariant_setUint8        (GBL_SELF, uint8_t value)       GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a uint16
GBL_EXPORT GBL_RESULT GblVariant_setUint16       (GBL_SELF, uint16_t value)      GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to an int16
GBL_EXPORT GBL_RESULT GblVariant_setInt16        (GBL_SELF, int16_t value)       GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a uint32
GBL_EXPORT GBL_RESULT GblVariant_setUint32       (GBL_SELF, uint32_t value)      GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to an int32
GBL_EXPORT GBL_RESULT GblVariant_setInt32        (GBL_SELF, int32_t value)       GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a uint64
GBL_EXPORT GBL_RESULT GblVariant_setUint64       (GBL_SELF, uint64_t value)      GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to an int64
GBL_EXPORT GBL_RESULT GblVariant_setInt64        (GBL_SELF, int64_t value)       GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a float
GBL_EXPORT GBL_RESULT GblVariant_setFloat        (GBL_SELF, float value)         GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a double
GBL_EXPORT GBL_RESULT GblVariant_setDouble       (GBL_SELF, double value)        GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a copy of the given string
GBL_EXPORT GBL_RESULT GblVariant_setString       (GBL_SELF, const char* pValue)  GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a copy of the given string view
GBL_EXPORT GBL_RESULT GblVariant_setStringView   (GBL_SELF, GblStringView value) GBL_NOEXCEPT;
//! Convenience wrapper move assigning the value of a constructed variant to take the given reference
GBL_EXPORT GBL_RESULT GblVariant_setStringRef    (GBL_SELF, GblStringRef* pRef)  GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a GblType
GBL_EXPORT GBL_RESULT GblVariant_setTypeValue    (GBL_SELF, GblType value)       GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to a size_t
GBL_EXPORT GBL_RESULT GblVariant_setSize         (GBL_SELF, size_t  value)       GBL_NOEXCEPT;
//! Convenience wrapper copy assigning the value of a constructed variant to a GblDateTime*
GBL_EXPORT GBL_RESULT GblVariant_setDateTime     (GBL_SELF,
                                                  const GblDateTime* pDt)        GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to that of an enum-derived type
GBL_EXPORT GBL_RESULT GblVariant_setEnum         (GBL_SELF,
                                                  GblType enumType,
                                                  GblEnum value)                 GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to that of a GblFlags-derived type
GBL_EXPORT GBL_RESULT GblVariant_setFlags        (GBL_SELF,
                                                  GblType  flagsType,
                                                  GblFlags value)                GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a constructed variant to that of a pointer-derived type
GBL_EXPORT GBL_RESULT GblVariant_setPointer      (GBL_SELF,
                                                  GblType ptrType,
                                                  void*   pValue)                GBL_NOEXCEPT;
//! Convenience wrapper copy assigning the value of a constructed variant to that of an opaque-derived type
GBL_EXPORT GBL_RESULT GblVariant_setOpaqueCopy   (GBL_SELF,
                                                  GblType opaqueType,
                                                  void*   pValue)                GBL_NOEXCEPT;
//! Convenience wrapper move assigning the value of a constructed variant to that of an opaque-derived type
GBL_EXPORT GBL_RESULT GblVariant_setOpaqueMove   (GBL_SELF,
                                                  GblType opaqueType,
                                                  void*   pValue)                GBL_NOEXCEPT;
//! Convenience wrapper assigning the value of a GblVariant to a GblInstance*
GBL_EXPORT GBL_RESULT GblVariant_setInstance     (GBL_SELF, GblInstance* pValue) GBL_NOEXCEPT;
//! Convenience wrapper which assigns the value of the given variant to that of a new reference to a GblBox
GBL_EXPORT GBL_RESULT GblVariant_setBoxCopy      (GBL_SELF, GblBox* pValue)      GBL_NOEXCEPT;
//! Convenience wrapper which assigns the value of the given variant to take ownership of a GblBox reference
GBL_EXPORT GBL_RESULT GblVariant_setBoxMove      (GBL_SELF, GblBox* pValue)      GBL_NOEXCEPT;
//! Convenience wrapper which assigns the value of the given variant to that of a new reference to a GblObject
GBL_EXPORT GBL_RESULT GblVariant_setObjectCopy   (GBL_SELF, GblObject* pValue)   GBL_NOEXCEPT;
//! Convenience wrapper which assings the value of the given variant to take ownership of a GblBox reference
GBL_EXPORT GBL_RESULT GblVariant_setObjectMove   (GBL_SELF, GblObject* pValue)   GBL_NOEXCEPT;
//! @}

//! Retrieves the name of the type of value contained by the given variant
GBL_EXPORT GblType     GblVariant_typeOf   (GBL_CSELF) GBL_NOEXCEPT;
//! Retrives the GblType UUID of the value contained by the given variant
GBL_EXPORT const char* GblVariant_typeName (GBL_CSELF) GBL_NOEXCEPT;

/*! \name Getters
 *  \brief Methods for retrieving the value of a constructed GblVariant
 *  \relatesalso GblVariant
 *  @{
 */
//! Attempts to retrieve a copy of the variant's value and store it into the provided pointer
GBL_EXPORT GBL_RESULT          GblVariant_valueCopy   (GBL_CSELF,
                                                      ...)          GBL_NOEXCEPT;
//! va_list* variant of GblVariant_valueCopy(), where the destination pointer comes from a va_list*
GBL_EXPORT GBL_RESULT          GblVariant_valueCopyVa (GBL_CSELF,
                                                       va_list* pVa) GBL_NOEXCEPT;
//! Attempts to retrieve the actual value stored within the variant and store it into the provided pointer (no movement or copying)
GBL_EXPORT GBL_RESULT          GblVariant_valuePeek   (GBL_CSELF,
                                                       ...)          GBL_NOEXCEPT;
//! va_list* variant of GblVariant_valuePeek(), where destination pointer comes from a va_list*
GBL_EXPORT GBL_RESULT          GblVariant_valuePeekVa (GBL_SELF,
                                                       va_list* pVa) GBL_NOEXCEPT;
//! Attempts to move the value stored within the variant out of it and into the provided pointer
GBL_EXPORT GBL_RESULT          GblVariant_valueMove   (GBL_SELF,
                                                      ...)          GBL_NOEXCEPT;
//! va_list* variant of GblVariant_valueMove(), where destination pointer comes from a va_list*
GBL_EXPORT GBL_RESULT          GblVariant_valueMoveVa (GBL_SELF,
                                                       va_list* pVa) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the value held by the given variant is not of GBL_INVALID_TYPE
GBL_EXPORT GblBool             GblVariant_isValid     (GBL_CSELF)    GBL_NOEXCEPT;
//! Returns GBL_TRUE if the value held by the variant is not of type GBL_NIL_TYPE
GBL_EXPORT GblBool             GblVariant_isNil       (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a bool, raising an error upon type mismatch
GBL_EXPORT GblBool             GblVariant_bool        (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a char, raising an error upon type mismatch
GBL_EXPORT char                GblVariant_char        (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a uint8, raising an error upon type mismatch
GBL_EXPORT uint8_t             GblVariant_uint8       (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a uint16, raising an error upon type mismatch
GBL_EXPORT uint16_t            GblVariant_uint16      (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as an int16, raising an error upon type mismatch
GBL_EXPORT int16_t             GblVariant_int16       (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a uint32, raising an error upon type mismatch
GBL_EXPORT uint32_t            GblVariant_uint32      (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as an int32, raising an error upon type mismatch
GBL_EXPORT int32_t             GblVariant_int32       (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a uint64, raising an error upon type mismatch
GBL_EXPORT uint64_t            GblVariant_uint64      (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as an int64, raising an error upon type mismatch
GBL_EXPORT int64_t             GblVariant_int64       (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a generic enum value, raising an error upon type mismatch
GBL_EXPORT GblEnum             GblVariant_enum        (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a generic GblFlags value, raising an error upon type mismatch
GBL_EXPORT GblFlags            GblVariant_flags       (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a float, raising an error upon type mismatch
GBL_EXPORT float               GblVariant_float       (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a double, raising an error upon type mismatch
GBL_EXPORT double              GblVariant_double      (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a string, raising an error upon type mismatch
GBL_EXPORT const GblStringRef* GblVariant_string      (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a string, returning it within a view, raising an error upon type mismatch
GBL_EXPORT GblStringView       GblVariant_stringView  (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a GblType, raising an error upon type mismatch
GBL_EXPORT GblType             GblVariant_typeValue   (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a generic pointer, raising an error upon type mismatch
GBL_EXPORT void*               GblVariant_pointer     (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a size_t, raising an error upon type mismatch
GBL_EXPORT size_t              GblVariant_size        (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a GblDateTime, raising an error upon type mismatch
GBL_EXPORT GblDateTime*        GblVariant_dateTime    (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to fetch a copy of the value of the variant as a generic opaque pointer, raising an error upon type mismatch
GBL_EXPORT void*               GblVariant_opaqueCopy  (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempst to take the value of the variant (claiming ownerships) as a generic opaque pointer, raising an error upon type mismatch
GBL_EXPORT void*               GblVariant_opaqueMove  (GBL_SELF)     GBL_NOEXCEPT;
//! Attempst to fetch the value of the variant as a generic opaque pointer, raising an error upon type mismatch
GBL_EXPORT void*               GblVariant_opaquePeek  (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempts to fetch the value of the variant as a GblInstance derived type, returning a pointer to it
GBL_EXPORT GblInstance*        GblVariant_instance    (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempts to fetch a copy of the value of the variant by making a new generic GblBox reference, raising an error upon type mismatch
GBL_EXPORT GblBox*             GblVariant_boxCopy     (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempts to take the value stored within the variant as generic GblBox pointer, claiming ownerships of it
GBL_EXPORT GblBox*             GblVariant_boxMove     (GBL_SELF)     GBL_NOEXCEPT;
//! Attempts to fetch the value stored within the variant as a pointer to a GblBox derived type
GBL_EXPORT GblBox*             GblVariant_boxPeek     (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempts to fetch a copy of the value of the variant by making a new generic GblObject reference, raising an error upon type mismatch
GBL_EXPORT GblObject*          GblVariant_objectCopy  (GBL_CSELF)    GBL_NOEXCEPT;
//! Attempts to take the value stored within the variant as generic GblObject pointer, claiming ownerships of it
GBL_EXPORT GblObject*          GblVariant_objectMove  (GBL_SELF)     GBL_NOEXCEPT;
//! Attempts to fetch the value stored within the variant as a pointer to a GblObject derived type
GBL_EXPORT GblObject*          GblVariant_objectPeek  (GBL_CSELF)    GBL_NOEXCEPT;
//! @}
//!
/*! \name Table Accessors
 *  \brief Methods for managing and manipulating variants as tables
 *  \relatesalso GblVariant
 *  @{
 */
//! Looks up the table entry with the given \p pKey, storing its value in \p pValue and also returning it
GBL_EXPORT GblVariant* GblVariant_index      (GBL_CSELF,
                                              const GblVariant* pKey,
                                              GblVariant*       pValue) GBL_NOEXCEPT;
//! Looks up the talbe entry with the given string key, storing its value in \p pValue and also returning it
GBL_EXPORT GblVariant* GblVariant_field      (GBL_CSELF,
                                              const char* pName,
                                              GblVariant* pValue)       GBL_NOEXCEPT;
//! Looks up the table entry with the given integral key, storing its value in \p pValue and also returning it
GBL_EXPORT GblVariant* GblVariant_element    (GBL_CSELF,
                                              size_t      index,
                                              GblVariant* pValue)       GBL_NOEXCEPT;
//! Sets the value of the table entry with the corresponding GblVariant key to \p pValue, returning a status code
GBL_EXPORT GBL_RESULT  GblVariant_setIndex   (GBL_SELF,
                                              const GblVariant* pKey,
                                              GblVariant*       pValue) GBL_NOEXCEPT;
//! Sets the value of the table entry with the corresponding string key to \p pValue, returning a status code
GBL_EXPORT GBL_RESULT  GblVariant_setField   (GBL_SELF,
                                              const char* pName,
                                              GblVariant* pValue)       GBL_NOEXCEPT;
//! Sets the value of the table entry with the corresponding integral key to \p pValue, returning a status code
GBL_EXPORT GBL_RESULT  GblVariant_setElement (GBL_SELF,
                                              size_t      index,
                                              GblVariant* pValue)       GBL_NOEXCEPT;
//! Given the current entry's key (or NIL for the first iteration), sets the next key as well as its value, returning GBL_FALSE after the final entry
GBL_EXPORT GblBool     GblVariant_next       (GBL_CSELF,
                                              GblVariant* pKey,
                                              GblVariant* pValue)       GBL_NOEXCEPT;
//! Returns the number of table entries within the given GblVariant
GBL_EXPORT size_t      GblVariant_count      (GBL_CSELF)                GBL_NOEXCEPT;
//! @}

/*! \name Builtin Conversions
 *  \brief Methods for converting between types
 *  \relatesalso GblVariant
 *  @{
 */
//! Convenience method that attempts to convert the variant's type to bool and return its value
GBL_EXPORT GblBool             GblVariant_toBool       (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to char and return its value
GBL_EXPORT char                GblVariant_toChar       (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to uint8 and return its value
GBL_EXPORT uint8_t             GblVariant_toUint8      (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to uint16 and return its value
GBL_EXPORT uint16_t            GblVariant_toUint16     (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to int16 and return its value
GBL_EXPORT int16_t             GblVariant_toInt16      (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to uint32 and return its value
GBL_EXPORT uint32_t            GblVariant_toUint32     (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to int32 and return its value
GBL_EXPORT int32_t             GblVariant_toInt32      (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to uint64 and return its value
GBL_EXPORT uint64_t            GblVariant_toUint64     (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to int64 and return its value
GBL_EXPORT int64_t             GblVariant_toInt64      (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to a generic enum and return its value
GBL_EXPORT GblEnum             GblVariant_toEnum       (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to a generic GblFlags and return its value
GBL_EXPORT GblFlags            GblVariant_toFlags      (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to float and return its value
GBL_EXPORT float               GblVariant_toFloat      (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to double and return its value
GBL_EXPORT double              GblVariant_toDouble     (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to pointer and return its value
GBL_EXPORT void*               GblVariant_toPointer    (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to string and return its value
GBL_EXPORT const GblStringRef* GblVariant_toString     (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to string view and return its value
GBL_EXPORT GblStringView       GblVariant_toStringView (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to GblType and return its value
GBL_EXPORT GblType             GblVariant_toTypeValue  (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to size_t and return its value
GBL_EXPORT size_t              GblVariant_toSize       (GBL_SELF) GBL_NOEXCEPT;
//! Convenience method that attempts to convert the variant's type to GblDateTime and return its value
GBL_EXPORT GblDateTime*        GblVariant_toDateTime   (GBL_SELF) GBL_NOEXCEPT;
//! @}

/*! \name Generic Conversions
 *  \brief Generic conversion registration and operations
 *  \relatesalso GblVariant
 *  @{
 */
//! Registers a converter with the type system for going from one type of variant to another
GBL_EXPORT GBL_RESULT GblVariant_registerConverter   (GblType               fromType,
                                                      GblType               toType,
                                                      GblVariantConverterFn pFnConv) GBL_NOEXCEPT;
//! Unregisters a converter with the type system from going from one type to another type of variant
GBL_EXPORT GBL_RESULT GblVariant_unregisterConverter (GblType fromType,
                                                      GblType toType)                GBL_NOEXCEPT;
//! Retrieves the current number of type conversion routines held by the type registry
GBL_EXPORT size_t     GblVariant_converterCount      (void)                          GBL_NOEXCEPT;
//! Returns GBL_TRUE if a method can be found to convert from one type to another type of variant
GBL_EXPORT GblBool    GblVariant_canConvert          (GblType fromType,
                                                      GblType toType)                GBL_NOEXCEPT;
//! Fills \p pToVariant with the value of \p pSelf, converted to the type contained by \p pToVariant
GBL_EXPORT GBL_RESULT GblVariant_convert             (GBL_CSELF,
                                                      GblVariant* pToVariant)        GBL_NOEXCEPT;
//! @}

/*! \name Utilities
 *  \brief Methods offering various operators and utility functionality
 *  \relatesalso GblVariant
 *  @{
 */
//! Peforms a comparison between the two types of variants, implicitly converting between them if necessary
GBL_EXPORT int        GblVariant_compare (GBL_CSELF,
                                          const GblVariant* pOther)    GBL_NOEXCEPT;
//! Returns GBL_TRUE if the result of the comparison between two variants is 0 (meaning both equal)
GBL_EXPORT GblBool    GblVariant_equals  (GBL_CSELF,
                                          const GblVariant* pOther)    GBL_NOEXCEPT;
//! Attempts to serialize the value stored within the given variant to a GblStringBuffer
GBL_EXPORT GBL_RESULT GblVariant_save    (GBL_CSELF,
                                          GblStringBuffer* pString)    GBL_NOEXCEPT;
//! Attempts to deserialize a value within a GblStringBuffer to store within the given variant
GBL_EXPORT GBL_RESULT GblVariant_load    (GBL_SELF,
                                          const GblStringBuffer* pStr) GBL_NOEXCEPT;
//! Calculates a 32-bit hash value corresponding to the given variant's value
GBL_EXPORT GblHash    GblVariant_hash    (GBL_CSELF)                   GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#ifndef __DREAMCAST__
#   define GBL_VARIANT_CONSTRUCT_GENERIC_PLATFORM_ENTRIES()
#else
#   define GBL_VARIANT_CONSTRUCT_GENERIC_PLATFORM_ENTRIES() \
    (int, GblVariant_constructInt32),
#endif

//! \cond
#define GBL_VARIANT_CONSTRUCT_TABLE  (                                  \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,                              \
        (                                                               \
            (char,                   GblVariant_constructChar),         \
            (uint8_t,                GblVariant_constructUint8),        \
            (uint16_t,               GblVariant_constructUint16),       \
            (int16_t,                GblVariant_constructInt16),        \
            (uint32_t,               GblVariant_constructUint32),       \
            GBL_VARIANT_CONSTRUCT_GENERIC_PLATFORM_ENTRIES()            \
            (int32_t,                GblVariant_constructInt32),        \
            (uint64_t,               GblVariant_constructUint64),       \
            (int64_t,                GblVariant_constructInt64),        \
            (float,                  GblVariant_constructFloat),        \
            (double,                 GblVariant_constructDouble),       \
            (const char*,            GblVariant_constructString),       \
            (char*,                  GblVariant_constructString),       \
            (GblStringView,          GblVariant_constructStringView),   \
            (void*,                  GblVariant_constructPointer),      \
            (const void*,            GblVariant_constructPointer),      \
            (const GblVariant*,      GblVariant_constructCopy),         \
            (GblVariant*,            GblVariant_constructCopy)          \
        )                                                               \
    )
#define GblVariant_construct_N(pVariant, ...)   GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_CONSTRUCT_TABLE, GBL_TUPLE_FIRST (__VA_ARGS__))(pVariant, __VA_ARGS__)
#define GblVariant_construct_1(pVariant)        GblVariant_constructNil(pVariant)
#define GblVariant_construct_(...)              GBL_VA_OVERLOAD_CALL(GblVariant_construct, GBL_VA_OVERLOAD_SUFFIXER_1_N, __VA_ARGS__)

#define GBL_VARIANT_SET_TABLE  (                                    \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,                          \
        (                                                           \
            (char,                   GblVariant_setChar),           \
            (uint8_t,                GblVariant_setUint8),          \
            (uint16_t,               GblVariant_setUint16),         \
            (int16_t,                GblVariant_setInt16),          \
            (uint32_t,               GblVariant_setUint32),         \
            (int32_t,                GblVariant_setInt32),          \
            (uint64_t,               GblVariant_setUint64),         \
            (int64_t,                GblVariant_setInt64),          \
            (float,                  GblVariant_setFloat),          \
            (double,                 GblVariant_setDouble),         \
            (char*,                  GblVariant_setString),         \
            (const char*,            GblVariant_setString),         \
            (GblStringView,          GblVariant_setStringView),     \
            (const void*,            GblVariant_setPointer),        \
            (void*,                  GblVariant_setPointer),        \
            (const GblVariant*,      GblVariant_setCopy),           \
            (GblVariant*,            GblVariant_setCopy)            \
        )                                                           \
    )
#define GblVariant_set_(pVariant, ...) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_SET_TABLE, GBL_TUPLE_FIRST(__VA_ARGS__))(pVariant, __VA_ARGS__)
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_VARIANT_H
