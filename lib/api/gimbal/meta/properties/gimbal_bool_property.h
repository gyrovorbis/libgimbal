#ifndef GIMBAL_BOOL_PROPERTY_H
#define GIMBAL_BOOL_PROPERTY_H

#include "gimbal_property.h"

#define GBL_BOOL_PROPERTY_TYPE                  (GblBoolProperty_type())

#define GBL_BOOL_PROPERTY(instance)             (GBL_INSTANCE_CAST(instance, GBL_BOOL_PROPERTY_TYPE, GblBoolProperty))
#define GBL_BOOL_PROPERTY_CLASS(klass)          (GBL_CLASS_CAST(klass, GBL_BOOL_PROPERTY_TYPE, GblBoolPropertyClass))
#define GBL_BOOL_PROPERTY_GET_CLASS(instance)   (GBL_INSTANCE_GET_CLASS(instance, GBL_BOOL_PROPERTY_TYPE, GblBoolPropertyClass))

#define GBL_SELF    GblBoolProperty* pSelf
#define GBL_CSELF   const GBL_SELF

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblBoolProperty, GblProperty)

GBL_INSTANCE_DERIVE(GblBoolProperty, GblProperty)
    GblBool defaultValue;
GBL_INSTANCE_END

GBL_EXPORT GblType          GblBoolProperty_type     (void)                     GBL_NOEXCEPT;

GBL_EXPORT GblBoolProperty* GblBoolProperty_create   (GblType     ownerType,
                                                      const char* pName,
                                                      GblFlags    flags,
                                                      GblBool     defaultValue) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblBoolProperty_construct(GBL_SELF,
                                                      GblType     ownerType,
                                                      const char* pName,
                                                      GblFlags    flags,
                                                      GblBool     defaultValue) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_CSELF
#undef GBL_SELF

#endif // GIMBAL_BOOL_PROPERTY_H
