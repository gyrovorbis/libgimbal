/*! \file
 *  \brief GblIPlugin abstract plugin interface
 *  \ingroup interfaces
 */

#ifndef GIMBAL_PLUGIN_H
#define GIMBAL_PLUGIN_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

#define GBL_IPLUGIN_TYPE                    (GblIPlugin_type())
#define GBL_IPLUGIN_STRUCT                  GblIPlugin
#define GBL_IPLUGIN_CLASS_STRUCT            GblIPluginIFace
#define GBL_IPLUGIN(instance)               GBL_INSTANCE_CAST_PREFIX        (instance,  GBL_IPLUGIN)
#define GBL_IPLUGIN_CHECK(instance)         GBL_INSTANCE_CHECK_PREFIX       (instance,  GBL_IPLUGIN)
#define GBL_IPLUGIN_TRY(instance)           GBL_INSTANCE_TRY_PREFIX         (instance,  GBL_IPLUGIN)
#define GBL_IPLUGIN_IFACE(klass)            GBL_CLASS_CAST_PREFIX            (klass,     GBL_IPLUGIN)
#define GBL_IPLUGIN_IFACE_CHECK(klass)      GBL_CLASS_CHECK_PREFIX           (klass,     GBL_IPLUGIN)
#define GBL_IPLUGIN_IFACE_TRY(klass)        GBL_CLASS_TRY_PREFIX             (klass,     GBL_IPLUGIN)
#define GBL_IPLUGIN_GET_IFACE(instance)     GBL_INSTANCE_GET_CLASS_PREFIX  (instance,  GBL_IPLUGIN)
#define GBL_IPLUGIN_TRY_IFACE(instance)     GBL_INSTANCE_TRY_CLASS_PREFIX   (instance,  GBL_IPLUGIN)

#define SELF    GblIPlugin* pSelf
#define CSELF   const GblIPlugin* pSelf

GBL_DECLS_BEGIN

GBL_INTERFACE_DERIVE(GblIPlugin)
    GBL_RESULT (*pFnUse)            (SELF);
    GBL_RESULT (*pFnUnuse)          (SELF);
    GBL_RESULT (*pFnTypeInfo)       (CSELF,
                                     GblType        dynamicType,
                                     GblTypeInfo*   pCompleteInfo);
    GBL_RESULT (*pFnInterfacePeek)  (CSELF,
                                     GblType        dynamicType,
                                     GblType        ifaceType,
                                     GblInterface** ppInterface);
GBL_INTERFACE_END

GBL_EXPORT GblType    GblIPlugin_type        (void)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblIPlugin_use         (SELF)                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIPlugin_unuse       (SELF)                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIPlugin_typeInfo    (CSELF,
                                              GblType       type,
                                              GblTypeInfo*  pInfo)  GBL_NOEXCEPT;


GBL_DECLS_END

#undef SELF
#undef CSELF


#endif // GIMBAL_PLUGIN_H
