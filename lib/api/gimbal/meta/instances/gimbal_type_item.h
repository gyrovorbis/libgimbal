/*! \file
 *  \brief   GblTypeItem associated type data structure and API.
 *  \ingroup meta
 *
 *  GblTypeItem is a GblBox-derived abstract type which acts as
 *  as an a type of data structure which gets associated with a
 *  particular GblType.
 * 
 *  This type serves as the base class for both GblProperty and
 *  GblSignal, both of which are a type of structure which can
 *  be registered and associated with a given GblType.
 *
 *  \author     2025 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_TYPE_ITEM_H
#define GIMBAL_TYPE_ITEM_H

#include "gimbal_box.h"
#include "../../containers/gimbal_linked_list.h"

/*! \name Type System
 *  \brief Type UUID and Cast operators.
 *  @{
 */
#define GBL_TYPE_ITEM_TYPE          GBL_TYPEID(GblTypeItem)             //!< GblType UUID of a GblTypeItem
#define GBL_TYPE_ITEM(self)         GBL_CAST(GblTypeItem, self)         //!< Casts a GblInstance to a GblTypeItem
#define GBL_TYPE_ITEM_CLASS(klass)  GBL_CLASS_CAST(GblTypeItem, klass)  //!< Casts a GblClass to a GblTypeItemClass
#define GBL_TYPE_ITEM_CLASSOF(self) GBL_CLASSOF(self)                   //!< Retrieves a GblTypeItemClass from a GblInstance
//! @}

#define GBL_SELF_TYPE GblTypeItem

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTypeItem);

//! Callback called for each matching \p pItem, getting optionally passed back \p pClosure. Returns GBL_TRUE to end iteration early.
typedef GblBool (*GblTypeItemIterFn)(GblTypeItem* pItem, void* pClosure);

/*! \struct  GblTypeItemClass
 *  \ingroup meta    
 *  \brief   GblClass VTable/static data structure for GblTypeItem.
 *
 *  This class provides the polymorphic back-end for implementing a
 *  generic registry that associates a series of GblTypeItems with a
 *  given owning GblType, such as a registry of properties or signals.
 *
 *  \sa GblTypeItem, GblPropertyClass, GblSignalClass
 */
GBL_CLASS_DERIVE(GblTypeItem, GblBox)
    //! Minimal base type required to own an instance of a GblTypeItem.
    GblType baseType;
    //! Virtual function for checking whether a given owning type has implemented all of its requireemnts for its type items.
    GBL_RESULT (*pFnCheck)    (GBL_CKLASS, GblType owner);
    //! Virtual function for implementing iteration behavior over a series of type items associated with a given owner within the registry.
    GBL_RESULT (*pFnNext)     (GBL_CKLASS, GblType owner, GblTypeItem** ppItem, GblFlags mask);
    //! Virtual function for installing a new type item onto an owning type within the registry.
    GBL_RESULT (*pFnInstall)  (GBL_SELF);
    //! Virtual function for uninstalling a new type item onto an owning type within the registry.
    GBL_RESULT (*pFnUninstall)(GBL_SELF)
    //! Virtual function for checking the validity of a type item based on its current values.
    GBL_RESULT (*pFnValidate) (GBL_CSELF);
    //! Virtual function for returning the GblHash associated with a given type item.
    GBL_RESULT (*pFnHash)     (GBL_CSELF, GblHash* pHash);
    //! Virtual function for comparing two given type items.
    GBL_RESULT (*pFnCompare)  (GBL_CSELF, const GblTypeItem* pOther, int* pResult);
GBL_CLASS_END

/*! \struct  GblTypeItem
 *  \extends GblBox
 *  \ingroup meta
 *  \brief   GblInstance structure which can be associated with an owning GblType.
 * 
 *  A GblTypeItem is a type of instance which associates its member data with a
 *  GblType within a type registry which is held by its class. It serves as the
 *  base abstract class for implementing GblProperty and GblSignal, containing
 *  their commonality.
 * 
 *  \sa GblTypeItemClass, GblProperty, GblSignal
 */
GBL_INSTANCE_DERIVE(GblTypeItem, GblBox)
    union {             //!< Convenience unionization between generic list node and strongly typed next pointer.
        GblLinkedListNode   listNode;   //!< Generic listNode entry, linking to the next item within the registry.
        struct GblTypeItem* pNext;      //!< Strongly-typed pointer to the next GblTypeItem within the registry.
    };
    GblType  owner;     //!< The GblType UUID of the owning type associated with this GblTypeItem.
    GblQuark name;      //!< Name of this GblTypeItem, as an optimized GblQuark value.
    GblFlags flags;     //!< Arbitrary flags associated with the given GblTypeItem, given meaning by their derived classes.
GBL_INSTANCE_END

//! Returns the GblType UUID associated with the GblTypeItem type.
GBL_EXPORT GblType GblTypeItem_type(void) GBL_NOEXCEPT;

/*! \name  Combination Values
 *  \brief Methods for retrieving totals and values derived from multiple items.
 *  @{
 */
//! Returns the total number of GblTypeItem instances registered within the registry for \p typeItem.
GBL_EXPORT size_t   GblTypeItem_totalCount    (GblType typeItem) GBL_NOEXCEPT;
//! Returns the total number of GblTypeItem instances installed onto \p owner within the \p typeItem registry, optionally including inherited items as well.
GBL_EXPORT size_t   GblTypeItem_count         (GblType typeItem, 
                                               GblType owner, 
                                               GblBool inherited) GBL_NOEXCEPT;
//! Returns the bitwise combination of every flag present for the given \p owner type within the given \p typeItem registry, optionally including inherited item flags as well.
GBL_EXPORT GblFlags GblTypeItem_combinedFlags (GblType typeItem,
                                               GblType owner,
                                               GblBool inherited) GBL_NOEXCEPT;
//! @}

/*! \name  Owner Type Operations
 *  \brief Methods operating on the items associated with a given owner type.
 *  @{
 */
//! Returns GBL_TRUE if the \p owner type satisfies all of its type requirements within the \p typeItem registry.
GBL_EXPORT GblBool    GblTypeItem_check        (GblType typeItem,
                                                GblType owner) GBL_NOEXCEPT;
//! Uninstalls all GblTypeItem instances associated with \p owner within the \p typeItem registry.
GBL_EXPORT GBL_RESULT GblTypeItem_uninstallAll (GblType typeItem,
                                                GblType owner) GBL_NOEXCEPT;
//! @}

/*! \name  Searching
 *  \brief Methods for finding a specific item.
 *  @{
 */
//! Retrieves the item from the \p typeItem registry which is owned by \p owner and named \p pName.
GBL_EXPORT GblTypeItem* GblTypeItem_find      (GblType     typeItem, 
                                               GblType     owner,
                                               const char* pName) GBL_NOEXCEPT;
//! Equivalent to GblTypeItem_find(), except taking \p name as a GblQuark, which is faster for lookups.
GBL_EXPORT GblTypeItem* GblTypeItem_findQuark (GblType  typeItem,
                                               GblType  owner,
                                               GblQuark name)     GBL_NOEXCEPT;
//! @}

/*! \name  Iterating
 *  \brief Methods for finding a specific item.
 *  @{
 */
//! Returns the next item in the \p typeItem registry belonging to \p owner, coming after \p pPrevious (or  NULL if first), containing one of the big flags set in \p mask. 
GBL_EXPORT GblTypeItem* GblTypeItem_next    (GblType            typeItem, 
                                             GblType            owner, 
                                             const GblTypeItem* pPrevious, 
                                             GblFlags           mask)        GBL_NOEXCEPT;
//! Iterates over each item in the \p typeItem registry belonging to \p owner, with at least one flag set in \p mask, calling \p pFnInit, optionally passing back \p pCl. Returns GBL_TRUE if the iterator did, which also breaks iteration early.
GBL_EXPORT GblBool      GblTypeItem_iterate (GblType           typeItem,
                                             GblType           owner, 
                                             GblFlags          mask,
                                             GblTypeItemIterFn pFnIt,
                                             void*             pCl/*=NULL*/) GBL_NOEXCEPT;
//! @}

/*! \name  Installation
 *  \brief Methods for installing and uninstalling data items.
 *  @{
 */
//! Installs the given item into its associated type registry on its owner type.
GBL_EXPORT GBL_RESULT GblTypeItem_install   (GBL_SELF)  GBL_NOEXCEPT;
//! Uninstalls the given item from its associated type registry from its owner type.
GBL_EXPORT GBL_RESULT GblTypeItem_uninstall (GBL_SELF)  GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given item has been registered with its owner type within the associated type registry.
GBL_EXPORT GblBool    GblTypeItem_installed (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name Accessors
 *  \brief Methods for accessing item properties and members.
 *  @{
 */
//! Returns the name of the item as a C string.
GBL_EXPORT const char* GblTypeItem_name (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name  Operations
 *  \brief Methods for performing various operations on a type item.
 *  @{
 */
//! Returns GBL_TRUE if the given type item is considered to be valid, based on its values.
GBL_EXPORT GBL_RESULT GblTypeItem_validate (GBL_CSELF)                 GBL_NOEXCEPT;
//! Returns a GblHash representing the combination of the owner type and name fields on the given item.
GBL_EXPORT GblHash    GblTypeItem_hash     (GBL_CSELF)                 GBL_NOEXCEPT;
//! Compares the given item with another item from the same type, returning 0 if equal, or their difference.
GBL_EXPORT int        GblTypeItem_compare  (GBL_CSELF,
                                            const GblTypeItem* pOther) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#define GblTypeItem_iterate(...) \
    GblTypeItem_iterateDefault_(__VA_ARGS__)
#define GblTypeItem_iterateDefault_(...) \
    GblTypeItem_iterateDefault__(__VA_ARGS__, NULL)
#define GblTypeItem_iterateDefault__(typeItem, owner, mask, it, cl, ...) \
    (GblTypeItem_iterate)(typeItem, owner, mask, it, cl)

#endif