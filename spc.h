#include <inttypes.h>

#define U32TO8_LE(p, v)            \
    (p)[0] = (uint8_t)((v));       \
    (p)[1] = (uint8_t)((v) >> 8);  \
    (p)[2] = (uint8_t)((v) >> 16); \
    (p)[3] = (uint8_t)((v) >> 24);

#define U64TO8_LE(p, v)              \
    U32TO8_LE((p), (uint32_t)((v))); \
    U32TO8_LE((p) + 4, (uint32_t)((v) >> 32));

#define U8TO64_LE(p) \
    (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) | ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) | ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) | ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56))

#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define SIPROUND                \
    do {                        \
        v0 += v1;               \
        v1 = v0 ^ ROTL(v1, 13); \
        v0 = ROTL(v0, 32);      \
        v2 += v3;               \
        v3 = v2 ^ ROTL(v3, 16); \
        v0 += v3;               \
        v3 = v0 ^ ROTL(v3, 21); \
        v2 += v1;               \
        v1 = v2 ^ ROTL(v1, 17); \
        v2 = ROTL(v2, 32);      \
    } while (0)

#define SHA4(V)                             \
    do {                                    \
        X = V;                              \
        v0 = 0x50726f736563636f ^ k0;       \
        v1 = 0x43686f636f6c6174 ^ k1;       \
        v2 = 0x01f32d1f4361f48e ^ k0;       \
        v3 = ((uint64_t)i << 56) ^ tw ^ k1; \
        v3 ^= X;                            \
        SIPROUND;                           \
        v0 ^= X;                            \
        v2 ^= 16;                           \
        SIPROUND;                           \
        SIPROUND;                           \
        X = v0 ^ v1 ^ v2 ^ v3;              \
    } while (0)

#define SIGMA                            \
    do {                                 \
        X = L & 0xffffffffull;           \
        L = (X << 32) | ((L >> 32) ^ X); \
    } while (0)

#define UNSIGMA                        \
    do {                               \
        X = (L >> 32) & 0xffffffffull; \
        L = ((L ^ X) << 32) | X;       \
    } while (0)

void encrypt(const uint8_t* in, uint8_t* out, const uint8_t* key, const uint8_t* tweak)
{

    uint64_t L = U8TO64_LE(in);
    uint64_t R = U8TO64_LE(in + 8);
    uint64_t k0 = U8TO64_LE(key);
    uint64_t k1 = U8TO64_LE(key + 8);
    uint64_t tw = U8TO64_LE(tweak);
    uint64_t v0, v1, v2, v3, X;

    for (int i = 0; i < 4; ++i) {

        SHA4(L ^ R);
        L ^= X;
        R ^= X;
        SIGMA;
    }

    U64TO8_LE(out, L);
    U64TO8_LE(out + 8, R);
}

void decrypt(const uint8_t* in, uint8_t* out, const uint8_t* key, const uint8_t* tweak)
{

    uint64_t L = U8TO64_LE(in);
    uint64_t R = U8TO64_LE(in + 8);
    uint64_t k0 = U8TO64_LE(key);
    uint64_t k1 = U8TO64_LE(key + 8);
    uint64_t tw = U8TO64_LE(tweak);
    uint64_t v0, v1, v2, v3, X;

    for (int i = 4 - 1; i >= 0; --i) {

        UNSIGMA;
        SHA4(L ^ R);
        L ^= X;
        R ^= X;
    }

    U64TO8_LE(out, L);
    U64TO8_LE(out + 8, R);
}
