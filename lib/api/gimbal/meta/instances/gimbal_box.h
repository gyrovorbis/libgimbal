/*! \file
 *  \brief GblBox (reference-counted, opaque userdata), and related functions
 *  \ingroup metaBuiltinTypes
 *
 *  GblBox represents the most minimal type within the type
 *  system that is still runtime extensible and still offers
 *  everything typically required for an object to have
 *  bindings to another language. A GblBox extends GblInstance with:
 *
 *  * Reference counting/shared pointer semantics
 *  * Arbitrary userdata storage corresponding destructors
 *
 *  \author 2023 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_BOX_H
#define GIMBAL_BOX_H

#include "gimbal_instance.h"
#include "../ifaces/gimbal_ivariant.h"
#include "../../containers/gimbal_array_map.h"

/*! \name Type System
 *  \brief Type UUID and Cast operators
 *  @{
 */
#define GBL_BOX_TYPE            (GBL_TYPEOF(GblBox))                    //!< GblType UUID of a GblBox
#define GBL_BOX(self)           (GBL_INSTANCE_CAST(self, GblBox))       //!< Casts a GblInstance to GblBox
#define GBL_BOX_CLASS(klass)    (GBL_CLASS_CAST(klass, GblBox))         //!< Casts a GblClass to GblBoxClass
#define GBL_BOX_GET_CLASS(self) (GBL_INSTANCE_GET_CLASS(self, GblBox))  //!< Gets a GblBoxClass from a GblInstance
//! @}

#define GBL_REF(box)    (GblBox_ref(GBL_BOX(box)))                    //!< Auto-casting convenience macro around GblBox_ref()
#define GBL_UNREF(box)  (GblBox_unref(GBL_INSTANCE_TRY(box, GblBox))) //!< Auto-casting convenience macro around GblBox_unref()

#define GBL_SELF_TYPE   GblBox

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblBox);

/*! \struct     GblBoxClass
 *  \extends    GblClass
 *  \implements GblIVariantClass
 *  \ingroup    metaBuiltinTypes
 *  \brief      GblClass VTable/static data structure for GblBox
 *
 *  GblBoxClass extends GblClass and implements the GblIVariant
 *  interface for GblVariant container support. It also provides
 *  arbitrary userdata storage at the class-level.
 *
 *  \sa GblBox
 */
GBL_CLASS_BASE(GblBox, GblIVariant)
    GBL_PRIVATE()
        //! PRIVATE: Internal storage for userdata fields
        GblArrayMap* pFields;
    GBL_PRIVATE_END

    //! Virtual method invoked when a GblBox is being destroyed
    GBL_RESULT (*pFnDestructor)(GBL_SELF);
GBL_CLASS_END

/*! \struct     GblBox
 *  \extends    GblInstance
 *  \implements GblIVariant
 *  \ingroup    metaBuiltinTypes
 *  \brief      Minimally bindable GblInstance with reference semantics and opaque userdata
 *
 *  A GblBox is a GblInstance-derived type which simply adds the bare minimal set of
 *  functionality that is typically required for language bindings and interop. This
 *  includes:
 *      - destructors
 *      - reference counting semantics
 *      - a dictionary for storing arbitrary associated userdata
 *      - interop with GblVariant types
 *      - arbitrary flag bits
 *
 *  \note
 *  A GblBox is 12 or 20 bytes total (32 or 64 bit respectively).
 *
 *  \sa GblBoxClass
 */
GBL_INSTANCE_BASE(GblBox)                       // Size (32-64 bit)
    GBL_PRIVATE()                               // 12/20 Bytes Total
        GblArrayMap* pFields;                   //!< PRIVATE: Storage for extended userdata fields
        GblRefCount  refCounter;                //!< PRIVATE: Atomic reference counter
        uint16_t     contextType         : 1;   //!< PRIVATE: GblContext type flag \deprecated
        uint16_t     constructedInPlace  : 1;   //!< PRIVATE: Flag for whether memory is deallocated upon destruction
        uint16_t     derivedFlags        : 14;  //!< PRIVATE: Extra flags for use in derived classes
    GBL_PRIVATE_END
GBL_INSTANCE_END

/*! \name Floating Classes
 *  \brief Methods for managing floating classes
 *  @{
 */
//! Creates an extended floating class, setting its properties to the given values
GBL_EXPORT GblBoxClass* GblBoxClass_createFloating    (GblType           derivedType,
                                                       size_t            publicSize,
                                                       void*             pUserdata,
                                                       GblArrayMapDtorFn pFnUdDtor) GBL_NOEXCEPT;
//! Constructs an extended floating class in-place, setting its properties to the given values
GBL_EXPORT GBL_RESULT   GblBoxClass_constructFloating (GBL_KLASS,
                                                       GblType           derivedType,
                                                       void*             pUserdata,
                                                       GblArrayMapDtorFn pFnUdDtor) GBL_NOEXCEPT;
//! @}

/*! \name Userdata
 *  \brief Methods for setting and destroying userdata
 *  \relatesalso GblBoxClass
 *  @{
 */
//! Returns the void* userdata pointer that has been associated with the given GblBoxClass
GBL_EXPORT void*      GblBoxClass_userdata          (GBL_CKLASS)                  GBL_NOEXCEPT;
//! Sets the void* userdata pointer, associating it with the given GblBoxClass
GBL_EXPORT GBL_RESULT GblBoxClass_setUserdata       (GBL_KLASS, void* pUserdata)  GBL_NOEXCEPT;
//! Adds an extra destructor callback to the field list, passing the given class back as the destructed value
GBL_EXPORT GBL_RESULT GblBoxClass_setUserDestructor (GBL_KLASS,
                                                     GblArrayMapDtorFn pFnUdDtor) GBL_NOEXCEPT;
//! @}

/*! \name Fields
 *  \brief Methods for managing extended userdata fields
 *  \relatesalso GblBoxClass
 *  @{
 */
//! Returns the generic userdata field value associated with the given key on the class, or 0 if there isn't one
GBL_EXPORT uintptr_t  GblBoxClass_getField   (GBL_CKLASS, GblQuark key)  GBL_NOEXCEPT;
//! Extracts the generic userdata field value associated with the given key on the class, without destroying it
GBL_EXPORT uintptr_t  GblBoxClass_takeField  (GBL_KLASS, GblQuark key)   GBL_NOEXCEPT;
//! Destroys the generic userdata field value associated with the given key on the class, returning GBL_TRUE upon success
GBL_EXPORT GblBool    GblBoxClass_clearField (GBL_KLASS, GblQuark key)   GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given class has generic userdata field value associated with the given key
GBL_EXPORT GblBool    GblBoxClass_hasField   (GBL_CKLASS, GblQuark key)  GBL_NOEXCEPT;
//! Sets the generic userdata value and its optional destructor for the given key on the class, destroying the previous value if there was one
GBL_EXPORT GBL_RESULT GblBoxClass_setField   (GBL_KLASS,
                                              GblQuark          key,
                                              uintptr_t         ud,
                                              GblArrayMapDtorFn pFnDtor) GBL_NOEXCEPT;
//! @}

//! Returns the GblType UUID associated with GblBox
GBL_EXPORT GblType GblBox_type (void) GBL_NOEXCEPT;

/*! \name Constructors
 *  \brief Overloaded constructor methods for GblBox
 *  @{
 */
//! Creates a GblBox instance of the derived type and returns a pointer to it
GBL_EXPORT GblBox*    GblBox_create                (GblType derivedType)           GBL_NOEXCEPT;
//! Creates a GblBox instance, setting its class immediately to \p pClass, but not sinking it to take ownership
GBL_EXPORT GblBox*    GblBox_createWithClass       (GblBoxClass* pClass)           GBL_NOEXCEPT;
//! Constructs a GblBox instance of the derived type in-place, returning a result status code
GBL_EXPORT GBL_RESULT GblBox_construct             (GBL_SELF, GblType derivedType) GBL_NOEXCEPT;
//! Constructs a GblBox instance in-place, setting its class immediately to \p pClass, but not sinking it to take ownership
GBL_EXPORT GBL_RESULT GblBox_constructWithClass    (GBL_SELF, GblBoxClass* pClass) GBL_NOEXCEPT;
//! Creates an extended GblBox of the derived type, setting the given attributes, returning a pointer to it
GBL_EXPORT GblBox*    GblBox_createExt             (GblType           derivedType,
                                                    size_t            totalSize,
                                                    void*             pUserdata,
                                                    GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;
//! Constructs an extended GblBox of the derived type in-place, setting the given attributes and returning a status code
GBL_EXPORT GBL_RESULT GblBox_constructExt          (GBL_SELF,
                                                    GblType           derivedType,
                                                    void*             pUserdata,
                                                    GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;
//! Creates an extended GblBox of the derived type with the given attributes, also setting its class (but not taking ownership of it)
GBL_EXPORT GblBox*    GblBox_createExtWithClass    (GblBoxClass*      pClass,
                                                    size_t            totalSize,
                                                    void*             pUserdata,
                                                    GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;
//! Constructs an extended GblBox of the derived type in-place with the given attributes, also setting its class (but not taking ownership of it)
GBL_EXPORT GBL_RESULT GblBox_constructExtWithClass (GBL_SELF,
                                                    GblBoxClass*      pClass,
                                                    void*             pUserdata,
                                                    GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;
//! @}

/*! \name Reference Counting
 *  \brief Methods for managing shared reference lifetime
 *  \relatesalso GblBox
 *  @{
 */
//! Increments the given GblBox's reference count by 1, returning a pointer to it
GBL_EXPORT GblBox*     GblBox_ref      (GBL_SELF)  GBL_NOEXCEPT;
//! Decrements the given GblBox's reference count by 1, destructing it when it hits 0
GBL_EXPORT GblRefCount GblBox_unref    (GBL_SELF)  GBL_NOEXCEPT;
//! Returns the number of active references held to the given GblBox
GBL_EXPORT GblRefCount GblBox_refCount (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name Userdata
 *  \brief Methods for managing and destroying userdata
 *  \relatesalso GblBox
 *  @{
 */
//! Returns the userdata pointer stored within the given GblBox
GBL_EXPORT void*      GblBox_userdata          (GBL_CSELF)                   GBL_NOEXCEPT;
//! Stores the untyped userdata pointer within the GblBox
GBL_EXPORT GBL_RESULT GblBox_setUserdata       (GBL_SELF, void* pUserdata)   GBL_NOEXCEPT;
//! Sets an additional user destructor to be invoked with the GblBox passed back to it when its being destructed
GBL_EXPORT GBL_RESULT GblBox_setUserDestructor (GBL_SELF,
                                                GblArrayMapDtorFn pFnUdDtor) GBL_NOEXCEPT;
//! @}

/*! \name Fields
 *  \brief Methods for managing extended userdata fields
 *  \relatesalso GblBox
 *  @{
 */
//! Returns the generic userdata field value for the given GblBox associated with the key
GBL_EXPORT uintptr_t  GblBox_getField   (GBL_CSELF, GblQuark key)   GBL_NOEXCEPT;
//! Extracts the generic userdata field value for the given key, without destroying it
GBL_EXPORT uintptr_t  GblBox_takeField  (GBL_SELF, GblQuark key)    GBL_NOEXCEPT;
//! Clears the field value for the given key, if it exists, calling its destructor (if it has one) and returning GBL_TRUE
GBL_EXPORT GblBool    GblBox_clearField (GBL_SELF, GblQuark key)    GBL_NOEXCEPT;
//! Returns GBL_TRUE if there is a field value on the given GblBox corresponding to the key
GBL_EXPORT GblBool    GblBox_hasField   (GBL_CSELF, GblQuark key)   GBL_NOEXCEPT;
//! Inserts \p ud into the the GblBox with the given \p key and optional destructor, destroying any overwritten value
GBL_EXPORT GBL_RESULT GblBox_setField   (GBL_SELF,
                                         GblQuark          key,
                                         uintptr_t         ud,
                                         GblArrayMapDtorFn pFnDtor) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_BOX_H
