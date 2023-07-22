/*! \file
 *  \brief GblInstance structure and related functions
 *  \ingroup meta
 *  \sa gimbal_type.h, gimbal_class.h
 *
 *  This file contains the API for the GblInstance type,
 *  which is the most low-level, minimalistic primitive
 *  instantiable type within the type-system. It's the
 *  root of all other instantiables and conatins nothing
 *  other than a pointer to its GblClass.
 *
 *  \author    2023 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_INSTANCE_H
#define GIMBAL_INSTANCE_H

#include "../../core/gimbal_ctx.h"
#include "../classes/gimbal_class.h"

//! Type UUID for GblInstance
#define GBL_INSTANCE_TYPE                              (GBL_BUILTIN_TYPE(INSTANCE))

//! Casts GblInstance-compatible to GblInstance
#define GBL_INSTANCE(instance)                         ((GblInstance*)instance)
//! Extracts the class from a GblInstance
#define GBL_INSTANCE_CLASS(instance)                   (GblInstance_class(GBL_INSTANCE(instance)))
//! Returns the type UUID for a GblInstance
#define GBL_INSTANCE_TYPEOF(instance)                  (GblInstance_typeOf(GBL_INSTANCE(instance)))
//! Returns the private data segment on a GblInstance for the corresponding type
#define GBL_INSTANCE_PRIVATE(instance, type)           (GblInstance_private(GBL_INSTANCE(instance), type))
//! Return the public GblInstance structure from the private data segment for the corresponding type
#define GBL_INSTANCE_PUBLIC(instPriv, type)            (GblInstance_public((const void*)instPriv, type))
//! Returns GBL_TRUE if the instance is compatible with the given type
#define GBL_INSTANCE_CHECK(instance, cType)            (GblInstance_check((GblInstance*)instance, GBL_TYPEOF(cType)))

//! Casts the GblInstance to the given type, returning NULL and emitting an error upon failure
#define GBL_INSTANCE_CAST(instance, cType)             ((cType*)GblInstance_cast((GblInstance*)instance, GBL_TYPEOF(cType)))
//! Casts the GblInstance to the given type, gracefully returning NULL with no errors raised upon failure
#define GBL_INSTANCE_TRY(instance, cType)              ((cType*)GblInstance_try((GblInstance*)instance, GBL_TYPEOF(cType)))
//! Extracts the class structure for the given type from a GblInstance, returning NULL and raising an error upon failure

#define GBL_INSTANCE_GET_CLASS(instance, cType)        ((GBL_CLASSOF(cType)*)GblClass_cast(GblInstance_class((GblInstance*)instance), GBL_TYPEOF(cType)))
//! Extracts the class structure for the given type from a GblInstance, gracefully returning NULl with no errors raised upon failure
#define GBL_INSTANCE_TRY_CLASS(instance, cType)        ((GBL_CLASSOF(cType)*)GblClass_try(GblInstance_class((GblInstance*)instance), GBL_TYPEOF(cType)))

//! Creates an instance of the given type on the heap, automatically casting the return value
#define GBL_INSTANCE_CREATE(cType)                     ((cType*)GblInstance_create(GBL_TYPEOF(cType)))
//! Constructs an instance of the given type in-place
#define GBL_INSTANCE_CONSTRUCT(cType, instance)        (GblInstance_construct(instance, GBL_TYPEOF(cType)))

//! Invokes a virtual method on the given type's class, passing the variadic arguments directly through to it as method arguments
#define GBL_INSTANCE_VCALL(cType, method, ...)         GBL_INSTANCE_VCALL_(cType, method, __VA_ARGS__)
//! Invokes a virtual method on the default class for a given type, used for calling into parent or default methods
#define GBL_INSTANCE_VCALL_DEFAULT(cType, method, ...) GBL_INSTANCE_VCALL_DEFAULT_(cType, method, __VA_ARGS__)

#define GBL_SELF_TYPE GblInstance

GBL_DECLS_BEGIN

/*! \brief Base struct for all instantiable meta types.
 *  \ingroup meta
 *
 * An Instance simply represents an object that can be created from a given
 * GblType which shares some data with other instances of the same type,
 * located within their class.
 *
 * GblInstance is the base structure which is to be inherited by instances of all
 * instantiable types. This means placing it or a type inheriting from it as the
 * first member of an instance struct.
 *
 * \note As the root instantiable type, an instance contains nothing but a
 * pointer to its corresponding class. Its as lightweight as possible.
 * \sa GblClass, GblType, GblObject
 */
typedef struct GblInstance {
    GblClass*   pClass; //!< READ-ONLY Pointer to Instance's Class, do not modify directly
} GblInstance;

GBL_EXPORT GblInstance* GblInstance_create             (GblType type)                    GBL_NOEXCEPT;
//! \todo implement
GBL_EXPORT GblInstance* GblInstance_createExt          (GblType type, size_t size)       GBL_NOEXCEPT;
GBL_EXPORT GblInstance* GblInstance_createWithClass    (GblClass* pClass)                GBL_NOEXCEPT;
//! \todo implement
GBL_EXPORT GblInstance* GblInstance_createExtWithClass (GblClass* pClass, size_t size)   GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblInstance_destroy            (GBL_SELF)                        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblInstance_construct          (GBL_SELF, GblType type)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblInstance_constructWithClass (GBL_SELF, GblClass* pClass)      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblInstance_destruct           (GBL_SELF)                        GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblInstance_check              (GBL_CSELF, GblType toType)       GBL_NOEXCEPT;
GBL_EXPORT GblInstance* GblInstance_cast               (GBL_SELF, GblType toType)        GBL_NOEXCEPT;
GBL_EXPORT GblInstance* GblInstance_try                (GBL_SELF, GblType toType)        GBL_NOEXCEPT;

GBL_EXPORT void*        GblInstance_private            (GBL_CSELF, GblType base)         GBL_NOEXCEPT;
GBL_EXPORT GblInstance* GblInstance_public             (const void* pPriv, GblType base) GBL_NOEXCEPT;

GBL_EXPORT GblType      GblInstance_typeOf             (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_EXPORT size_t       GblInstance_size               (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_EXPORT size_t       GblInstance_privateSize        (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_EXPORT size_t       GblInstance_totalSize          (GBL_CSELF)                       GBL_NOEXCEPT;

GBL_EXPORT GblClass*    GblInstance_class              (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblInstance_swizzleClass       (GBL_SELF, GblClass* pClass)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblInstance_sinkClass          (GBL_SELF)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblInstance_floatClass         (GBL_SELF)                        GBL_NOEXCEPT;

// ========== IMPL ==========

///\cond
#define GBL_INSTANCE_VCALL_(cType, method, ...)                                                       \
    GBL_STMT_START {                                                                                  \
        GBL_CLASSOF(cType)* pClass = GBL_INSTANCE_GET_CLASS(GBL_TUPLE_FIRST (__VA_ARGS__, 1), cType); \
        GBL_CTX_VERIFY(pClass, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);                                \
        GBL_CTX_VERIFY(pClass->method, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);                        \
        GBL_CTX_CALL(pClass->method(__VA_ARGS__));                                                    \
    } GBL_STMT_END

#define GBL_INSTANCE_VCALL_DEFAULT_(cType, method, ...)                                               \
    GBL_STMT_START {                                                                                  \
        GBL_CLASSOF(cType)* pClass = (GBL_CLASSOF(cType)*)GblClass_weakRefDefault(GBL_TYPEOF(cType)); \
        GBL_CTX_VERIFY(pClass, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);                                \
        GBL_CTX_VERIFY(pClass->method, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);                        \
        GBL_CTX_CALL(pClass->method(__VA_ARGS__));                                                    \
    } GBL_STMT_END
///\endcond

/*! \fn GblInstance* GblInstance_create(GblType type)
 * Creates an instance on the heap with a reference to the default class for the given type.
 * \note Only meta types with the GBL_TYPE_ROOT_FLAGS_INSTANTIABLE and without the
 *       GBL_TYPE_FLAG_ABSTRACT type flags set may be instantiated.
 * \relatesalso GblInstance
 * \param type instantiable type ID
 * \returns new, heap-allocated instance
 * \sa GblInstance_construct, GblInstance_createWithClass
 */

/*! \fn GblInstance* GblInstance_createWithClass(GblClass* pClass)
* Creates an instance using the given floating class to override the type's default class.
* \note The instance does not automatically assume ownership of the class. Use GblInstance_sinkClass
*       to bind the lifetime of a floating class to an instance.
* \relatesalso GblInstance
* \param pClass floating class
* \returns new, heap-allocated instance
* \sa GblInstance_constructWithClass
*/

/*! \fn GBL_RESULT GblInstance_construct(GblInstance* pSelf, GblType type)
* Equivalent to GblInstance_create(), except for using an existing GblInstance* allocation.
* This is equivalent to a placement new operation in C++.
* \note Only instances without private data can be constructed in this manner.
* \relatesalso GblInstance
* \param pSelf existing allocation
* \param type instantiable type ID
* \returns error code
* \sa GblInstance_create, GblInstance_constructWithClass
*/

/*! \fn GBL_RESULT GblInstance_constructWithClass(GblInstance* pSelf, GblClass* pClass)
* Equivalent to GblInstance_createWithClass(), except for using an existing GblInstance* allocation.
* This is equivalent to a placement new operation in C++.
* \relatesalso GblInstance
* \param pSelf existing allocation
* \param pClass floating class
* \returns error code
*/

/*! \fn GblRefCount GblInstance_destroy(GblInstance* pSelf)
* Finalizes then deletes an instance that was created on the heap, either unreferencing.
* its class if using the default or destroying it if using a sunk floating class.
* \relatesalso GblInstance
* \param pSelf heap-callocated instance
* \returns remaining number of active instances of the associated type
* \sa GblInstance_destruct
*/

/*! \fn GblRefCount GblInstance_destruct(GblInstance* pSelf)
* Equivalent to GblInstance_destroy(), except that the allocation is not deallocated.
* This is equivalent to a placement delete operation in C++.
* \relatesalso GblInstance
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
* \sa GblInstance_try
*/

/*! \fn GblInstance* GblInstance_cast(GblInstance* pSelf, GblType toType)
* Casts the given instance to the given type, erroring-out upon failure.
* \relatesalso GblInstance
* \param pSelf source instance
* \param toType casted result type
* \returns pointer to casted instance or NULL upon failure
* \sa GblInstance_check, GblInstance_try
*/

/*! \fn GblInstance* GblInstance_try(GblInstance* pSelf, GblType toType)
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

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_INSTANCE_H
