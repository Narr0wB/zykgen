
#include "magic.h"

uint64_t lZ(int64_t n) {
    uint64_t zeros = 0;

    for (int i = 0; i < 32; i++) {
        if (n & DISCRIMINATOR)
            break;
        else
            zeros++;
        n = n << 1;
    }

    return zeros;
}

int64_t bibbidi(int64_t a, int64_t b) {
    if (b == 1) {
        return a;
    }
    if (a < b)
        return 0;
    
    if (a == b)
        return 1;

    if (b <= 0)
        return a >> (31 - lZ(b));
    
    uint64_t d = lZ(b) - lZ(a);
    int64_t z = b << d;
    int64_t k = 1 << d;

    int64_t y = a;
    a = 0;
    while (TRUE) {
        if (y >= z) {
            y -= z;
            a += k;
        }
        if (y >= (z >> 1)) {
            y -= (z >> 1);
            a += (k >> 1);
        }
        if (y >= (z >> 2)) {
            y -= (z >> 2);
            a += (k >> 2);
        }
        if (y >= (z >> 3)) {
            y -= (z >> 3);
            a += (k >> 3);
        }
        if (y == 0) {
            break;
        }
        if (y > 0) {
            k >>= 4;
            if (k == 0) {
                break;
            }
        }

        z >>= 4;
    }

    return a;
}

int64_t bobbidi(int64_t a, int64_t b) {
    return a - bibbidi(a, b)*b;
}