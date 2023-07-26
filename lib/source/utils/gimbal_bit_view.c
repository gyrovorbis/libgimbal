#include <gimbal/utils/gimbal_bit_view.h>
#include <string.h>

#include <gimbal/strings/gimbal_string_buffer.h>

static uint8_t GblBitView_lsbMaskLut_[8] = {
    0xff, 0xfe, 0xfc, 0xf8,
    0xf0, 0xe0, 0xc0, 0x80
};

static uint8_t GblBitView_msbMaskLut_[8] = {
    0x01, 0x03, 0x07, 0x0f,
    0x1f, 0x3f, 0x7f, 0xff
};

GBL_EXPORT const char* (GblBitView_string)(const GblBitView* pSelf, GblStringBuffer* pBuffer, size_t  index, size_t  count) {
    if(!count) count = pSelf->length;

    GblStringBuffer_reserve(pBuffer, count);
    for(size_t  i = 0; i < count; ++i) {
       const GblBool value = GblBitView_at(pSelf, index + i);
       GblStringBuffer_append(pBuffer, value? GBL_STRV("1") : GBL_STRV("0"));
    }

    return GblStringBuffer_data(pBuffer);
}

GBL_EXPORT uint8_t* GblBitView_byteFromBit_(const GblBitView* pSelf, size_t  bit) {
    GBL_ASSERT(bit < pSelf->length + pSelf->offset);
    return &pSelf->pBytes[bit >> 3];
}

GBL_EXPORT GblBool GblBitView_wordAligned(const GblBitView* pSelf) GBL_NOEXCEPT {
    return pSelf->offset == 0;
}

GBL_EXPORT GblBitView* (GblBitView_fromBuffer)(GblBitView* pSelf, void* pStorage, size_t  bits, size_t  offset) GBL_NOEXCEPT {
    pSelf->length = bits;
    pSelf->offset = offset & 0x7;
    pSelf->pBytes = (uint8_t*)pStorage + (offset >> 3);
    return pSelf;
}

GBL_EXPORT GblBitView* (GblBitView_fromView)(GblBitView* pSelf, const GblBitView* pSource, size_t  count, size_t  offset) GBL_NOEXCEPT {
    GBL_ASSERT(offset+count <= pSource->length);
    pSelf->length = count;
    pSelf->offset = pSource->offset + offset;
    pSelf->pBytes = GblBitView_byteFromBit_(pSource, offset);
    return pSelf;
}

GBL_EXPORT size_t  GblBitView_bytes(const GblBitView* pSelf) GBL_NOEXCEPT {
    return GBL_BIT_VIEW_BYTES(pSelf->length);
}

GBL_EXPORT size_t  GblBitView_words(const GblBitView* pSelf) GBL_NOEXCEPT {
    return GBL_BIT_VIEW_WORDS(pSelf->length);
}

GBL_EXPORT GblBool GblBitView_at(const GblBitView* pSelf, size_t  index) GBL_NOEXCEPT {
    GblBool value = GBL_FALSE;
    if(index < pSelf->length) {
        index += pSelf->offset;
        value = (*GblBitView_byteFromBit_(pSelf, index) >> (index & 0x7)) & 0x1;
    } else {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();
    }
    return value;
}

GBL_EXPORT size_t  (GblBitView_count)(const GblBitView* pSelf, size_t  index, size_t  count) GBL_NOEXCEPT {
/* Would've been epic as shit, but doesn't do any good
 * when not on a byte boundary...
    static const uint8_t lut[16] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
    };
*/
    size_t  bits = 0;

    const size_t  start = pSelf->offset + index;
    const size_t  end = start + count;
    if(start/4 < end/4) {
        pSelf->pBytes[start/8] |= GblBitView_msbMaskLut_[start%8];

        for(size_t  i = start/8+1; i < end/8; ++i)
            pSelf->pBytes[i] = 0xff;

        pSelf->pBytes[end/8] |= GblBitView_lsbMaskLut_[end%8];
    } else {
        pSelf->pBytes[start/8] |= (GblBitView_msbMaskLut_[start%8] &
                                   GblBitView_lsbMaskLut_[end%8]);
    }

    return bits;
}

GBL_EXPORT GblBool (GblBitView_any)(const GblBitView* pSelf, size_t  index, size_t  count) GBL_NOEXCEPT {
    return GblBitView_count(pSelf, index, count) != 0;
}

GBL_EXPORT GblBool (GblBitView_all)(const GblBitView* pSelf, size_t  index, size_t  count) GBL_NOEXCEPT {
    return GblBitView_count(pSelf, index, count) == count;
}

GBL_EXPORT GblBool (GblBitView_none)(const GblBitView* pSelf, size_t  index, size_t  count) GBL_NOEXCEPT {
    return GblBitView_count(pSelf, index, count) == 0;
}

GBL_EXPORT GblBitView* (GblBitView_assign)(GblBitView* pSelf, GblBool value, size_t  index, size_t  count) GBL_NOEXCEPT {
    if(value) return GblBitView_set(pSelf, index, count);
    return GblBitView_reset(pSelf, index, count);
}

GBL_EXPORT GblBitView* (GblBitView_set)(GblBitView* pSelf, size_t  index, size_t  count) GBL_NOEXCEPT {
    const size_t  start = pSelf->offset + index;
    const size_t  end = start + count;
    if(start/8 < end/8) {
        pSelf->pBytes[start/8] |= GblBitView_msbMaskLut_[start%8];
        for(size_t  i = start/8+1; i < end/8; ++i)
            pSelf->pBytes[i] = 0xff;
        pSelf->pBytes[end/8] |= GblBitView_lsbMaskLut_[end%8];
    } else {
        pSelf->pBytes[start/8] |= (GblBitView_msbMaskLut_[start%8] |
                                   GblBitView_lsbMaskLut_[end%8]);
    }
    return pSelf;
}

GBL_EXPORT GblBitView* (GblBitView_reset)(GblBitView* pSelf, size_t  index, size_t  count) GBL_NOEXCEPT {
    const size_t  start = pSelf->offset + index;
    const size_t  end = start + count;
    if(start/8 < end/8) {
        pSelf->pBytes[start/8] &= ~GblBitView_msbMaskLut_[start%8];
        for(size_t  i = start/8+1; i < end/8; ++i)
            pSelf->pBytes[i] = 0x00;
        pSelf->pBytes[end/8] &= ~GblBitView_lsbMaskLut_[end%8];
    } else {
        pSelf->pBytes[start/8] &= ~(GblBitView_msbMaskLut_[start%8] |
                                   GblBitView_lsbMaskLut_[end%8]);
    }
    return pSelf;
}

GBL_EXPORT GblBitView* (GblBitView_flip)(GblBitView* pSelf, size_t  index, size_t  count) GBL_NOEXCEPT {
    const size_t  start = pSelf->offset + index;
    const size_t  end   = start + count;

    const size_t  startByte = start / 8;
    const size_t  endByte   = end   / 8;
    const size_t  startBit  = start % 8;
    const size_t  endBit    = end   % 8;

    if(startByte < endByte) {
        pSelf->pBytes[startByte] ^= GblBitView_msbMaskLut_[startBit];

        for(size_t  i = startByte + 1; i < endByte; ++i)
            pSelf->pBytes[i] ^= 0xff;

        pSelf->pBytes[endByte] ^= GblBitView_lsbMaskLut_[endBit];

    } else {
        pSelf->pBytes[startByte] ^= (GblBitView_msbMaskLut_[startBit] |
                                     GblBitView_lsbMaskLut_[endBit]);
    }
    return pSelf;
}

GBL_EXPORT GblBool GblBitView_eq(const GblBitView* pSelf, const GblBitView* pOther) GBL_NOEXCEPT {
#if 0
    const size_t  start = pSelf->offset + index;
    const size_t  end = start + count;
    if(start/8 < end/8) {
        if(start/32 < end/32) {

        } else { // per-byte
            pSelf->pBytes[start/8] ^= GblBitView_msbMaskLut_[start%8];

            for(size_t  i = start/8+1; i < end/8; ++i)
                if(pSelf->pBytes[i] != pOther->pBytes[i]) return;

            pSelf->pBytes[end/8] ^= GblBitView_lsbMaskLut_[end%8];
        }
    } else { // single byte
        pSelf->pBytes[start/8] ^= (GblBitView_msbMaskLut_[start%8] &
                                   GblBitView_lsbMaskLut_[end%8]);
    }
#else
    GblBool result = GBL_TRUE;
    for(size_t  i = 0; i < pSelf->length; ++i)
        if(GblBitView_at(pSelf, i) != GblBitView_at(pOther, i)) {
            result = GBL_FALSE;
            break;
        }
    return result;
#endif
}

GBL_EXPORT GblBool GblBitView_lt(const GblBitView* pSelf, const GblBitView* pOther) GBL_NOEXCEPT {
    GblBool result = GBL_FALSE;
    for(size_t  i = 0; i < pSelf->length; ++i) {
        const GblBool lvalue = GblBitView_at(pSelf, i);
        const GblBool rvalue = GblBitView_at(pOther, i);
        if(!lvalue && rvalue) {
            result = GBL_TRUE;
            break;
        } else if(lvalue && !rvalue) {
            result = GBL_FALSE;
            break;
        }
    }
    return result;
}

GBL_EXPORT GblBool GblBitView_leq(const GblBitView* pSelf, const GblBitView* pOther) GBL_NOEXCEPT {
    GblBool result = GBL_TRUE;
    for(size_t  i = 0; i < pSelf->length; ++i) {
        const GblBool lvalue = GblBitView_at(pSelf, i);
        const GblBool rvalue = GblBitView_at(pOther, i);
        if(lvalue && !rvalue) {
            result = GBL_FALSE;
            break;
        }
    }
    return result;
}

GBL_EXPORT GblBitView* (GblBitView_setString)(GblBitView* pSelf, const char* pString, size_t  index, size_t  count) GBL_NOEXCEPT {
    GBL_ASSERT(strlen(pString) >= count);
    if(!count) count = strlen(pString);
    GBL_ASSERT(index + count < pSelf->length);

    for(size_t  i = 0; i < count; ++i) {
        const char digit = pString[i];
        GBL_ASSERT(digit == '0' || digit == '1');
        const GblBool value = digit == '1'? GBL_TRUE : GBL_FALSE;
        GblBitView_assign(pSelf, value, index+i);
    }

    return pSelf;
}

GBL_EXPORT uint64_t (GblBitView_uint64)(const GblBitView* pSelf, size_t  index, size_t  count) GBL_NOEXCEPT {
    uint64_t value = 0;
    const size_t  end = index + count;
    GBL_ASSERT(end <= pSelf->length);
    for(size_t  b = 0; b < count; ++b) {
        value |= (GblBitView_at(pSelf, index+b) << b);
    }
    return value;
}

GBL_EXPORT GblBitView* (GblBitView_setUint64)(GblBitView* pSelf, uint64_t value, size_t  index, size_t  count) GBL_NOEXCEPT {
    for(size_t  b = 0; b < count; ++b) {
        GblBitView_assign(pSelf, (value>>b)&0x1, index+b);
    }
    return pSelf;
}

GBL_EXPORT GblBitView* (GblBitView_rotate)(GblBitView* pSelf, ptrdiff_t n, size_t  offset, size_t  count) GBL_NOEXCEPT {
    return pSelf;
}

GBL_EXPORT GblBitView* GblBitView_shiftLeft(GblBitView* pSelf, const GblBitView* pSrc, size_t  amount) GBL_NOEXCEPT {
    return pSelf;
}

GBL_EXPORT GblBitView* GblBitView_shiftRight(GblBitView* pSelf, const GblBitView* pSrc, size_t  amount) GBL_NOEXCEPT {
    return pSelf;
}

#if 0

GBL_EXPORT GBL_RESULT GblBitView_and(GblBitView* pSelf, const GblBitView* pValue1, const GblBitView* pValue2) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->length == pValue1->length && pSelf->length == pValue2->length);
    for(size_t  i = 0; i < pSelf->length; ++i) {
        GblBitView_assign(pSelf, GblBitView_at(pValue1, i) & GblBitView_at(pValue2, i), i);
    }
}

GBL_EXPORT GBL_RESULT GblBitView_or(GblBitView* pSelf, const GblBitView* pValue1, const GblBitView* pValue2) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->length == pValue1->length && pSelf->length == pValue2->length);
    for(size_t  i = 0; i < pSelf->length; ++i) {
        GblBitView_assign(pSelf, GblBitView_at(pValue1, i) | GblBitView_at(pValue2, i), i);
    }
}

GBL_EXPORT GBL_RESULT GblBitView_xor(GblBitView* pSelf, const GblBitView* pValue1, const GblBitView* pValue2) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->length == pValue1->length && pSelf->length == pValue2->length);
    for(size_t  i = 0; i < pSelf->length; ++i) {
        GblBitView_assign(pSelf, GblBitView_at(pValue1, i) ^ GblBitView_at(pValue2, i), i);
    }
}

GBL_EXPORT GBL_RESULT GblBitView_andNot(GblBitView* pSelf, const GblBitView* pValue1, const GblBitView* pValue2) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->length == pValue1->length && pSelf->length == pValue2->length);
    for(size_t  i = 0; i < pSelf->length; ++i) {
        GblBitView_assign(pSelf, GblBitView_at(pValue1, i) & ~GblBitView_at(pValue2, i), i);
    }
}
#endif
