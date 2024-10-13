#ifndef SHIFT_OPS_H
#define SHIFT_OPS_H

#include "core.h"

extern int (*shiftOp[4])(uint32_t, uint8_t, uint32_t*);
int SHFT_LSL(uint32_t value, uint8_t shift_amount, uint32_t* result);
int SHFT_LSR(uint32_t value, uint8_t shift_amount, uint32_t* result);
int SHFT_ASR(uint32_t value, uint8_t shift_amount, uint32_t* result);
int SHFT_ROR(uint32_t value, uint8_t shift_amount, uint32_t* result);

#endif