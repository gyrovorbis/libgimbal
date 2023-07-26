/*! \file
 *  \ingroup signals
 *  \brief   GblClassClosure, virtual method invoking closure, and API
 *
 *  This file contains GblClassClosure and its associated API.
 *  It is used to invoke a virtual method on given GblInstance
 *
 *  \sa GblClassClosure
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_CLASS_CLOSURE_H
#define GIMBAL_CLASS_CLOSURE_H

#include "gimbal_closure.h"

#define GBL_CLASS_CLOSURE_TYPE              (GBL_TYPEID(GblClassClosure))           //!< GblType UUID for GblClassClosure

#define GBL_CLASS_CLOSURE(self)             (GBL_CAST(GblClassClosure, self))       //!< Cast a GblInstance to GblClassClosure
#define GBL_CLASS_CLOSURE_CLASS(klass)      (GBL_CLASS_CAST(GblClassClosure, self)) //!< Cast a GblClass ot GblClassClosureClass
#define GBL_CLASS_CLOSURE_GET_CLASS(self)   (GBL_CLASSOF(GblClassClosure, self))    //!< Get a GblClassClosureClass from GblInstance

#define GBL_SELF_TYPE GblClassClosure

GBL_DECLS_BEGIN

/*! \struct  GblClassClosureClass
 *  \extends GblClosureClass
 *  \brief   GblClass VTable structure for GblClassClosure
 *  \sa      GblClassClosure
 */
GBL_CLASS_DERIVE_EMPTY(GblClassClosure, GblClosure)

/*! \struct  GblClassClosure
 *  \extends GblClosure
 *  \ingroup signals
 *  \brief   GblClosure type for calling into virtual methods
 *
 *  GblClassClosure allows for mapping a generic marshal call
 *  to invoke a virtual method on the GblClass of a GblInstance.
 */
GBL_INSTANCE_DERIVE(GblClassClosure, GblClosure)
    GBL_PRIVATE_BEGIN
        GblType         classType; //!< PRIVATE: Type of class to invoke the method on
        size_t          offset;    //!< PRIVATE: Offset of the virtual function to invoke
        GblInstance*    pInstance; //!< PRIVATE: Instance to invoke the class method on
    GBL_PRIVATE_END
GBL_INSTANCE_END

//! Returns the GblType UUID associated with GblClassClosure
GBL_EXPORT GblType          GblClassClosure_type   (void)                   GBL_NOEXCEPT;

//! Creates a new GblClassClosure with the given values, returning a pointer to it
GBL_EXPORT GblClassClosure* GblClassClosure_create (GblType      classType,
                                                    size_t       offset,
                                                    GblInstance* pInstance,
                                                    void*        pUserdata) GBL_NOEXCEPT;

/*! \name Accessor Methods
 *  \brief Setter methods for GblClassClosure values
 *  \relatesalso GblClassClosure
 *  @{
 */
//! Sets the offset of the virtual method pointer into the GblClass to invoke the closure on
GBL_EXPORT void GblClassClosure_setMethod   (GBL_SELF,
                                             GblType classType,
                                             size_t  offset)         GBL_NOEXCEPT;
//! Sets the GblInstance to call the GblClass virtual method on
GBL_EXPORT void GblClassClosure_setInstance (GBL_SELF,
                                             GblInstance* pInstance) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_CLASS_CLOSURE_H
