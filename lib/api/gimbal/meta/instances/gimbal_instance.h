/*! \file
 *  \ingroup meta
 *  \brief   GblInstance structure and related functions.
 *
 *  This file contains the API for the GblInstance type,
 *  which is the most low-level, minimalistic primitive
 *  instantiable type within the type-system. It's the
 *  root of all other instantiables and contains nothing
 *  other than a pointer to its GblClass.
 *
 *  \sa gimbal_type.h, gimbal_class.h
 *
 *  \author    2023, 2025 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_INSTANCE_H
#define GIMBAL_INSTANCE_H

#include "../../core/gimbal_ctx.h"
#include "../classes/gimbal_class.h"

/*! \name Instance Root Type
 *  \brief UUID and cast operators
 *  @{
 */
#define GBL_INSTANCE_TYPE               (GBL_BUILTIN_TYPE(INSTANCE))            //!< Type UUID for GblInstance
#define GBL_INSTANCE(self)              ((GblInstance*)self)                    //!< Casts GblInstance-compatible to GblInstance
#define GBL_INSTANCE_GET_CLASS(self)    (GblInstance_class(GBL_INSTANCE(self))) //!< Extracts the class from a GblInstance
//! @}

/*! \name  Instance Operators
 *  \brief Builtin operations for Instance-derived types
 *  \relatesalso GblInstance
 *  @{
 */
//! Returns the type UUID for a GblInstance
#define GBL_TYPEOF(self)                      GBL_TYPEOF_(self)
//! Wraps GblInstance_private(), passing the given type and returning its private structure
#define GBL_PRIVATE(cType, self)              GBL_PRIVATE_(cType, self)
//! Wraps GblInstance_public(), passing the given type, and returning its public struct
#define GBL_PUBLIC(cType, selfPriv)           GBL_PUBLIC_(cType, selfPriv)
//! Returns GBL_TRUE if the instance can be casted to the given type safely
#define GBL_TYPECHECK(cType, self)            GBL_TYPECHECK_(cType, self)
//! Dynamically validated, error throwing cast in `DEBUG` builds, 0 overhead C-style cast in `RELEASE` builds (with `NDEBUG` defined).
#define GBL_CAST(cType, self)                 GBL_CAST_(cType, self)
//! Wraps GblInstance_as() for the given type, providing a gracefully-failing case
#define GBL_AS(cType, self)                   GBL_AS_(cType, self)
//! Casts to the given instance type, then returns its class, returning NULL and raising an error upon failure
#define GBL_CLASSOF(cType, self)              GBL_CLASSOF_(cType, self)
//! Uses GblInstance_as(), then gets its class (if successful) or returns NULL without raising an error
#define GBL_CLASSOF_AS(cType, self)           GBL_CLASSOF_AS_(cType, self)
//! Invokes a virtual method on the given type's class, passing the variadic arguments directly through to it as method arguments
#define GBL_VCALL(cType, method, ...)         GBL_VCALL_(cType, method, __VA_ARGS__)
//! Invokes a virtual method on the default class for a given type, used for calling into parent or default methods
#define GBL_VCALL_DEFAULT(cType, method, ...) GBL_VCALL_DEFAULT_(cType, method, __VA_ARGS__)
//! @}

#define GBL_SELF_TYPE GblInstance

GBL_DECLS_BEGIN

/*! \brief Base struct for all instantiable meta types.
 *  \ingroup meta
 *
 *  An Instance simply represents an object that can be created from a given
 *  GblType which shares some data with other instances of the same type,
 *  located within their class.
 *
 *  GblInstance is the base structure which is to be inherited by instances of all
 *  instantiable types. This means placing it or a type inheriting from it as the
 *  first member of an instance struct.
 *
 *  \note
 *  As the root instantiable type, an instance contains nothing but a
 *  pointer to its corresponding class. Its as lightweight as possible.
 *
 *  \sa GblClass, GblType, GblBox, GblObject
 */
typedef struct GblInstance {
    GblClass*   pClass; //!< READ-ONLY Pointer to Instance's Class, do not modify directly
} GblInstance;

/*! \name  Lifetime Management
 *  \brief Constructors and Destructors
 *  @{
 */
//! Creates and returns an instance, optionally with an extended size and/or non-default class
GBL_EXPORT GblInstance* GblInstance_create    (GblType   type,
                                               size_t    publicSize/*=DEFAULT*/,
                                               GblClass* pClass    /*=NULL*/) GBL_NOEXCEPT;
//! Constructs an instance, optionally with a non-default class, returning a result code
GBL_EXPORT GBL_RESULT   GblInstance_construct (GBL_SELF,
                                               GblType   type,
                                               GblClass* pClass/*=NULL*/)     GBL_NOEXCEPT;
//! Destructs and deallocates an instance. It must have been created with GblInstance_create().
GBL_EXPORT GblRefCount  GblInstance_destroy   (GBL_SELF)                      GBL_NOEXCEPT;
//! Destructs but doesn't deallocate an instance. It must have been created with GblInstance_construct().
GBL_EXPORT GblRefCount  GblInstance_destruct  (GBL_SELF)                      GBL_NOEXCEPT;
//! @}

/*! \name  Type Conversions
 *  \brief Methods for type casting and checking
 *  \relatesalso GblInstance
 *  @{
 */
//! Returns GBL_TRUE if the given instance's type is compatible with \p toType
GBL_EXPORT GblBool      GblInstance_check (GBL_CSELF, GblType toType) GBL_NOEXCEPT;
//! Attempts to cast the given instance to \p toType, raising an error and returning NULL upon failure
GBL_EXPORT GblInstance* GblInstance_cast  (GBL_SELF, GblType toType)  GBL_NOEXCEPT;
//! Attempts to cast the given instance to \p toType, gracefully returning NULL upon failure
GBL_EXPORT GblInstance* GblInstance_as    (GBL_SELF, GblType toType)  GBL_NOEXCEPT;
//! @}

/*! \name  Public and Private Data
 *  \brief Methods for accessing public and private segments
 *  \relatesalso GblInstance
 *  @{
 */
//! Returns the private structure associated with the given \p base type of the instance
GBL_EXPORT void*        GblInstance_private (GBL_CSELF, GblType base)         GBL_NOEXCEPT;
//! Casts back to the instance type from a base type's private data segment structure
GBL_EXPORT GblInstance* GblInstance_public  (const void* pPriv, GblType base) GBL_NOEXCEPT;
//! @}

/*! \name  Type Info
 *  \brief Methods for getting instance information
 *  \relatesalso GblInstance
 *  @{
 */
//! Returns the GblType associated with the given GblInstance
GBL_EXPORT GblType GblInstance_typeOf      (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the size of the given GblInstance's public data segment
GBL_EXPORT size_t  GblInstance_size        (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the size of the given GblInstance's private data segment
GBL_EXPORT size_t  GblInstance_privateSize (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the instance's combined DEFAULT size (not extended allocation size)
GBL_EXPORT size_t  GblInstance_totalSize   (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name Classes
 *  \brief Methods for managing classes
 *  \relatesalso GblInstance
 *  @{
 */
//! Returns the GblClass associated with the given GblInstance
GBL_EXPORT GblClass*  GblInstance_class        (GBL_CSELF)                  GBL_NOEXCEPT;
//! Swaps out the class associated with the instance, without taking ownership of it
GBL_EXPORT GBL_RESULT GblInstance_swizzleClass (GBL_SELF, GblClass* pClass) GBL_NOEXCEPT;
//! Takes ownership of the class associated with the instance, deallocating it with the instance
GBL_EXPORT GBL_RESULT GblInstance_sinkClass    (GBL_SELF)                   GBL_NOEXCEPT;
//! Relinquishes ownership of the class assosciated with the instance, but maintaining their association
GBL_EXPORT GBL_RESULT GblInstance_floatClass   (GBL_SELF)                   GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond
#define GblInstance_create(...) \
    GblInstance_createDefault_(__VA_ARGS__)
#define GblInstance_createDefault_(...) \
    GblInstance_createDefault__(__VA_ARGS__, 0, GBL_NULL)
#define GblInstance_createDefault__(type, size, klass, ...) \
    (GblInstance_create)(type, size, klass)

#define GblInstance_construct(...) \
    GblInstance_constructDefault_(__VA_ARGS__)
#define GblInstance_constructDefault_(...) \
    GblInstance_constructDefault__(__VA_ARGS__, GBL_NULL)
#define GblInstance_constructDefault__(self, type, klass, ...) \
    (GblInstance_construct)(self, type, klass)

#define GBL_TYPEOF_(self)            (GblInstance_typeOf(GBL_INSTANCE(self)))
#define GBL_PRIVATE_(cType, self)    ((GBL_INSTANCE_PRIVATE_STRUCT(cType)*) GblInstance_private(GBL_INSTANCE(self), GBL_TYPEID(cType)))
#define GBL_PUBLIC_(cType, selfPriv) ((cType*)GblInstance_public((const void*)selfPriv, GBL_TYPEID(cType)))
#define GBL_TYPECHECK_(cType, self)  (GblInstance_check(GBL_INSTANCE(self), GBL_TYPEID(cType)))
#ifndef NDEBUG
#   define GBL_CAST_(cType, self)    ((cType*)GblInstance_cast((GblInstance*)self, GBL_TYPEID(cType)))
#else
#   define GBL_CAST_(cType, self)    ((cType*)self)
#endif
#define GBL_AS_(cType, self)         ((cType*)GblInstance_as((GblInstance*)self, GBL_TYPEID(cType)))
#define GBL_CLASSOF_(cType, self)    ((GBL_CLASS_STRUCT(cType)*)GblClass_cast(GblInstance_class((GblInstance*)self), GBL_TYPEID(cType)))
#define GBL_CLASSOF_AS_(cType, self) ((GBL_CLASSOF(cType)*)GblClass_as(GblInstance_class((GblInstance*)self), GBL_TYPEID(cType)))

#define GBL_VCALL_(cType, method, ...)                                                          \
    GBL_STMT_START {                                                                            \
        GBL_CLASS_STRUCT(cType)* pClass = GBL_CLASSOF(cType, GBL_TUPLE_FIRST (__VA_ARGS__, 1)); \
        GBL_CTX_VERIFY(pClass, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);                          \
        GBL_CTX_VERIFY(pClass->method, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);                  \
        GBL_CTX_CALL(pClass->method(__VA_ARGS__));                                              \
    } GBL_STMT_END

#define GBL_VCALL_DEFAULT_(cType, method, ...)                                      \
    GBL_STMT_START {                                                                \
        GBL_CLASS_STRUCT(cType)* pClass =                                           \
            (GBL_CLASS_STRUCT(cType)*)GblClass_weakRefDefault(GBL_TYPEID(cType));   \
        GBL_CTX_VERIFY(pClass, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);              \
        GBL_CTX_VERIFY(pClass->method, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);      \
        GBL_CTX_CALL(pClass->method(__VA_ARGS__));                                  \
    } GBL_STMT_END
//! \endcond

/*! \fn GblRefCount GblInstance_destroy(GblInstance* pSelf)
* Finalizes then deletes an instance that was created on the heap, either unreferencing.
* its class if using the default or destroying it if using a sunk floating class.
* \param pSelf heap-callocated instance
* \returns remaining number of active instances of the associated type
* \sa GblInstance_destruct
*/

/*! \fn GblRefCount GblInstance_destruct(GblInstance* pSelf)
* Equivalent to GblInstance_destroy(), except that the allocation is not deallocated.
* This is equivalent to a placement delete operation in C++.
* \param pSelf existing allocation
* \returns remaining number of active instances of the associated type
* \sa GblInstance_destroy
*/

/*! \fn GBL_RESULT GblInstance_swizzleClass(GblInstance* pSelf, GblClass* pClass)
* Releases ownership of the instance's current class, replacing it with a type-compatible class.
* This is analogous to Objective-C's "is-a swizzling."
* \note The instance does not automatically assume ownersip of the new class.
* \relatesalso GblInstance
* \param pSelf instance
* \param pClass replacement GblClass of compatible type
* \returns result code
* \sa GblInstance_sinkClass
*/

/*! \fn GBL_RESULT GblInstance_sinkClass(GblInstance* pSelf)
* Claims ownership of the previously floating class contained by the instance, binding
* the lifetime of the two, so that the class will be finalized when the instance is finalized.
* \attention ONLY a GblClass that has been created or constructed as a floating class can be
* sunk. Attempting to sink the default class for a given type will result in an erro!
* \relatesalso GblInstance
* \param pSelf instance
* \returns result code
* \sa GblInstance_floatClass, GblClass_isFloating(), GblClass_createFloating(), GblClass_constructFloating()
*/

/*! \fn GBL_RESULT GblInstance_floatClass(GblInstance* pSelf)
* Releases ownership of the owned class contained by the instance. The class will no
* longer be finalized along with the given instance.
* \relatesalso GblInstance
* \param pSelf instance
* \returns result code
* \sa GblInstance_sinkClass
*/

/*! \fn GblBool GblInstance_check(const GblInstance* pSelf, GblType toType)
* Returns true if the given instance can be casted to an instance of the given type.
* \relatesalso GblInstance
* \param pSelf source instance
* \param toType casted result type
* \returns true if cast is valid
* \sa GblInstance_as
*/

/*! \fn GblInstance* GblInstance_cast(GblInstance* pSelf, GblType toType)
* Casts the given instance to the given type, erroring-out upon failure.
* \relatesalso GblInstance
* \param pSelf source instance
* \param toType casted result type
* \returns pointer to casted instance or NULL upon failure
* \sa GblInstance_check, GblInstance_as
*/

/*! \fn GblInstance* GblInstance_as(GblInstance* pSelf, GblType toType)
* Equivalent to GblInstance_cast(), except gracefully returning NULL with no errors upon failure.
* This is analogous to a dynamic_cast<> operation in C++.
* \relatesalso GblInstance
* \param pSelf source instance
* \param toType casted result type
* \returns pointer to casted instance or NULL upon failure
* \sa GblInstance_cast
*/


/*! \fn void* GblInstance_private(const GblInstance* pSelf, GblType base)
* Returns the private instance data associated with the base or actual type of the given
* public instance structure.
* \relatesalso GblInstance
* \param pSelf source instance
* \param base type ID to fetch private data for
* \returns pointer to private data upon success, NULL upon failure
*/


/*! \fn GblInstance* GblInstance_public(const void* pPrivate, GblType base);
* Returns the public GblInstance structure for the given private data and its associated base
* or actual type.
* \relatesalso GblInstance
* \param pPrivate private data pointer
* \param base type corresponding to the private data
* \returns pointer to instance upon success, NULL upon failure
*/

/*! \fn GblType GblInstance_typeOf(const GblInstance* pSelf)
* Returns the type ID associated with the given instance.
* \relatesalso GblInstance
* \param pSelf instance pointer
* \returns type ID or NULL if the instance is NULL
*/

/*! \fn size_t  GblInstance_size(const GblInstance* pSelf)
* Returns the size of the public instance struct for the given instance.
* \relatesalso GblInstance
* \param pSelf instance pointer
* \returns size or 0 if instance is NULL
* \sa GblInstance_privateSize, GblInstance_totalSize
*/

/*! \fn size_t  GblInstance_privateSize(const GblInstance* pSelf)
* Returns the size of the private instance struct for the given instance.
* \relatesalso GblInstance
* \param pSelf instance pointer
* \returns size or 0 if instance is NULL
*/

/*! \fn size_t  GblInstance_totalSize(const GblInstance* pSelf)
* Returns the total size for both public and private instance data for the given instance.
* \relatesalso GblInstance
* \param pSelf instance pointer
* \returns size or 0 if instance is NULL
*/

/*! \fn GblClcass* GblInstance_class(const GblInstance* pSelf)
* Returns the public class structure associated with the given instance.
* \relatesalso GblInstance
* \param pSelf instance pointer
* \returns class pointer or NULL if instance is NULL
*/

#undef GBL_SELF_TYPE

#endif // GIMBAL_INSTANCE_H
