#ifndef GIMBAL_IVARIANT_H
#define GIMBAL_IVARIANT_H

#include "gimbal_interface.h"

#if 0
#define GBL_IVARIANT_FLAGS_TABLE (                                                                                                                         \
        ( GBL_IVARIANT_FLAGS, IVarantFlags, "Variant-Compatible Type Flags", gblIVariantFlagsString),                                                                             \
        (                                                                                                                                               \
            (GBL_IVARIANT_FLAGS_CONSTRUCT_DEFAULT,    0x0, ConstructDefault, "Unknown"),                                 \
            (GBL_IVARIANT_FLAGS_CONSTRUCT_VALUE,      0x1, ConstructValue,   "Success"),                                 \
            (GBL_IVARIANT_FLAGS_CONSTRUCT_COPY,       25,  ConstructCopy,    "Invalid Type"),\
            (GBL_IVARIANT_FLAGS_CONSTRUCT_MOVE,       26,  ConstructMove,    "# of Different Error Codes")               \
            (GBL_IVARIANT_FLAGS_ASSIGN_COPY,          26,  AssignCopy,       "# of Different Error Codes")               \
            (GBL_IVARIANT_FLAGS_ASSIGN_MOVE,          26,  AssignMove,       "# of Different Error Codes")               \
            (GBL_IVARIANT_FLAGS_COUNT,                26,  Count,            "# of Different Error Codes")               \
    )                                                                                                                                               \
)

GBL_FLAGS_TABLE_DECLARE(GBL_IVARIANT_FLAGS_TABLE);

#else
typedef enum GBL_IVARIANT_FLAGS {
    GBL_IVARIANT_CONSTRUCT_DEFAULT  = 0x1,
    GBL_IVARIANT_CONTRUCT_VALUE     = 0x2,
    GBL_IVARIANT_CONTRUCT_COPY      = 0x4,
    GBL_IVARIANT_CONSTRUCT_MOVE     = 0x8,
    GBL_IVARIANT_ASSIGN_COPY        = 0x10,
    GBL_IVARIANT_ASSIGN_MOVE        = 0x20
} GBL_IVARIANT_FLAGS;

#endif
typedef GBL_RESULT (*GblIVariantConstructFn)(GblVariant*, const GblVariant*, GblBool);
typedef GBL_RESULT (*GblIVariantDestructFn) (GblVariant*);
typedef GBL_RESULT (*GblIVariantAssignFn)   (GblVariant*, const GblVariant*, GblBool);
typedef GBL_RESULT (*GblIVariantCompareFn)  (const GblVariant*, const GblVariant*, GblInt*);

typedef struct GblIVariant {
    GblInterface            base;
    GBL_IVARIANT_FLAGS      flags;
    GblIVariantConstructFn  pFnConstruct;
    GblIVariantDestructFn   pFnDestruct;
    GblIVariantAssignFn     pFnAssign;
    GblIVariantCompareFn    pFnCompare;
} GblIVariant;




#endif // GIMBAL_IVARIANT_H
