#include <gimbal/types/gimbal_buffer.h>
#include <stdatomic.h>
#include <threads.h>

static once_flag    onceInit_= ONCE_FLAG_INIT;
static GblType      typeId_ = GBL_TYPE_INVALID


void GblBuffer_register_(void) {
        typeId_ = gblTypeRegister(GBL_INSTANCE_TYPE, "GblBuffer",
                        &(const GblTypeInfo){

                        }, 0);


GblType GblBuffer_type(void) {
    call_once(&onceInit_, GblBuffer_register_);
    return typeId_;
}
