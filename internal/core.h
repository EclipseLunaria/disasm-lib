#ifndef CORE_H
#define CORE_H

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

typedef int (*decoder_t)(uint32_t, char*);
typedef uint8_t flag_t;
typedef uint8_t reg_t;

typedef struct OpFlags {
    flag_t I : 1;
    flag_t P : 1;
    flag_t U : 1;
    flag_t B : 1;
    flag_t L : 1;
    flag_t W : 1;
    flag_t H : 1;
    flag_t S : 1;
    flag_t N : 1;
    flag_t F : 1;
    flag_t A : 1;
} OpFlags;

typedef struct TokenBuilder {
    char tokens[8][16];
    char _buffer[BUFFER_SIZE];
    int count;
    char* mnemonic;

} TokenBuilder;
#endif