#ifndef DECODER_THUMB_H
#define DECODER_THUMB_H

#include <stdint.h>

#define IS_MOVE_SHIFTED_REGISTER(x) ((x & 0xE000) == 0 && ((x >> 11) & 0b11) != 0b11)
#define IS_ADD_SUBTRACT(x) ((x & 0xF800) == 0x1800)
#define IS_IMMEDIATE_OP(x) (x & 0xE000) == 0x2000
#define IS_ALU_OPERATION(x) (x & 0xFC00) == 0x4000
#define IS_BX_HI_REG_OP(x) (x & 0xFC00) == 0x4400
#define IS_PC_RELATIVE_LOAD(x) (x & 0xF800) == 0x4800
#define IS_TRANSFER_REGISTER_OFFSET(x) (x & 0xF200) == 0x5000
#define IS_TRANSFER_SIGNED_BYTE_HALFWORD(x) (x & 0xF200) == 0x5200
#define IS_TRANSFER_IMMEDIATE_OFFSET(x) (x & 0xE000) == 0x6000
#define IS_TRANSFER_HALFWORD(x) (x & 0xF000) == 0x8000
#define IS_SP_RELATIVE_TRANSFER(x) (x & 0xF000) == 0x9000
#define IS_LOAD_ADDRESS(x) (x & 0xF000) == 0xA000
#define IS_ADD_STACK_POINTER_OFFSET(x) (x & 0xFF00) == 0xA000
#define IS_PUSH_POP_REGISTERS(x) (x & 0xF600) == 0xB400
#define IS_MULTIPLE_TRANSFER(x) (x & 0xF) == 0xC000
#define IS_CONDITIONAL_BRANCH(x) (x & 0xF) == 0xD000
#define IS_THUMB_SOFTWARE_INTERRUPT(x) (x & 0xFF00) == 0xDF00
#define IS_UNCONDITIONAL_BRANCH(x) (x & 0xF800) == 0xE000
#define IS_LONG_BRANCH_WITH_LINK(x) (x & 0xF000) == 0xF000


int decode_thumb_move_shift_register(uint16_t instruction, char* s);
int decode_thumb_add_subtract(uint16_t instruction, char* s);
int decode_thumb_immediate_operation(uint16_t instruction, char* s);
int decode_thumb_alu_operation(uint16_t instruction, char* s);
int decode_thumb_hi_reg_branch_exchange(uint16_t instruction, char* s);
int decode_thumb_pc_relative_load(uint16_t instruction, char* s);
int decode_thumb_conditional_branch(uint16_t instruction, char* s);
int decode_thumb_unconditional_branch(uint16_t instruction, char* s);
int decode_thumb_transfer_register_offset(uint16_t instruction, char* s);
int decode_thumb_transfer_immediate_offset(uint16_t instruction, char* s);
int decode_thumb_transfer_signed_byte_halfword(uint16_t instruction, char* s);
int decode_thumb_transfer_halfword(uint16_t instruction, char* s);
int decode_thumb_sp_relative_transfer(uint16_t instruction, char* s);
int decode_thumb_load_address(uint16_t instruction, char* s);
int decode_thumb_add_offset_to_stack_pointer(uint16_t instruction, char* s);
int decode_thumb_block_transfer(uint16_t instruction, char* s);
int decode_thumb_software_interrupt(uint16_t instruction, char* s);
int decode_thumb_push_pop_registers(uint16_t instruction, char* s);
int decode_thumb_long_branch_with_link(uint16_t instruction, char* s);
#endif
