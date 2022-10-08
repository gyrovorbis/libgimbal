#if 0
#include <gimbal/containers/gimbal_bit_view.h>
#include <gimbal/strings/gimbal_string_buffer.h>

uint8_t GblBitView_lsbMaskLut_[8] = {
    0xff, 0xfe, 0xfc, 0xf8,
    0xf0, 0xe0, 0xc0, 0x80
};

uint8_t GblBitView_msbMaskLut_[8] = {
    0x01, 0x03, 0x07, 0x0f,
    0x1f, 0x3f, 0x7f, 0xff
};

GBL_EXPORT GBL_RESULT (GblBitView_toString)(const GblBitView* pSelf, GblStringBuffer* pBuffer, GblSize index, GblSize count) {
    if(!count) count = pSelf->length;

    GblStringBuffer_reserve(pBuffer, count);
    for(GblSize i = 0; i < count; ++i) {
       const GblBool value = GblBitView_at(pSelf, index + i);
       GblStringBuffer_append(pBuffer, value? GBL_STRV("1") : GBL_STRV("0"));
    }

}
#endif
