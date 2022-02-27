#ifndef GIMBAL_IBUFFER_H
#define GIMBAL_IBUFFER_H

#include "../meta/gimbal_type.h"
#include "../meta/gimbal_instance.h"

GBL_BEGIN_DECLS

#define GBL_IBUFFER_TYPE    (GblIBuffer_type())


struct GblIBuffer;

struct GblIBuffer_vtable {
    GblSize     (*const bytes)  (const struct GblIBuffer* pSelf);
    const void* (*const data)   (const struct GblIBuffer* pSelf);
};

typedef struct GblIBuffer {
    union {
        const struct GblIBuffer_vtable* pVptr;
        GblInstance                     instance;
    };
} GblIBuffer;


GBL_EXPORT GblType GblIBuffer_type(void);






GBL_END_DECLS



#endif // GIMBAL_IBUFFER_H
