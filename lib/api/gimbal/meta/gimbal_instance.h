/*! \file
 *  \brief GblInstance structure and related functions
 *  \ingroup meta
 *  \sa gimbal_type.h, gimbal_class.h
 */

#ifndef GIMBAL_INSTANCE_H
#define GIMBAL_INSTANCE_H

#include "gimbal/core/gimbal_api_frame.h"
#include "gimbal_class.h"

#define GBL_INSTANCE_TYPE                                       (GBL_BUILTIN_TYPE(INSTANCE))

#define GBL_INSTANCE(instance)                                  ((GblInstance*)instance)
#define GBL_INSTANCE_CLASS(instance)                            (GblInstance_class(GBL_INSTANCE(instance)))
#define GBL_INSTANCE_CLASS_SUPER(instance)                      (GblInstance_classSuper(GBL_INSTANCE(instance)))
#define GBL_INSTANCE_CLASS_DEFAULT(instance)                    (GblInstance_classDefault(GBL_INSTANCE(instance)))
#define GBL_INSTANCE_TYPEOF(instance)                           (GblInstance_typeOf(GBL_INSTANCE(instance)))
#define GBL_INSTANCE_PRIVATE(instance, type)                    (GblInstance_private(GBL_INSTANCE(instance), type))
#define GBL_INSTANCE_PUBLIC(instPriv, type)                     (GblInstance_public((const void*)instPriv, type))

#define GBL_INSTANCE_CHECK(instance, toType)                    (GblInstance_check((GblInstance*)instance, toType))
#define GBL_INSTANCE_CHECK_PREFIX(instance, typePrefix)         (GBL_INSTANCE_CHECK(instance, typePrefix##_TYPE))
#define GBL_INSTANCE_CAST(instance, toType, cType)              ((cType*)GblInstance_cast((GblInstance*)instance, toType))
#define GBL_INSTANCE_CAST_PREFIX(instance, typePrefix)          (GBL_INSTANCE_CAST(instance, typePrefix##_TYPE, typePrefix##_STRUCT))
#define GBL_INSTANCE_TRY(instance, toType, cType)               ((cType*)GblInstance_try((GblInstance*)instance, toType))
#define GBL_INSTANCE_TRY_PREFIX(instance, typePrefix)           (GBL_INSTANCE_TRY(instance, typePrefix##_TYPE, typePrefix##_STRUCT))

#define GBL_INSTANCE_GET_CLASS(instance, toType, cType)         ((cType*)GblClass_cast(GblClass_peekFromInstance((GblInstance*)instance), toType))
#define GBL_INSTANCE_GET_CLASS_PREFIX(instance, typePrefix)     (GBL_INSTANCE_GET_CLASS(instance, typePrefix##_TYPE, typePrefix##_CLASS_STRUCT))
#define GBL_INSTANCE_TRY_CLASS(instance, toType, cType)         ((cType*)GblClass_try(GblClass_peekFromInstance((GblInstance*)instance), toType))
#define GBL_INSTANCE_TRY_CLASS_PREFIX(instance, typePrefix)     (GBL_INSTANCE_TRY_CLASS(instance, typePrefix##_TYPE, typePrefix##_CLASS_STRUCT))

#define GBL_INSTANCE_VCALL(type, classType, method, ...)        GBL_INSTANCE_VCALL_((void*), type, classType, method, __VA_ARGS__)
#define GBL_INSTANCE_VCALL_PREFIX(prefix, method, ...)          GBL_INSTANCE_VCALL(prefix##_TYPE, prefix##_CLASS_STRUCT, method, __VA_ARGS__)
#define GBL_INSTANCE_VCALL_SUPER(type, classType, method, ...)  GBL_INSTANCE_VCALL_(GBL_CLASS_SUPER, type, classType, method, __VA_ARGS__)
#define GBL_INSTANCE_VCALL_SUPER_PREFIX(prefix, method, ...)    GBL_INSTANCE_VCALL_SUPER(prefix##_TYPE, prefix##_CLASS_STRUCT, method, __VA_ARGS__)
#define GBL_INSTANCE_VCALL_DEFAULT(type, classType, method, ...)GBL_INSTANCE_VCALL_(GBL_CLASS_DEFAULT, type, classType, method, __VA_ARGS__)
#define GBL_INSTANCE_VCALL_DEFAULT_PREFIX(prefix, method, ...)  GBL_INSTANCE_VCALL_DEFAULT(prefix##_TYPE, prefix##_CLASS_STRUCT, method, __VA_ARGS__)

#define SELF    GblInstance*       pSelf
#define CSELF   const GblInstance* pSelf

GBL_DECLS_BEGIN

/*! \brief Base struct for all instantiable meta types.
 *  \ingroup meta
 *\details
 * An Instance simply represents an object that can be created from a given
 * GblType which shares some data with other instances of the same type,
 * located within their class.
 *
 * GblInstance is the base structure which is to be inherited by instances of all
 * instantiable types. This means placing it or a type inheriting from it as the
 * first member of an instance struct.
 * \sa GblClass, GblType, GblObject
 */
typedef struct GblInstance {
    GblClass*   pClass; ///< Pointer to Instance's Class
} GblInstance;

GBL_EXPORT GblInstance* GblInstance_create              (GblType type)              GBL_NOEXCEPT;
GBL_EXPORT GblInstance* GblInstance_createWithClass     (GblClass* pClass)          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblInstance_construct           (SELF, GblType type)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblInstance_constructWithClass  (SELF, GblClass* pClass)    GBL_NOEXCEPT;

GBL_EXPORT GblRefCount  GblInstance_destroy             (SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblInstance_destruct            (SELF)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblInstance_classSwizzle        (SELF, GblClass* pClass)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblInstance_classSink           (SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblInstance_classFloat          (SELF)                      GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblInstance_check               (CSELF, GblType toType)     GBL_NOEXCEPT;
GBL_EXPORT GblInstance* GblInstance_cast                (SELF, GblType toType)      GBL_NOEXCEPT;
GBL_EXPORT GblInstance* GblInstance_try                 (SELF, GblType toType)      GBL_NOEXCEPT;

GBL_EXPORT void*        GblInstance_private             (CSELF, GblType base)       GBL_NOEXCEPT;
GBL_EXPORT GblInstance* GblInstance_public              (const void* pPrivate,
                                                         GblType     base)          GBL_NOEXCEPT;

GBL_EXPORT GblType      GblInstance_typeOf              (CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT const char*  GblInstance_typeName            (CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblSize      GblInstance_size                (CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblSize      GblInstance_privateSize         (CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblSize      GblInstance_totalSize           (CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblClass*    GblInstance_class               (CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblClass*    GblInstance_classSuper          (CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblClass*    GblInstance_classDefault        (CSELF)                     GBL_NOEXCEPT;

// ========== IMPL ==========

///\cond
#define GBL_INSTANCE_VCALL__N(method, ...)  GBL_API_CALL(pClass->method(__VA_ARGS__))

#define GBL_INSTANCE_VCALL__2(method, inst) GBL_API_VCALL__N(method, inst)

#define GBL_INSTANCE_VCALL__(...)               \
    GBL_VA_OVERLOAD_SELECT(GBL_INSTANCE_VCALL_, GBL_VA_OVERLOAD_SUFFIXER_2_N, __VA_ARGS__)(__VA_ARGS__);

#define GBL_INSTANCE_VCALL_(op, type, classType, method, ...)                                   \
    GBL_STMT_START {                                                                            \
        classType* pClass = GBL_INSTANCE_GET_CLASS(GBL_ARG_1(__VA_ARGS__), type, classType);   \
        pClass = (classType*)op(pClass);                                                        \
        GBL_API_VERIFY(pClass, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);                          \
        GBL_API_VERIFY(pClass->method, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);                  \
        GBL_API_CALL(pClass->method(__VA_ARGS__));                                              \
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
* \note The instance does not automatically assume ownership of the class. Use GblInstance_classSink
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

/*! \fn GBL_RESULT GblInstance_classSwizzle(GblInstance* pSelf, GblClass* pClass)
* Releases ownership of the instance's current class, replacing it with a type-compatible class.
* This is analogous to Objective-C's "is-a swizzling."
* \note The instance does not automatically assume ownersip of the new class.
* \relatesalso GblInstance
* \param pSelf instance
* \param pClass replacement GblClass of compatible type
* \returns result code
* \sa GblInstance_classSink
*/

/*! \fn GBL_RESULT GblInstance_classSink(GblInstance* pSelf)
* Claims ownership of the previously floating class contained by the instance, binding
* the lifetime of the two, so that the class will be finalized when the instance is finalized.
* \relatesalso GblInstance
* \param pSelf instance
* \returns result code
* \sa GblInstance_classFloat
*/

/*! \fn GBL_RESULT GblInstance_classFloat(GblInstance* pSelf)
* Releases ownership of the owned class contained by the instance. The class will no
* longer be finalized along with the given instance.
* \relatesalso GblInstance
* \param pSelf instance
* \returns result code
* \sa GblInstance_classSink
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

/*! \fn const char* GblInstance_typeName(const GblInstance* pSelf)
* Returns the type name associated with the given instance.
* \relatesalso GblInstance
* \param pSelf instance pointer
* \returns name or NULL if the instance is NULL
*/

/*! \fn GblSize GblInstance_size(const GblInstance* pSelf)
* Returns the size of the public instance struct for the given instance.
* \relatesalso GblInstance
* \param pSelf instance pointer
* \returns size or 0 if instance is NULL
* \sa GblInstance_privateSize, GblInstance_totalSize
*/

/*! \fn GblSize GblInstance_privateSize(const GblInstance* pSelf)
* Returns the size of the private instance struct for the given instance.
* \relatesalso GblInstance
* \param pSelf instance pointer
* \returns size or 0 if instance is NULL
*/

/*! \fn GblSize GblInstance_totalSize(const GblInstance* pSelf)
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

/*! \fn GblClass* GblInstance_classSuper(const GblInstance* pSelf)
* Returns the super (parent) class structure of the class associated with the given instance.
* \relatesalso GblInstance
* \param pSelf instance pointer
* \returns super class pointer or NULL if instance is NULL
*/

/*! \fn GblClass* GblInstance_classDefault(const GblInstance* pSelf)
* Returns the default class structure of the type associated with the given instance.
* \relatesalso GblInstance
* \param pSelf instance pointer
* \returns default class pointer or NULL if instance is NULL
*/
GBL_DECLS_END

#undef SELF
#undef CSELF


#endif // GIMBAL_INSTANCE_H
