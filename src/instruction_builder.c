#include "../internal/instruction_builder.h"

int create_token_builder(TokenBuilder* b) {
    memset(b, 0, sizeof(TokenBuilder));
    b->mnemonic = b->tokens[0];
    return 0;
}
int build_instruction(TokenBuilder* b, char* buffer) {
    char instruction_buffer[1024];
    memset(instruction_buffer, 0, 1024);

    for (int i = 0; i < b->count; i++) {
        char join_buf[1024];
        memset(join_buf, 0, 1024);
        if (i != 0 && i != b->count - 1) {
            sprintf(join_buf, "%s, ", b->tokens[i]);
        } else {

            sprintf(join_buf, "%s", b->tokens[i]);
        }
        strcat(instruction_buffer, join_buf);
    }
    strncpy(buffer, instruction_buffer, strlen(instruction_buffer));
    return 0;
}

int append_fmt_token(TokenBuilder* b, const char* buf, ...) {
    va_list args;
    va_start(args, buf);
    vsprintf(b->_buffer, buf, args);
    va_end(args);
    append_token(b, b->_buffer);
    memset(b->_buffer, 0, BUFFER_SIZE);
    return 0;
}

int append_token(TokenBuilder* b, char* s) {
    if (b->count >= 8)
        return -1;

    strcpy(b->tokens[b->count], s);
    if (s[0])
        b->count++;
    return 0;
}

int append_register(TokenBuilder* b, reg_t r) {
    if (r > 16 || b->count >= 8)
        return -1;
    build_register_token(r, b->tokens[b->count]);

    b->count++;
    return 0;
}

int append_immediate(TokenBuilder* b, uint32_t value) {
    if (b->count >= 8)
        return -1;

    sprintf(b->tokens[b->count], "#0x%x", value);
    b->count += 1;
    return 0;
}

int append_address_token(TokenBuilder* builder, reg_t rn, uint16_t offset, OpFlags flags) {
    char address_buffer[1024];
    memset(address_buffer, 0, 1024);

    strcat(address_buffer, "[");

    char rn_buffer[1024];
    memset(rn_buffer, 0, 1024);
    build_register_token(rn, rn_buffer);
    strcat(address_buffer, rn_buffer);
    // build offset buffer
    char offset_buffer[32];
    memset(offset_buffer, 0, 32);
    if (flags.I) {
        if (offset) {
            sprintf(offset_buffer, "#%s0x%x", flags.U ? "" : "-", offset);
        }
    } else {
        build_reg_shift_token(offset, offset_buffer);
        if (flags.P) {

            strcat(address_buffer, ", ");
            strcat(address_buffer, flags.U ? "" : "-");
            strcat(address_buffer, offset_buffer);
        }
    }

    strcat(address_buffer, "]");
    strcat(address_buffer, offset && flags.W && flags.P ? "!" : "");
    append_token(builder, address_buffer);

    if (!flags.P)
        append_token(builder, offset_buffer);
    return 0;
}

int append_number(TokenBuilder* b, uint32_t n) {
    sprintf(b->tokens[b->count], "%d", n);
    b->count += 1;
    return 0;
}
int append_proc_number(TokenBuilder* b, uint8_t n) {
    sprintf(b->tokens[b->count], "p%d", n);
    b->count += 1;
    return 0;
}
int append_proc_register(TokenBuilder* b, reg_t r) {
    sprintf(b->tokens[b->count], "c%d", r);
    b->count += 1;
    return 0;
}

int build_register_token(reg_t r, char* s) {
    if (r == 14) {
        strcpy(s, "SP");
    } else if (r == 15)
        strcpy(s, "PC");
    else
        sprintf(s, "R%d", r);
    return 0;
}

int build_reg_shift_token(uint16_t shift_bits, char* s) {
    reg_t rm = shift_bits & 0xF;
    uint8_t shift_type = (shift_bits >> 5) & 0b11;
    flag_t reg_shift_flag = (shift_bits >> 4) & 1;
    char reg_token[8];
    memset(reg_token, 0, 8);
    build_register_token(rm, reg_token);

    char shift_buffer[32];
    memset(shift_buffer, 0, 32);

    if (reg_shift_flag) {
        reg_t rs = (shift_bits >> 8) & 0xF;
        char rs_buffer[8];
        memset(rs_buffer, 0, 8);

        build_register_token(rs, rs_buffer);

        sprintf(shift_buffer, ", %s %s", SHIFT_OP_STRS[shift_type], rs_buffer);

    } else {
        // immediate register offset
        uint8_t imm = (shift_bits >> 7) & 0x1F;
        if (imm)
            sprintf(shift_buffer, ", %s #%d", SHIFT_OP_STRS[shift_type], imm);
    }

    // build full shift token
    strcat(s, reg_token);
    strcat(s, shift_buffer);
    return 0;
}

int combine_stack(reg_t* reg_stack, uint8_t* stk_length, char* s) {
    if (!*stk_length) {
        return 0;
    }
    if (*stk_length == 1) {
        // build_register_token(reg_stack[0], s);
        sprintf(s, "R%d", reg_stack[0]);
    } else if (*stk_length > 1) {
        char reg_buf[16];
        memset(reg_buf, 0, 16);
        sprintf(s, "R%d-R%d", reg_stack[0], reg_stack[*stk_length - 1]);
    }
    *stk_length = 0;
    return 0;
}

int build_register_list(uint16_t reg_list, char* s) {
    assert(s[0] == 0);
    uint8_t reg_buf_size = 0;
    char reg_list_buffers[32][32];
    memset(reg_list_buffers, 0, sizeof(reg_list_buffers));

    reg_t reg_stack[16];
    uint8_t stack_length = 0;

    for (int i = 0; i < 16; ++i) {
        if ((reg_list >> i) & 1) {
            reg_stack[stack_length] = i;
            stack_length++;
        } else {
            if (!stack_length) {
                continue;
            }
            combine_stack(reg_stack, &stack_length, reg_list_buffers[reg_buf_size]);
            stack_length = 0;
            reg_buf_size++;
        }
    }
    if (stack_length) {
        combine_stack(reg_stack, &stack_length, reg_list_buffers[reg_buf_size]);
        reg_buf_size++;
    }
    for (int i = 0; i < reg_buf_size; i++) {
        char join_buf[32];
        memset(join_buf, 0, 16);
        strcat(join_buf, i != 0 ? ", " : "");
        strcat(join_buf, reg_list_buffers[i]);
        strcat(s, join_buf);
    }
    return 0;
}
