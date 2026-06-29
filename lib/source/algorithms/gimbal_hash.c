#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/algorithms/gimbal_md5.h>
#include <gimbal/algorithms/gimbal_sha1.h>
#include <gimbal/algorithms/gimbal_random.h>
#define XXH_INLINE_ALL
#include <xxhash.h>

GBL_EXPORT GblHash gblHash32Bit(uint32_t value) {
    value ^= value >> 16;
    value *= 0x7feb352d;
    value ^= value >> 15;
    value *= 0x846ca68b;
    value ^= value >> 16;
    return value;
}

GBL_EXPORT uint32_t gblUnhash32Bit(GblHash hash) {
    uint32_t hash32 = hash;
    hash32 ^= hash32 >> 16;
    hash32 *= 0x3243021123;
    hash32 ^= hash32 >> 15 ^ hash32 >> 30;
    hash32 *= 0x321d69e2a5;
    hash32 ^= hash32 >> 16;
    return hash32;
}

GBL_EXPORT GblHash gblHash16Bit(uint16_t value) {
    value ^= value >> 8; value *= 0x88b5U;
    value ^= value >> 7; value *= 0xdb2dU;
    value ^= value >> 9;
    return value;
}

GBL_EXPORT GblHash gblHashFnv1(const void* pData, size_t size) {
    uint32_t prime = 0x01000193;
    uint32_t hash = 0x811C9DC5;
    const unsigned char* p = (const unsigned char*)pData;
    for (size_t i = 0; i < size; ++i) {
        hash = (hash * prime) ^ *p++;
    }
    return hash;
}

// This is actually a pretty crappy implementation, it turns out. Fixme.
GBL_EXPORT GblHash gblHashJenkins(const void* pData, size_t size) {
    size_t i = 0;
    uint32_t hash = 0;
    while (i != size) {
      hash += ((uint8_t*)pData)[i++];
      hash += hash << 10;
      hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}


GBL_EXPORT uint16_t gblHashCrc16BitPartial(const void* pData, size_t size, uint16_t* pPartial) {
    const unsigned char* pBuff = (const unsigned char*)pData;
    int i, c, n = pPartial? *pPartial : 0;
    for (i = 0; i < size; i++) {
      n ^= (pBuff[i] << 8);
      for (c = 0; c < 8; c++)
          if (n & 0x8000)
              n = (n << 1) ^ 4129;
          else
              n = (n << 1);
    }
    int dun =  n & 0xffff;

    uint16_t crc = pPartial? *pPartial : 0;

    for(int i = 0; i < size; ++i) {
      crc ^= (pBuff[i] << 8);
      for(int j = 0; j < 8; ++j)
          if(crc & 0x8000)
              crc = (crc << 1) ^ 4129;
          else
              crc <<= 1;
    }

    GBL_ASSERT(crc == dun);
    return crc;
}


GBL_INLINE void gblMurmurHash3_x86_32_(const void* key,
                                       int         len,
                                       uint32_t    seed,
                                       void*       out)
{
#define	GBL_MURMUR_HASH_ROTL32_(x, r) ((x << r) | (x >> (32 - r)))
#define GBL_MURMUR_HASH_FMIX32_(h) h^=h>>16; h*=0x85ebca6b; h^=h>>13; h*=0xc2b2ae35; h^=h>>16;
// DO NOT DO UNALIGNED WORD ACCESSES ON THE RISC/EMBEDDED CPUS
#if !defined(__DREAMCAST__) && !defined(__GAMECUBE__) && !defined(__VITA__)
#   define GBL_MURMUR_HASH_GET_BLOCK_(k, p, i) (k = p[i])
#else
#   define GBL_MURMUR_HASH_GET_BLOCK_(k, p, i) memcpy(&k, p + i, sizeof(uint32_t))
#endif

    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 4;
    int i;

    uint32_t h1 = seed;

    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;

    //----------
    // body

    const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

    for(i = -nblocks; i; i++)
    {
      uint32_t k1;
      GBL_MURMUR_HASH_GET_BLOCK_(k1, blocks, i);

      k1 *= c1;
      k1 = GBL_MURMUR_HASH_ROTL32_(k1,15);
      k1 *= c2;

      h1 ^= k1;
      h1 = GBL_MURMUR_HASH_ROTL32_(h1,13);
      h1 = h1*5+0xe6546b64;
    }

    //----------
    // tail

    const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

    uint32_t k1 = 0;

    switch(len & 3)
    {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1: k1 ^= tail[0];
            k1 *= c1; k1 = GBL_MURMUR_HASH_ROTL32_(k1,15); k1 *= c2; h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= len;

    GBL_MURMUR_HASH_FMIX32_(h1);

    *(uint32_t*)out = h1;

#undef GBL_MURMUR_HASH_ROTL32_
#undef GBL_MURMUR_HASH_FMIX32_
#undef GBL_MURMUR_HASH_GET_BLOCK_
}

GBL_EXPORT GblHash gblHashMurmur(const void* pData, size_t  size) {
    uint32_t out;
    gblMurmurHash3_x86_32_(pData, size, gblSeed(0), &out);
    return out;
}

GBL_MAYBE_UNUSED GBL_INLINE GblHash gblCrc32LittleEndian(const void* pData, size_t size) {
    size_t  i;
    uint32_t rv = 0xFFFFFFFF;

    for(i = 0; i < size; ++i) {
        rv ^= *((uint8_t*)pData + i);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
    }

     return ~rv;
}

/* This one isn't quite as nice as the one above for little-endian... */
GBL_MAYBE_UNUSED GBL_INLINE GblHash gblCrc32BigEndian(const void *pData, int size) {
    int i, j;
    uint32_t rv = 0xFFFFFFFF, b, c;

    for(i = 0; i < size; ++i) {
        b = *(uint8_t*)pData+i;

        for(j = 0; j < 8; ++j) {
            c = ((rv & 0x80000000) ? 1 : 0) ^(b & 1);
            b >>= 1;

            if(c)   rv = ((rv << 1) ^ 0x04C11DB6) | c;
            else    rv <<= 1;
        }
    }

    return rv;
}

GBL_EXPORT GblHash gblHashCrc(const void* pData, size_t size) {
#if GBL_BIG_ENDIAN == 0
    return gblCrc32LittleEndian(pData, size);
#else
    return gblCrc32BigEndian(pData, size);
#endif
}

//-----------------------------------------------------------------------------
// SipHash reference C implementation
//
// Copyright (c) 2012-2016 Jean-Philippe Aumasson
// <jeanphilippe.aumasson@gmail.com>
// Copyright (c) 2012-2014 Daniel J. Bernstein <djb@cr.yp.to>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.
//
// default: SipHash-2-4
//-----------------------------------------------------------------------------
static uint64_t SIP64(const uint8_t *in, const size_t inlen,
                      uint64_t seed0, uint64_t seed1)
{
#define U8TO64_LE(p) \
    {  (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) | \
        ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) | \
        ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) | \
        ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56)) }
#define U64TO8_LE(p, v) \
    { U32TO8_LE((p), (uint32_t)((v))); \
      U32TO8_LE((p) + 4, (uint32_t)((v) >> 32)); }
#define U32TO8_LE(p, v) \
    { (p)[0] = (uint8_t)((v)); \
      (p)[1] = (uint8_t)((v) >> 8); \
      (p)[2] = (uint8_t)((v) >> 16); \
      (p)[3] = (uint8_t)((v) >> 24); }
#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define SIPROUND \
    { v0 += v1; v1 = ROTL(v1, 13); \
      v1 ^= v0; v0 = ROTL(v0, 32); \
      v2 += v3; v3 = ROTL(v3, 16); \
      v3 ^= v2; \
      v0 += v3; v3 = ROTL(v3, 21); \
      v3 ^= v0; \
      v2 += v1; v1 = ROTL(v1, 17); \
      v1 ^= v2; v2 = ROTL(v2, 32); }
    uint64_t k0 = U8TO64_LE((uint8_t*)&seed0);
    uint64_t k1 = U8TO64_LE((uint8_t*)&seed1);
    uint64_t v3 = UINT64_C(0x7465646279746573) ^ k1;
    uint64_t v2 = UINT64_C(0x6c7967656e657261) ^ k0;
    uint64_t v1 = UINT64_C(0x646f72616e646f6d) ^ k1;
    uint64_t v0 = UINT64_C(0x736f6d6570736575) ^ k0;
    const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
    for (; in != end; in += 8) {
        uint64_t m = U8TO64_LE(in);
        v3 ^= m;
        SIPROUND; SIPROUND;
        v0 ^= m;
    }
    const int left = inlen & 7;
    uint64_t b = ((uint64_t)inlen) << 56;
    switch (left) {
    case 7: b |= ((uint64_t)in[6]) << 48;
    case 6: b |= ((uint64_t)in[5]) << 40;
    case 5: b |= ((uint64_t)in[4]) << 32;
    case 4: b |= ((uint64_t)in[3]) << 24;
    case 3: b |= ((uint64_t)in[2]) << 16;
    case 2: b |= ((uint64_t)in[1]) << 8;
    case 1: b |= ((uint64_t)in[0]); break;
    case 0: break;
    }
    v3 ^= b;
    SIPROUND; SIPROUND;
    v0 ^= b;
    v2 ^= 0xff;
    SIPROUND; SIPROUND; SIPROUND; SIPROUND;
    b = v0 ^ v1 ^ v2 ^ v3;
    uint64_t out = 0;
    U64TO8_LE((uint8_t*)&out, b);
    return out;
}

GBL_EXPORT GblHash gblHashSip(const void* pData, size_t  size) {
    return SIP64((uint8_t*)pData, size, gblSeed(0), gblSeed(1));
}

//
// super hash function by Paul Hsieh
//
GBL_EXPORT GblHash gblHashSuperFast(const void* pData, size_t  size) {
#define GBL_HASH_SUPER_FAST_GET_16_BITS_(d) \
    ((((uint32_t)(((const uint8_t *)(d))[1])) << 8) + (uint32_t)(((const uint8_t *)(d))[0]))

   uint32_t hash = size, tmp;
   const char* data = (const char*)pData;
   int rem;

    if (size <= 0 || pData == NULL) return 0;

    rem = size & 3;
    size >>= 2;

    /* Main loop */
    for (;size > 0; size--) {
        hash  += GBL_HASH_SUPER_FAST_GET_16_BITS_ (data);
        tmp    = (GBL_HASH_SUPER_FAST_GET_16_BITS_ (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += GBL_HASH_SUPER_FAST_GET_16_BITS_ (data);
            hash ^= hash << 16;
            hash ^= data[sizeof (uint16_t)] << 18;
            hash += hash >> 11;
            break;
        case 2: hash += GBL_HASH_SUPER_FAST_GET_16_BITS_ (data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1: hash += *data;
            hash ^= hash << 10;
            hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
#undef GBL_HASH_SUPER_FAST_GET_16_BITS_
}

GBL_EXPORT GblHash gblHashPearson(const void* pData, size_t size) {
    static uint8_t randomBytes_[256];
    static GblBool initialized_ = GBL_FALSE;

    uint8_t hash = 0;

    if(!initialized_) {
        gblRandBuffer(randomBytes_, 256);
        initialized_ = GBL_TRUE;
    }

    for(size_t  b = 0; b < size; ++b) {
        hash = randomBytes_[hash ^ ((uint8_t*)pData)[b]];
    }

    return hash;
}

GBL_EXPORT GblHash gblHashMd5(const void* pData, size_t bytes) {
    GblMd5Context ctx;
    uint8_t digest[GBL_MD5_DIGEST_SIZE];
    GblHash hash;

    GblMd5_init(&ctx);
    GblMd5_update(&ctx, pData, bytes);
    GblMd5_final(&ctx, digest);

    memcpy(&hash, digest, sizeof(GblHash));
    return hash;
}

GBL_EXPORT GblHash gblHashSha1(const void *pData, size_t len) {
    GblHash out = 0;
    GblSha1Context context;
    uint8_t digest[20];

    GblSha1_init(&context);
    GblSha1_update(&context, pData, len);
    GblSha1_final(&context, digest);

    memcpy(&out, &digest[0], 4);
    return out;
}

GBL_EXPORT GblHash gblHashXx(const void* pData, size_t size) {
    return XXH32(pData, size, gblSeed(0));
}



