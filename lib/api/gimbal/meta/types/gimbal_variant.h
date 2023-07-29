/*! \file
 *  \brief GblVariant structure and related functions
 *  \ingroup meta
 *
 *  This file contains the GblVariant type and its associated
 *  API. GblVariant is the core, fundamental dynamically-typed
 *  structure capable of representing any type that is known
 *  to the type sytem.
 *
 *  \todo
 *      - UNIX timestamp? Timespec?
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
#include <time.h>

#define GBL_VARIANT_INIT    { .type = GBL_INVALID_TYPE }
#define GBL_VARIANT(name)   GblVariant name = GBL_VARIANT_INIT

#define GBL_SELF_TYPE GblVariant

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblBox)

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
//need to postfix everything with '_' for private
typedef struct GblVariant {
    GblType             type;
    union {
        char            character;
        GblBool         boolean;
        uint8_t         u8;
        uint16_t        u16;
        int16_t         i16;
        uint32_t        u32;
        int32_t         i32;
        uint64_t        u64;
        int64_t         i64;
        GblEnum         enumeration;
        GblFlags        flags;
        float           f32;
        double          f64;
        void*           pVoid;
        GBL_RESULT      result;
        time_t          time;
        GblFnPtr        pFnPtr;
        GblStringRef*   pString;
        GblType         typeValue;
        GblInstance*    pInstance;
        GblBox*         pBox;
        GblObject*      pObject;
    };
} GblVariant;

GBL_EXPORT GblBool       GblVariant_checkTypeCompatible      (GblType type)                  GBL_NOEXCEPT;

/*! \name Lifetime Management
 *  \brief Methods for construction and destruction
 *  \relatesalso GblVariant
 *  @{
 */
GBL_EXPORT GBL_RESULT GblVariant_constructDefault         (GBL_SELF, GblType type)        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructCopy            (GBL_SELF,
                                                           const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructMove            (GBL_SELF,
                                                           GblVariant* pOther)            GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructValueCopy       (GBL_SELF, GblType type, ...)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructValueCopyVaList (GBL_SELF,
                                                           GblType type,
                                                           va_list* pList)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructValueMove       (GBL_SELF, GblType type, ...)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructValueMoveVaList (GBL_SELF,
                                                           GblType type,
                                                           va_list* pList)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructNil             (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructBool            (GBL_SELF, GblBool value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructChar            (GBL_SELF, char value)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructUint8           (GBL_SELF, uint8_t value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructUint16          (GBL_SELF, uint16_t value)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructInt16           (GBL_SELF, int16_t value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructUint32          (GBL_SELF, uint32_t value)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructInt32           (GBL_SELF, int32_t value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructUint64          (GBL_SELF, uint64_t value)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructInt64           (GBL_SELF, int64_t value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructFloat           (GBL_SELF, float value)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructDouble          (GBL_SELF, double value)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructString          (GBL_SELF, const char* pValue)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructStringView      (GBL_SELF, GblStringView value) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructTypeValue       (GBL_SELF, GblType type)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructSize            (GBL_SELF, size_t  value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructEnum            (GBL_SELF,
                                                           GblType type,
                                                           GblEnum value)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructFlags           (GBL_SELF,
                                                           GblType  type,
                                                           GblFlags value)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructPointer         (GBL_SELF,
                                                           GblType ptrType,
                                                           void*   pValue)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructOpaqueCopy      (GBL_SELF,
                                                           GblType opaqueType,
                                                           void*   pValue)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructOpaqueMove      (GBL_SELF,
                                                           GblType opaqueType,
                                                           void*   pValue)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_constructInstance        (GBL_SELF, GblInstance* pValue) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructBoxCopy         (GBL_SELF, GblBox* pValue)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructBoxMove         (GBL_SELF, GblBox* pValue)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructObjectCopy      (GBL_SELF, GblObject* pValue)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_constructObjectMove      (GBL_SELF, GblObject* pValue)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_destruct                 (GBL_SELF)                      GBL_NOEXCEPT;
//! @}


/*! \name Setters
 *  \brief Methods for modifying the value
 *  \relatesalso GblVariant
 *  @{
 */
GBL_EXPORT GBL_RESULT GblVariant_setCopy            (GBL_SELF,
                                                     const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setMove            (GBL_SELF,  GblVariant* pOther) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setValueCopy       (GBL_SELF,  GblType type, ...)  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setValueCopyVaList (GBL_SELF,
                                                     GblType  type,
                                                     va_list* pVarArgs)             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setValueMove        (GBL_SELF,  GblType type, ...)  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setValueMoveVaList  (GBL_SELF,
                                                      GblType  type,
                                                      va_list* pVarArgs)             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_invalidate          (GBL_SELF)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setNil              (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setBool             (GBL_SELF, GblBool value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setChar             (GBL_SELF, char value)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setUint8            (GBL_SELF, uint8_t value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setUint16           (GBL_SELF, uint16_t value)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setInt16            (GBL_SELF, int16_t value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setUint32           (GBL_SELF, uint32_t value)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setInt32            (GBL_SELF, int32_t value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setUint64           (GBL_SELF, uint64_t value)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setInt64            (GBL_SELF, int64_t value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setFloat            (GBL_SELF, float value)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setDouble           (GBL_SELF, double value)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setString           (GBL_SELF, const char* pValue)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setStringView       (GBL_SELF, GblStringView value) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setStringRef        (GBL_SELF, GblStringRef* pRef)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setTypeValue        (GBL_SELF, GblType value)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setSize             (GBL_SELF, size_t  value)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setEnum             (GBL_SELF,
                                                      GblType enumType,
                                                      GblEnum value)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setFlags            (GBL_SELF,
                                                      GblType  flagsType,
                                                      GblFlags value)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setPointer          (GBL_SELF,
                                                      GblType ptrType,
                                                      void*   pValue)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setOpaqueCopy       (GBL_SELF,
                                                      GblType opaqueType,
                                                      void*   pValue)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setOpaqueMove       (GBL_SELF,
                                                      GblType opaqueType,
                                                      void*   pValue)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblVariant_setInstance         (GBL_SELF, GblInstance* pValue) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setBoxCopy          (GBL_SELF, GblBox* pValue)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setBoxMove          (GBL_SELF, GblBox* pValue)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setObjectCopy       (GBL_SELF, GblObject* pValue)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblVariant_setObjectMove       (GBL_SELF, GblObject* pValue)   GBL_NOEXCEPT;
//! @}


GBL_EXPORT GBL_RESULT    GblVariant_copyValue             (GBL_CSELF,  ...)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_copyValueVaList       (GBL_CSELF, va_list* pVarArgs)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_peekValue             (GBL_CSELF,  ...)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_peekValueVaList       (GBL_SELF, va_list* pVarArgs)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_moveValue             (GBL_SELF,  ...)                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    GblVariant_moveValueVaList       (GBL_SELF, va_list* pVarArgs)   GBL_NOEXCEPT;

GBL_EXPORT GblType       GblVariant_typeOf                   (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT const char*   GblVariant_typeName                 (GBL_CSELF)                     GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblVariant_isValid                  (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblVariant_isNil                    (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblVariant_getBool                  (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT char          GblVariant_getChar                  (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT uint8_t       GblVariant_getUint8                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT uint16_t      GblVariant_getUint16                (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT int16_t       GblVariant_getInt16                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT uint32_t      GblVariant_getUint32                (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT int32_t       GblVariant_getInt32                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT uint64_t      GblVariant_getUint64                (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT int64_t       GblVariant_getInt64                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblEnum       GblVariant_getEnum                  (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblFlags      GblVariant_getFlags                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT float         GblVariant_getFloat                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT double        GblVariant_getDouble                (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT const char*   GblVariant_getString                (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblVariant_getStringView            (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblVariant_getStringRef             (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblType       GblVariant_getTypeValue             (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT void*         GblVariant_getPointer               (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT size_t        GblVariant_getSize                  (GBL_CSELF)                     GBL_NOEXCEPT;

GBL_EXPORT void*         GblVariant_getOpaqueCopy            (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT void*         GblVariant_getOpaqueMove            (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT void*         GblVariant_getOpaquePeek            (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblInstance*  GblVariant_getInstance              (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblBox*       GblVariant_getBoxCopy               (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblBox*       GblVariant_getBoxMove               (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblBox*       GblVariant_getBoxPeek               (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblObject*    GblVariant_getObjectCopy            (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblObject*    GblVariant_getObjectMove            (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblObject*    GblVariant_getObjectPeek            (GBL_CSELF)                     GBL_NOEXCEPT;

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

/*! \name Conversions
 *  \brief Methods for converting between types
 *  \relatesalso GblVariant
 *  @{
 */
GBL_EXPORT GblBool       GblVariant_toBool              (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT char          GblVariant_toChar              (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT uint8_t       GblVariant_toUint8             (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT uint16_t      GblVariant_toUint16            (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT int16_t       GblVariant_toInt16             (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT uint32_t      GblVariant_toUint32            (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT int32_t       GblVariant_toInt32             (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT uint64_t      GblVariant_toUint64            (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT int64_t       GblVariant_toInt64             (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblEnum       GblVariant_toEnum              (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblFlags      GblVariant_toFlags             (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT float         GblVariant_toFloat             (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT double        GblVariant_toDouble            (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT void*         GblVariant_toPointer           (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT const char*   GblVariant_toString            (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblVariant_toStringView        (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblType       GblVariant_toTypeValue         (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT size_t        GblVariant_toSize              (GBL_SELF)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_registerConverter   (GblType               fromType,
                                                         GblType               toType,
                                                         GblVariantConverterFn pFnConv) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_unregisterConverter (GblType fromType,
                                                         GblType toType)                GBL_NOEXCEPT;

GBL_EXPORT size_t        GblVariant_converterCount      (void)                          GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblVariant_canConvert          (GblType fromType,
                                                         GblType toType)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_convert             (GBL_CSELF,
                                                         GblVariant* pToVariant)        GBL_NOEXCEPT;
//! @}

GBL_EXPORT int           GblVariant_compare                  (GBL_CSELF,
                                                              const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_EXPORT GblBool       GblVariant_equals                   (GBL_CSELF,
                                                              const GblVariant* pOther)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_save                     (GBL_CSELF,
                                                              GblStringBuffer* pString)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT    GblVariant_load                     (GBL_SELF,
                                                              const GblStringBuffer* pStr)   GBL_NOEXCEPT;

GBL_EXPORT GblHash       GblVariant_hash                     (GBL_CSELF)                     GBL_NOEXCEPT;


GBL_DECLS_END

//! \cond
#define GBL_VARIANT_CONSTRUCT_TABLE  (                                  \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,                              \
        (                                                               \
            (char,                   GblVariant_constructChar),         \
            (uint8_t,                GblVariant_constructUint8),        \
            (uint16_t,               GblVariant_constructUint16),       \
            (int16_t,                GblVariant_constructInt16),        \
            (uint32_t,               GblVariant_constructUint32),       \
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
#define GblVariant_construct(...)               GBL_VA_OVERLOAD_CALL(GblVariant_construct, GBL_VA_OVERLOAD_SUFFIXER_1_N, __VA_ARGS__)

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
#define GblVariant_set(pVariant, ...) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_SET_TABLE, GBL_TUPLE_FIRST(__VA_ARGS__))(pVariant, __VA_ARGS__)

//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_VARIANT_H
