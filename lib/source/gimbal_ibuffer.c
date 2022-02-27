#include <gimbal/ifaces/gimbal_ibuffer.h>


static GblSize GblIBuffer_bytes_(const GblIBuffer* pSelf) {
    GBL_UNUSED(pSelf);
    return 0;
}

static const void* GblIBuffer_data_(const GblIBuffer* pSelf) {
    GBL_UNUSED(pSelf);
    return NULL;
}

const static struct {
    GblVTableHeader                 header;
    const struct GblIBuffer_vtable  vtable;
} GblIBuffer_vtable = {
    .header = {
        .typeId = GBL_TYPE_INVALID
    },
    .vtable = {
        .bytes  = GblIBuffer_bytes_,
        .data   = GblIBuffer_data_
    }
};

GBL_EXPORT GblType GblIBuffer_type(void) {
    static GblType typeId = GBL_TYPE_INVALID;
    GBL_API_BEGIN_ONCE(NULL);
    {
        typeId = gblTypeRegisterClass(GBL_TYPE_OBJECT,
                                      "GblIBuffer",
                                      &(const GblClassInfo){
                                            .classSize      = sizeof(GblIBuffer_vtable),
                                            .pDefaultVTable = &GblIBuffer_vtable,
                                            .instanceSize   = sizeof(GblIBuffer),
                                      }, 0);
    }
    GBL_API_END_ONCE();
    return typeId;
}
