/*! \file
 *  \brief   GblClass virtual-table structure and APIE
 *  \ingroup meta
 *
 *  This file contains the GblClass structure,
 *  which is the inherited base type and first
 *  data member of all derievd class structures.
 *
 *  While GblClass itself is empty other than its
 *  basic type info, a derived class typically
 *  contains data which is shared by all instances
 *  of a type, for deduplication and efficiency.
 *
 *  This data is typically in the form of:
 *  - Overridable virtual methods (in the form of function pointers)
 *  - Static class data
 *
 *  \sa gimbal_instance.h, gimbal_type.h
 *
 *  \author    2023 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_CLASS_H
#define GIMBAL_CLASS_H

#include "../types/gimbal_type.h"

/*! \name  Class Operators
 *  \brief Builtin operations for class structures
 *  \relatesalso GblClass
 *  @{
 */
//! Function-style cast to GblClass*: will always pass, here for consistence with other operators
#define GBL_CLASS(klass)                    ((GblClass*)klass)
//! Wraps GblClass_typeOf(), returning the type associated with the given class
#define GBL_CLASS_TYPEOF(klass)             GBL_CLASS_TYPEOF_(klass)
//! Wraps GblClass_private(), returning the private structure on the class associated with the given type
#define GBL_CLASS_PRIVATE(cType, klass)     GBL_CLASS_PRIVATE_(cType, klass)
//! Wraps GblClass_public(), returning the public class structure from the given type's private structure
#define GBL_CLASS_PUBLIC(cType, priv)       GBL_CLASS_PUBLIC_(cType, privKlass)
//! Returns GBL_TRUE if the given class is type-compatible with and can be safely casted to the given type
#define GBL_CLASS_TYPECHECK(cType, klass)   GBL_CLASS_TYPECHECK_(type, klass)
//! Wraps GblClass_cast(), casting the given class to another type, returning NULL and raising an error upon failure
#define GBL_CLASS_CAST(cType, klass)        GBL_CLASS_CAST_(cType, klass)
//! Wraps GblClass_as(), returning the given class as another type or gracefully returning NULL upon failure
#define GBL_CLASS_AS(cType, klass)          GBL_CLASS_AS_(cType, klass)
//! @}

/*! \name  Static Class Type UUID and cast operators
 *  \brief UUID and cast operators for builtin static root class type
 *  @{
 */
#define GBL_STATIC_CLASS_TYPE    (GBL_BUILTIN_TYPE(STATIC_CLASS)) //!< GblType UUID for builtin static class primitive
#define GBL_STATIC_CLASS(klass)  GBL_STATIC_CLASS_(klass)         //!< Casts a class to a static class
//! @}

#define GBL_SELF_TYPE GblClass

GBL_DECLS_BEGIN

/*! \brief   Base struct for all type classes.
 *  \ingroup meta
 *
 *  A class represents a collection of data that is shared among
 *  all instances of a given type. This data is typically in the
 *  form of function pointers for modeling overridable methods
 *  or regular data for modeling static member variables.
 *
 *  GblClass is the base structure which is to be inherited by all
 *  class structures within the meta type system. This means placing
 *  it or a type "inheriting" from it as the first member of a
 *  class struct, when using C.

 *  \sa GblInstance, GblType
 */
typedef struct GblClass {
    GBL_PRIVATE_BEGIN
        uintptr_t metaClassInfo;  //!< PRIVATE: Pointer-sized opaque member
    GBL_PRIVATE_END
} GblClass;

/*! \name  Default Class Management
 *  \brief Methods for managing references to internally-managed default class instances
 *  @{
 */
//! Returns a reference to the default class for \p type, instantiating it if necessary
GBL_EXPORT GblClass*   GblClass_refDefault     (GblType type) GBL_NOEXCEPT;
//! Releases a reference to the default version of the given class, possibly freeing it
GBL_EXPORT GblRefCount GblClass_unrefDefault   (GBL_SELF)     GBL_NOEXCEPT;
//! Returns a reference to the default class for \p type or NULL if it hasn't been created
GBL_EXPORT GblClass*   GblClass_weakRefDefault (GblType type) GBL_NOEXCEPT;
//! @}

/*! \name  Floating Class Mangement
 *  \brief Methods for managing lifetime of dynamic override class instances
 *  @{
 */
//! Creates a standalone, unowned, "floating" class for the given type, which can override defaults
GBL_EXPORT GblClass*  GblClass_createFloating    (GblType type,
                                                  size_t  size/*=DEFAULT*/) GBL_NOEXCEPT;
//! Constructs a standalone, unowned, overridable, "floating" class for the given type in-place
GBL_EXPORT GBL_RESULT GblClass_constructFloating (GBL_SELF, GblType type)   GBL_NOEXCEPT;
//! Destroys the standalone, "floating" class, releasing its memory allocation
GBL_EXPORT GBL_RESULT GblClass_destroyFloating   (GBL_SELF)                 GBL_NOEXCEPT;
//! Destructs the standalone, "floating" class, without releasing any allocated resources
GBL_EXPORT GBL_RESULT GblClass_destructFloating  (GBL_SELF)                 GBL_NOEXCEPT;
//! @}

/*! \name  Type Conversions
 *  \brief Methods for type casting and checking
 *  \relatesalso GblClass
 *  @{
 */
//! Returns GBL_TRUE if the given class can be casted to \p toType
GBL_EXPORT GblBool   GblClass_check (GBL_CSELF, GblType toType) GBL_NOEXCEPT;
//! Casts the given class to a \p toType class, or returns NULL and emits an error if unsuccessful
GBL_EXPORT GblClass* GblClass_cast  (GBL_SELF, GblType toType)  GBL_NOEXCEPT;
//! Casts the given class to a \p toType class, or gracefully returns NULL without erroring
GBL_EXPORT GblClass* GblClass_as    (GBL_SELF, GblType toType)  GBL_NOEXCEPT;
//! @}

/*! \name  Public and Private Data
 *  \brief Methods for accessing public and private segments
 *  \relatesalso GblClass
 *  @{
 */
//! Returns the private structure associated with the given \p base type of the class
GBL_EXPORT void*     GblClass_private (GBL_CSELF, GblType base)            GBL_NOEXCEPT;
//! Casts back to the class structure from a base type's private data segment structure
GBL_EXPORT GblClass* GblClass_public  (const void* pPrivate, GblType base) GBL_NOEXCEPT;
//! @}

/*! \name  Static Type Info
 *  \brief Methods for getting static class information
 *  \relatesalso GblClass
 *  @{
 */
//! Returns the GblType UUID associated with the given class
GBL_EXPORT GblType GblClass_typeOf      (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the size of the given class's public class structrue
GBL_EXPORT size_t  GblClass_size        (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the combined size of the class's private data segments
GBL_EXPORT size_t  GblClass_privateSize (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the class's combined DEFAULT size (not extended allocation size)
GBL_EXPORT size_t  GblClass_totalSize   (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name  Dynamic Flags
 *  \brief Methods for checking dyanmic runtime flags
 *  \relatesalso GblClass
 */
//! Returns GBL_TRUE if the class is the internally-managed default for its type
GBL_EXPORT GblBool GblClass_isDefault       (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the class differs from the default class for its type
GBL_EXPORT GblBool GblClass_isOverridden    (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the class is an interfaced type
GBL_EXPORT GblBool GblClass_isInterface     (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the class is another class's implementation of an interface
GBL_EXPORT GblBool GblClass_isInterfaceImpl (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the class is a non-default floating class and is unowned by an instance
GBL_EXPORT GblBool GblClass_isFloating      (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the class is a non-default floating class is uowned by an instance
GBL_EXPORT GblBool GblClass_isOwned         (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the class was constructed in-place with an existing allocation
GBL_EXPORT GblBool GblClass_isInPlace       (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name  Related Classes
 *  \brief Methods for fetching associated classes
 *  \relatesalso GblClass
 */
//! Returns the default class of the class's parent type or NULL if it's a root class
GBL_EXPORT GblClass* GblClass_super   (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the default implementation of the given class
GBL_EXPORT GblClass* GblClass_default (GBL_CSELF) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond
#define GblClass_createFloating(...) \
    GblClass_createFloatingDefault_(__VA_ARGS__)
#define GblClass_createFloatingDefault_(...) \
    GblClass_createFloatingDefault__(__VA_ARGS__, 0)
#define GblClass_createFloatingDefault__(type, size, ...) \
    (GblClass_createFloating)(type, size)

#define GBL_CLASS_TYPEOF_(klass)           (GblClass_typeOf(GBL_CLASS(klass)))
#define GBL_CLASS_PRIVATE_(cType, klass)   ((GBL_CLASS_PRIVATE_STRUCT(cType)*)GblClass_private(GBL_CLASS(klass), GBL_TYPEID(cType)))
#define GBL_CLASS_PUBLIC_(cType, priv)     ((GBL_CLASS_STRUCT(cType)*)GblClass_public(klassPriv, GBL_TYPEID(cType))
#define GBL_CLASS_TYPECHECK_(cType, klass) (GblClass_check((GblClass*)klass, GBL_TYPEID(cType)))
#define GBL_CLASS_CAST_(cType, klass)      ((GBL_CLASS_STRUCT(cType)*)GblClass_cast((GblClass*)klass, GBL_TYPEID(cType)))
#define GBL_CLASS_AS_(cType, klass)        ((GBL_CLASS_STRUCT(cType)*)GblClass_as((GblClass*)klass, GBL_TYPEID(cType)))
#define GBL_STATIC_CLASS_(klass) )         (GblClass_cast((GblClass*)klass, GBL_STATIC_CLASS_TYPE))
//! \endcond

/*! \def GBL_STATIC_CLASS_TYPE
 *   Builtin type ID for a class-only types
 *  \ingroup metaBuiltinTypes
 */

/*! \fn GBL_CLASS(klass)
 *  \param klass pointer to a GblClass-compatible
 *  \returns klass C-style casted to GblClass*
 *  \relatesalso GblClass
 * Convenience function-style casting macro.
 */

/*! \fn GBL_CLASS_TYPEOF(klass)
 *  \param klass pointer to a GblClass-compatible
 *  \returns type associated with the class, or GBL_INVALID_TYPE if NULL
 *  \relatesalso GblClass
 *  \sa GblClass_typeOf()
 * Convenience wrapper around GblClass_typeOf() which automatically casts.
 */

/*! \fn GBL_CLASS_SUPER(klass)
 *  \param klass pointer to a GblClass-compatible
 *  \returns parent class of the given class, or GBL_INVALID_TYPE if NULL
 *  \relatesalso GblClass
 *  \sa GblClass_super()
 * Convenience wrapper around GblClass_super() which automatically casts.
 */

/*! \fn GBL_CLASS_DEFAULT(klass)
 *  \param klass pointer to a GblClass-compatible
 *  \returns default class of the given class, or GBL_INVALID_TYPE if NULL
 *  \relatesalso GblClass
 *  \sa GblClass_default()
 * Convenience wrapper around GblClass_default() which automatically casts.
 */

/*! \fn GBL_CLASS_PRIVATE(klass, type)
 *  \param klass pointer to a GblClass-compatible
 *  \param type supertype owning the private data
 *  \returns private data owned by the given superType or NULL if type there is
 *  none or type is not a parent, or type is invalid
 *  \relatesalso GblClass
 *  \sa GblClass_private()
 * Convenience wrapper around GblClass_private() which automatically casts.
 */

/*! \fn GBL_CLASS_PUBLIC(klassPrivate, type)
 *  \param klassPrivate void* pointer to private class data
 *  \param type supertype owning the private data
 *  \returns public GblClass* pointer or NULL if none could be found
 *  \relatesalso GblClass
 *  \sa GblClass_public()
 * Convenience wrapper around GblClass_public().
 */

/*! \fn GBL_CLASS_TYPECHECK(klass, toType)
 *  \param klass pointer to a GblClass-compatible
 *  \param toType the desired destination type
 *  \returns true if the given class can be casted to a calss of toType
 *  \relatesalso GblClass
 *
 *  \sa GblClass_check(), GBL_CLASS_TYPECHECK_REFIX()
 * Convenience wrapper around GblClass_check() which automatically casts.
 */
/*! \fn GBL_CLASS_TYPECHECK_PREFIX(klass, typePrefix)
 *  \param klass pointer to a GblClass-compatible
 *  \param typePrefix prefix of the given type (type name macro identifier minus _TYPE postfix)
 *  \returns true if the given class can be casted to a calss of toType
 *  \relatesalso GblClass
 *  \sa GBL_CLASS_TYPECHECK_PREFIX(), GblClass_check()
 *
 * Convenience wrapper around GBL_CLASS_TYPECHECK() which automatically appends _TYPE suffix.
 *  \note
 * This macro is typically used for definining convenience function-style casting utilities
 * for derived types.
 */

/*! \fn GBL_CLASS_CAST(klass, toType, cType)
 *  \param klass pointer to a GblClass-compatible
 *  \param toType desination type for resulting class
 *  \param cType C structure for the given type's class
 *  \returns klass casted to cType if the cast was successful, otherwise NULL
 *  \relatesalso GblClass
 *  \sa GBL_CLASS_CAST_PREFIX(), GblClass_cast(
 *
 * Convenience wrapper around GblClass_cast() which automatically performs casting.
 */

/*! \fn GBL_CLASS_CAST_PREFIX(klass, typePrefix)
 *  \param klass pointer to a GblClass-compatible
 *  \param typePrefix prefix of destination type (NAME macro minus the _TYPE suffix)
 *  \note This macro also requires a macro be defined that is the given typePrefix
 *   appended with _CLASS_STRUCT, which it uses as the destination class type for casting.
 *  \returns klass casted to the formed type identifier upon success, NULL otherwise
 *  \relatesalso GblClass
 *  \sa GBL_CLASS_CAST(), GblClass_cast()
 *
 * Convenience wrapper around GBL_CLASS_CAST() which automatically deduces class structure
 * and type identifier.
 * \note
 * This macro is typically used for definining convenience function-style casting utilities
 * for derived types.
 */

/*! \fn GBL_CLASS_TRY(klass, toType, cType)
 *  \param klass pointer to a GblClass-compatible
 *  \param toType desination type for resulting class
 *  \param cType C structure for the given type's class
 *  \returns klass casted to cType if the cast was successful, otherwise NULL
 *  \relatesalso GblClass
 *  \sa GBL_CLASS_TRY_PREFIX(), GblClass_as(
 *
 * Convenience wrapper around GblClass_as() which automatically performs casting.
 */

/*! \fn GBL_CLASS_TRY_PREFIX(klass, typePrefix)
 *  \param klass pointer to a GblClass-compatible
 *  \param typePrefix prefix of destination type (NAME macro minus the _TYPE suffix)
 *  \note This macro also requires a macro be defined that is the given typePrefix
 *   appended with _CLASS_STRUCT, which it uses as the destination class type for casting.
 *  \returns klass casted to the formed type identifier upon success, NULL otherwise
 *  \relatesalso GblClass
 *  \sa GBL_CLASS_TRY(), GblClass_as()
 *
 * Convenience wrapper around GBL_CLASS_TRY() which automatically deduces class structure
 * and type identifier.
 * \note
 * This macro is typically used for defining convenience function-style casting utilities
 * for derived types.
 *
 */

/*!
 * \fn GblClass* GblClass_refDefault(GblType type)
 *  Returns a new reference to the existing internally managed default class for the given type or
 *  lazily allocates a new one with an initial refCount of 1.
 * \note
 *  When you're done using the reference, it must be released using GblClass_unrefDefault() in order to avoid
 *  memory leaks!
 * \relatesalso GblClass
 * \param type classed type ID
 * \returns reference to internally managed class
 * \sa GblClass_weakRefDefault, GblClass_unrefDefault
 */

/*!
 * \fn GblRefCount GblClass_unrefDefault(GblClass* pSelf)
 *  Release a reference to the default class associated with pSelf,
 *  destroying the class if it's reference count hits 0.
 *  \note
 *  This should not be called to release a previously floating
 *  class, as it is not reference-counted. See GblClass_destructFloating
 *  and GblClass_destroyFloating
 *  \note
 *  It is safe to call this function with a NULL class, as
 *  libGimbal tries to be consistent with free() semantics.
 *  \note Types may be flagged as "persisting", despite
 *  their reference counts hitting zero. This is used for
 *  builtin types. See GBL_TYPE_FLAG_CLASS_PINNED.
 *  \param pSelf reference to class
 *  \returns remaining reference count
 * \relatesalso GblClass
 * \sa GblClass_refDefault
 */

/*!
 * \fn GblClass* GblClass_weakRefDefault(GblType type)
 *   Returns a pointer to the default class for the given type without incrementing refCount.
 *   \note
 *   If there has not yet been a class created, or one is no longer active, this call will
 *   raise an error followed by returning NULL. Since no references are being created, only
 *   use this within contexts where you know a class reference already exists.
 * \relatesalso GblClass
 * \param type type of desired class
 * \returns pointer to a GblClss or NULL upon failure
 * \sa GblClass_refDefault
 */

/*!
 * \fn GblClass* GblClass_createFloating(GblType type)
 *  Creates a new, unowned class for the given type on the heap.
 * \relatesalso GblClass
 * \param type classed type ID
 * \returns dynamically allocated unowned class
 * \sa GblClass_constructFloating, GblClass_destroyFloating, floatingClasses
 */

/*!
 * \fn GBL_RESULT GblClass_destroyFloating(GblClass* pSelf)
 * \relatesalso GblClass
*  Destructs then frees the given heap-allocated, floating
*  class.
*  \note
*  It is safe to call this function with a NULL class, as
*  libGimbal tries to be consistent with free() semantics.
*  \attention
*  Do not call this with a stack-constructed in-place floating class
*  constructed with GblClass_constructFloating()
*  Luckily libGyro will catch it for you and return an error.
*  \param pSelf GblClass pointer
*  \returns destructor result code or GBL_RESULT_ERROR_INVALID_OPERATION
*  \sa GblClass_destructFloating, GblClass_createFloating
 */

/*!
 * \fn GBL_RESULT GblClass_constructFloating(GblClass* pSelf, GblType type)
 *  Placement constructs an unowned class for the given type using an existing
 *  user-provided allocation.
 * \relatesalso GblClass
 * \param pSelf pointer to instance allocation
 * \param type type of instance to construct
 * \returns result code
 * \sa GblClass_createFloating, GblClass_destructFloating
 */

/*!
 * \fn GBL_RESULT GblClass_destructFloating(GblClass* pSelf)
 *  Calls placement delete on the given floating class which was
 *  constructed in-place, using GblClass_constructFloating.
 *  It is safe to call this function with a NULL class, as
 *  libGimbal tries to be consistent with free() semantics.
 *  \attention
 *  Do not call this with a heap-allocated floating class,
 *  created with GblClass_createFloating().
 *  Luckily libGyro will catch it for you and return an error.
 *  \param pSelf class pointer
 *  \return destructor result code or GBL_RESULT_ERROR_INVALID_OPERATION
 * \relatesalso GblClass
 * \sa GblClass_destroyFloating, GblClass_constructFloating
*/

/*!
 * \fn GblBool GblClass_check(const GblClass* pSelf, GblType toType)
 * Returns true if the given class can be casted to the given class type.
 * \note Also returns true if pClass is NULL AND toType is GBL_TYPE_INVALID,
 * since that would semantically be a valid cast to the invalid type.
 * \param pSelf class pointer
 * \param toType desired destination type
 * \returns true upon success
 * \relatesalso GblClass
 * \sa GblClass_cast, GblClass_as
*/

/*!
 * \fn GblClass* GblClass_cast(GblClass* pSelf, GblType toType)
 * Casts the given class to a class of the given type, raising errors and
 * returning NULL upon failure.
 * \param pSelf source class pointer
 * \param toType desired destination type
 * \returns pointer to GblClass upon success, NULL upon failure
 * \relatesalso GblClass
 * \sa GBL_CLASS_CAST(), GblClass_check, GblClass_as
*/

/*!
 * \fn GblClass* GblClass_as(GblClass* pSelf, GblType toType)
 * Equivalent to GblClass_cast(), except gracefully returning NULL
 * with no errors getting raised if the given class could not be
 * casted to toType.
 * \note This is equivalent to a C++ dynamic cast.
 * \param pSelf source class
 * \param toType desired destination type
 * \returns pointer to GblClass or NULL upon failure
 * \relatesalso GblClass
 * \sa GBL_CLASS_TRY(), GblClass_cast(), GblClass_check()
*/

/*!
 * \fn void* GblClass_private(const GblClass* pSelf, GblType type)
 * Retrieves the private data pointer associated with the given base type or
 * NULL if there isn't one.
 * \param pSelf class pointer
 * \param type type owning the private data
 * \returns pointer to private data or NULL upon failure
 * \relatesalso GblClass
 * \sa GblClass_public()
*/

/*!
 * \fn GblClass* GblClass_public(const void* pPrivate, GblType base)
 * Retrieves the public GblClass corresponding to the given private data
 * pointer for the given type or base type or NULL upon failure.
 * \param pPrivate pointer to private data
 * \param base type or base type that registered the private data
 * \returns pointer to public GblClass* or NULL upon failure
 * \relatesalso GblClass
*/

/*!
 * \fn GblType GblClass_typeOf(const GblClass* pSelf)
 * Returns the type associated with the given GblClass pointer,
 * or GBL_INVALID_TYPE if the pointer is NULL.
 * \param pSelf pointer to a GblClass
 * \returns type ID of the class or GBL_INVALID_TYPE if NULL
 * \relatesalso GblClass
*/

/*!
 * \fn size_t  GblClass_size(const GblClass* pSelf)
 * Returns the size of the public class structure for the
 * given GblClass pointer or 0 if NULL
 * \param pSelf pointer to a GblClass
 * \returns size or 0 if class is NULL
 * \relatesalso GblClass
 * \sa GblClass_privateSize(), GblClass_totalSize()
*/

/*!
 * \fn size_t  GblClass_privateSize(const GblClass* pSelf)
 * Returns the TOTAL size of the private data structure
 * associated with the type of the given class and all
 * of its base types.
 * \param pSelf pointer to a GblClass
 * \returns size or 0 if class is NULL
 * \relatesalso GblClass
 * \sa GblClass_size(), GblClass_totalSize()
*/

/*!
 * \fn size_t  GblClass_totalSize(const GblClass* pSelf)
 * Returns the total combined size of the public and private
 * class structures for the class type and all of its inherited
 * bases.
 * \param pSelf pointer to a GblClass
 * \returns size or 0 if class is NULL
 * \relatesalso GblClass
 * \sa GblClass_size(), GblClass_privateSize()
*/

/*!
 * \fn GblClass* GblClass_super(const GblClass* pSelf)
 * Returns the default class for the parent type of
 * the given class type. This is typically used in the implementation of
 * overridden methods to allow you to call into the base class's
 * implementations.
 * \note The returned class is a weak reference and does not impact
 * the reference count for the parent's default class.
 * \param pSelf pointer to a GblClass
 * \returns pointer to parent GblClass or NULL if the given class is
 * NULL or has no parent
 * \relatesalso GblClass
 * \sa GblClass_default()
*/

/*!
 * \fn GblClass* GblClass_default(const GblClass* pSelf)
 * Returns a weak reference to the internally-managed default class
 * corresonding to the given class's type. This will only be different
 * than the given argument if the class has been created either as a
 * floating class, or if it is a pointer to the implementation of an
 * interface. In the case of an interface, this is how you access the
 * interface's default implementation as well.
 * \note The returned class is a weak reference and does not impact
 * the reference count for the default class.
 * \param pSelf pointer to a GblClass
 * \returns pointer to default GblClass or NULL if the given class is
 * NULL
 * \relatesalso GblClass
 * \sa GblClass_isDefault()
*/

/*!
 * \fn GblBool GblClass_isDefault(const GblClass* pSelf)
 * Returns true if the given GblClass pointer points to the
 * internally-managed default class for its type or false if it
 * not the default.
 * \note Note: The class will not be the default if it was created
 * as separate floating class, or if it's the implementation of a
 * GblInterface on another GblClass.
 * \param pSelf pointer to a GblClass
 * \returns true if default, false if not or if pSelf is NULL
 * \relatesalso GblClass
 * \sa GblClass_default()
*/

/*!
 * \fn GblBool GblClass_isOverridden(const GblClass* pSelf)
 * Inspects the internal memory of the given GblClass pointer
 * and compares it to that of the internally managed default
 * class, returning true if they differ or false if they are the
 * same.
 * \note Note: Just because a class is a floating class or an
 * interface implementation doesn't necessarily mean it has
 * actually been overridden to have non-default values!
 * \param pSelf pointer to a GblClass
 * \returns true if overridden, false if not or if pSelf is NULL
 * \relatesalso GblClass
*/

/*!
 * \fn GblBool GblClass_isFloating(const GblClass* pSelf)
 * Returns true if the given GblClass is an unowned, floating
 * class which must be manually managed, or false if it is
 * either the default class or has been sunk by a GblInstance.
 * \param pSelf pointer to a GblClass
 * \returns true if floating, false if not or if pSelf is NULL
 * \sa GblClass_createFloating(), GblClass_constructFloating(),
 * GblInstance_sinkClass(),
 * GblInstance_floatClass()
 * \relatesalso GblClass
*/

/*!
 * \fn GblBool GblClass_isFloating(const GblClass* pSelf)
 * Returns true if the given GblClass is an unowned, floating
 * class which must be manually managed, or false if it is
 * either the default class or has been sunk by a GblInstance.
 * \param pSelf pointer to a GblClass
 * \returns true if floating, false if not or if pSelf is NULL
 * \sa GblClass_createFloating(), GblClass_constructFloating(),
 * GblInstance_sinkClass(),
 * GblInstance_floatClass()
 * \relatesalso GblClass
*/

/*!
 * \fn GblBool GblClass_isInterface(const GblClass* pSelf)
 * Returns true if the given GblClass is actually of or
 * derived from GBL_INTERFACE_TYPE, false otherwise.
 * \param pSelf pointer to a GblClass
 * \returns true if pSelf is a GblInterface, false if not or if pSelf is NULL
 * \sa GblClass_isInterfaceImpl
 * \relatesalso GblClass
*/

/*!
 * \fn GblBool GblClass_isInterfaceImpl(const GblClass* pSelf)
 * Returns true if the given GblClass an interface *implementation*
 * wihch has been mapped onto another GblClass, false otherwise.
 * \param pSelf pointer to a GblClass
 * \returns true if pSelf is a mapped GblInterface, false if not or if pSelf is NULL
 * \sa GblClass_isInterface
 * \relatesalso GblClass
*/

/*!
 * \fn GblBool GblClass_isOwned(const GblClass* pSelf)
 * Returns true if the given GblClass is a previously floating, nondefault
 * class which has been sunk and is now owned by a GblInstance.
 * \note This means that its lifetime has been bound to that of the instance,
 * and it will be destructed (and deleted if necessary) with the instance.
 * \param pSelf pointer to a GblClass
 * \returns true if pSelf is a owned by a GblInstance, false if not or if pSelf is NULL
 * \sa GblInstance_sinkClass(), GblInstance_floatClass()
 * \relatesalso GblClass
*/

/*!
 * \fn GblBool GblClass_isInPlace(const GblClass* pSelf)
 * Returns true if the given GblClass was constructed from an existing,
 * user-managed allocation, using GblClass_constructFloating().
 * \note If a GblClass is both floating and in-place, it means that you are
 * responsible for uninitializing it manually via GblClass_destructFloating()
 * \param pSelf pointer to a GblClass
 * \returns true if pSelf was constructed in-place, false if not or if pSelf is NULL
 * \sa GblClass_constructFloating(), GblClass_destructFloating()
 * \relatesalso GblClass
*/

/*! \page Classes Classes
 *  \brief Overview of classed types
 *  \tableofcontents
 *
 * ## GblClass
 *  \copydoc GblClass
 *
 * ## Floating Classes
 *
 *  Typically, when you wish to override virtual methods on a class to provide
 *  custom functionality, you would need to register a new, derived static type
 *  inheriting from the existing type and overriding the desired methods within
 *  the class constructor.
 *
 *  This is good for when you need to implement lots of custom state and logic or
 *  for when this logic is to be applied to all instances of a class... However,
 *  this can be extremely inconvenient and cumbersome when you simply wish to
 *  quickly override one or two methods on a single instance.
 *
 *  This is where libGimbal's floating classes come into play. Lets say you
 *  wish to quickly create a new GblObject with a custom event handler:
 *
 *  \code{.c}
 *      GblObject* pMyObj = GblInstance_create(GBL_OBJECT_TYPE);
 *      GblIEventHandler* pHandler = GBL_EVENT_HANDLER(pMyObj);
 *
 *      // I want to override this, but don't want to register a new type!
 *      GblIEventHandler_eventNotify(pHandler, NULL);
 *  \endcode
 *  By creating a separate "floating" class, we are able to perform ad-hoc
 *  method overriding for a single instance:
 *  \code{.c}
 *      // We have a brand-new, unowned class just for us!
 *      GblObjectClass* pClass = GBL_OBJECT_CLASS(GblClass_createFloating(GBL_OBJECT_TYPE));
 *
 *      // override a virtual method on the event handler interface to do our own thing
 *      pClass->iEventHandlerIFace.pFnEventNotify = my_event_notifier_function_;
 *
 *      // now create an instance *using our floating class* instead of the default class:
 *      GblObject* pObj = GBL_OBJECT(GblInstance_createWithClass(pClass));
 *
 *      // Tell the class to assume ownership of our "floating" class by "sinking" it.
 *      // If you don't sink the class, it's assumed you are maintaining its lifetime manually,
 *      // which can be useful if you want to share among multiple instances.
 *      GblInstance_sinkClass(GBL_INSTANCE(pObj));
 *
 *      // now it calls into our method when the interface is used!
 *      GblIEventHandler_eventNotify(GBL_IEVENT_HANDLER(pObj), NULL);
 *
 *      // Destroys the sunk class with it
 *      GblInstance_destroy(GBL_INSTANCE(pObj));
 *  \endcode
 *  \note
 *  If we do not "sink" the floating class, then we have to destory it manually,
 *  because its lifetime will not yet be bound to the instance. This can actually
 *  be useful for allowing you to manually manage the class's lifetime as it's
 *  shared among multiple instances.
 *
 *  For extra credit, we can do the same trick with stack-allocated types as well:
 *  \code{.c}
 *      // call placement constructor on existing memory
 *      GblObjectClass klass;
 *      GblClass_constructFloating(GBL_CLASS(&klass), GBL_OBJECT_TYPE);
 *
 *      pClass->iEventHandlerIFace.pFnEventNotify = my_event_notifier_function_;
 *
 *      // call placement constructor on existing memory
 *      GblObject object;
 *      GblInstance_constructWithClass(GBL_INSTANCE(&object), &klass);
 *
 *      GblInstance_sinkClass(GBL_INSTANCE(&object));
 *
 *      // now it calls into our shit when the interface is used!
 *      GblIEventHandler_eventNotify(GBL_IEVENT_HANDLER(&object), NULL);
 *
 *      // Call placement destructor which doesn't deallocate
 *      GblInstance_destruct(GBL_INSTANCE(&object));
 *  \endcode
 * ## Class Swizzling
 *
 *  Sometimes you would like to do something similar to the previous example,
 *  with ad-hoc virtual method overriding, but the creation of the object is
 *  not actually under your control. This is a time for libGimbal's class
 *  swizzling, inspired by Objective-C's "is-a" swizzling:
 *  \code{.c}
 *      GblObject* pObject = GblObject_create(GBL_OBJECT_TYPE,
 *                                            "name",    "MyObject",
 *                                            "userdata", (void*)0xdeadbabe,
 *                                            NULL);
 *
 *      GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pObject);
 *
 *      //nothing fancy, regular-ass reference counted internally managed, instance-shared class...
 *      assert(GblClass_isDefault(GBL_CLASS(pClass));
 *      assert(!GblClass_isOverridden(GBL_CLASS(pClass));
 *      assert(!GblClass_isFloating(GBL_CLASS(pClass));
 *
 *      // k, now lets make a new class and do some C++'y shit upon events...
 *      GblObjectClass* pClass2 = GBL_OBJECT_CLASS(GblClass_createFloating(GBL_OBJECT));
 *
 *      // a lambda is nice for being all ad-hoc
 *      pClass2->iEventHandlerIFace.pFnEvent = [](GblIEventHandler* pSelf, GblEvent* pEvent) {
 *          GBL_CTX_BEGIN(NULL);
 *          GBL_CTX_VERBOSE("So %s just sent a %s to a C++ lambda...",
 *                          GblType_name(GBL_CLASS_TYPEOF(pSelf)),
 *                          GblType_name(GblEvent_typeof(pEvent)));
 *          GblEvent_accept(pEvent);
 *          GBL_CTX_END();
 *      };
 *
 *      // k, this class is now officially interesting
 *      assert(!GblClass_isDefault(pClass2));
 *      assert(GblClass_isOverridden(pClass2));
 *      assert(!GblClass_isOwned(pClass2));
 *
 *      // K NOW LETS JUST SWAP THE FUCKER OUT RANDOMLY EVEN THOUGH ITS ALREADY INSTANTIATED LULUL
 *      // releases reference to default class and gets our override
 *      GblInstance_swizzleClass(GBL_INSTANCE(pObj), GBL_CLASS(pClass2));
 *
 *      // tell the instance to take exclusive ownership:
 *      GblInstance_classClassSink(GBL_INSTANCE(pObj));
 *
 *      GblEvent event;
 *      GblInstance_construct(&event, GBL_EVENT_TYPE);
 *      GblIEventHandler_eventNotify(GBL_IEVENT_HANDLER(pObj), pSomeEventObject);
 *
 *      // K clean up our fuckery
 *      GblInstance_destroy(GBL_INSTANCE(pObj);
 *  \endcode
 */
/*! \page TypeTemplate Type Templates
 *  \brief Boilerplate macros for type operations
 *
 *  At the top of the file for most libGimbal derived types, along with the type identifier,
 *  you will find a collection of convenience utility macros implementing function-style
 *  casts for the given type.
 *  \code{.c}
 *      // type UUID macro (expands to GblObject_type() registration function)
 *      #define GBL_OBJECT_TYPE                     (GBL_TYPEID(GblObject))
 *
 *      // instance function-style cast macro
 *      #define GBL_OBJECT(instance)                (GBL_CAST(instance, GblObject))
 *
 *      // class cast function-style cast macro
 *      #define GBL_OBJECT_CLASS(klass)             (GBL_CLASS_CAST(klass, GblObject))
 *
 *      // instance-to-class cast macro
 *      #define GBL_OBJECT_GET_CLASS(instance)      (GBL_CLASSOF(instance, GblObject))
 *  \endcode
 *  While these macros are obviously optional, they do add a lot to the codebase in terms of readability
 *  and convenience as well as uniform styling. With these sorts of macros defined, we then gain access to
 *  lots of convenience:
 *  \code{.c}
 *      // create a floating object class
 *      //    - easy cast to particular class
 *      GblObjectClass* pClass  = GBL_OBJECT_CLASS(GblClass_createFloating(GBL_OBJECT_TYPE));
 *
 *      // lets override just its implementation of the GblIEventHandlerClass interface
 *      //    - easy cast to particular interface
 *      GblIEventHandlerClass* pIEvent = GBL_IEVENT_HANDLER_CLASS(pClass);
 *      // override virtual method
 *      pIEvent->pFnEvent = [&](GblIEventHandler* pSelf, GblEvent* pEvent) {
 *           // easy cast to particular instance type
 *           GblObject* pSelfObj = GBL_OBJECT(pSelf);
 *
 *           printf("%s received an Event!", GblObject_name(pSelfObj));
 *
 *
 *           if(GBL_TYPECHECK(pEvent, MyEventType)) {
 *               // easy cast to particular event instance
 *               MyCustomEvent* pEvent = MY_CUSTOM_EVENT(pEvent);
 *
 *               // do something
 *           } else if(GBL_TYPECHECK(pEvent, MyOtherEventType)) {
 *               // easy cast to particular event instance
 *               SomeOtherEvent* pEvent = SOME_OTHER_EVENT(pEvent);
 *
 *               // do something
 *           }
 *      }
 *
 *      GblObject* pObject = GblObject_createWithClass(pClass);
 *      // easy cast from object to GblInstance base
 *      GblInstance_sinkClass(GBL_INSTANCE(pObject));
 *      // easy cast from object to GblBox base
 *      GblBox_unref(GBL_BOX(pObject));
 *  \endcode
 */

#undef GBL_SELF_TYPE

#endif // GIMBAL_CLASS_H
