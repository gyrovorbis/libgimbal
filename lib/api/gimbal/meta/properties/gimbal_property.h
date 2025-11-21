/*! \file
 *  \ingroup meta
 *  \brief   GblProperty instance, DSL, and management API
 *
 *  This file contains the type declarations and methods
 *  for GblProperty, the root property instance from
 *  which all other properties are derived.
 *
 *  Properties are string-keyed values of any data type
 *  which can be added to a GblObject.
 *
 *  \author     2023, 2025 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_PROPERTY_H
#define GIMBAL_PROPERTY_H

#include "../instances/gimbal_box.h"

/*! \name  Type System
 *  \brief UUID and Cast Operators
 *  @{
 */
#define GBL_PROPERTY_TYPE               (GBL_TYPEID(GblProperty))            //!< Type UUID for GblProperty
#define GBL_PROPERTY(self)              (GBL_CAST(GblProperty, self))        //!< Casts a GblInstance to GblProperty
#define GBL_PROPERTY_CLASS(klass)       (GBL_CLASS_CAST(GblProperty, klass)) //!< Casts a GblClass to GblPropertyClass
#define GBL_PROPERTY_GET_CLASS(self)    (GBL_CLASSOF(GblProperty, self))     //!< Gets a GblPropertyClass from GblInstance
//! @}

//! Alternate type identifier for GblProperty
#define GBL_GENERIC_PROPERTY_TYPE       GBL_PROPERTY_TYPE

/*! \name  Helper DSL
 *  \brief Helper macros for declaration and registration
 *  @{
 */
//! Declares a list of properties for the given object/instance structure
#define GBL_PROPERTIES(object, ...)              GBL_PROPERTIES_(object, __VA_ARGS__)
//! Registers the list of properties which were declared with GBL_PROPERTIES()
#define GBL_PROPERTIES_REGISTER(/*object,*/ ...) GBL_PROPERTIES_REGISTER_(__VA_ARGS__)
//! @}

#define GBL_SELF_TYPE GblProperty

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblProperty);

//! Function signature used as an iterator with GblProperty_foreach(), returns GBL_FALSE when iteration should cease.
typedef GblBool (*GblPropertyIterFn)(const GblProperty* pProp, void* pClosure);

/*! \enum  GBL_PROPERTY_FLAG
 *  \brief Flags used to denote property attributes.
 */
GBL_DECLARE_ENUM(GBL_PROPERTY_FLAG) {
    GBL_PROPERTY_FLAG_CONSTRUCT  = 0x001, //!< Property must be given to the constructor.
    GBL_PROPERTY_FLAG_READ       = 0x002, //!< Property value can be read.
    GBL_PROPERTY_FLAG_WRITE      = 0x004, //!< Property value can be modified.
    GBL_PROPERTY_FLAG_IN         = 0x008, //!< Property assumes ownership of data when writing.
    GBL_PROPERTY_FLAG_OUT        = 0x010, //!< Property releases ownership of data when reading.
    GBL_PROPERTY_FLAG_SAVE       = 0x020, //!< Property is serialized when saving.
    GBL_PROPERTY_FLAG_LOAD       = 0x040, //!< Property is deserialized when loading.
    GBL_PROPERTY_FLAG_ABSTRACT   = 0x080, //!< Property must be implemented by deriving type.
    GBL_PROPERTY_FLAG_OVERRIDE   = 0x100, //!< Property overrides an existing property.
    GBL_PROPERTY_FLAG_READ_WRITE = 0x006, //!< Property is both readable and writable.
    GBL_PROPERTY_FLAG_SAVE_LOAD  = 0x060, //!< Property is both savable and loadable.
    GBL_PROPERTY_FLAG_ALL        = 0xfff  //!< Mask for all property flags combined.
};

/*! \struct  GblPropertyClass
 *  \extends GblBoxClass
 *  \brief   GblClass VTable structure for GblProperty
 *
 *  GblPropertyClass provides overridable virtual methods which
 *  allow for a derived type to implement type-specific functionality
 *  for a property. This functionality includes:
 *      - additional constructor argument handling
 *      - default value handling
 *      - type checking
 *      - value validation
 *      - (fuzzy) comparisons
 *
 *  \sa GblProperty
 */
GBL_CLASS_DERIVE(GblProperty, GblBox)
    //! Virtual method invoked during constructor to manage additional, non-default arguments
    GBL_RESULT (*pFnInitOptionalArgs)(GBL_SELF, size_t argCount, va_list* pVaList);
    //! Virtual method returning the default value for the property
    GBL_RESULT (*pFnDefaultValue)    (GBL_CSELF, GblVariant* pValue);
    //! Checks whether a variant is of the right type or can even be accepted as the new property value
    GBL_RESULT (*pFnCheckValue)      (GBL_CSELF, const GblVariant* pValue);
    //! Updates the variant's value to be within range and a valid value for the given property
    GBL_RESULT (*pFnValidateValue)   (GBL_CSELF, GblVariant* pValue);
    //! Compares the values of the two given variants to see if they're even different
    GBL_RESULT (*pFnCompareValues)   (GBL_CSELF, const GblVariant* pV1, const GblVariant* pV2, int* pResult);
GBL_CLASS_END

/*! \struct  GblProperty
 *  \extends GblBox
 *  \ingroup meta
 *  \brief   Represents a string-indexed member of a GblObject
 *
 *  GblProperty represents a single GblObject member which can be
 *  dynamically looked-up by a string key, passed to the Gblobject
 *  constructor, or which can be programmatically iterated over.
 *
 *  \sa GblPropertyClass
 */
GBL_INSTANCE_DERIVE(GblProperty, GblBox)
    GBL_PRIVATE_BEGIN
        GblProperty* pNext;      //!< Pointer to next property contained by the same type.
        GblType      objectType; //!< UUID of the owning type.
    GBL_PRIVATE_END
    GblQuark    name;            //!< Interned string name of property.
    size_t      id;              //!< Integer ID of property.
    GblFlags    flags;           //!< Combined flags of property.
    GblType     valueType;       //!< Type of the value represented by the property.
GBL_INSTANCE_END

//! Returns the UUID associated with the GblProperty type, registering it if it hasn't been already.
GBL_EXPORT GblType GblProperty_type (void) GBL_NOEXCEPT;

/*! \name  Management
    \brief Routines for installing and uninstalling properties.
    @{
*/
//! Installs the given property onto the given type (which must be GblObject compatible).
GBL_EXPORT GBL_RESULT GblProperty_install        (GblType objType, GblProperty* pProp) GBL_NOEXCEPT;
//! Uninstalls the property with the given name from the given object type.
GBL_EXPORT GblBool    GblProperty_uninstall      (GblType objType, const char* pName)  GBL_NOEXCEPT;
//! Uninstalls the property with the given name quark from the given object type.
GBL_EXPORT GblBool    GblProperty_uninstallQuark (GblType objType, GblQuark name)      GBL_NOEXCEPT;
//! Uninstalls all properties which were previously installed onto the given object type.
GBL_EXPORT GblBool    GblProperty_uninstallAll   (GblType objType)                     GBL_NOEXCEPT;
//! @}

/*! \name  Info
    \brief Querying for miscellaneous property info.
    @{
*/
//! Returns the total number of properties which have been registered onto any GblType.
GBL_EXPORT size_t   GblProperty_totalCount    (void)               GBL_NOEXCEPT;
//! Returns the number of properties which were explicitly registered to the given type (not including inherited properties).
GBL_EXPORT size_t   GblProperty_count         (GblType objectType) GBL_NOEXCEPT;
//! Returns the combined, bitwise OR'd flags from all properties registered onto the given type.
GBL_EXPORT GblFlags GblProperty_combinedFlags (GblType objectType) GBL_NOEXCEPT;
//! @}

/*! \name  Querying
    \brief Routines for looking up installed properties.
    @{
*/
//! Looks up the given property by string name which was previously installed onto the given GblObject type.
GBL_EXPORT const GblProperty* GblProperty_find      (GblType objectType, const char* pName) GBL_NOEXCEPT;
//! Looks up the given property by GblQuark name which was previously installed onto the given GblObject type.
GBL_EXPORT const GblProperty* GblProperty_findQuark (GblType objectType, GblQuark name)     GBL_NOEXCEPT;
//! @}

/*! \name  Iterating
    \brief Routines for iterating over installed properties.
    @{
*/
//! Returns the next property installed onto the given type, given the current property (or NULL for the first) and the flag mask for which properties to include.
GBL_EXPORT const GblProperty* GblProperty_next    (GblType            objectType,
                                                   const GblProperty* pPrev,
                                                   GblFlags           mask)     GBL_NOEXCEPT;
//! Iterates over the properties installed onto the given type containing any of the given flags, calling the given callback, which gets passed back an optional closure. Returns false if the callback returns false to end iteration.
GBL_EXPORT GblBool            GblProperty_foreach (GblType            objectType,
                                                   GBL_PROPERTY_FLAG  flags,
                                                   GblPropertyIterFn  pFnIt,
                                                   void*              pClosure) GBL_NOEXCEPT;
//! @}

/*! \name  Initializing
    \brief Routines for creating and constructing new properties.
    @{
*/
//! Creates a new property on the heap with the given type, name, id, flags, and any optional arguments handled by its optional argument initializer.
GBL_EXPORT GblProperty* GblProperty_create          (GblType     derivedType,
                                                     const char* pName,
                                                     size_t      id,
                                                     GblFlags    flags,
                                                     size_t      optionalArgCount,
                                                     ...)                            GBL_NOEXCEPT;
//! Equivalent to GblProperty_create(), except taking a va_list rather than variadic argument list.
GBL_EXPORT GblProperty* GblProperty_createVaList    (GblType     derivedType,
                                                     const char* pName,
                                                     size_t      id,
                                                     GblFlags    flags,
                                                     size_t      optionalArgCount,
                                                     va_list*    pList)              GBL_NOEXCEPT;
//! Constructs a preallocated property with the given type, name, id, flags, and any optional arguments handled by its optional argument initializer.
GBL_EXPORT GBL_RESULT   GblProperty_construct       (GBL_SELF,
                                                     GblType     derivedType,
                                                     const char* pName,
                                                     size_t      id,
                                                     GblFlags    flags,
                                                     size_t      optionalArgCount,
                                                     ...)                            GBL_NOEXCEPT;
//! Equivalent to GblProperty_construct(), except taking a va_list rather than variadic argument list.
GBL_EXPORT GBL_RESULT   GblProperty_constructVaList (GBL_SELF,
                                                     GblType     derivedType,
                                                     const char* pName,
                                                     size_t      id,
                                                     GblFlags    flags,
                                                     size_t      optionalArgCount,
                                                     va_list*    pList)              GBL_NOEXCEPT;
//! Either calls GblProperty_create() or GblProperty_construct() based on whether ppSelf points to an existing property or not. If not, it will point to a newly created property, upon success, after the function returns.
GBL_EXPORT GBL_RESULT GblProperty_createOrConstruct (GblProperty** ppSelf,
                                                     GblType       derivedType,
                                                     const char*   pName,
                                                     size_t        id,
                                                     GblFlags      flags,
                                                     size_t        optionalArgCount,
                                                     ...)                            GBL_NOEXCEPT;
//! @}

/*! \name  Accessors
    \brief Routines for accessing internal property data.
    @{
*/
//! Returns the UUID of the type associated with the given property.
GBL_EXPORT GblType     GblProperty_objectType (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the string name of the given property.
GBL_EXPORT const char* GblProperty_name       (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name  Methods
    \brief Routines for calling class methods on properties.
    @{
*/
//! Sets the given variant to contain the default value of the given property, calling GblPropertyClass::pFnDefaultValue().
GBL_EXPORT GBL_RESULT GblProperty_defaultValue  (GBL_CSELF, GblVariant* pValue)       GBL_NOEXCEPT;
//! Checks whether the value contained by the given variant is valid for the given property, calling GblPropertyClass::pFnCeckValue().
GBL_EXPORT GblBool    GblProperty_checkValue    (GBL_CSELF, const GblVariant* pValue) GBL_NOEXCEPT;
//! Modifies the value contained within the given variant to be a valid value, if possible, calling GblPropertyClass::pFnValidateValue().
GBL_EXPORT GBL_RESULT GblProperty_validateValue (GBL_CSELF, GblVariant* pValue)       GBL_NOEXCEPT;
//! Compares the values contained by the given variants, based on the rules of the given property, calling GblPropertyClass::pFnCompareValues().
GBL_EXPORT int        GblProperty_compareValues (GBL_CSELF,
                                                 const GblVariant* pV1,
                                                 const GblVariant* pV2)               GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond PRIVATE MACRO IMPLEMENTATIONS
#define GBL_PROPERTIES_(object, ...)                        \
    GBL_PROPERTIES_IDS_(object, __VA_ARGS__)                \
    GBL_PROPERTIES_REGISTER_DEFINE_(object, __VA_ARGS__)

#define GBL_PROPERTIES_REGISTER_(...) \
    GBL_VA_OVERLOAD_CALL(GBL_PROPERTIES_REGISTER, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

#define GBL_PROPERTIES_REGISTER_1(object) \
    GBL_PROPERTIES_REGISTER_2(object, NULL)

#define GBL_PROPERTIES_REGISTER_2(object, list)                     \
    GBL_STMT_START {                                                \
        for(size_t  p = 0; p < object##_Property_Id_count; ++p) {   \
            object##_registerProperty_(p,                           \
                                       list? &list[p] : GBL_NULL);  \
        }                                                           \
    } GBL_STMT_END

#define GBL_PROPERTIES_IDS_(object, ...)                            \
    typedef enum object##_Property_Id_ {                            \
        GBL_TUPLE_FOREACH(GBL_PROPERTY_ID_, object, (__VA_ARGS__))  \
        object##_Property_Id_count                                  \
    } object##_Property_Id_;

#define GBL_PROPERTY_ID_(object, property)  \
    GBL_PROPERTY_ID__(object, GBL_EVAL property)

#define GBL_PROPERTY_ID__(...) \
    GBL_PROPERTY_ID___(__VA_ARGS__)

#define GBL_PROPERTY_ID___(object, name, type, flags, ...) \
    object##_Property_Id_##name,

#define GBL_PROPERTIES_REGISTER_DEFINE_(object, ...)                                    \
    GBL_EXPORT GblType object##_type(void) GBL_NOEXCEPT;                                \
    GBL_INLINE GBL_RESULT object##_registerProperty_(GblEnum id,                        \
                                                     GblProperty** ppProp) GBL_NOEXCEPT \
    {                                                                                   \
        GBL_CTX_BEGIN(NULL);                                                            \
        GblProperty* pProp = ppProp? *ppProp : GBL_NULL;                                \
        switch(id) {                                                                    \
            GBL_TUPLE_FOREACH(GBL_PROPERTY_REGISTER_, object, (__VA_ARGS__))            \
            default: GBL_CTX_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_INVALID_PROPERTY);      \
        }                                                                               \
        if(pProp) {                                                                     \
            GBL_CTX_CALL(GblProperty_install(object##_type(), pProp));                  \
            if(ppProp) *ppProp = pProp;                                                 \
        }                                                                               \
        GBL_CTX_END();                                                                  \
    }

#define GBL_PROPERTY_REGISTER_(object, property) \
    GBL_PROPERTY_REGISTER__(object, GBL_EVAL property)

#define GBL_PROPERTY_REGISTER__(...) \
    GBL_PROPERTY_REGISTER___(__VA_ARGS__)

#define GBL_PROPERTY_REGISTER___(object, name, type, ...)              \
    case object##_Property_Id_##name:                                  \
        GBL_CTX_VERIFY_CALL(GblProperty_createOrConstruct(&pProp,      \
                type##_PROPERTY_TYPE,                                  \
                GblQuark_internStatic(GBL_STRINGIFY(name)),      \
                id,                                                    \
                GBL_PROPERTY_FLAGS_MASK_ GBL_TUPLE_FIRST(__VA_ARGS__), \
                GBL_PROPERTY_VARARGS_((__VA_ARGS__))));                \
        break;

#define GBL_PROPERTY_VARARGS_(...) \
    GBL_VA_OVERLOAD_CALL(GBL_PROPERTY_VARARGS_, GBL_VA_OVERLOAD_SUFFIXER_1_N, __VA_ARGS__)

#define GBL_PROPERTY_VARARGS__1(flags) \
    0

#define GBL_PROPERTY_VARARGS__N(...) \
    GBL_NARG __VA_ARGS__ - 1, GBL_TUPLE_REST __VA_ARGS__

#define GBL_PROPERTY_FLAGS_MASK_(...) \
    GBL_MAP(GBL_PROPERTY_FLAGS_MASK__, __VA_ARGS__)0

#define GBL_PROPERTY_FLAGS_MASK__(suffix)  \
    GBL_PROPERTY_FLAG_##suffix |
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_PROPERTY_H
