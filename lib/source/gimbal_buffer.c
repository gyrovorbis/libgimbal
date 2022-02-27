#include <gimbal/types/gimbal_buffer.h>



static GblSize GblBuffer_bytes(const GblBuffer* pSelf) {
    return pSelf->bytes;
}

static const void* GblBuffer_data(const GblBuffer* pSelf) {
    return pSelf->pData;
}


const struct {
    GblVTableHeader                 header;
    const struct GblBuffer_vtable   vtable;
} GblBuffer_vtable = {
    .header = {
        .typeId = GBL_TYPE_INVALID
    },
    .vtable = {
        .IBuffer = {
            .bytes  = (GblSize(*)(const GblIBuffer*))GblBuffer_bytes,
            .data   = (const void*(*)(const GblIBuffer*))GblBuffer_data,
        }
    }
};


GblType GblBuffer_type(void) {
    static GblType typeId = GBL_TYPE_INVALID;
    GBL_API_BEGIN_ONCE(NULL);
    {
        typeId = gblTypeRegisterClass(GBL_IBUFFER_TYPE,
                                      "GblBuffer",
                                      &(const GblClassInfo){
                                            .classSize          = sizeof(GblBuffer_vtable),
                                            .pDefaultVTable     = &GblBuffer_vtable,
                                            .instanceSize       = sizeof(GblBuffer),
                                            .pFnInstanceInit    = (GblInstanceInitFn)GblBuffer_init
                                      }, 0);
    }
    GBL_API_END_ONCE();
    return typeId;
}
