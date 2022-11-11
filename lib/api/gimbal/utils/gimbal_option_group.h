#ifndef GIMBAL_OPTION_GROUP_H
#define GIMBAL_OPTION_GROUP_H

#include "../meta/instances/gimbal_object.h"

#define GBL_OPTION_GROUP_TYPE                (GBL_TYPEOF(GblOptionGroup))

#define GBL_OPTION_GROUP(instance)           (GBL_INSTANCE_CAST(instance, GblOptionGroup))
#define GBL_OPTION_GROUP_CLASS(klass)        (GBL_CLASS_CAST(klass, GblOptionGroup))
#define GBL_OPTION_GROUP_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GblOptionGroup))

#define GBL_SELF_TYPE GblOptionGroup

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE(GblOptionGroup, GblObject)

GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblOptionGroup, GblObject)

GBL_INSTANCE_END





GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_OPTION_GROUP_H
