/*! \file
 *  \brief GblInterface and related functions.
 *  \ingroup meta
 */

#ifndef GIMBAL_INTERFACE_H
#define GIMBAL_INTERFACE_H

#include "gimbal_class.h"
#include "gimbal_instance.h"

/*! \details GblType ID of a GblInterface
 * \ingroup metaBuiltinTypes
 */
#define GBL_INTERFACE_TYPE                      GBL_BUILTIN_TYPE(INTERFACE)

/*!
 * Convenience macro around GblInterface_try which automatically casts
 * the given class to a GblClass.
 * \param klass pointer to GblClass
 * \returns pointer to a GblInterface or NULL if not an interface
 * \sa GblInterface_try
 */
#define GBL_INTERFACE(klass)                    (GblInterface_try(GBL_CLASS(klass)))

/*!
 * Convenience macro around GblInterface_outerClass which automatically
 * casts the given interface to a GblInterface.
 * \param iface pointer to GblInterface or derived
 * \returns pointer to a GblClass
 * \sa GblInterface_outerClass
 */
#define GBL_INTERFACE_OUTER_CLASS(iface)        (GblInterface_outerClass(GBL_INTERFACE(iface)))

/*!
 * Convenience macro around GblInterface_outerMostClass which automatically
 * casts the given interface to a GblInterface.
 * \param iface pointer to GblInterface or derived
 * \returns pointer to a GblClass
 * \sa GblInterface_outerMostClass
 */
#define GBL_INTERFACE_OUTER_MOST_CLASS(iface)   (GblInterface_outerMostClass(GBL_INTERFACE(iface)))

#define SELF    GblInterface* pSelf
#define CSELF   const GblInterface* pSelf

GBL_DECLS_BEGIN

/*! \brief Base struct for all interfaces
 * \ingroup meta
 * \details
 * An interface represents an abstract collection of data which
 * may be "implemented" by a class and then queried for later.
 * This data is typically in the form of function pointers,
 * which may or may not have default values, which can then be
 * implemented or overridden by a the class constructor of a
 * type.
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
 *
 * In terms of implementation, a GblInterface is actually implemented
 * as a type of mappable "subclass" which is then embedded within a
 * the structure of a GblClass-inheriting structure. When a type
 * which implements an interface is registered, its location within
 * the class is provided to the type system via GblTypeInfo::pInterfaceMap.
 *\sa GblClass, GblInstace, GblType
 */
typedef struct GblInterface {
    GblClass    base;               ///< inherited GblClass base info
    int16_t     outerClassOffset;   ///< offset from the interface to the class containing it
} GblInterface;

/*!
 * Queries for the given interface type within the specified class.
 * \relatesalso GblInterface
 * \param pClass source class
 * \param ifaceType type ID of the requested interface
 * \returns GblInterface pointer to the requested interface or NULL
 *          if the interface was not present
 * \sa GblClass_try
 */
GBL_INLINE GblInterface* GblInterface_peek              (GblClass* pClass, GblType ifaceType)   GBL_NOEXCEPT;

/*!
 * Casts the given class structure to an interface structure, if
 * it represents an interface.
 * \relatesalso GblInterface
 * \param pClass source class
 * \returns GblInterface pointer if the given class is actually
 *          an interface, or NULL otherwise
 * \sa GblClass_try
 */
GBL_INLINE GblInterface* GblInterface_try               (GblClass* pClass)                      GBL_NOEXCEPT;

/*!
 * Returns the GblClass implementing the given interface.
 * \relatesalso GblInterface
 * \note If the specified interface is a default implementation,
 *       its own GblClass base is returned.
 * \param pSelf interface
 * \returns GblClass pointer to the implementing class.
 * \sa GblClass_outerMost
 */
GBL_INLINE GblClass*     GblInterface_outerClass        (SELF)                                  GBL_NOEXCEPT;

/*!
 * Returns the top-level GblClass implementing the interface.
 *
 * The top-level class will either be its outer class, in the
 * case of a regular embedded interface, or it could be multiple
 * levels out, in the case of an interface mapping an interface.
 * \relatesalso GblInterface
 * \param pSelf interface
 * \returns GblClass pointer to the top-most implementing class.
 * \sa GblClass_try
 */
GBL_INLINE GblClass*     GblInterface_outerMostClass    (SELF)                                  GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE GblClass* GblInterface_outerClass(SELF) GBL_NOEXCEPT {
    return pSelf? (GblClass*)((uintptr_t)pSelf + pSelf->outerClassOffset) : NULL;
}

GBL_INLINE GblClass* GblInterface_outerMostClass(SELF) GBL_NOEXCEPT {
    GblClass* pClass = GBL_CLASS(pSelf);
    while(GblClass_isInterfaceImpl(pClass)) {
        pClass = GblInterface_outerClass((GblInterface*)pClass);
    }
    return pClass;
}

GBL_INLINE GblInterface* GblInterface_try(GblClass* pClass) GBL_NOEXCEPT {
    return GblClass_isInterfaceImpl(pClass)? (GblInterface*)pClass : NULL;
}

GBL_INLINE GblInterface* GblInterface_peek(GblClass* pClass, GblType ifaceType) GBL_NOEXCEPT {
    return (GblInterface*)GblClass_try(pClass, ifaceType);
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_INTERFACE_H
