#include "../internal/shift_ops.h"
int (*shiftOp[4])(uint32_t, uint8_t, uint32_t*) = {SHFT_LSL, SHFT_LSR, SHFT_ASR, SHFT_ROR};

int SHFT_LSL(uint32_t value, uint8_t shift_amount, uint32_t* result) {
    if (shift_amount == 0) {
        *result = value;
        return 0;
    }
    *result = shift_amount < 32 ? value << shift_amount : 0;
    return 0;
}

int SHFT_LSR(uint32_t value, uint8_t shift_amount, uint32_t* result) {
    if (shift_amount == 0) {
        *result = value;
        return 0;
    }
    *result = shift_amount < 32 ? value >> shift_amount : 0;
    return 0;
}

int SHFT_ASR(uint32_t value, uint8_t shift_amount, uint32_t* result) {
    uint32_t msb = value & 0x80000000;
    if (shift_amount >= 32) {
        *result = msb ? 0xFFFFFFFF : 0;
        return 0;
    }
    *result = value >> shift_amount;

    if (msb) {
        uint32_t mask = (1 << (32 - shift_amount)) - 1;
        *result |= ~mask;
    }

    return 0;
}

int SHFT_ROR(uint32_t value, uint8_t shift_amount, uint32_t* result) {
    shift_amount %= 32;
    *result = (value >> shift_amount) | (value << (32 - shift_amount));
    return 0;
}
