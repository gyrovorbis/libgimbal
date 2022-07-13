#ifndef GIMBAL_BUILTIN_TYPES_H
#define GIMBAL_BUILTIN_TYPES_H

#define GBL_TYPE_BUILTIN_INDEX_PROTOCOL             0
#define GBL_TYPE_BUILTIN_INDEX_STATIC_CLASS         1
#define GBL_TYPE_BUILTIN_INDEX_INTERFACE            2
#define GBL_TYPE_BUILTIN_INDEX_INSTANCE             3
#define GBL_TYPE_BUILTIN_INDEX_IVARIANT             4
#define GBL_TYPE_BUILTIN_INDEX_NIL                  5
#define GBL_TYPE_BUILTIN_INDEX_BOOL                 6
#define GBL_TYPE_BUILTIN_INDEX_CHAR                 7
#define GBL_TYPE_BUILTIN_INDEX_UINT8                8
#define GBL_TYPE_BUILTIN_INDEX_INT16                9
#define GBL_TYPE_BUILTIN_INDEX_UINT16               10
#define GBL_TYPE_BUILTIN_INDEX_INT32                11
#define GBL_TYPE_BUILTIN_INDEX_UINT32               12
#define GBL_TYPE_BUILTIN_INDEX_INT64                13  //lazy
#define GBL_TYPE_BUILTIN_INDEX_UINT64               14  //lazy
#define GBL_TYPE_BUILTIN_INDEX_FLOAT                15
#define GBL_TYPE_BUILTIN_INDEX_DOUBLE               16  //lazy
#define GBL_TYPE_BUILTIN_INDEX_STRING               17
#define GBL_TYPE_BUILTIN_INDEX_TYPE                 18  //lazy
#define GBL_TYPE_BUILTIN_INDEX_ENUM                 19  //lazy
#define GBL_TYPE_BUILTIN_INDEX_FLAGS                20  //lazy
#define GBL_TYPE_BUILTIN_INDEX_POINTER              21
#define GBL_TYPE_BUILTIN_INDEX_BOXED                22  //lazy
#define GBL_TYPE_BUILTIN_INDEX_ITABLE               23
#define GBL_TYPE_BUILTIN_INDEX_IEVENT_HANDLER       24  //combine?
#define GBL_TYPE_BUILTIN_INDEX_IEVENT_FILTER        25  //combine? IEventSink?
#define GBL_TYPE_BUILTIN_INDEX_OBJECT               26
#define GBL_TYPE_BUILTIN_INDEX_IALLOCATOR           27
#define GBL_TYPE_BUILTIN_INDEX_ILOGGER              28
#define GBL_TYPE_BUILTIN_INDEX_CONTEXT              29

#define GBL_TYPE_BUILTIN_COUNT                      30

#define GBL_BUILTIN_TYPE(prefix) \
    (GblType_fromBuiltinIndex(GBL_TYPE_BUILTIN_INDEX_##prefix))


#endif // GIMBAL_BUILTIN_TYPES_H
