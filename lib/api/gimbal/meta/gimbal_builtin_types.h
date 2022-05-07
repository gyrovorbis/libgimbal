#ifndef GIMBAL_BUILTIN_TYPES_H
#define GIMBAL_BUILTIN_TYPES_H

#define GBL_TYPE_BUILTIN_INDEX_INTERFACE            0
#define GBL_TYPE_BUILTIN_INDEX_IVARIANT             1
#define GBL_TYPE_BUILTIN_INDEX_NIL                  2
#define GBL_TYPE_BUILTIN_INDEX_BOOL                 3
#define GBL_TYPE_BUILTIN_INDEX_CHAR                 4
#define GBL_TYPE_BUILTIN_INDEX_UINT8                5
#define GBL_TYPE_BUILTIN_INDEX_INT16                6
#define GBL_TYPE_BUILTIN_INDEX_UINT16               7
#define GBL_TYPE_BUILTIN_INDEX_INT32                8
#define GBL_TYPE_BUILTIN_INDEX_UINT32               9
#define GBL_TYPE_BUILTIN_INDEX_INT64                10
#define GBL_TYPE_BUILTIN_INDEX_UINT64               11
#define GBL_TYPE_BUILTIN_INDEX_ENUM                 12
#define GBL_TYPE_BUILTIN_INDEX_FLAGS                13
#define GBL_TYPE_BUILTIN_INDEX_FLOAT                14
#define GBL_TYPE_BUILTIN_INDEX_DOUBLE               15
#define GBL_TYPE_BUILTIN_INDEX_POINTER              16
#define GBL_TYPE_BUILTIN_INDEX_STRING               17
#define GBL_TYPE_BUILTIN_INDEX_TYPE                 18
#define GBL_TYPE_BUILTIN_INDEX_BOXED                19
#define GBL_TYPE_BUILTIN_INDEX_ITABLE               20
#define GBL_TYPE_BUILTIN_INDEX_IEVENT_HANDLER       21
#define GBL_TYPE_BUILTIN_INDEX_IEVENT_FILTER        22
#define GBL_TYPE_BUILTIN_INDEX_OBJECT               23
#define GBL_TYPE_BUILTIN_INDEX_IALLOCATOR           24
#define GBL_TYPE_BUILTIN_INDEX_ILOGGER              25
#define GBL_TYPE_BUILTIN_INDEX_CONTEXT              26
#define GBL_TYPE_BUILTIN_INDEX_EVENT                27

#define GBL_TYPE_BUILTIN_COUNT                      28

#define GBL_BUILTIN_TYPE(prefix) \
    (GblType_fromBuiltinIndex(GBL_TYPE_BUILTIN_INDEX_##prefix))


#endif // GIMBAL_BUILTIN_TYPES_H
