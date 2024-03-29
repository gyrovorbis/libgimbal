/*! \file
 *  \brief GblInterface and related functions.
 *  \ingroup meta
 *
 *  \author Falco Girgis
 */
#ifndef GIMBAL_INTERFACE_H
#define GIMBAL_INTERFACE_H

#include "../classes/gimbal_class.h"

#define GBL_INTERFACE_TYPE                      GBL_BUILTIN_TYPE(INTERFACE)

#define GBL_INTERFACE(klass)                    ((GblInterface*)GblClass_cast(GBL_CLASS(klass), GBL_INTERFACE_TYPE))
#define GBL_INTERFACE_TRY(klass)                ((GblInterface*)GblClass_as(GBL_CLASS(klass), GBL_INTERFACE_TYPE))
#define GBL_INTERFACE_OUTER_CLASS(iface)        (GblInterface_outerClass(GBL_INTERFACE(iface)))
#define GBL_INTERFACE_OUTER_MOST_CLASS(iface)   (GblInterface_outerMostClass(GBL_INTERFACE(iface)))

#define GBL_SELF_TYPE GblInterface

GBL_DECLS_BEGIN

/// Base struct for all interfaces, inherits from GblClass
typedef struct GblInterface {
    GblClass    base;               ///< inherited GblClass base info
    size_t      outerClassOffset_;  ///< offset from the interface to the class containing it (private, managed by internals)
} GblInterface;

GBL_EXPORT GblClass* GblInterface_outerClass     (GBL_SELF) GBL_NOEXCEPT;
GBL_EXPORT GblClass* GblInterface_outerMostClass (GBL_SELF) GBL_NOEXCEPT;

GBL_DECLS_END

/*! \def GBL_INTERFACE_TYPE
 *  Builtin type ID associated with GblInterface
 *  \ingroup metaBuiltinTypes
 *  \sa GblInterface
 */

/*! \fn GBL_INTERFACE(klass)
 * Convenience function-style cast operator that returns the
 * given class as a GblInterface, provided it is actually one,
 * erroring out upon failure.
 * \relatedalso GblInterface
 * \param klass pointer to a GblClass or derived
 * \returns GblInterface pointer or NULL if klass isn't one
 * \sa GBL_INTERFACE_TRY()
 */

/*! \fn GBL_INTERFACE_TRY(klass)
 * Convenience function-style cast operator that returns the
 * given class as a GblInterface, provided it is actually one.
 * \relatedalso GblInterface
 * \param klass pointer to a GblClass or derived
 * \returns GblInterface pointer or NULL if klass isn't one
 * \sa GBL_INTERFACE()
 */

/*! \fn GBL_INTERFACE_OUTER_CLASS(iface)
 * Convenience macro wrapping GblInterface_outerClass(),
 * automatically casting the input parameter.
 * \relatedalso GblInterface
 * \param iface pointer to a GblInterface or derived
 * \returns GblClass pointer or NULL if it hasn't beenm mapped
 *          to one
 * \sa GblInterface_outerClass
 */

/*! \fn GBL_INTERFACE_OUTER_MOST_CLASS(iface)
 * Convenience macro wrapping GblInterface_outerMostClass(),
 * automatically casting the input parameter.
 * \relatedalso GblInterface
 * \param iface pointer to a GblInterface or derived
 * \returns GblClass pointer or NULL if it hasn't beenm mapped
 *          to one
 * \sa GblInterface_outerMostClass
 */

/*! \struct GblInterface
 *  \ingroup meta
 * \details
 * \extends GblClass
 *
 * An interface is a special type of GblClass which represents an
 * abstract collection of data which may be "implemented" by
 * another class and then queried for later. This data is
 * typically in the form of function pointers, which may or may
 * not have default values, which can then be set  by a the class
 * constructor of another type.
 *
 *\note
 * Since a GblInterface inherits from GblClass, it is compatible
 * with all of the methods associated with a GblClass, such as
 * GblClass_check(), GblClass_cast(), and GblClass_as(). These
 * are used to verify and query for GblInterface objects
 * which are contained within a given GblClass.
 *
 * The main advantage of modeling overriddable methods within
 * an interface as opposed to just putting them within a class
 * is that the interface can be implemented by any class,
 * without having to inherit or derive from it.
 *
 * GblInterface is the base structure which is to be inherited by all
 * interface structures within the meta type system. This means placing
 * it or a type "inheriting" from it as the first member of an
 * interface struct, when using C.
 * \code{.c}
 *     typedef struct ICallableInterface {
 *         GblInterface base;
 *         GBL_RESULT   (*pFnVCall)(ICallable* pSelf);
 *     } ICallableInterface
 * \endcode
 *
 * In terms of implementation, a GblInterface is actually implemented
 * as a type of mappable "subclass" which is then embedded within a
 * the structure of a GblClass-inheriting structure. When a type
 * which implements an interface is registered, its location within
 * the class is provided to the type system via GblTypeInfo::pInterfaceImpls.
 *\sa GblClass, GblInstance, GblType
 */

/*!
 * \fn GblInterface_outerClass(GblInterface* pSelf)
 * Returns the GblClass implementing the given interface.
 * \relatesalso GblInterface
 * \note If the specified interface is a default implementation,
 *       its own GblClass base is returned.
 * \param pSelf interface
 * \returns GblClass pointer to the implementing class.
 * \sa GblClass_outerMost
 */

/*!
  \fn GblClass* GblInterface_outerMostClass(GblInterface* pSelf)
 * Returns the top-level GblClass implementing the interface.
 *
 * The top-level class will either be its outer class, in the
 * case of a regular embedded interface, or it could be multiple
 * levels out, in the case of an interface mapping an interface.
 * \relatesalso GblInterface
 * \param pSelf interface
 * \returns GblClass pointer to the top-most implementing class.
 * \sa GblClass_as
 */

/*! \page Interfaces Interfaces
 *  \brief Overview of interfaced types
 *  \tableofcontents
 *
 *  LibGimbal's object model supports the concept of the C# or
 *  Java-style "Interface," which is a polymorphic type
 *  used to model abstract behavior on a class or object.
 *
 *  This is fancy object-oriented speak for allowing you
 *  to define a set of methods which can be implemented by
 *  any class which can then be queried for later. The main
 *  advantage of modeling overridable methods with this approach
 *  is that it doesn't require a type to inherit or derive from
 *  a common subtype. Any class inheriting from any other can
 *  implement any number of interfaces and inherit their
 *  implementations from parent classes.
 *
 *  LibGimbal interfaces support:
 *  - abstract overridable methods
 *  - overridable methods with a default implementation
 *  - static members
 *  - implementing other interfaces
 *  - inheriting from other interfaces
 *
 *  ## Declaring
 *  All interfaces in libGimbal derive from the base type,
 *  #GBL_INTERFACE_TYPE. This is a class-only, abstract type which
 *  defines the base class which we will use: GblInterface.
 *
 * ### Structures
 *  We can create our own interface class sructure by
 *  deriving from GblInterface:
 * \code{.c}
 *      // lets create an interface to implement saving/loading
 *      GBL_INTERFACE_DERIVE(ISerializable)
 *          // declare virtual methods for implementing save/load
 *          GBL_RESULT  (*pFnSave)(const ISerializable* pSelf, GblStringBuffer* pBuffer);
 *          GBL_RESULT  (*pFnLoad)(ISerializable* pSelf, const GblStringBuffer* pBuffer);
 *      GBL_INTERFACE_END
 * \endcode
 *  \note
 *  It is not a hard requirement that interface methods must return a GBL_RESULT type;
 *  however, this can be extremely convenient for propagating errors should one occur
 *  within the implementation. Since these methods are not what are typically called
 *  by a user, it doesn't make your API any uglier to do so (see the next section).
 *
 * ### Macro Utilities
 *  As is typical with most libGimbal types, it is often most convenient on
 *  the user (and you) to define a set of common macro operators for working with your
 *  type.
 *
 *  For our serializable interface, we will use the following:
 *  \code{.c}
 *      // convenience macro returning type ID from registration
 *      #define ISERIALIZABLE_TYPE                (GBL_TYPEID(ISerializable))
 *
 *      // function-style cast operator from an generic instance to our instance type
 *      #define ISERIALIZABLE(instance)           (GBL_CAST(instance, ISerializable))
 *
 *      // function-style cast operator from a generic class to our class/interface type
 *      #define ISERIALIZABLE_CLASS(klass)        (GBL_CLASS_CAST(klass, ISerializable))
 *
 *      // convenience macro to extract our interface from a generic instance
 *      #define ISERIALIZABLE_GET_CLASS(instance) (GBL_CLASSOF(instance, ISerializable))
 *  \endcode
 *
 * ### Public Methods
 *  Typically, when working with interface methods, we would rather provide a
 *  user-friendly API function wrapping the virtual method and handling any
 *  errors, rather than making a user reach into an interface and call a
 *  function pointer directly.
 *
 *  We do this for our virtual save method:
 *  \code{.c}
 *      GBL_RESULT ISerializable_save(const ISerializable* pSelf, GblStringBuffer* pBuffer) {
 *          GBL_CTX_BEGIN(NULL);
 *
 *          // exttract our interface from the given instance
 *          ISerializableClass* pClass = ISERIALIZABLE_GET_CLASS(pSelf);
 *
 *          // check whether we managed to find the interface
 *          if(pClass) {
 *
 *              // check whether the virtual method has been implemented
 *              if(pClass->pFnSave) {
 *
 *                  // call virtual method implementation,
 *                  // propagating any error code returned
 *                  GBL_CTX_VERIFY_CALL(pClass->pFnSave(pSelf, pBuffer);
 *              }
 *          }
 *          GBL_CTX_END();
 *      }
 *  \endcode
 *  As you can see, when we expose our virtual methods via a public API wrapper,
 *  the entry-point becomes prettier than calling directly into a function pointer,
 *  and we are able to do type checking and error handling. We can check to see
 *  whether the given instance was even compatible with our interface or whether
 *  the class implementing the interface actually overrode the method or not.
 *
 *  ## Registering
 *  Once we've defined our structures, created our utility macros, and created
 *  a public API around our virtual methods, it's time to register our type.
 *  To do this, we implement the ISerializable_type() function declared
 *  earlier to register a new meta type if we haven't already:
 *  \code{.c}
 *      GblType ISerializable_type(void) {
 *          // declare static variable, so we store the value
 *          static GblType type = GBL_INVALID_TYPE;
 *
 *          // only register if this is our first time getting called
 *          if(type == GBL_INVALID_TYPE) {
 *
 *              // register our interface's class, deriving from GBL_INTERFACE_TYPE
 *              type = GblType_register("ISerializable",
 *                                            GBL_INTERFACE_TYPE,
 *                                            &(const GblTypeInfo) {
 *                                                .classSize = sizeof(ISerializableClass)
 *                                            },
 *                                            GBL_TYPE_FLAG_ABSTRACT);
 *          }
 *
 *          return type;
 *      }
 *  \endcode
 *  \note
 *  If we wish to provide a default implementation of our virtual methods, we would
 *  also set GblTypeInfo::pFnClassInit to a ::GblClassInitFn function
 *  where we would initialize our class structure with some default values.
 *
 *  ## Implementing
 *  In order to use your interface with a given type, the type must
 *  implement the interface and then provide the meta type system with
 *  a mapping during type registration.
 *
 *  ### Structures
 *  If we wish to implement our interface on another type, we embed it
 *  within that type's class structure. Here we will use the libGimbal
 *  macro DSL which will handle generating our structures for us.
 *
 *  \code{.c}
 *     // Class structure (base class with one interface being implemented)
 *     GBL_CLASS_BASE(IntSerializable, ISerializable)
 *     // any extra static variables or virtual functions here
 *     GBL_CLASS_END
 *
 *     // could've used GBL_CLASS_BASE_EMPTY() if we had nothing extra to add
 *
 *     // Instance structure
 *     GBL_INSTANCE_BASE(IntSerializable)
 *         int integer; //single instance member
 *     GBL_INSTANCE_END
 *  \endcode
 *
 * ### Overrides
 *
 *  Finally, lets create an implementation of the save and load functions
 *  from the ISerializableIFace class, along with a class constructor for
 *  initializing IntSerializableClass:
 *
 *  \code{.c}
 *      // IntSerializable's implementation of ISerializable_load()
 *      GBL_RESULT IntSerializable_load_(ISerializable* pSelf, const GblStringBuffer* pBuffer) {
 *          IntSerializable* pInt = (IntSerializable*)pSelf;
 *
 *          // load an integer from the string buffer
 *          pInt->integer = GblStringView_toInt(GblStringBuffer_view(pBuffer));
 *
 *      }
 *
 *      // IntSerializable's implementation of ISerialiable_save();
 *      GBL_RESULT IntSerializable_save_(const ISerializable* pSelf, GblStringBuffer* pBuffer) {
 *          IntSerializable* pInt = (IntSerializable*)pSelf;
 *
 *          // save the integer to the string buffer
 *          return GblStringBuffer_appendInt(pBuffer, pInt->integer);
 *      }
 *
 *      // IntSerializable's class initializer, called when the class is created
 *      GBL_RESULT IntSerializable_initializeClass(GblClass* pClass, const void* pUd, GblContext* pCtx) {
 *          IntSerializableClass* pSelfClass = (IntSerializableClass*)pClass;
 *
 *          // override the interface implementation for the class
 *          pSelfClass->iSerializable.pFnLoad = IntSerializable_load_;
 *          pSelfClass->iSerializable.pFnSave = IntSerializable_save_;
 *      }
 *  \endcode
 * ### Registration
 *
 *  In order to register a type as having implemented an interface, we have to tell the
 *  meta type system how to "map" between the interface and the class. In order to achieve
 *  this, we pass an array of interface mappings to GblType_register() via
 *  GblTypeInfo.pInterfaceImpls:
 *  \code{.c}
 *      GblType IntSerializableType_type(void) {
 *          static GblType type = GBL_INVALID_TYPE;
 *
 *          if(type == GBL_INVALID_TYPE) {
 *
 *              type = GblType_register("IntSerializable",
 *                                     GBL_INSTANCE_TYPE,
 *                                     &(const GblTypeInfo) {
 *                                     .classSize       = sizeof(IntSerializableClass),
 *                                     .pFnClassInit    = IntSerializable_initializeClass,
 *                                     .instanceSize    = sizeof(IntSerializable),
 *                                     .interfaceCount  = 1,
 *                                     .pInterfaceImpls   = (const GblInterfaceImpl[]) {
 *                                         {
 *                                             .interfaceType = ISERIALIZABLE_TYPE,
 *                                             .classOffset   = offsetof(IntSerializableClass, iSerializable)
 *                                         }
 *                                 },
 *                                 GBL_TYPE_FLAGS_NONE);
 *           }
 *           return type;
 *      }
 *  \endcode
 *  As you can see, we provided a single entry into the interface mapping, which we used to
 *  associate our interface type with the given class offset. Now the meta type system
 *  knows everything it needs to be able to cast to and from your interface!
 *
 *  ## Querying
 *
 *  Querying for interfaces is extremely simple. Since they're essentially a type of GblClass,
 *  you use the same set of functions you would use to cast between class types. Lets create
 *  an instance of the IntSerializable type and try to serialize it with our interface using
 *  the utility macros defined earlier to handle casting:
 *  \code{.c}
 *      // create an instance
 *      IntSerializable* pIntInstance = GblInstance_create(intSerializeType);
 *
 *      // create a new string buffer to hold the data
 *      GblStringBuffer buffer;
 *      GblStringBuffer_construct(&buffer, GBL_STRV("7"));
 *
 *      // cast our type to an ISerializable and call the virtual function
 *      ISerializable_load(ISERIALIZABLE(pIntInstance), &buffer);
 *
 *      // look, we loaded the initial value of 7!
 *      assert(pIntInstance->integer == 7);
 *
 *      pIntInstance->integer = -7;
 *
 *      // we can now save a value back too!
 *      GblStringBuffer_clear(&buffer);
 *      ISerializable_save(ISERIALIZABLE(pIntInstance), &buffer));
 *
 *      // look, it saved the new value!
 *      assert(GblStringView_toInt(GblStringBuffer_view(&buffer)) == -7);
 *
 *      // destroy out instance when we're done
 *      GblInstance_destroy(pIntInstance);
 *  \endcode
 *  Querying for the GblInterface structure without having defined the convenience
 *  macros from the previous section is still possible, but it's much uglier and
 *  more verbose.
 *  \code{.c}
 *      // A. Cast to an ISerializable instance from IntSerializable
 *      ISerializable* pSerializable = (ISerializable*)GblInstance_cast((GblInstance*)pIntInstance,
 *                                                                      GBL_ISERIALIZABLE_TYPE);
 *
 *      // B. Extract the ISerializableIFace class from IntSerializable
 *
 *      //    1. retrieve the class from the instance
 *      GblClass* pClass = GblInstance_class((GblInstance*)pSerializable);
 *
 *      //    2. retrieve the interface from the class
 *      ISerializableIFace* pIFace = (ISerializableIFace*)GblClass_cast(pClass, GBL_ISERIALIZABLE_TYPE);
 * \endcode
 */

#undef GBL_SELF_TYPE

#endif // GIMBAL_INTERFACE_H
