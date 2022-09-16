/*! \file
 *  \brief GblIPlugin abstract plugin interface
 *  \ingroup interfaces
 */

#ifndef GIMBAL_PLUGIN_H
#define GIMBAL_PLUGIN_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

#define GBL_IPLUGIN_TYPE                (GBL_TYPEOF(GblIPlugin))
#define GBL_IPLUGIN(instance)           (GBL_INSTANCE_CAST(instance, GblIPlugin))
#define GBL_PLUGIN_CLASS(klass)         (GBL_CLASS_CAST(klass, GblIPlugin))
#define GBL_IPLUGIN_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GblIPlugin))

#define GBL_SELF_TYPE GblIPlugin

GBL_DECLS_BEGIN

GBL_INTERFACE_DERIVE(GblIPlugin)
    GBL_RESULT (*pFnUse)            (GBL_SELF);
    GBL_RESULT (*pFnUnuse)          (GBL_SELF);
    GBL_RESULT (*pFnTypeInfo)       (GBL_CSELF,
                                     GblType        dynamicType,
                                     GblTypeInfo*   pCompleteInfo);
    GBL_RESULT (*pFnInterfacePeek)  (GBL_CSELF,
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
