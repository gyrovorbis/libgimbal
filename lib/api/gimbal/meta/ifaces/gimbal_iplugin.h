/*! \file
 *  \brief GblIPlugin abstract plugin interface
 *  \ingroup interfaces
 *  \sa GblModule
 *  \todo
 *      - finalize with GblModule
 *
 *  \author 2023 Falco Girgis
 *  \copyright MIT License
 */

#ifndef GIMBAL_PLUGIN_H
#define GIMBAL_PLUGIN_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_IPLUGIN_TYPE                (GBL_TYPEID(GblIPlugin))
#define GBL_IPLUGIN(instance)           (GBL_CAST(GblIPlugin, instance))
#define GBL_IPLUGIN_CLASS(klass)        (GBL_CLASS_CAST(GblIPlugin, klass))
#define GBL_IPLUGIN_GET_CLASS(instance) (GBL_CLASSOF(GblIPlugin, instance))
//! @}

#define GBL_SELF_TYPE GblIPlugin

GBL_DECLS_BEGIN

/*! \struct GblIPluginClass
 *  \extends GblInterface
 *  \brief Interface providing vtable for GblPlugin instances
 *
 *  GblIPluginClass provides a virtual table of overridable
 *  methods for implementing a plugin for use with the type
 *  system.
 *
 *  \sa GblModule
 */
GBL_INTERFACE_DERIVE(GblIPlugin)
    GBL_RESULT (*pFnUse)          (GBL_SELF);
    GBL_RESULT (*pFnUnuse)        (GBL_SELF);
    GBL_RESULT (*pFnTypeInfo)     (GBL_CSELF,
                                   GblType        dynamicType,
                                   GblTypeInfo*   pCompleteInfo);
    GBL_RESULT (*pFnInterfacePeek)(GBL_CSELF,
                                   GblType        dynamicType,
                                   GblType        ifaceType,
                                   GblInterface** ppInterface);
GBL_INTERFACE_END

GBL_EXPORT GblType    GblIPlugin_type     (void)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblIPlugin_use      (GBL_SELF)             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIPlugin_unuse    (GBL_SELF)             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblIPlugin_typeInfo (GBL_CSELF,
                                           GblType       type,
                                           GblTypeInfo*  pInfo)  GBL_NOEXCEPT;


GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_PLUGIN_H
