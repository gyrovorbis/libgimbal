/*! \file
 *  \brief GblBitView bitvector view container and related functions
 *  \ingroup utilities
 *  \copydoc GblBitView
 */

#ifndef GIMBAL_BIT_VIEW_H
#define GIMBAL_BIT_VIEW_H

#include "gimbal_byte_array.h"

#define GBL_BIT_VIEW_BPW            (8*sizeof(uint32_t))
#define GBL_BIT_VIEW_BYTES(bits)    ((((bits) + 8 - 1)&(~(8-1)))/8)
#define GBL_BIT_VIEW_WORDS(bits)    ((((bits) + GBL_BIT_VIEW_BPW - 1) & (~(GBL_BIT_VIEW_BPW-1)))/GBL_BIT_VIEW_BPW)

#define GBL_SELF_TYPE GblBitView

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblStringBuffer);

/*! \brief Mutable view to a field of bits within some
 *         block of memory.
 *  \ingroup utilities
 *
 *  GblBitView is a view-based container, which does not
 *  own, but simply references, an existing field of bits
 *  within another buffer. It provides a comparable API
 *  to that of a "BitArray," "BitVector," "BitMap,"
 *  or "BitSet," as they're often known.
 *
 *  GblBitView allows you to conveniently refer a range
 *  of bits as though they were a regular array, allowing
 *  you to perform operations on them individually.
 *  Certain operations such as count, set, reset, and flip,
 *  are implemented with optimized algorithms that
 *  operate either per-word or per-byte,
 *  rather than requiring iteration over every bit.
 *
 *  \note Even though GblBitView is a view, which does
 *  not "own" the memory it is backed by, certain
 *  function overloads are able to create alloca()-backed
 *  stack memory on-the-fly to associate with the view,
 *  so you don't have to provide a backing buffer for it.
 *  \sa GblByteArray
 *
 *  \todo
 *    - finish implementation
 *    - decide on API for bitwise ops against others
 */
typedef struct GblBitView {
    union {
        void*     pData;
        uint8_t*  pBytes;
        uint32_t* pWords;
    };
    GblSize length;
    uint8_t offset;
} GblBitView;

// ===== Public methods =====
GBL_INLINE GblBitView* GblBitView_fromBuffer (GBL_SELF, void* pBuffer, GblSize bitCount, GblSize offset)         GBL_NOEXCEPT;
GBL_INLINE GblBitView* GblBitView_fromView   (GBL_SELF, const GblBitView* pView, GblSize count, GblSize offset)  GBL_NOEXCEPT;

GBL_EXPORT const char* GblBitView_string     (GBL_CSELF, GblStringBuffer* pBuffer, GblSize index, GblSize count) GBL_NOEXCEPT;
GBL_INLINE uint64_t    GblBitView_uint64     (GBL_CSELF, GblSize index, GblSize count)                           GBL_NOEXCEPT;

GBL_INLINE GblBitView* GblBitView_setString  (GBL_SELF, const char* pString, GblSize index, GblSize count)       GBL_NOEXCEPT;
GBL_INLINE GblBitView* GblBitView_setUint64  (GBL_SELF, uint64_t value, GblSize index, GblSize count)            GBL_NOEXCEPT;

GBL_INLINE GblSize     GblBitView_bytes      (GBL_CSELF)                                                         GBL_NOEXCEPT;
GBL_INLINE GblSize     GblBitView_words      (GBL_CSELF)                                                         GBL_NOEXCEPT;

GBL_INLINE GblBool     GblBitView_at         (GBL_CSELF, GblSize index)                                          GBL_NOEXCEPT;
GBL_INLINE GblSize     GblBitView_count      (GBL_CSELF, GblSize index, GblSize count)                           GBL_NOEXCEPT;
GBL_INLINE GblBool     GblBitView_any        (GBL_CSELF, GblSize index, GblSize count)                           GBL_NOEXCEPT;
GBL_INLINE GblBool     GblBitView_all        (GBL_CSELF, GblSize index, GblSize count)                           GBL_NOEXCEPT;
GBL_INLINE GblBool     GblBitView_none       (GBL_CSELF, GblSize index, GblSize count)                           GBL_NOEXCEPT;

GBL_INLINE GblBitView* GblBitView_assign     (GBL_SELF, GblBool value, GblSize index, GblSize count)             GBL_NOEXCEPT;
GBL_INLINE GblBitView* GblBitView_set        (GBL_SELF, GblSize index, GblSize count)                            GBL_NOEXCEPT;
GBL_INLINE GblBitView* GblBitView_reset      (GBL_SELF, GblSize index, GblSize count)                            GBL_NOEXCEPT;
GBL_INLINE GblBitView* GblBitView_flip       (GBL_SELF, GblSize index, GblSize count)                            GBL_NOEXCEPT;
GBL_INLINE GblBitView* GblBitView_rotate     (GBL_SELF, ptrdiff_t n, GblSize offset, GblSize count)              GBL_NOEXCEPT;
GBL_INLINE GblBitView* GblBitView_shiftLeft  (GBL_SELF, const GblBitView* pSrc, GblSize amount)                  GBL_NOEXCEPT;
GBL_INLINE GblBitView* GblBitView_shiftRight (GBL_SELF, const GblBitView* pSrc, GblSize amount)                  GBL_NOEXCEPT;

/* todo
GBL_EXPORT GBL_RESULT  GblBitView_and        (GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBitView_or         (GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBitView_xor        (GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBitView_andNot     (GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2)    GBL_NOEXCEPT;

GBL_INLINE GblBool     GblBitView_eq         (GBL_CSELF, const GblBitView* pOther)                               GBL_NOEXCEPT;
GBL_INLINE GblBool     GblBitView_lt         (GBL_CSELF, const GblBitView* pOther)                               GBL_NOEXCEPT;
GBL_INLINE GblBool     GblBitView_leq        (GBL_CSELF, const GblBitView* pOther)                               GBL_NOEXCEPT;
*/

// ===== Macro overrides =====
#define GblBitView_fromBuffer(...)           GblBitView_fromBufferDefalt_(__VA_ARGS__)
#define GblBitView_fromView(...)             GblBitView_fromViewDefault_(__VA_ARGS__)
#define GblBitView_string(...)               GblBitView_stringDefault_(__VA_ARGS__)
#define GblBitView_uint64(...)               GblBitView_uint64Default_(__VA_ARGS__)
#define GblBitView_setString(...)            GblBitView_setStringDefault_(__VA_ARGS__)
#define GblBitView_setUint64(...)            GblBitView_setUUint64Default_(__VA_ARGS__)
#define GblBitView_count(...)                GblBitView_countDefault_(__VA_ARGS__)
#define GblBitView_any(...)                  GblBitView_anyDefault_(__VA_ARGS__)
#define GblBitView_all(...)                  GblBitView_allDefault_(__VA_ARGS__)
#define GblBitView_none(...)                 GblBitView_noneDefault_(__VA_ARGS__)
#define GblBitView_assign(...)               GblBitView_assignDefault_(__VA_ARGS__)
#define GblBitView_set(...)                  GblBitView_setDefault_(__VA_ARGS__)
#define GblBitView_reset(...)                GblBitView_resetDefault_(__VA_ARGS__)
#define GblBitView_flip(...)                 GblBitView_flipDefault_(__VA_ARGS__)
#define GblBitView_rotate(...)               GblBitView_rotateDefault_(__VA_ARGS__)

// ===== Implementation =====
/// \cond
#define GblBitView_fromBuffer_(...) \
    GblBitView_fromBufferDefault__(__VA_ARGS__, 0)
#define GblBitView_initDefault__(self, buffer, count, offset, ...) \
    (GblBitView_fromBuffer)(self, buffer, count, offset)

#define GblBitView_fromViewDefault_(...) \
    GblBitView_viewDefault__(__VA_ARGS__, GBL_TUPLE_FIRST(__VA_ARGS__)->length, 0)
#define GblBitView_fromViewDefault__(self, view, count, offset, ...) \
    (GblBitView_fromView)(self, view, count, offset)

#define GblBitView_stringDefault_(...) \
    GblBitView_stringDefault_(__VA_ARGS__, 0, GBL_TUPLE_FIRST(__VA_ARGS__)->length)
#define GblBitView_stringDefault__(self, buffer, index, count, ...) \
    (GblBitView_string)(self, buffer, index, count)

#define GblBitView_setStringDefault_(...) \
    GblBitView_setStringDefault__(__VA_ARGS__, 0, 0)
#define GblBitView_setStringDefault__(self, string, index, count, ...) \
    (GblBitView_fromString)(self, string, index, count)

#define GblBitView_countDefault_(...) \
    GblBitView_countDefault__(__VA_ARGS__, 0, 0)
#define GblBitView_countDefault__(self, index, count, ...) \
    (GblBitView_count)(self, index, count)

#define GblBitView_anyDefault_(...) \
    GblBitView_anyDefault__(__VA_ARGS__, 0, 0)
#define GblBitView_anyDefault__(self, index, count, ...) \
    (GblBitView_any)(self, index, count)

#define GblBitView_allDefault_(...) \
    GblBitView_allDefault__(__VA_ARGS__, 0, 0)
#define GblBitView_allDefault__(self, index, count, ...) \
    (GblBitView_all)(self, index, count)

#define GblBitView_noneDefault_(...) \
    GblBitView_noneDefault__(__VA_ARGS__, 0, 0)
#define GblBitView_noneDefault__(self, index, count, ...) \
    (GblBitView_none)(self, index, count)

#define GblBitView_assignDefault_(...) \
    GblBitView_assignDefault__(__VA_ARGS__, 0, 0)
#define GblBitView_assignDefault__(self, value, index, count, ...) \
    (GblBitView_assign)(self, value, index, count)

#define GblBitView_setDefault_(...) \
    GblBitView_setDefault__(__VA_ARGS__, 0, 0)
#define GblBitView_setDefault__(self, index, count, ...) \
    (GblBitView_set)(self, index, count)

#define GblBitView_resetDefault_(...) \
    GblBitView_resetDefault__(__VA_ARGS__, 0, 0)
#define GblBitView_resetDefault__(self, index, count, ...) \
    (GblBitView_reset)(self, index, count)

#define GblBitView_flipDefault_(...) \
    GblBitView_filpDefault__(__VA_ARGS__, 0, 0)
#define GblBitView_flipDefault__(self, index, count, ...) \
    (GblBitView_flip)(self, index, count)

#define GblBitView_rotateDefault_(...) \
    GblBitView_rotateDefault__(__VA_ARGS__, 0, 0)
#define GblBitView_rotateDefault__(self, offset, index, count, ...) \
    (GblBitView_rotate)(self, offset, index, count)

extern uint8_t GblBitView_lsbMaskLut_[8];
extern uint8_t GblBitView_msbMaskLut_[8];

GBL_INLINE uint8_t* GblBitView_byteFromBit_(GBL_CSELF, GblSize bit) {
    GBL_ASSERT(bit < pSelf->length);
    return &pSelf->pBytes[bit/8];
}

/// \endcond

GBL_INLINE GblBool GblBitView_wordAligned(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->offset == 0;
}

GBL_INLINE GblBitView* (GblBitView_fromBuffer)(GBL_SELF, void* pStorage, GblSize bits, GblSize offset) GBL_NOEXCEPT {
    pSelf->length = bits;
    pSelf->offset = offset % 8;
    pSelf->pBytes = (uint8_t*)pStorage + (offset/8);
    return pSelf;
}

GBL_INLINE GblBitView* (GblBitView_fromView)(GBL_SELF, const GblBitView* pSource, GblSize count, GblSize offset) GBL_NOEXCEPT {
    GBL_ASSERT(offset+count <= pSource->length);
    pSelf->length = count;
    pSelf->offset = offset;
    pSelf->pBytes = GblBitView_byteFromBit_(pSource, offset);
    return pSelf;
}

GBL_INLINE GblSize GblBitView_bytes(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_BIT_VIEW_BYTES(pSelf->length);
}

GBL_INLINE GblSize GblBitView_words(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_BIT_VIEW_WORDS(pSelf->length);
}

GBL_EXPORT GblBool GblBitView_at(GBL_CSELF, GblSize index) GBL_NOEXCEPT {
    GblBool value = GBL_FALSE;
    if(index < pSelf->length) {
        value = (*GblBitView_byteFromBit_(pSelf, index) >> (index%8)) & 0x1;
    } else {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();
    }
    return value;
}

GBL_INLINE GblSize (GblBitView_count)(GBL_CSELF, GblSize index, GblSize count) GBL_NOEXCEPT {
/* Would've been epic as shit, but doesn't do any good
 * when not on a byte boundary...
    static const uint8_t lut[16] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
    };
*/
    GblSize bits = 0;

    const GblSize start = pSelf->offset + index;
    const GblSize end = start + count;
    if(start/4 < end/4) {
        pSelf->pBytes[start/8] |= GblBitView_msbMaskLut_[start%8];
        for(GblSize i = start/8+1; i < end/8; ++i)
            pSelf->pBytes[i] = 0xff;
        pSelf->pBytes[end/8] |= GblBitView_lsbMaskLut_[end%8];
    } else {
        pSelf->pBytes[start/8] |= (GblBitView_msbMaskLut_[start%8] &
                                   GblBitView_lsbMaskLut_[end%8]);
    }

    return bits;
}

GBL_INLINE GblBool (GblBitView_any)(GBL_CSELF, GblSize index, GblSize count) GBL_NOEXCEPT {
    return GblBitView_count(pSelf, index, count) != 0;
}

GBL_INLINE GblBool (GblBitView_all)(GBL_CSELF, GblSize index, GblSize count) GBL_NOEXCEPT {
    return GblBitView_count(pSelf, index, count) == count;
}

GBL_INLINE GblBool (GblBitView_none)(GBL_CSELF, GblSize index, GblSize count) GBL_NOEXCEPT {
    return GblBitView_count(pSelf, index, count) == 0;
}

GBL_INLINE GblBitView* (GblBitView_assign)(GBL_SELF, GblBool value, GblSize index, GblSize count) GBL_NOEXCEPT {
    if(value) return GblBitView_set(pSelf, index, count);
    return GblBitView_reset(pSelf, index, count);
}

GBL_INLINE GblBitView* (GblBitView_set)(GBL_SELF, GblSize index, GblSize count) GBL_NOEXCEPT {
    const GblSize start = pSelf->offset + index;
    const GblSize end = start + count;
    if(start/8 < end/8) {
        pSelf->pBytes[start/8] |= GblBitView_msbMaskLut_[start%8];
        for(GblSize i = start/8+1; i < end/8; ++i)
            pSelf->pBytes[i] = 0xff;
        pSelf->pBytes[end/8] |= GblBitView_lsbMaskLut_[end%8];
    } else {
        pSelf->pBytes[start/8] |= (GblBitView_msbMaskLut_[start%8] &
                                   GblBitView_lsbMaskLut_[end%8]);
    }
    return pSelf;
}

GBL_INLINE GblBitView* (GblBitView_reset)(GBL_SELF, GblSize index, GblSize count) GBL_NOEXCEPT {
    const GblSize start = pSelf->offset + index;
    const GblSize end = start + count;
    if(start/8 < end/8) {
        pSelf->pBytes[start/8] &= ~GblBitView_msbMaskLut_[start%8];
        for(GblSize i = start/8+1; i < end/8; ++i)
            pSelf->pBytes[i] = 0x00;
        pSelf->pBytes[end/8] &= ~GblBitView_lsbMaskLut_[end%8];
    } else {
        pSelf->pBytes[start/8] &= ~(GblBitView_msbMaskLut_[start%8] &
                                   GblBitView_lsbMaskLut_[end%8]);
    }
    return pSelf;
}

GBL_INLINE GblBitView* (GblBitView_flip)(GBL_SELF, GblSize index, GblSize count) GBL_NOEXCEPT {
    const GblSize start = pSelf->offset + index;
    const GblSize end = start + count;
    if(start/8 < end/8) {
        pSelf->pBytes[start/8] ^= GblBitView_msbMaskLut_[start%8];
        for(GblSize i = start/8+1; i < end/8; ++i)
            pSelf->pBytes[i] ^= 0xff;
        pSelf->pBytes[end/8] ^= GblBitView_lsbMaskLut_[end%8];
    } else {
        pSelf->pBytes[start/8] ^= (GblBitView_msbMaskLut_[start%8] &
                                   GblBitView_lsbMaskLut_[end%8]);
    }
    return pSelf;
}

GBL_INLINE GblBool GblBitView_eq(GBL_CSELF, const GblBitView* pOther) GBL_NOEXCEPT {
#if 0
    const GblSize start = pSelf->offset + index;
    const GblSize end = start + count;
    if(start/8 < end/8) {
        if(start/32 < end/32) {

        } else { // per-byte
            pSelf->pBytes[start/8] ^= GblBitView_msbMaskLut_[start%8];

            for(GblSize i = start/8+1; i < end/8; ++i)
                if(pSelf->pBytes[i] != pOther->pBytes[i]) return;

            pSelf->pBytes[end/8] ^= GblBitView_lsbMaskLut_[end%8];
        }
    } else { // single byte
        pSelf->pBytes[start/8] ^= (GblBitView_msbMaskLut_[start%8] &
                                   GblBitView_lsbMaskLut_[end%8]);
    }
#else
    GblBool result = GBL_TRUE;
    for(GblSize i = 0; i < pSelf->length; ++i)
        if(GblBitView_at(pSelf, i) != GblBitView_at(pOther, i)) {
            result = GBL_FALSE;
            break;
        }
    return result;
#endif
}

GBL_INLINE GblBool GblBitView_lt(GBL_CSELF, const GblBitView* pOther) GBL_NOEXCEPT {
    GblBool result = GBL_FALSE;
    for(GblSize i = 0; i < pSelf->length; ++i) {
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

GBL_INLINE GblBool GblBitView_leq(GBL_CSELF, const GblBitView* pOther) GBL_NOEXCEPT {
    GblBool result = GBL_TRUE;
    for(GblSize i = 0; i < pSelf->length; ++i) {
        const GblBool lvalue = GblBitView_at(pSelf, i);
        const GblBool rvalue = GblBitView_at(pOther, i);
        if(lvalue && !rvalue) {
            result = GBL_FALSE;
            break;
        }
    }
    return result;
}

GBL_INLINE GblBitView* (GblBitView_setString)(GBL_SELF, const char* pString, GblSize index, GblSize count) GBL_NOEXCEPT {
    GBL_ASSERT(strlen(pString) >= count);
    if(!count) count = strlen(pString);
    GBL_ASSERT(index + count < pSelf->length);

    for(GblSize i = 0; i < count; ++i) {
        const char digit = pString[i];
        GBL_ASSERT(digit == '0' || digit == '1');
        const GblBool value = digit == '1'? GBL_TRUE : GBL_FALSE;
        GblBitView_assign(pSelf, value, index+i);
    }

    return pSelf;
}

GBL_EXPORT uint64_t (GblBitView_uint64)(GBL_CSELF, GblSize index, GblSize count) GBL_NOEXCEPT {
    uint64_t value = 0;
    const GblSize end = index + count;
    GBL_ASSERT(end <= pSelf->length);
    for(GblSize b = 0; b < count; ++b) {
        value |= (GblBitView_at(pSelf, index+b) << b);
    }
    return value;
}

GBL_EXPORT GblBitView* (GblBitView_setUint64)(GBL_SELF, uint64_t value, GblSize index, GblSize count) GBL_NOEXCEPT {
    for(GblSize b = 0; b < count; ++b) {
        GblBitView_assign(pSelf, (value>>b)&0x1, index+b);
    }
    return pSelf;
}

#if 0

GBL_EXPORT GBL_RESULT GblBitView_and(GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->length == pValue1->length && pSelf->length == pValue2->length);
    for(GblSize i = 0; i < pSelf->length; ++i) {
        GblBitView_assign(pSelf, GblBitView_at(pValue1, i) & GblBitView_at(pValue2, i), i);
    }
}

GBL_EXPORT GBL_RESULT GblBitView_or(GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->length == pValue1->length && pSelf->length == pValue2->length);
    for(GblSize i = 0; i < pSelf->length; ++i) {
        GblBitView_assign(pSelf, GblBitView_at(pValue1, i) | GblBitView_at(pValue2, i), i);
    }
}

GBL_EXPORT GBL_RESULT GblBitView_xor(GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->length == pValue1->length && pSelf->length == pValue2->length);
    for(GblSize i = 0; i < pSelf->length; ++i) {
        GblBitView_assign(pSelf, GblBitView_at(pValue1, i) ^ GblBitView_at(pValue2, i), i);
    }
}

GBL_EXPORT GBL_RESULT GblBitView_andNot(GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->length == pValue1->length && pSelf->length == pValue2->length);
    for(GblSize i = 0; i < pSelf->length; ++i) {
        GblBitView_assign(pSelf, GblBitView_at(pValue1, i) & ~GblBitView_at(pValue2, i), i);
    }
}
#endif

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_BIT_VIEW_H
