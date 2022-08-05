#ifndef GIMBAL_RECORD_H
#define GIMBAL_RECORD_H

#include "gimbal_instance.h"

#define GBL_RECORD_TYPE                 (GblRecord_type())

#define GBL_RECORD(instance)            (GBL_INSTANCE_CAST(instance, GBL_RECORD_TYPE, GblRecord))
#define GBL_RECORD_CLASS(klass)         (GBL_CLASS_CAST(klass, GBL_RECORD_TYPE, GblRecordClass))
#define GBL_RECORD_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GBL_RECORD_TYPE, GblRecordClass))

#define SELF GblRecord* pSelf
#define CSELF const SELF

GBL_DECLS_BEGIN



GBL_DECLS_END

#undef CSELF
#undef SELF


#endif // GIMBAL_RECORD_H
