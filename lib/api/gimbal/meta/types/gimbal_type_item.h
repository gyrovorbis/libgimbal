/*! \file
    \brief   Data structures for holding associated type data.
    \ingroup meta

    This file contains the data structures and their API for associating
    additional data with the type system, such as with GblProperty and
    GblSignal. 

    \author    2025 Falco Girgis
    \copyright MIT License
*/
#ifndef GIMBAL_TYPE_ITEM_H
#define GIMBAL_TYPE_ITEM_H

#include "../instances/gimbal_box.h"
#include "../../containers/gimbal_linked_list.h"
#include "../../containers/gimbal_hash_set.h"

/*! \name  Type System
    \brief UUID and Cast Operators
    @{
*/
#define GBL_TYPE_ITEM_TYPE          GBL_TYPEID(GblTypeItem)             //!< Type UUID
#define GBL_TYPE_ITEM(self)         GBL_CAST(GblTypeItem, self)         //!< Instance cast operator
#define GBL_TYPE_ITEM_CLASS(klass)  GBL_CLASS_CAST(GblTypeItem, klass)  //!< Class cast operator
#define GBL_TYPE_ITEM_CLASSOF(self) GBL_CLASSOF(GblTypeItem, self)      //!< Class extraction operator
//! @}

#define GBL_SELF_TYPE GblTypeItem

GBL_DECLS_BEGIN

/*! Data structure for holding type-associated data.

    A GblTypeItemTable is a container used to store data that gets
    associated with a specific GblType, such as GblProperty and
    GblSignal. It's a GblHashSet containing GblTypeItem entries,
    which form a linked list of data associated with each type.

    \note
    Derived types automatically inherit associated data of their parent
    types when looking them up or iterating.
*/
typedef GblTypeItemTable GblHashSet;

//! Iterator callback to be passed to GblTypeItemTable_foreach(), should return GBL_FALSE to stop iteration.
typedef GblBool (*GblTypeItemTableIterFn)(const GblTypeItemTable* pTable,
                                          GblTypeItem*            pItem,
                                          void*                   pClosure);

GBL_FORWARD_DECLARE_STRUCT(GblTypeItem);

/*! \struct  GblTypeItemClass
    \extends GblBoxClass
    \brief   GblClass VTable structure for GblTypeItem.

    Empty.

    \sa GblPropertyClass, GblSignalClass.
*/
GBL_CLASS_DERIVE_EMPTY(GblTypeItem, GblBox)

/*! \struct  GblTypeItem
    \extends GblBox

    A single entry into a GblTypeItemTable, representing some piece
    of data or member associated with the given type. 

    \sa GblProperty, GblSignal
*/
GBL_INSTANCE_DERIVE(GblTypeItem, GblBox)
    union {
        GblLinkedListNode listNode; //!< Linked list node pointing to next entry.
        GblTypeItem*      pNext;    //!< Strongly-typed next entry.
    };
    GblType  type;  //!< Type UUID the item is associated with.
    GblQuark name;  //!< Name of the associated item.
    GblFlags flags; //!< Generic flags for the associated item.
GBL_INSTANCE_END

/*! \name Item Management
    \brief Routines for installing and uninstalling items.
    @{
*/
//! Installs a pre-filled item into the given table, returning a result code.
GBL_EXPORT GBL_RESULT GblTypeItemTable_install        (GblTypeItemTable* pTable, 
                                                       GblTypeItem*      pItem)  GBL_NOEXCEPT;
//! Finds and uninstalls the item with the given type ID and string name from the given table.
GBL_EXPORT GblBool    GblTypeItemTable_uninstall      (GblTypeItemTable* pTable, 
                                                       GblType           type,
                                                       const char*       pName)  GBL_NOEXCEPT;
//! Finds and uninstalls the item with the given type ID and quark name from the given table.
GBL_EXPORT GblBool    GblTypeItemTable_uninstallQuark (GblTypeItemTable* pTable,
                                                       GblType           type,
                                                       GblQuark          name)   GBL_NOEXCEPT;
//! Uninstalls all items which have been associated with the given type UUID.
GBL_EXPORT GblBool    GblTypeItemTable_uninstallAll   (GblType type)             GBL_NOEXCEPT;
//! @}

/*! \name  Searching
    \brief Routines for looking up items within a table.
    @{
*/
//! Finds and returns the item with the given type and name string within the given table or NULL if there was none.
GBL_EXPORT GblTypeItem* GblTypeItemTable_find      (const GbltypeItemTable* pTable,
                                                    GblType                 type,
                                                    const char*             pName)    GBL_NOEXCEPT;
//! Finds and returns the item with the given type and name quark within the given table or NULL if there was none.
GBL_EXPORT GblTypeItem* GblTypeItemTable_findQuark (const GblTypeItemTable* pTable,
                                                    GblType                 type,
                                                    GblQuark                name)     GBL_NOEXCEPT;
//! @}

/*! \name  Iteration
    \brief Routines for traversing a table. 
    @{
*/
//! Returns the item following the current within the given table, respecting the inheritance hierarchy of the given type.
GBL_EXPORT GblTypeItem* GblTypeItemTable_next      (const GblTypeItemTable* pTable,
                                                    GblType                 type,
                                                    const GblTypeItem*      pCurrent,
                                                    GblFlags                mask);    GBL_NOEXCEPT;
//! Iterates over all items associated with the given type within the given table, calling the given callback, respecting the inheritance heirarchy
GBL_EXPORT GblBool      GblTypeItemTable_foreach   (const GblTypeItemTable* pTable,
                                                    GblType                 type,
                                                    GblTypeItemTableIterFn  pIterFn,
                                                    void*                   pClosure) GBL_NOEXCEPT;
//! @}

/*! \name  Miscellaneous
    \brief Other routines and table operations.
    @{
*/
//! Returns the total number of GblTypeItems contained within the table for all types.
GBL_EXPORT size_t     GblTypeItemTable_totalCount     (const GblTypeItemTable* pTable) GBL_NOEXCEPT;
//! Returns the total number of items associated with the given type UUID.
GBL_EXPORT size_t     GblTypeItemTable_count          (const GblTypeItemTable* pTable,
                                                       GblType                 type)   GBL_NOEXCEPT;
//! Returns the flags for every item within the given table bitwise OR'd together.
GBL_EXPORT GblFlags   GblTypeItemTable_combinedFlags  (const GblTypeItemTable* pTable,
                                                       GblType                 type)   GBL_NOEXCEPT;
//! @}

//! Returns the GblType UUID associated with GblTypeItem.
GBL_EXPORT GblType  GblTypeItem_type    (void) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif
