
#ifndef KEYGEN_H
#define KEYGEN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include "threads.h"
#include "magic.h"

#define SN_MAX 99999999

typedef struct {
    int n_start;
    int key_length;
    int keys_to_gen;
    char* serial;
    int func_idx;
    char* p_buffer;
} thr_keygen_args;

static const char* mojhs = "1234567890ilosabcdefghjkmnpqrtuvwxyz125690IOSZ3478ABCDEFGHJKLMNPQRTUVWXY125690IOSVWZ3478ABCDEFGHJKLMNPQRTUXY";
static const char* mojcs = "abcdefghjkmnpqrtuvwxyz125690IOSZ3478ABCDEFGHJKLMNPQRTUVWXY125690IOSVWZ3478ABCDEFGHJKLMNPQRTUXY";

static const char* neghs = "125690IOSZ3478ABCDEFGHJKLMNPQRTUVWXY125690IOSVWZ3478ABCDEFGHJKLMNPQRTUXY";
static const char* negcs = "3478ABCDEFGHJKLMNPQRTUVWXY125690IOSVWZ3478ABCDEFGHJKLMNPQRTUXY";

static const char* coshs = "125690IOSVWZ3478ABCDEFGHKLJMNPQRTUXY";
static const char* coscs = "3478ABCDEFGHJKLMNPQRTUXY";

typedef uint8_t (*hashfunc)(int32_t, int32_t);

void digest(char* str, uint8_t out[16]);

uint8_t transform(int32_t c, int32_t a, int32_t b);
void keygen(char* buf, int len, char* serial, int slen, hashfunc func);

#ifdef _WIN32
DWORD keygen_thread(void* args);
#else 
void* keygen_thread(void* args);
#endif


uint8_t pick(const char* haystack, const char* chrset, uint8_t needle, int32_t base, int32_t max, int32_t v);

uint8_t mojhash(int32_t needle, int32_t base);
uint8_t neghash(int32_t needle, int32_t base);
uint8_t coshash(int32_t needle, int32_t base);

#endif