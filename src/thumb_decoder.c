#include "thumb_decoder.h"
#include "instruction_builder.h"

int decode_thumb_instruction(uint16_t instruction, char* s) {
    if (IS_MOVE_SHIFTED_REGISTER(instruction)) {
	decode_thumb_move_shift_register(instruction, s);
        return 0;
    }
    if (IS_ADD_SUBTRACT(instruction)) {
        decode_thumb_add_subtract(instruction, s);
        return 0;
    }
    if (IS_IMMEDIATE_OP(instruction)) {
        decode_thumb_immediate_operation(instruction, s);
        return 0;
    }
    if (IS_ALU_OPERATION(instruction)) {
        decode_thumb_alu_operation(instruction, s);
        return 0;
    }
    if (IS_BX_HI_REG_OP(instruction)) {
        decode_thumb_hi_reg_branch_exchange(instruction, s);
        return 0;
    }
    if (IS_PC_RELATIVE_LOAD(instruction)) {
        decode_thumb_pc_relative_load(instruction, s);
        return 0;
    }
    if (IS_TRANSFER_REGISTER_OFFSET(instruction)) {
        decode_thumb_transfer_register_offset(instruction, s);
        return 0;
    }
    if (IS_TRANSFER_SIGNED_BYTE_HALFWORD(instruction)) {
        decode_thumb_transfer_signed_byte_halfword(instruction, s);
        return 0;
    }
    if (IS_TRANSFER_IMMEDIATE_OFFSET(instruction)) {
        decode_thumb_transfer_immediate_offset(instruction, s);
        return 0;
    }
    if (IS_TRANSFER_HALFWORD(instruction)) {
        decode_thumb_transfer_halfword(instruction, s);
        return 0;
    }
    if (IS_SP_RELATIVE_TRANSFER(instruction)) {
        decode_thumb_sp_relative_transfer(instruction, s);
        return 0;
    }
    if (IS_LOAD_ADDRESS(instruction)) {
        decode_thumb_load_address(instruction, s);
        return 0;
    }
    if (IS_ADD_STACK_POINTER_OFFSET(instruction)) {
        decode_thumb_add_offset_to_stack_pointer(instruction, s);
        return 0;
    }
    if (IS_PUSH_POP_REGISTERS(instruction)) {
        decode_thumb_push_pop_registers(instruction, s);
        return 0;
    }
    if (IS_MULTIPLE_TRANSFER(instruction)) {
        decode_thumb_block_transfer(instruction, s);
        return 0;
    }
    if (IS_CONDITIONAL_BRANCH(instruction)) {
        decode_thumb_conditional_branch(instruction, s);
        return 0;
    }
    if (IS_THUMB_SOFTWARE_INTERRUPT(instruction)) {
        decode_thumb_software_interrupt(instruction, s);
        return 0;
    }
    if (IS_UNCONDITIONAL_BRANCH(instruction)) {
        decode_thumb_unconditional_branch(instruction, s);
        return 0;
    }
    if (IS_LONG_BRANCH_WITH_LINK(instruction)) {
        decode_thumb_long_branch_with_link(instruction, s);
        return 0;
    }

    strcpy(s, "UNDEFINED");
    return 0;
}

int decode_thumb_move_shift_register(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);
    uint8_t op = (instruction >> 11) & 0b11;
    uint8_t offset5 = (instruction >> 6) & 0x1F;
    reg_t rs = (instruction >> 3) & 0b111;
    reg_t rd = instruction & 0b111;

    append_fmt_token(&builder, "%s ", SHIFT_OP_STRS[op]);

    append_register(&builder, rs);
    append_register(&builder, rd);

    append_immediate(&builder, offset5);
    build_instruction(&builder, s);
    return 0;
}

int decode_thumb_add_subtract(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    flag_t I = (instruction >> 10) & 1;
    flag_t op = (instruction >> 10) & 1;
    uint8_t rn_offset3 = (instruction >> 6) & 0b111;
    reg_t rs = (instruction >> 3) & 0b111;
    reg_t rd = instruction & 0b111;
    append_token(&builder, op ? "SUB " : "ADD ");
    append_register(&builder, rd);
    append_register(&builder, rs);
    I ? append_immediate(&builder, rn_offset3) : append_register(&builder, rn_offset3);

    build_instruction(&builder, s);
    return 0;
}

int decode_thumb_immediate_operation(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t op = (instruction >> 11) & 0b11;
    reg_t rd = (instruction >> 8) & 0b111;
    uint8_t offset8 = instruction & 0xFF;

    append_fmt_token(&builder, "%s ", IMMEDIATE_THUMB_OPS[op]);

    append_register(&builder, rd);
    append_immediate(&builder, offset8);

    build_instruction(&builder, s);
    return 0;
}

int decode_thumb_alu_operation(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t op = (instruction >> 6) & 0xF;
    reg_t rs = (instruction >> 3) & 0b111;
    reg_t rd = instruction & 0b111;

    append_fmt_token(&builder, "%s ", THUMB_ALU_OPCODES[op]);

    append_register(&builder, rd);
    append_register(&builder, rs);

    build_instruction(&builder, s);

    return 0;
}

int decode_thumb_hi_reg_branch_exchange(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t op = (instruction >> 8) & 0b11;
    flag_t H1 = (instruction >> 7) & 1;
    flag_t H2 = (instruction >> 6) & 1;
    reg_t rs_hs = (instruction >> 3) & 0b111;
    reg_t rd_hd = instruction & 0b111;

    append_fmt_token(&builder, "%s ", op < 0b11 ? IMMEDIATE_THUMB_OPS[op] : "BX");
    append_register(&builder, H1 ? rd_hd + 8 : rd_hd);
    append_register(&builder, H2 ? rs_hs + 8 : rs_hs);

    build_instruction(&builder, s);

    return 0;
}

int decode_thumb_pc_relative_load(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    reg_t rd = (instruction >> 8) & 0b111;
    uint8_t word8 = instruction & 0xFF;

    append_token(&builder, "LDR ");
    append_register(&builder, rd);

    OpFlags flags = {
        .I = 1,
        .U = 1,
    };
    append_address_token(&builder, 15, word8, flags);
    build_instruction(&builder, s);
    return 0;
}

int decode_thumb_transfer_register_offset(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    flag_t L = (instruction >> 11) & 1;
    flag_t B = (instruction >> 10) & 1;
    reg_t ro = (instruction >> 6) & 0b111;
    reg_t rb = (instruction >> 3) & 0b111;
    reg_t rd = instruction & 0b111;

    append_fmt_token(&builder, "%s%s ", L ? "LDR" : "STR", B ? "B" : "");
    append_register(&builder, rd);
    append_fmt_token(&builder, "[R%d, R%d]", rb, ro);
    build_instruction(&builder, s);
    return 0;
}

int decode_thumb_transfer_immediate_offset(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    flag_t B = (instruction >> 12) & 1;
    flag_t L = (instruction >> 11) & 1;
    reg_t offset5 = (instruction >> 6) & 0x1F;
    reg_t rb = (instruction >> 3) & 0b111;
    reg_t rd = instruction & 0b111;

    append_fmt_token(&builder, "%s%s ", L ? "LDR" : "STR", B ? "B" : "");
    append_register(&builder, rd);
    append_fmt_token(&builder, "[R%d, #0x%x]", rb, offset5);

    build_instruction(&builder, s);

    return 0;
}

int decode_thumb_transfer_signed_byte_halfword(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    flag_t H = (instruction >> 11) & 1;
    flag_t S = (instruction >> 10) & 1;
    reg_t ro = (instruction >> 6) & 0b111;
    reg_t rb = (instruction >> 3) & 0b111;
    reg_t rd = instruction & 0b111;

    char mnemonics[4][5] = {"STRH", "LDRH", "LDSB", "LDSH"};
    append_fmt_token(&builder, "%s ", mnemonics[S << 1 | H]);
    append_register(&builder, rd);
    append_fmt_token(&builder, "[R%d, R%d]", rb, ro);
    build_instruction(&builder, s);
    return 0;
}

int decode_thumb_transfer_halfword(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    flag_t L = (instruction >> 11) & 1;
    reg_t offset5 = (instruction >> 6) & 0x1F;
    reg_t rb = (instruction >> 3) & 0b111;
    reg_t rd = instruction & 0b111;

    append_token(&builder, L ? "LDRH " : "STRH ");
    append_register(&builder, rd);
    append_fmt_token(&builder, "[R%d, #0x%x]", rb, offset5);

    build_instruction(&builder, s);

    return 0;
}

int decode_thumb_sp_relative_transfer(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    flag_t L = (instruction >> 11) & 1;
    reg_t rd = (instruction >> 8) & 0b111;
    uint8_t word8 = instruction & 0xFF;
    append_token(&builder, L ? "LDR " : "STR ");

    append_register(&builder, rd);
    append_fmt_token(&builder, "[SP, #0x%x]", word8);

    build_instruction(&builder, s);

    return 0;
}

int decode_thumb_load_address(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    flag_t SP = (instruction >> 11) & 1;
    reg_t rd = (instruction >> 8) & 0b111;
    uint8_t word8 = instruction & 0xFF;
    append_token(&builder, "ADD ");
    append_token(&builder, SP ? "SP" : "PC");
    append_register(&builder, rd);
    append_fmt_token(&builder, "#%s0x%x", SP ? "-" : "", word8);

    build_instruction(&builder, s);

    return 0;
}

int decode_thumb_add_offset_to_stack_pointer(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    flag_t S = (instruction >> 7) & 1;
    uint8_t word8 = instruction & 0x7F;
    append_token(&builder, "ADD ");
    append_token(&builder, "SP");
    append_fmt_token(&builder, "#%s0x%x", S ? "-" : "", word8);

    build_instruction(&builder, s);

    return 0;
}

int decode_thumb_push_pop_registers(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    flag_t L = (instruction >> 11) & 1;
    flag_t R = (instruction >> 8) & 1;
    reg_t rlist = (instruction >> 6) & 0xF;

    append_token(&builder, L ? "PUSH " : "POP ");

    char list_buffer[BUFFER_SIZE];
    memset(list_buffer, 0, BUFFER_SIZE);
    build_register_list((uint16_t)rlist, list_buffer);
    append_fmt_token(&builder, "{%s%s%s}", list_buffer, R && L ? ", PC" : "",
                     R && !L ? ", LR" : "");
    build_instruction(&builder, s);

    return 0;
}

int decode_thumb_block_transfer(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    flag_t L = (instruction >> 11) & 1;
    reg_t rb = (instruction >> 8) & 0b111;
    reg_t rlist = instruction & 0xFF;

    append_token(&builder, L ? "LDMIA " : "STMIA ");

    append_fmt_token(&builder, "R%d!", rb);

    char list_buffer[BUFFER_SIZE];
    memset(list_buffer, 0, BUFFER_SIZE);
    build_register_list(rlist, list_buffer);

    append_token(&builder, list_buffer);

    build_instruction(&builder, s);
    return 0;
}
int decode_thumb_conditional_branch(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t cond = (instruction >> 8) & 0xF;
    uint8_t soffset8 = instruction & 0xFF;

    append_fmt_token(&builder, "B%s ", COND_TYPE_STRS[cond]);
    append_immediate(&builder, soffset8);
    build_instruction(&builder, s);
    return 0;
}

int decode_thumb_software_interrupt(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t value8 = instruction & 0xFF;
    append_token(&builder, "SWI ");
    append_immediate(&builder, value8);
    build_instruction(&builder, s);
    return 0;
}
int decode_thumb_unconditional_branch(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t offset11 = instruction & 0xFFF;

    append_token(&builder, "B ");
    append_immediate(&builder, offset11);
    build_instruction(&builder, s);

    return 0;
}

int decode_thumb_long_branch_with_link(uint16_t instruction, char* s) {
    TokenBuilder builder;
    create_token_builder(&builder);

    // flag_t H = (instruction >> 11) & 1;
    uint8_t offset = instruction & 0x7FF;

    append_token(&builder, "BL ");
    append_immediate(&builder, offset);

    build_instruction(&builder, s);

    return 0;
}
