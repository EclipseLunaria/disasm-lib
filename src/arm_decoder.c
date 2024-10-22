#include "arm_decoder.h"
#include "disasm.h"
#include "instruction_builder.h"
#include "shift_ops.h"

int decode_arm_instruction(uint32_t instruction, char* buffer) {
    decoder_t decoder = select_decoder(instruction);

    if (((instruction >> 28) & 0xF) == 0xF || !decoder) {
        strcpy(buffer, "UNKNOWN");
        return -1;
    }

    decoder(instruction, buffer);
    return 0;
}

decoder_t select_decoder(uint32_t instruction) {
    if (IS_MULTIPLY(instruction)) {
        return decode_multiply;
    }
    if (IS_LONG_MULTIPLY(instruction)) {
        return decode_long_multiply;
    }
    if (IS_BRANCH_EXCHANGE(instruction)) {
        return decode_branch_exchange;
    }
    if (IS_SWAP(instruction)) {
        return decode_swap;
    }
    if (IS_HALFWORD_REGISTER_TRANSFER(instruction) || IS_HALFWORD_IMMEDIATE_TRANSFER(instruction) ||
        IS_SIGNED_DATA_TRANSFER(instruction)) {
        return decode_halfword_transfer;
    }

    if (IS_REGISTER_MSR(instruction) || IS_MRS_OPERATION(instruction) ||
        IS_FLAG_BIT_MSR(instruction)) {
        return decode_psr_operation;
    }

    if (IS_ALU_OPERTAION(instruction)) {
        return decode_alu_operation;
    }
    if (IS_LOAD_STORE_REGISTER_UBYTE(instruction)) {
        return decode_load_store_data_ubyte;
    }
    if (IS_UNDEFINED(instruction)) {
        return decode_undefined;
    }
    if (IS_BLOCK_DATA_TRANSFER(instruction)) {
        return decode_block_data_transfer;
    }
    if (IS_BRANCH(instruction)) {
        return decode_branch;
    }
    if (IS_COPROCESSOR_DATA_TRANSFER(instruction)) {
        return decode_coprocessor_data_transfer;
    }
    if (IS_COPROCESSOR_DATA_OPERATION(instruction)) {
        return decode_coprocessor_data_operation;
    }
    if (IS_COPROCESSOR_REGISTER_TRANSFER(instruction)) {
        return decode_coprocessor_register_transfer;
    }
    if (IS_SOFTWARE_INTERRUPT(instruction)) {
        return decode_software_interrupt;
    }

    return NULL;
}

int decode_multiply(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t cond = (instruction >> 28) & 0xFF;
    flag_t A = (instruction >> 21) & 1;
    flag_t S = (instruction >> 20) & 1;

    reg_t rd = (instruction >> 16) & 0xF;
    reg_t rn = (instruction >> 12) & 0xF;
    reg_t rs = (instruction >> 8) & 0xF;
    reg_t rm = instruction & 0xF;

    append_fmt_token(&builder, "%s%s%s ", A ? "MLA" : "MUL", COND_TYPE_STRS[cond], S ? "S" : "");

    append_register(&builder, rd);
    append_register(&builder, rm);
    append_register(&builder, rs);
    if (A)
        append_register(&builder, rn);

    build_instruction(&builder, buffer);
    return 0;
}
int decode_long_multiply(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t cond = (instruction >> 28) & 0xFF;
    OpFlags flags = {
        .U = (instruction >> 20) & 1,
        .A = (instruction >> 21) & 1,
        .S = (instruction >> 20) & 1,
    };

    reg_t rdhi = (instruction >> 16) & 0xF;
    reg_t rdlo = (instruction >> 12) & 0xF;
    reg_t rs = (instruction >> 8) & 0xF;
    reg_t rm = instruction & 0xF;

    append_fmt_token(&builder, "%s%s%s%s ", flags.U ? "S" : "U", flags.A ? "MLAL" : "MULL",
                     COND_TYPE_STRS[cond], flags.S ? "S" : "");

    append_register(&builder, rdlo);
    append_register(&builder, rdhi);
    append_register(&builder, rm);
    append_register(&builder, rs);

    build_instruction(&builder, buffer);
    return 0;
}
#define IS_REGISTER_SHIFTED(x)
int decode_alu_operation(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t cond = (instruction >> 28) & 0xFF;
    flag_t I = (instruction >> 25) & 1;
    flag_t S = (instruction >> 20) & 1;

    uint8_t opcode = (instruction >> 21) & 0xF;
    reg_t rn = (instruction >> 16) & 0xF;
    reg_t rd = (instruction >> 12) & 0xF;
    uint16_t operand2 = instruction & 0xFFF;

    append_fmt_token(&builder, "%s%s%s ", ARM_ALU_OPCODES[opcode], COND_TYPE_STRS[cond],
                     S ? "S" : "");

    if (strncmp(builder.mnemonic, "CMP", 3) && strncmp(builder.mnemonic, "CMN", 3) &&
        strncmp(builder.mnemonic, "TST", 3) && strncmp(builder.mnemonic, "TEQ", 3)) {
        append_register(&builder, rd);
    }

    if (strncmp(builder.mnemonic, "MOV", 3) && strncmp(builder.mnemonic, "MVN", 3)) {
        append_register(&builder, rn);
    }

    if (I) {
        uint32_t imm = operand2 & 0xFF;
        uint32_t rotate = (operand2 >> 8) & 0xF;
        uint32_t result_imm;
        SHFT_ROR(imm, rotate, &result_imm);
        if (result_imm) {
            append_immediate(&builder, result_imm);
        }
    } else {
        reg_t rm = operand2 & 0xF;
        flag_t is_register_shifted = (operand2 >> 4) & 1;
        uint8_t shift_type = (operand2 >> 5) & 0b11;
        append_register(&builder, rm);
        if (is_register_shifted) {
            // shift by register
            reg_t rs = (operand2 >> 8) & 0xF;
            append_register(&builder, rs);
        } else {
            // shift by immediate
            char shift_buffer[16];
            memset(shift_buffer, 0, 16);
            uint8_t imm_rm_shift = (operand2 >> 7) & 0x1F;
            if (imm_rm_shift != 0) {
                sprintf(shift_buffer, "%s #%d", SHIFT_OP_STRS[shift_type], imm_rm_shift);
            }
            append_token(&builder, shift_buffer);
        }
    }

    build_instruction(&builder, buffer);

    return 0;
}

int decode_psr_operation(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t cond = (instruction >> 28) & 0xF;
    OpFlags flags = {
        .I = (instruction >> 25) & 1, .P = (instruction >> 22) & 1, .F = (instruction >> 16) & 1};

    reg_t rd = (instruction >> 12) & 0xF;
    reg_t rm = instruction & 0xF;
    uint16_t src_operand = instruction & 0xFFF;
    uint8_t msr = IS_MRS_OPERATION(instruction);

    append_fmt_token(&builder, "%s%s ", msr ? "MRS" : "MSR", COND_TYPE_STRS[cond]);

    if (IS_MRS_OPERATION(instruction)) {
        append_register(&builder, rd);
        append_token(&builder, flags.P ? "SPSR" : "CPSR");
    } else {
        append_fmt_token(&builder, "%s%s", flags.P ? "SPSR" : "CPSR", flags.F ? "" : "_flg");

        if (flags.F && !flags.I) {
            append_register(&builder, rm);
        } else {
            uint32_t immediate_shift;
            SHFT_ROR(src_operand & 0xFF, (src_operand >> 8) & 0xF, &immediate_shift);
            append_immediate(&builder, immediate_shift);
        }
    }
    build_instruction(&builder, buffer);

    return 0;
}

int decode_branch_exchange(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t cond = (instruction >> 28) & 0xFF;
    reg_t rn = instruction & 0xF;

    append_fmt_token(&builder, "BX%s ", COND_TYPE_STRS[cond]);
    append_register(&builder, rn);
    build_instruction(&builder, buffer);
    return 0;
}

int decode_branch(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);
    uint8_t cond = (instruction >> 28) & 0xF;
    flag_t L = (instruction >> 28) & 0xFF;
    uint32_t shift = instruction & 0xFFFF;

    append_fmt_token(&builder, "B%s%s ", L ? "L" : "", COND_TYPE_STRS[cond]);

    // needs to test sign maybe needs extenstion
    append_immediate(&builder, shift << 2);
    build_instruction(&builder, buffer);
    return 0;
}

int decode_swap(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t cond = (instruction >> 28) & 0xFF;
    flag_t B = (instruction >> 22) & 1;
    reg_t rn = (instruction >> 16) & 0xF;
    reg_t rd = (instruction >> 12) & 0xF;
    reg_t rm = instruction & 0xF;

    append_fmt_token(&builder, "SWP%s%s ", B ? "B" : "", COND_TYPE_STRS[cond]);

    append_register(&builder, rd);
    append_register(&builder, rm);

    // possibly break this down into a method
    char b[16];
    memset(b, 0, 16);
    sprintf(b, "[%d]", rn);
    append_token(&builder, b);

    build_instruction(&builder, buffer);
    return 0;
}

// memory instructions

int decode_load_store_data_ubyte(uint32_t instruction, char* buffer) {
    // extract parameters
    uint8_t cond = (instruction >> 28) & 0xF;
    flag_t I = (instruction >> 25) & 1;
    flag_t P = (instruction >> 24) & 1;
    flag_t U = (instruction >> 23) & 1;
    flag_t B = (instruction >> 22) & 1;
    flag_t W = (instruction >> 21) & 1;
    flag_t L = (instruction >> 20) & 1;
    reg_t rn = (instruction >> 16) & 0xF;
    reg_t rd = (instruction >> 12) & 0xF;
    uint32_t offset = instruction & 0xFFF;

    TokenBuilder builder;
    create_token_builder(&builder);

    append_fmt_token(&builder, "%s%s%s%s ", L ? "LDR" : "STR", COND_TYPE_STRS[cond], B ? "B" : "",
                     W && !P ? "T" : "");
    append_register(&builder, rd);

    // TODO: migrate memory method
    char address_buffer[64];
    memset(address_buffer, 0, 64);

    strcat(address_buffer, "[");

    // add rn register
    char rn_buffer[16];
    memset(rn_buffer, 0, 16);
    build_register_token(rn, rn_buffer);
    strcat(address_buffer, rn_buffer);

    // build offset buffer
    char offset_buffer[32];
    memset(offset_buffer, 0, 32);
    if (I) {
        if (offset) {
            sprintf(offset_buffer, "#%s0x%x", U ? "" : "-", offset);
        }
    } else {
        build_reg_shift_token(offset, offset_buffer);
        if (P) {

            strcat(address_buffer, ", ");
            strcat(address_buffer, U ? "" : "-");
            strcat(address_buffer, offset_buffer);
        }
    }

    strcat(address_buffer, "]");
    strcat(address_buffer, offset && W && P ? "!" : "");
    append_token(&builder, address_buffer);

    if (!P)
        append_token(&builder, offset_buffer);

    build_instruction(&builder, buffer);
    return 0;
}

int decode_block_data_transfer(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t cond = (instruction >> 28) & 0xF;
    OpFlags flags = {.P = (instruction >> 24) & 1,
                     .U = (instruction >> 23) & 1,
                     .S = (instruction >> 22) & 1,
                     .W = (instruction >> 21) & 1,
                     .L = (instruction >> 20) & 1};

    reg_t rn = (instruction >> 16) & 0xF;
    uint16_t reg_list = instruction & 0xFFFF;
    uint8_t transfer_type = flags.L << 2 | flags.P << 1 | flags.U;

    char reg_list_buffer[BUFFER_SIZE];
    memset(reg_list_buffer, 0, BUFFER_SIZE);

    append_fmt_token(&builder, "%s%s ",
                     rn == 14 ? STACK_BLOCK_SUFFIXS[transfer_type]
                              : OTHER_BLOCK_SUFFIXS[transfer_type],
                     COND_TYPE_STRS[cond]);

    append_fmt_token(&builder, "R%d%s", rn, flags.W ? "!" : "");

    build_register_list(reg_list, reg_list_buffer);
    append_fmt_token(&builder, "{%s}%s", reg_list_buffer, flags.S ? "^" : "");

    build_instruction(&builder, buffer);
    return 0;
}

int decode_halfword_transfer(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t cond = instruction >> 28;
    OpFlags flags = {
        .P = (instruction >> 24) & 1,
        .U = (instruction >> 23) & 1,
        .I = (instruction >> 22) & 1,
        .W = (instruction >> 21) & 1,
        .L = (instruction >> 20) & 1,
        .H = (instruction >> 5) & 1,
        .S = (instruction >> 6) & 1,
    };

    reg_t rn = (instruction >> 16) & 0xF;
    reg_t rd = (instruction >> 12) & 0xF;

    assert(((flags.S << 1 | flags.H) & 0b11) != 0);

    append_fmt_token(&builder, "%s%s%s%s ", flags.L ? "LDR" : "STR", COND_TYPE_STRS[cond],
                     flags.S ? "S" : "", flags.H ? "H" : "B");
    append_register(&builder, rd);
    append_address_token(
        &builder, rn,
        flags.I ? (instruction & 0xF) | ((instruction >> 8) & 0xF0) : instruction & 0xF, flags);

    build_instruction(&builder, buffer);
    return 0;
}

int decode_software_interrupt(uint32_t instruction, char* buffer) {
    strcat(buffer, "SWI");
    return 0;
}

int decode_undefined(uint32_t instruction, char* buffer) {
    strcat(buffer, "<Undefined>");
    return 0;
}

int decode_coprocessor_data_transfer(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);

    uint8_t cond = (instruction >> 28) & 0xF;
    OpFlags flags = {.P = (instruction >> 24) & 1,
                     .U = (instruction >> 23) & 1,
                     .N = (instruction >> 22) & 1,
                     .W = (instruction >> 21) & 1,
                     .L = (instruction >> 20) & 1,
                     .I = 1};
    reg_t rn = (instruction >> 16) & 0xF;
    reg_t crd = (instruction >> 12) & 0xF;
    uint8_t cpn = (instruction >> 8) & 0xF;
    uint8_t offset = instruction & 0xFF;

    append_fmt_token(&builder, "%s%s%s ", flags.L ? "LDC" : "STC", flags.N ? "L" : "",
                     COND_TYPE_STRS[cond]);

    append_proc_number(&builder, cpn);
    append_proc_register(&builder, crd);

    append_address_token(&builder, rn, offset, flags);

    build_instruction(&builder, buffer);

    return 0;
}
int decode_coprocessor_data_operation(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);
    uint8_t cond = (instruction >> 28) & 0xF;
    uint8_t opc = (instruction >> 20) & 0xF;
    reg_t crn = (instruction >> 16) & 0xF;
    reg_t crd = (instruction >> 12) & 0xF;
    reg_t cpn = (instruction >> 8) & 0xF;
    reg_t cp = (instruction >> 5) & 0b111;
    reg_t crm = instruction & 0xF;

    append_fmt_token(&builder, "CDP%s ", COND_TYPE_STRS[cond]);
    append_proc_number(&builder, cpn);
    append_number(&builder, opc);

    append_proc_register(&builder, crd);
    append_proc_register(&builder, crn);
    append_proc_register(&builder, crm);
    append_number(&builder, cp);

    build_instruction(&builder, buffer);

    return 0;
}

int decode_coprocessor_register_transfer(uint32_t instruction, char* buffer) {
    TokenBuilder builder;
    create_token_builder(&builder);
    uint8_t cond = (instruction >> 28) & 0xF;

    uint8_t opc = (instruction >> 21) & 0b111;
    flag_t L = (instruction >> 20) & 1;
    reg_t crn = (instruction >> 16) & 0xF;
    reg_t crd = (instruction >> 12) & 0xF;
    reg_t cpn = (instruction >> 8) & 0xF;
    reg_t cp = (instruction >> 5) & 0b111;
    reg_t crm = instruction & 0xF;

    append_fmt_token(&builder, "%s%s ", L ? "MRC" : "MCR", COND_TYPE_STRS[cond]);
    append_proc_number(&builder, cpn);
    append_number(&builder, opc);

    append_register(&builder, crd);
    append_proc_register(&builder, crn);

    append_proc_register(&builder, crm);
    append_number(&builder, cp);

    build_instruction(&builder, buffer);
    return 0;
}
