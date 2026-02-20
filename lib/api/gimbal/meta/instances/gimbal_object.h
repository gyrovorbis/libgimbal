/*! \file
 *  \brief GblObject structure and related functions
 *  \ingroup meta
 *
 *  This file contains the type definitions and API for
 *  GblObject and GblObjectClass. This is the instantiable
 *  type which provides the OO meat of the instantiable types--
 *  such as properties, hierarchical relationships, event management,
 *  generic construction, named instances, etc.
 *
 *  \author    2023, 2024, 2025 Falco Girgis
 *  \author                2025 Agustín Bellagamba
 *  \copyright MIT License
 *
 *  \todo
 *      - Uninstall all signals upon class destructor (with property uninstallation)
 *      - Get rid of GblObject_findContext()
 *
 *  \test
 *      - GblObjectClass::pFnConstructed() with CONSTRUCTOR properties.
 *      - GblObjectClass::pFnInitialized() with WRITE properties.
 *      - GblObject_isConstructing().
 *      - GblObject_isInstantiating().
 *      - GblObject implementing ABSTRACT properties from GblInterface.
 */
#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "gimbal_box.h"
#include "../ifaces/gimbal_ievent_receiver.h"
#include "../properties/gimbal_property.h"
#include "../../containers/gimbal_ring_list.h"

/*! \name  Type System
 *  \brief Type UUID and Cast operators
 *  @{
 */
#define GBL_OBJECT_TYPE             GBL_TYPEID(GblObject)            //!< GblType UUID for GblObject
#define GBL_OBJECT(self)            GBL_CAST(GblObject, self)        //!< Casts a GblInstance to a GblObject
#define GBL_OBJECT_CLASS(klass)     GBL_CLASS_CAST(GblObject, klass) //!< Casts a GblClass to a GblObjectClass
#define GBL_OBJECT_GET_CLASS(self)  GBL_CLASSOF(GblObject, self)     //!< Gets a GblObjectClass from a GblInstance
//! @}

/*! \name  Construction Macros
 *  \brief Helper macros for creating new GblObject instances.
 *  @{
 */

 /*! \brief DSL macro used to heap-construct a GblObject-derived type.
 *  It takes the struct name followed by an optional list of property KV pairs.
 */
#define GBL_NEW(/*type, ("key", value),*/ ...)           GBL_OBJECT_NEW(__VA_ARGS__)

/*! \brief DSL macro used to placement-construct a GblObject-derived type
 *  It takes the struct name followed by an optional list of property KV pairs.
 */
#define GBL_NEW_AT(/*type, self, ("key", value),*/ ...)  GBL_OBJECT_NEW_AT(__VA_ARGS__)

//! @}

/*! \brief For-loop style iteration over the children of a GblObject
 *
 *  Iterates over every child within the given object, setting the \p item variable to the current child.
 *  Optionally takes in a \p type parameter to specify the type of the children to iterate over.
*/
#define GblObject_foreachChild(/*GBL_SELF, name, type=GblObject* */ ...) GblObject_foreachChildDefault_(__VA_ARGS__)

#define GBL_SELF_TYPE GblObject

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblObject);

/*! \struct     GblObjectClass
 *  \extends    GblBoxClass
 *  \implements GblITableVariantClass
 *  \implements GblIEventReceiverClass
 *  \brief      GblClass structure for GblObject
 *
 *  GblObjectClass provides the virtual table for all types
 *  deriving from GblObject. It provides a constructor, event
 *  handlers, as well as accessors for reading and writing properties.
 */
GBL_CLASS_DERIVE(GblObject, GblBox,
                 GblITableVariant, GblIEventReceiver)
    //! Virtual method called during construction after CONSTRUCT properties but before extra WRITE properties are set
    GBL_RESULT (*pFnConstructed) (GBL_SELF);
    //! Virtual method called when the object has been fully instantiated with all properties it was created with (including extra WRITE properties being set).
    GBL_RESULT (*pFnInstantiated)(GBL_SELF);
    //! Virtual method for reading properties
    GBL_RESULT (*pFnProperty)    (GBL_CSELF, const GblProperty* pProp, GblVariant* pValue);
    //! Virtual method for writing properties
    GBL_RESULT (*pFnSetProperty) (GBL_SELF, const GblProperty* pProp, GblVariant* pValue);
GBL_CLASS_END

/*! \name  User-Operator Callbacks
 *  \brief Typedefs for applying various user-supplied functions over a GblObject.
 *  @{
*/
//! Iterator callback function which gets called over every child of a GblObject, being passed back a GblObject and an arbitrary closure, returning true should iteration cease early.
typedef GblBool (*GblObjectIterFn)(GblObject* pChild, void* pClosure);
//! @}

/*! \class      GblObject
 *  \extends    GblBox
 *  \implements GblITableVariant
 *  \implements GblIEventReceiver
 *  \ingroup    meta
 *  \brief      Main Object-Oriented Instance with Properties, EventReceivers, and Parenting
 *
 *  A GblObject is the default, full-blown object-oriented instantiable type.
 *  It is analogous to Qt's "QObject" or GNOME's "GObject" root type. It extends
 *  GblBox to add the following additional functionality:
 *      - properties
 *      - GblVariant table operations
 *      - constructor / post-constructor
 *      - event handling and filtering
 *      - object parenting and hierarchy
 *      - object name
 *
 *  \note
 *  A GblObject by default has the same size of a GblBox. Additional data for
 *  fields such as name, parent, children, event filters, etc are created lazily
 *  and are stored within GblBox::pFields.
 *
 *  \sa GblBox, GblObjectClass, GblProperty
 */
GBL_INSTANCE_DERIVE_EMPTY(GblObject, GblBox)

//! \cond
GBL_PROPERTIES(GblObject,
    (name,     GBL_GENERIC, (READ, WRITE, LOAD, SAVE), GBL_STRING_TYPE),
    (parent,   GBL_GENERIC, (READ, WRITE),             GBL_OBJECT_TYPE),
    (children, GBL_GENERIC, (READ, WRITE, IN, OUT),    GBL_RING_LIST_TYPE),
    (userdata, GBL_GENERIC, (READ, WRITE),             GBL_POINTER_TYPE),
    (refCount, GBL_GENERIC, (READ),                    GBL_UINT16_TYPE)
//  (class,    GBL_GENERIC, (READ, WRITE, CONSTRUCT),  GBL_POINTER_TYPE)
)

GBL_SIGNALS(GblObject,
    (propertyChange, (GBL_INSTANCE_TYPE, pReceiver), (GBL_BOX_TYPE, pProperty))
)
//! \endcond

//! Returns the GblType UUID associated with GblObject
GBL_EXPORT GblType GblObject_type(void) GBL_NOEXCEPT;

/*! \name Constructors
 *  \brief Methods for constructing GblObject-derived types.
 *  @{
 */
//! Creates an object-derived type on the heap, initializing it with a NULL-terminated K,V pair listing of properties
GBL_EXPORT GblObject* GblObject_create               (GblType type, ...)                         GBL_NOEXCEPT;
//! Variant of GblObject_create(), where the object is created with an extended size
GBL_EXPORT GblObject* GblObject_createExt            (GblType type, size_t size, ...)            GBL_NOEXCEPT;
//! Constructs an object-derived type in-place, initializing it with a NULL-terminated K,V pair listing of properties
GBL_EXPORT GBL_RESULT GblObject_construct            (GBL_SELF, GblType type, ...)               GBL_NOEXCEPT;
//! Creates an object-derived type on the heap, with the given class, initializing it with a NULL-terminated K,V pair property list
GBL_EXPORT GblObject* GblObject_createWithClass      (GblObjectClass* pClass, ...)               GBL_NOEXCEPT;
//! Variant of GblObject_createWithClass(), where the object is created with an extended size
GBL_EXPORT GblObject* GblObject_createExtWithClass   (GblObjectClass* pClass, size_t size, ...)  GBL_NOEXCEPT;
//! Constructs an object-derived type in-place, with the given class, initializing it with a NULL-terminated K,V pair property list
GBL_EXPORT GBL_RESULT GblObject_constructWithClass   (GBL_SELF, GblObjectClass* pClass, ...)     GBL_NOEXCEPT;
//! Variant of GblObject_create(), where the property list is provided as a va_list pointer
GBL_EXPORT GblObject* GblObject_createVa             (GblType type, va_list* pList)              GBL_NOEXCEPT;
//! Variant of GblObject_createVaList(), where the object is created with an extended size
GBL_EXPORT GblObject* GblObject_createExtVa          (GblType type, size_t size, va_list* pList) GBL_NOEXCEPT;
//! Variant of GblObject_construct(), where the property list is provided as a va_list pointer
GBL_EXPORT GBL_RESULT GblObject_constructVa          (GBL_SELF, GblType type, va_list* pList)    GBL_NOEXCEPT;
//! Variant of GblObject_createWithClass(), where the property list is provided as a va_list pointer
GBL_EXPORT GblObject* GblObject_createVaWithClass    (GblObjectClass* pClass, va_list* pList)    GBL_NOEXCEPT;
//! Variant of GblObject_createVaListWithClass(), where the object is allocated with an extended size
GBL_EXPORT GblObject* GblObject_createExtVaWithClass (GblObjectClass* pClass,
                                                      size_t          size,
                                                      va_list*        pList)                     GBL_NOEXCEPT;
//! Variant of GblObject_constructWithClass(), where the property list is provided as a va_list pointer
GBL_EXPORT GBL_RESULT GblObject_constructVaWithClass (GBL_SELF,
                                                      GblObjectClass* pClass,
                                                      va_list*        pList)                     GBL_NOEXCEPT;
//! Creates an object-derived type on the heap, initializing it with an array of property values and a corresponding array of names
GBL_EXPORT GblObject* GblObject_createVariants       (GblType     type,
                                                      size_t      propertyCount,
                                                      const char* pNames[],
                                                      GblVariant* pValues)                       GBL_NOEXCEPT;
//! Variant of GblObject_createVariants(), where the object is allocated with an extended size
GBL_EXPORT GblObject* GblObject_createVariantsExt    (GblType     type,
                                                      size_t      size,
                                                      size_t      propertyCount,
                                                      const char* pNames[],
                                                      GblVariant* pValues)                       GBL_NOEXCEPT;
//! Creates an object-derived type in-place, initializing it with an array of property values and a corresponding array of names
GBL_EXPORT GBL_RESULT GblObject_constructVariants    (GBL_SELF,
                                                      GblType     type,
                                                      size_t      propertyCount,
                                                      const char* pNames[],
                                                      GblVariant* pValues)                       GBL_NOEXCEPT;
//! Variant of GblObject_createVariants(), where the object is constructed with a weak reference to the given class
GBL_EXPORT GblObject* GblObject_createVariantsWithClass
                                                     (GblObjectClass* pClass,
                                                      size_t          propertyCount,
                                                      const char*     pNames[],
                                                      GblVariant*     pValues)                   GBL_NOEXCEPT;
//! Variant of GblObject_createVariantsWithClass(), where the object is allocated with an extended size
GBL_EXPORT GblObject* GblObject_createVariantsExtWithClass
                                                     (GblObjectClass* pClass,
                                                      size_t          size,
                                                      size_t          propertyCount,
                                                      const char*     pNames[],
                                                      GblVariant*     pValues)                   GBL_NOEXCEPT;
//! Variant of GblObject_constructVariants(), where the object is constructed with a weak reference to the given class
GBL_EXPORT GBL_RESULT GblObject_constructVariantsWithClass
                                                     (GBL_SELF,
                                                      GblObjectClass* pClass,
                                                      size_t          propertyCount,
                                                      const char*     pNames[],
                                                      GblVariant*     pValues)                   GBL_NOEXCEPT;
//! @}

/*! \name  Reading Properties
 *  \brief Methods for retrieving property values.
 *  \relatesalso GblObject
 *  @{
 */
//! Looks up the property on the object by name, storing its value in the pointer passed as a variadic argument
GBL_EXPORT GBL_RESULT GblObject_property               (GBL_CSELF, const char* pName, ...)                GBL_NOEXCEPT;
//! Looks up the property on the object by quark (faster), storing its value in the pointer passed as a variadic argument
GBL_EXPORT GBL_RESULT GblObject_propertyByQuark        (GBL_CSELF, GblQuark name, ...)                    GBL_NOEXCEPT;
//! Variant of GblObject_property() with the pointer for storing the value specified by a pointer to a va_list
GBL_EXPORT GBL_RESULT GblObject_propertyVaList         (GBL_CSELF, const char* pName, va_list* pList)     GBL_NOEXCEPT;
//! Variant of GblObject_propertyVaList() with the name being specified by a quark (faster lookups)
GBL_EXPORT GBL_RESULT GblObject_propertyVaByQuark      (GBL_CSELF, GblQuark name, va_list* pList)         GBL_NOEXCEPT;
//! Fetches the value of the specified property, storing it within a variant
GBL_EXPORT GBL_RESULT GblObject_propertyVariant        (GBL_CSELF, const char* pName, GblVariant* pValue) GBL_NOEXCEPT;
//! Variant of GblObject_propertyVariant(), with the property name specified as a quark (faster lookups)
GBL_EXPORT GBL_RESULT GblObject_propertyVariantByQuark (GBL_CSELF, GblQuark name, GblVariant* pValue)     GBL_NOEXCEPT;
//! Takes a NULL-terminated K,V pair list of string property names and pointers to store their values within
GBL_EXPORT GBL_RESULT GblObject_properties             (GBL_CSELF, ...)                                   GBL_NOEXCEPT;
//! Variant of GblObject_properties() with the NULL-terminated K,V pair list being specified as a pointer to a va_list
GBL_EXPORT GBL_RESULT GblObject_propertiesVa           (GBL_CSELF, va_list* pList)                        GBL_NOEXCEPT;
//! Looks up multiple properties by the names provided by the \p pNames list and stores them within the array of GblVariants
GBL_EXPORT GBL_RESULT GblObject_propertiesVariants     (GBL_CSELF,
                                                        size_t      count,
                                                        const char* pNames[],
                                                        GblVariant* pValue)                               GBL_NOEXCEPT;

/*! \name  Writing Properties
 *  \brief Methods for setting property values.
 *  \relatesalso GblObject
 *  @{
 */
//! Sets the property with the given string name to the value given by the pointer passed through the variadic argument list
GBL_EXPORT GBL_RESULT GblObject_setProperty               (GBL_SELF, const char* pName, ...)                GBL_NOEXCEPT;
//! Variant of GblObject_setProperty() with the name being specified as a quark (for faster lookups)
GBL_EXPORT GBL_RESULT GblObject_setPropertyByQuark        (GBL_SELF, GblQuark quark, ...)                   GBL_NOEXCEPT;
//! Variant of GblObject_setProperty() with the pointer for storing the value being specified as a pointer to a va_list
GBL_EXPORT GBL_RESULT GblObject_setPropertyVa             (GBL_SELF, const char* pName, va_list* pList)     GBL_NOEXCEPT;
//! Variant of GblObject_setPropertyVaList() with the name being specified as a quark (for faster lookups)
GBL_EXPORT GBL_RESULT GblObject_setPropertyVaListByQuark  (GBL_SELF, GblQuark name, va_list* pList)         GBL_NOEXCEPT;
//! Variant of GblObject_setProperty() where the property value is provided as a GblVariant
GBL_EXPORT GBL_RESULT GblObject_setPropertyVariant        (GBL_SELF, const char* pName, GblVariant* pValue) GBL_NOEXCEPT;
//! Variant of GblObject_setPropertyVariant() where the property name is provided as a quark (for faster lookups)
GBL_EXPORT GBL_RESULT GblObject_setPropertyVariantByQuark (GBL_SELF, GblQuark name, GblVariant* pValue)     GBL_NOEXCEPT;
//! Sets multiple properties by taking a NULL-terminated K,V pairs list of string names and pointers to store the values within
GBL_EXPORT GBL_RESULT GblObject_setProperties             (GBL_SELF, ...)                                   GBL_NOEXCEPT;
//! Variant of GblObject_setProperties() where the NULL-terminated K,V pairs list is specified as a pointer to a va_list
GBL_EXPORT GBL_RESULT GblObject_setPropertiesVa           (GBL_SELF, va_list* pList)                        GBL_NOEXCEPT;
//! Sets the properties specified by the list of names to the corresponding values specified as the list of GblVariants
GBL_EXPORT GBL_RESULT GblObject_setPropertiesVariants     (GBL_SELF,
                                                           size_t      count,
                                                           const char* pNames[],
                                                           GblVariant* pValue)                              GBL_NOEXCEPT;
//! @}

/*! \name  Property Notification
 *  \brief Methods involving property changed notifiers.
 *  \relatesalso GblObject
 *  @{
 */
//! Notifies any listeners that the value of a property has changed internally, by emitting the "propertyChanged" signal.
GBL_EXPORT GBL_RESULT GblObject_emitPropertyChange        (GBL_CSELF, const char* pName) GBL_NOEXCEPT;
//! Equivalent to GblObject_emitPropertyChange(), except that the property lookup is done more efficiently by using a quark for its name.
GBL_EXPORT GBL_RESULT GblObject_emitPropertyChangeByQuark (GBL_CSELF, GblQuark name)     GBL_NOEXCEPT;
//! Disables or enables emission of the "propertyChange" signal, returning its previous enablement. Typically you want to only do this temporarily and restore its previous state when done.
GBL_EXPORT GblBool    GblObject_blockPropertyChange       (GBL_SELF, GblBool blocked)    GBL_NOEXCEPT;
//! Returns true if emission of the "propertyChange" signal has been blocked on the current object or false if it is enabled.
GBL_EXPORT GblBool    GblObject_propertyChangeBlocked     (GBL_CSELF)                    GBL_NOEXCEPT;
//! @}

/*! \name State Management
 *  \brief Methods for querying GblObject state.
 *  \relatesalso GblObject
 *  @{
 */
//! Returns true from the time the object has been allocated to the time its final (possibly optional) initial properties have been set.
GBL_EXPORT GblBool GblObject_isInstantiating (GBL_CSELF) GBL_NOEXCEPT;
//! Returns true immediately after type initializers are called, but before GblObject::pFnConstructed() is called with the initial constructor properties.
GBL_EXPORT GblBool GblObject_isConstructing  (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name  Name Accessors
 *  \brief Methods for reading and writing object name
 *  \relatesalso GblObject
 *  @{
 */
//! Retrieves the name of the given object or returns an empty string if it doesn't have one
GBL_EXPORT GblStringRef* GblObject_name       (GBL_CSELF)                    GBL_NOEXCEPT;
//! Sets the name of the given object to \p pName, making a copy of its contents to store internally
GBL_EXPORT void          GblObject_setName    (GBL_SELF, const char* pName)  GBL_NOEXCEPT;
//! Sets the name of the given object to \p pRef, taking ownership of the given reference
GBL_EXPORT void          GblObject_setNameRef (GBL_SELF, GblStringRef* pRef) GBL_NOEXCEPT;
//! @}

/*! \name  Hierarchy Management
 *  \brief Methods for parenting, traversing, or querying object trees
 *  \relatesalso GblObject
 *  @{
 */
//! Returns a pointer to the parent of the given object, or NULL if it doesn't have one
GBL_EXPORT GblObject* GblObject_parent                  (GBL_CSELF)                                             GBL_NOEXCEPT;
//! Sets the parent of the given object to \p pParent, handing ownership to it.
GBL_EXPORT void       GblObject_setParent               (GBL_SELF, GblObject* pParent)                          GBL_NOEXCEPT;
//! Adds \p pChild as a child of the given object, setting itself as the parent and taking ownership of it.
GBL_EXPORT void       GblObject_addChild                (GBL_SELF, GblObject* pChild)                           GBL_NOEXCEPT;
//! Removes \p pChild from being a child of the given object, setting its parent to NULL
GBL_EXPORT GblBool    GblObject_removeChild             (GBL_SELF, GblObject* pChild)                           GBL_NOEXCEPT;
//! Adds all the objects in \p pList as children of the given object, taking ownership of them.
GBL_EXPORT void       GblObject_addChildren             (GBL_SELF, const GblRingList* pList)                    GBL_NOEXCEPT;
//! Removes all the objects in \p pList as children of the given object, giving up ownership of them.
GBL_EXPORT void       GblObject_removeChildren          (GBL_SELF, const GblRingList* pList)                    GBL_NOEXCEPT;
//! Clears the children list of the given object, giving up ownership of them.
GBL_EXPORT void       GblObject_clearChildren           (GBL_SELF)                                              GBL_NOEXCEPT;
//! Sets the children of the given object to \p pList, replacing the current children list and taking ownership of the new one.
GBL_EXPORT void       GblObject_setChildren             (GBL_SELF, const GblRingList* pList)                    GBL_NOEXCEPT;
/*! Iterates over every child of the given object, passing them to the given iterator function, which may optionally take a user data pointer.
    Iteration ends early when the iterator returns GBL_TRUE.
*/
GBL_EXPORT GblBool    GblObject_iterateChildren         (GBL_CSELF, GblObjectIterFn pFnIt, void* pUd/*=NULL*/) GBL_NOEXCEPT;
//! Returns the number of levels deep the given object is in the object tree formed by its ancestors
GBL_EXPORT size_t     GblObject_depth                   (GBL_CSELF)                                             GBL_NOEXCEPT;
//! Returns a pointer to the first child of the given object, or NULL if it has none
GBL_EXPORT GblObject* GblObject_childFirst              (GBL_CSELF)                                             GBL_NOEXCEPT;
//! Returns a pointer to the last child of the given object, or NULL if it has none
GBL_EXPORT GblObject* GblObject_childLast               (GBL_CSELF)                                             GBL_NOEXCEPT;
//! Returns the number of children with the given object as their parent
GBL_EXPORT size_t     GblObject_childCount              (GBL_CSELF)                                             GBL_NOEXCEPT;
//! Returns the index of the given object in its parent's list of children
GBL_EXPORT size_t     GblObject_childIndex              (GBL_CSELF)                                             GBL_NOEXCEPT;
//! Returns the next sibling after the given object, forming a linked list of their parents' children
GBL_EXPORT GblObject* GblObject_siblingNext             (GBL_CSELF)                                             GBL_NOEXCEPT;
//! Returns the next sibling after the given object that is of \p type, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_siblingNextByType       (GBL_CSELF, GblType type)                               GBL_NOEXCEPT;
//! Returns the next sibling after the given object with the given name, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_siblingNextByName       (GBL_CSELF, const char* pName)                          GBL_NOEXCEPT;
//! Returns the previous sibling before the given object, forming a linked list of their parents' children
GBL_EXPORT GblObject* GblObject_siblingPrevious         (GBL_CSELF)                                             GBL_NOEXCEPT;
//! Returns the previous sibling before the given object that is of \p type, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_siblingPreviousByType   (GBL_CSELF, GblType type)                               GBL_NOEXCEPT;
//! Returns the previous sibling before the given object with the given name, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_siblingPreviousByName   (GBL_CSELF, const char* pName)                          GBL_NOEXCEPT;

//! Returns a pointer to the closest ancestor object of the given object that is of \p ancestorType, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_findAncestorByType   (GBL_CSELF, GblType ancestorType) GBL_NOEXCEPT;
//! Returns a pointer to the first ancestor found with the given name or NULL if none were found
GBL_EXPORT GblObject* GblObject_findAncestorByName   (GBL_CSELF, const char* pName)    GBL_NOEXCEPT;
//! Returns a pointer to the ancestor object of the given object at the specified \p height, with 1 being the direct parent
GBL_EXPORT GblObject* GblObject_findAncestorByHeight (GBL_CSELF, size_t height)        GBL_NOEXCEPT;
//! Starting at the root object, returns a pointer to the ancestor of the given object at the specified depth, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_findBaseByDepth      (GBL_CSELF, size_t depth)         GBL_NOEXCEPT;
//! Searches through the list of children on the given object, returning a pointer to the first one with the given type, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_findChildByType      (GBL_CSELF, GblType childType)    GBL_NOEXCEPT;
//! Searches through the list of children on the given object, returning a pointer to the first one with the given name, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_findChildByName      (GBL_CSELF, const char* pName)    GBL_NOEXCEPT;
//! Iterates sequentially over the list of children on the given object, returning a pointer to the one at the given type, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_findChildByIndex     (GBL_CSELF, size_t index)         GBL_NOEXCEPT;
//! Iterates sequentially over the list of siblings to the given object, returning a pointer to the first one of the given type, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_findSiblingByType    (GBL_CSELF, GblType siblingType)  GBL_NOEXCEPT;
//! Iterates sequentially over the list of siblings to the given object, returning a pointer to the first one with the given name, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_findSiblingByName    (GBL_CSELF, const char* pName)    GBL_NOEXCEPT;
//! Iterates sequentially over the list of siblings to the given object, returning a pointer to the one at the given index, or NULL if there isn't one
GBL_EXPORT GblObject* GblObject_findSiblingByIndex   (GBL_CSELF, size_t  index)        GBL_NOEXCEPT;
//! @}

/*! \name  Event Management
 *  \brief Methods for sending, notifying, and filtering events
 *  \relatesalso GblObject
 *  @{
 */
//! Emits an event upwards through the object hierarchy, notifying the parent of the given object of the event
GBL_EXPORT GBL_RESULT         GblObject_sendEvent            (GBL_SELF, GblEvent* pEvent)           GBL_NOEXCEPT;
//! Notifies the given object of an event, which first propagates through its event filters before going through its event handler and propagating up if still unhandled
GBL_EXPORT GBL_RESULT         GblObject_notifyEvent          (GBL_SELF, GblEvent* pEvent)           GBL_NOEXCEPT;
//! Installs the event filter on the given object, allowing it to intercept event notifications
GBL_EXPORT GBL_RESULT         GblObject_installEventFilter   (GBL_SELF, GblIEventReceiver* pFilter) GBL_NOEXCEPT;
//! Removes the event filter from the given object, so that it no longer intercepts event notifications
GBL_EXPORT GBL_RESULT         GblObject_uninstallEventFilter (GBL_SELF, GblIEventReceiver* pFilter) GBL_NOEXCEPT;
//! Returns the number of event filters currently installed on the given object
GBL_EXPORT size_t             GblObject_eventFilterCount     (GBL_CSELF)                            GBL_NOEXCEPT;
//! Returns a pointer to the event filter installed on the given object at the \p index position
GBL_EXPORT GblIEventReceiver* GblObject_eventFilter          (GBL_CSELF, size_t index)              GBL_NOEXCEPT;
//! @}

//! \deprecated
GBL_EXPORT GblContext* GblObject_findContext (GBL_SELF) GBL_NOEXCEPT;

GBL_DECLS_END

///\cond
#define GBL_OBJECT_NEW(...)                         GBL_VA_OVERLOAD_CALL(GBL_OBJECT_NEW, GBL_VA_OVERLOAD_SUFFIXER_1_N, __VA_ARGS__)

#define GBL_OBJECT_NEW_1(typeName)                  ((typeName*)GblObject_create(GBL_TYPEID(typeName), GBL_NULL))
#define GBL_OBJECT_NEW_N(...)                       ((GBL_TUPLE_FIRST(__VA_ARGS__)*)GblObject_create(GBL_TYPEID(GBL_TUPLE_FIRST(__VA_ARGS__)), \
                                                                                                     GBL_TUPLE_REST(__VA_ARGS__), NULL))

#define GBL_OBJECT_NEW_AT(cType, instance, ...)   (GblObject_construct(instance,GBL_TYPEID(cType), __VA_ARGS__, NULL))

#define GblObject_foreachChildDefault_(...) \
    GblObject_foreachChildDefault__(__VA_ARGS__, GblObject*)
#define GblObject_foreachChildDefault__(self, name, type, ...) \
    GblObject_foreachChildImpl_(self, name, type)
#define GblObject_foreachChildImpl_(self, name, type)                               \
    for(type name       =   GblObject_childFirst(self) ?                            \
                            (type)GblObject_childFirst(self) :                      \
                            NULL,                                                   \
        *name##_next_   =   name ?                                                  \
                            (type)GblObject_siblingNext(GBL_OBJECT(name)) :         \
                            NULL;                                                   \
                                                                                    \
        name;                                                                       \
                                                                                    \
        name = name##_next_, name##_next_ = name ?                                  \
                             (type)GblObject_siblingNext(GBL_OBJECT(name))          \
                             : NULL)
///\endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_OBJECT_H
