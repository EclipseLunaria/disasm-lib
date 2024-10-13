#include "../internal/constants.h"

char* ARM_ALU_OPCODES[] = {"AND", "EOR", "SUB", "RSB", "ADD", "ADC", "SBC", "RSC",
                           "TST", "TEQ", "CMP", "CMN", "ORR", "MOV", "BIC", "MVN"};

char* THUMB_ALU_OPCODES[16] = {"AND", "EOR", "LSL", "LSR", "ASR", "ADC", "SBC", "ROR",
                               "TST", "NEG", "CMP", "CMN", "ORR", "MUL", "BIC", "MVN"};

char* COND_TYPE_STRS[] = {
    "EQ", "NE", "CS", "CC", "MI", "PL", "VS", "VC", "HI", "LS", "GE", "LT", "GT", "LE", "",
};

char* SHIFT_OP_STRS[4] = {"LSL", "LSR", "ASR", "ROR"};

char* STACK_BLOCK_SUFFIXS[8] = {
    "STMED\0", "STMEA\0", "STMFD\0", "STMFA\0", "LDMFA\0", "LDMFD\0", "LDMEA\0", "LDMED\0",
};
char* OTHER_BLOCK_SUFFIXS[8] = {
    "STMDA\0", "STMIA\0", "STMDB\0", "STMIB\0", "LDMDA\0", "LDMIA\0", "LDMDB\0", "LDMIB\0",
};

char* IMMEDIATE_THUMB_OPS[4] = {"MOV", "CMP", "ADD", "SUB"};
