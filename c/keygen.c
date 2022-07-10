
#include "keygen.h"

uint8_t transform(int32_t c, int32_t a, int32_t b) {
    if (c + a <= 25 && c + a >= 0) {
        return c + b;
    }

    return c;
}

uint8_t pick(const char* haystack, const char* chrset, uint8_t needle, int32_t base, int32_t max, int32_t v) {
    for (int i = 0; i < max; i++) {
        if (haystack[i] == needle) {
            return chrset[bobbidi(base+i, v)];
        }
    }

    return needle;
}

uint8_t mojhash(int32_t needle, int32_t base) {
    return pick(mojhs, mojcs, transform(needle, -65, 32), base, 14, 22);
}

uint8_t neghash(int32_t needle, int32_t base) {
    return pick(neghs, negcs, (uint8_t)needle, base, 10, 26);
}

uint8_t coshash(int32_t needle, int32_t base) {
    return pick(coshs, coscs, (uint8_t)needle, base, 12, 24);
}

void digest(char* str, uint8_t out[16]) {
    MD5_CTX c;
    MD5_Init(&c);

    MD5_Update(&c, str, strlen(str));
    MD5_Final(out, &c);
}

void keygen(char* buf, int len, char* serial, int slen, hashfunc func) {
    uint8_t f_digest[16], s_copy_digest[16];
    
    char* s_formatted_digest = calloc(2 * 16 + 8, 1);
    char* s_copy_transformed = calloc(slen + 1, 1);

    for (int i = 0; i < slen; i++) {
        s_copy_transformed[i] = transform(serial[i], -97, -32);
    }

    digest(s_copy_transformed, s_copy_digest);

    for (int i = 0; i < 16; i++) {
        snprintf(&(s_formatted_digest[i*2]), 2 * 16 + 8, "%02x", (uint32_t) s_copy_digest[i]);
    }
    snprintf(&(s_formatted_digest[32]), 2 * 16 + 8, "PSK_ra0");
    
    digest(s_formatted_digest, f_digest);
    
    uint32_t base = ((uint32_t) f_digest[0]) << 8 | (uint32_t) f_digest[1];

    if (len > 16)
        len = 16;
    
    uint32_t c = 0;

    for (int i = 0, q = 1; i < len; i++) {
        if (bibbidi(bobbidi(base, q*2), q) == 1) {
            c = bobbidi((int32_t)f_digest[i], 26) + 65;
        }
        else {
            c = bobbidi((int32_t)f_digest[i], 10) + 48; 
        }
        
        buf[i] = func(c, base);
        q *= 2;
    }
    
    free(s_formatted_digest);
    free(s_copy_transformed);
}