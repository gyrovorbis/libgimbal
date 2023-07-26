/*! \file
 *  \brief GblBitView bitvector view container and related functions
 *  \ingroup utils
 *  \copydoc GblBitView
 *
 *  \author Falco Girgis
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
 *  \ingroup utils
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
    size_t  length;
    uint8_t offset;
} GblBitView;

GBL_EXPORT GblBitView* GblBitView_fromBuffer (GBL_SELF, void* pBuffer, size_t bitCount, size_t offset)         GBL_NOEXCEPT;
GBL_EXPORT GblBitView* GblBitView_fromView   (GBL_SELF, const GblBitView* pView, size_t count, size_t offset)  GBL_NOEXCEPT;

GBL_EXPORT const char* GblBitView_string     (GBL_CSELF, GblStringBuffer* pBuffer, size_t index, size_t count) GBL_NOEXCEPT;
GBL_EXPORT uint64_t    GblBitView_uint64     (GBL_CSELF, size_t index, size_t count)                           GBL_NOEXCEPT;

GBL_EXPORT GblBitView* GblBitView_setString  (GBL_SELF, const char* pString, size_t index, size_t count)       GBL_NOEXCEPT;
GBL_EXPORT GblBitView* GblBitView_setUint64  (GBL_SELF, uint64_t value, size_t index, size_t count)            GBL_NOEXCEPT;

GBL_EXPORT size_t      GblBitView_bytes      (GBL_CSELF)                                                         GBL_NOEXCEPT;
GBL_EXPORT size_t      GblBitView_words      (GBL_CSELF)                                                         GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblBitView_at         (GBL_CSELF, size_t index)                                          GBL_NOEXCEPT;
GBL_EXPORT size_t      GblBitView_count      (GBL_CSELF, size_t index, size_t count)                           GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblBitView_any        (GBL_CSELF, size_t index, size_t count)                           GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblBitView_all        (GBL_CSELF, size_t index, size_t count)                           GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblBitView_none       (GBL_CSELF, size_t index, size_t count)                           GBL_NOEXCEPT;

GBL_EXPORT GblBitView* GblBitView_assign     (GBL_SELF, GblBool value, size_t index, size_t count)             GBL_NOEXCEPT;
GBL_EXPORT GblBitView* GblBitView_set        (GBL_SELF, size_t index, size_t count)                            GBL_NOEXCEPT;
GBL_EXPORT GblBitView* GblBitView_reset      (GBL_SELF, size_t index, size_t count)                            GBL_NOEXCEPT;
GBL_EXPORT GblBitView* GblBitView_flip       (GBL_SELF, size_t index, size_t count)                            GBL_NOEXCEPT;
GBL_EXPORT GblBitView* GblBitView_rotate     (GBL_SELF, ptrdiff_t n, size_t offset, size_t count)              GBL_NOEXCEPT;
GBL_EXPORT GblBitView* GblBitView_shiftLeft  (GBL_SELF, const GblBitView* pSrc, size_t amount)                  GBL_NOEXCEPT;
GBL_EXPORT GblBitView* GblBitView_shiftRight (GBL_SELF, const GblBitView* pSrc, size_t amount)                  GBL_NOEXCEPT;

/* todo
GBL_EXPORT GBL_RESULT  GblBitView_and        (GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBitView_or         (GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBitView_xor        (GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBitView_andNot     (GBL_SELF, const GblBitView* pValue1, const GblBitView* pValue2)    GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblBitView_eq         (GBL_CSELF, const GblBitView* pOther)                               GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblBitView_lt         (GBL_CSELF, const GblBitView* pOther)                               GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblBitView_leq        (GBL_CSELF, const GblBitView* pOther)                               GBL_NOEXCEPT;
*/

// ===== Implementation =====
/// \cond
#define GblBitView_fromBuffer(...)           GblBitView_fromBufferDefault_(__VA_ARGS__)
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

#define GblBitView_fromBufferDefault_(...) \
    GblBitView_fromBufferDefault__(__VA_ARGS__, 0)
#define GblBitView_fromBufferDefault__(self, buffer, count, offset, ...) \
    (GblBitView_fromBuffer)(self, buffer, count, offset)

#define GblBitView_fromViewDefault_(...) \
    GblBitView_fromViewDefault__(__VA_ARGS__, GBL_TUPLE_FIRST(__VA_ARGS__)->length, 0)
#define GblBitView_fromViewDefault__(self, view, count, offset, ...) \
    (GblBitView_fromView)(self, view, count, offset)

#define GblBitView_stringDefault_(...) \
    GblBitView_stringDefault__(__VA_ARGS__, 0, (GBL_TUPLE_FIRST(__VA_ARGS__))->length)
#define GblBitView_stringDefault__(self, buffer, index, count, ...) \
    (GblBitView_string)(self, buffer, index, count)

#define GblBitView_uint64Default_(...) \
    GblBitView_uint64Default__(__VA_ARGS__, 0, (GBL_TUPLE_FIRST(__VA_ARGS__))->length)
#define GblBitView_uint64Default__(self, index, count, ...) \
    (GblBitView_uint64)(self, index, count)

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
//! \endcond

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_BIT_VIEW_H
