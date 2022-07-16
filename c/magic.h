
#ifndef MAGIC_H
#define MAGIC_H

#include <stdint.h>
#include <stdio.h>

#define DISCRIMINATOR (int64_t) 1 << 31
#define TRUE 1
#define FALSE 0

int64_t bibbidi(int64_t a, int64_t b);
int64_t bobbidi(int64_t a, int64_t b);

uint64_t lZ(int64_t n);

#endif