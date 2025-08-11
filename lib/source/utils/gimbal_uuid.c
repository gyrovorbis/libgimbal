#include <gimbal/utils/gimbal_uuid.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/strings/gimbal_string_view.h>
#include <gimbal/strings/gimbal_string.h>
#include <gimbal/algorithms/gimbal_random.h>

GBL_EXPORT GBL_RESULT GblUuid_setNil(GblUuid* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);
    memset(pSelf, 0, sizeof(GblUuid));
    GBL_CTX_END();
}

// Implemented per RFC4122 section 4.4
GBL_EXPORT GBL_RESULT GblUuid_genV4(GblUuid* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);

    // First initialize all bytes to random/psuedo-random numbers
    for(unsigned b = 0; b < GBL_UUID_BYTE_COUNT; ++b) {
        pSelf->bytes[b] = gblRandEquilikely(0, 255);
    }

    // clear and set version number, per RFC 4122 section 4.1.3
    pSelf->private_.time_hi_and_version &= 0x0f;
    pSelf->private_.time_hi_and_version |= 0x4 << 4;

    // clear and set two most significant bits to 0 and 1
    pSelf->private_.clock_seq_hi_and_reserved &= 0x3f;
    pSelf->private_.clock_seq_hi_and_reserved |= 0x80;

    GBL_CTX_END();
}

GBL_EXPORT const char* GblUuid_string(const GblUuid* pSelf, char* pStrBuffer) {
    const char* pStr = NULL;
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY(sprintf(pStrBuffer, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x"
                                       "-%02x%02x%02x%02x%02x%02x",
#ifndef GBL_BIG_ENDIAN
                                       pSelf->bytes[ 0], pSelf->bytes[ 1], pSelf->bytes[ 2], pSelf->bytes[ 3],
                                       pSelf->bytes[ 4], pSelf->bytes[ 5], pSelf->bytes[ 6], pSelf->bytes[ 7],
                                       pSelf->bytes[ 8], pSelf->bytes[ 9], pSelf->bytes[10], pSelf->bytes[11],
                                       pSelf->bytes[12], pSelf->bytes[13], pSelf->bytes[14], pSelf->bytes[15])
#else
                                       pSelf->bytes[ 0], pSelf->bytes[ 1], pSelf->bytes[ 2], pSelf->bytes[ 3],
                                       pSelf->bytes[ 4], pSelf->bytes[ 5], pSelf->bytes[ 6], pSelf->bytes[ 7],
                                       pSelf->bytes[ 8], pSelf->bytes[ 9], pSelf->bytes[10], pSelf->bytes[11],
                                       pSelf->bytes[12], pSelf->bytes[13], pSelf->bytes[14], pSelf->bytes[15])
#endif
                   == GBL_UUID_STRING_LENGTH,
                   GBL_RESULT_ERROR_UNDERFLOW);
    pStr = pStrBuffer;
    GBL_CTX_END_BLOCK();
    return pStr;
}

GBL_EXPORT GBL_RESULT GblUuid_parse(GblUuid* pSelf, const char* pStrBuffer) {
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_CALL(GblUuid_setNil(pSelf));

    GblStringView strv = GblStringView_fromString(pStrBuffer);

    GBL_CTX_VERIFY(strv.length == GBL_UUID_STRING_LENGTH,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);

    for(size_t  b = 0, c = 0;
        b < GBL_UUID_BYTE_COUNT && c < strv.length;)
    {
        if(c == 8 || c == 13 || c == 18 || c == 23) {
            GBL_CTX_VERIFY(GblStringView_at(strv, c++) == '-',
                           GBL_RESULT_ERROR_INVALID_ARG);
            continue;
        }

        const int high = gblAsciiHexDigitValue(GblStringView_at(strv, c++));
        const int low  = gblAsciiHexDigitValue(GblStringView_at(strv, c++));

        GBL_CTX_VERIFY(high != -1,
                       GBL_RESULT_ERROR_INVALID_ARG);

        GBL_CTX_VERIFY(low != -1,
                       GBL_RESULT_ERROR_INVALID_ARG);

        pSelf->bytes[b++] = (high << 4) | low;

    }

#if GBL_BIG_ENDIAN
    pSelf->private_.time_low
        = GBL_BSWAP_U32(pSelf->private_.time_low);
    pSelf->private_.time_mid
        = GBL_BSWAP_U16(pSelf->private_.time_mid);
    pSelf->private_.time_hi_and_version
        = GBL_BSWAP_U16(pSelf->private_.time_hi_and_version);
#endif

    GBL_CTX_END();
}

GBL_EXPORT unsigned GblUuid_version(const GblUuid* pSelf) {
    unsigned version = 0;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);
    version = ((pSelf->private_.time_hi_and_version >> 4) & 0xf);
    GBL_CTX_END_BLOCK();
    return version;
}

GBL_EXPORT int GblUuid_compare(const GblUuid* pSelf, const GblUuid* pOther) {
#   define CHECK(f1, f2) if (f1 != f2) { result =  f1 < f2 ? -1 : 1; goto done; }

    int result = -1;
    if(!pSelf || !pOther) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY_POINTER(pSelf);
        GBL_CTX_VERIFY_POINTER(pOther);
        GBL_CTX_END_BLOCK();
    } else {

        CHECK(pSelf->private_.time_low,                  pOther->private_.time_low);
        CHECK(pSelf->private_.time_mid,                  pOther->private_.time_mid);
        CHECK(pSelf->private_.time_hi_and_version,       pOther->private_.time_hi_and_version);
        CHECK(pSelf->private_.clock_seq_hi_and_reserved, pOther->private_.clock_seq_hi_and_reserved);
        CHECK(pSelf->private_.clock_seq_low,             pOther->private_.clock_seq_low);

        for(unsigned b = 0; b < GBL_UUID_NODE_COUNT; ++b) {
            if(pSelf->private_.node[b] < pOther->private_.node[b])
                return -1;
            if(pSelf->private_.node[b] > pOther->private_.node[b])
                return 1;
        }
        result = 0;
    }
    done:
    return result;

#   undef CHECK
}

GBL_EXPORT GblBool GblUuid_isNil(const GblUuid* pSelf) {
    GblBool isNil = GBL_TRUE;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);
    for(unsigned b = 0; b < GBL_UUID_BYTE_COUNT; ++b) {
        if(pSelf->bytes[b] != 0) {
            isNil = GBL_FALSE;
            break;
        }
    }
    GBL_CTX_END_BLOCK();
    return isNil;
}
