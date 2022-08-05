#include <gimbal/types/gimbal_uuid.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/strings/gimbal_string_view.h>
#include <gimbal/strings/gimbal_string.h>

GBL_EXPORT GBL_RESULT GblUuid_initNil(GblUuid* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    memset(pSelf, 0, sizeof(GblUuid));
    GBL_API_END();
}

// Implemented per RFC4122 section 4.4
GBL_EXPORT GBL_RESULT GblUuid_initV4(GblUuid* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);

    // First initialize all bytes to random/psuedo-random numbers
    for(unsigned b = 0; b < GBL_UUID_BYTE_COUNT; ++b) {
        pSelf->bytes[b] = gblRandRange(0, 255);
    }

    // clear and set version number, per RFC 4122 section 4.1.3
    pSelf->private_.time_hi_and_version &= 0x0f;
    pSelf->private_.time_hi_and_version |= 0x4 << 4;

    // clear and set two most significant bits to 0 and 1
    pSelf->private_.clock_seq_hi_and_reserved &= 0x3f;
    pSelf->private_.clock_seq_hi_and_reserved |= 0x80;

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblUuid_toString(const GblUuid* pSelf, char* pStrBuffer) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY(sprintf(pStrBuffer, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x"
                                       "-%02x%02x%02x%02x%02x%02x",
                                       pSelf->bytes[ 0], pSelf->bytes[ 1], pSelf->bytes[ 2], pSelf->bytes[ 3],
                                       pSelf->bytes[ 4], pSelf->bytes[ 5], pSelf->bytes[ 6], pSelf->bytes[ 7],
                                       pSelf->bytes[ 8], pSelf->bytes[ 9], pSelf->bytes[10], pSelf->bytes[11],
                                       pSelf->bytes[12], pSelf->bytes[13], pSelf->bytes[14], pSelf->bytes[15])
                   == GBL_UUID_STRING_LENGTH,
                   GBL_RESULT_ERROR_UNDERFLOW);
    GBL_API_END();

}

GBL_EXPORT GBL_RESULT GblUuid_initFromString(GblUuid* pSelf, const char* pStrBuffer) {
    GBL_API_BEGIN(NULL);

    GBL_API_VERIFY_CALL(GblUuid_initNil(pSelf));

    GblStringView strv = GblStringView_fromString(pStrBuffer);

    GBL_API_VERIFY(strv.length == GBL_UUID_STRING_LENGTH,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);

    for(GblSize b = 0, c = 0;
        b < GBL_UUID_BYTE_COUNT && c < strv.length;)
    {
        if(c == 8 || c == 13 || c == 18 || c == 23) {
            GBL_API_VERIFY(GblStringView_at(strv, c++) == '-',
                           GBL_RESULT_ERROR_INVALID_ARG);
            continue;
        }

        int high    = gblAsciiXDigitValue(GblStringView_at(strv, c++));
        int low     = gblAsciiXDigitValue(GblStringView_at(strv, c++));

        GBL_API_VERIFY(high != -1,
                       GBL_RESULT_ERROR_INVALID_ARG);

        GBL_API_VERIFY(low != -1,
                       GBL_RESULT_ERROR_INVALID_ARG);

        pSelf->bytes[b++] = (high << 4) | low;

    }

    GBL_API_END();
}

GBL_EXPORT unsigned GblUuid_version(const GblUuid* pSelf) {
    unsigned version = 0;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    version = ((pSelf->private_.time_hi_and_version >> 4) & 0xf);
    GBL_API_END_BLOCK();
    return version;
}

GBL_EXPORT int GblUuid_compare(const GblUuid* pSelf, const GblUuid* pOther) {
#   define CHECK(f1, f2) if (f1 != f2) { result =  f1 < f2 ? -1 : 1; GBL_API_DONE(); }

    int result = -1;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);

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
    GBL_API_END_BLOCK();
    return result;

#   undef CHECK
}

GBL_EXPORT GblBool GblUuid_isNil(const GblUuid* pSelf) {
    GblBool isNil = GBL_TRUE;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    for(unsigned b = 0; b < GBL_UUID_BYTE_COUNT; ++b) {
        if(pSelf->bytes[b] != 0) {
            isNil = GBL_FALSE;
            break;
        }
    }
    GBL_API_END_BLOCK();
    return isNil;
}
