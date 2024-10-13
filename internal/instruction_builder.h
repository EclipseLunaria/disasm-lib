#ifndef TOKEN_BUILDER_H
#define TOKEN_BUILDER_H

#include "core.h"


int create_token_builder(TokenBuilder* b);
int build_instruction(TokenBuilder* b, char* buffer);

int append_token(TokenBuilder* b, char* s);
int append_fmt_token(TokenBuilder* b, const char* buf, ...);
int append_register(TokenBuilder* b, reg_t r);
int append_immediate(TokenBuilder* b, uint32_t value);
int append_address_token(TokenBuilder* builder, reg_t rn, uint16_t offset, OpFlags flags);

int append_number(TokenBuilder* b, uint32_t n);
int append_proc_number(TokenBuilder* b, uint8_t n);
int append_proc_register(TokenBuilder* b, reg_t r);

int build_register_token(reg_t r, char* s);
int build_reg_shift_token(uint16_t shift_bits, char* s);
int build_register_list(uint16_t reg_list, char* s);

#endif