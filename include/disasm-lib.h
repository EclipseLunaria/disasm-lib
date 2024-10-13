#ifndef DISASM_LIB_H
#define DISASM_LIB_H

#include <stdbool.h>
int decode_arm_instruction(uint32_t instruction, char * buffer);
int decode_thumb_instruction(uint32_t instruction, char * buffer);



#endif
