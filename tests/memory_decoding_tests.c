#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../internal/arm_decoder.h"
#include "test_macros.h"

void test_decode_store_from_reg_no_offset() {
    // LDR R1,[R2]  ; Load R1 from contents of R2+16, but don’t write back.
    char buffer[32];
    memset(buffer, 0, 32);
    uint32_t instruction;
    instruction = 0xE7021000;

    decode_load_store_data_ubyte(instruction, buffer);
    printf("buf: %s\n", buffer);
    CU_ASSERT_STRING_EQUAL(buffer, "STR R1, [R2]")
}
void test_decode_load_preindex_reg_offset_lsr() {
    // LDR R1,[R2,R3,LSL#2]  Load R1 from contents of R2+R3*4.
    char buffer[32];
    memset(buffer, 0, 32);
    uint32_t instruction;
    instruction = 0xE5921123;

    decode_load_store_data_ubyte(instruction, buffer);
    printf("buf: %s\n", buffer);

    char* expected = "LDR R1, [R2, R3, LSR #2]";
    CU_ASSERT_STRING_EQUAL(buffer, expected)
}
void test_decode_load_postindex_imm_offset() {
    // LDR R1,[R2,R3,LSL#2]  Load R1 from contents of R2+R3*4.
    char buffer[32];
    memset(buffer, 0, 32);
    uint32_t instruction;
    instruction = 0xE6921123;

    decode_load_store_data_ubyte(instruction, buffer);
    printf("buf: %s\n", buffer);

    char* expected = "LDR R1, [R2], #0x123";
    CU_ASSERT_STRING_EQUAL(buffer, expected)
}
void test_decode_load_preindex_reg_offset_lsr_with_writeback() {
    // LDR R1,[R2,R3,LSL#2]  Load R1 from contents of R2+R3*4.
    char buffer[32];
    memset(buffer, 0, 32);
    uint32_t instruction;
    instruction = 0xE5B21123;

    decode_load_store_data_ubyte(instruction, buffer);
    printf("buf: %s\n", buffer);

    char* expected = "LDR R1, [R2, R3, LSR #2]!";
    CU_ASSERT_STRING_EQUAL(buffer, expected)
}
void test_decode_load_negative_preindex_reg_offset_lsr() {
    // LDR R1,[R2,R3,LSL#2]  Load R1 from contents of R2+R3*4.
    char buffer[32];
    memset(buffer, 0, 32);
    uint32_t instruction;
    instruction = 0xE5121123;

    decode_load_store_data_ubyte(instruction, buffer);
    printf("buf: %s\n", buffer);

    char* expected = "LDR R1, [R2, -R3, LSR #2]";
    CU_ASSERT_STRING_EQUAL(buffer, expected)
}

void test_decode_block_store_all_registers() {
    // STMIA R0,{R0-R15}; save all registers
    char buffer[32];
    memset(buffer, 0, 32);
    uint32_t instruction;
    instruction = 0xE880FFFF;

    decode_block_data_transfer(instruction, buffer);
    printf("buf: %s\n", buffer);

    char* expected = "STMIA R0, {R0-R15}";
    CU_ASSERT_STRING_EQUAL(buffer, expected)
}

void test_decode_block_store_all_registers_with_writeback_and_psr() {
    // STMIA R0,{R0-R15}; save all registers
    char buffer[32];
    memset(buffer, 0, 32);
    uint32_t instruction;
    instruction = 0xE8E0FFFF;

    decode_block_data_transfer(instruction, buffer);
    printf("buf: %s\n", buffer);

    char* expected = "STMIA R0!, {R0-R15}^";
    CU_ASSERT_STRING_EQUAL(buffer, expected)
}

void test_block_transfer_without_stack_smash() {
    // LDMDAEQ R1!, {R0-R1, R3-R4, R6-R7, R9-R10, R12-R13, PC}
    char buffer[1024];
    memset(buffer, 0, 1024);
    uint32_t instruction = 0x0831b6db;
    decode_block_data_transfer(instruction, buffer);
    printf("buffer: %s\n", buffer);
    CU_ASSERT_STRING_EQUAL(buffer, "LDMDAEQ R1!, {R0-R1, R3-R4, R6-R7, R9-R10, R12-R13, R15}")
}

void test_decode_load_halfword_down_reg_offset() {
    // LDRH R1, [R2, -R3]
    char buffer[1024];
    memset(buffer, 0, 1024);
    uint32_t instruction;
    instruction = 0xE11210B3;

    decode_halfword_transfer(instruction, buffer);
    printf("buf: %s\n", buffer);

    char* expected = "LDRH R1, [R2, -R3]";
    CU_ASSERT_STRING_EQUAL(buffer, expected)
}
void test_decode_load_halfword_down_immediate_offset() {
    // LDRSB R8, [R2], #-0x8f
    char buffer[1024];
    memset(buffer, 0, 1024);
    uint32_t instruction;
    instruction = 0xE0528DDF;

    decode_halfword_transfer(instruction, buffer);
    printf("buf: %s\n", buffer);

    char* expected = "LDRSB R8, [R2], #-0x8f";
    CU_ASSERT_STRING_EQUAL(buffer, expected)
}

int add_memory_decoding_tests() {
    CU_pSuite suite = CU_add_suite("Memory Transfer Decoding Tests", 0, 0);

    if (suite == NULL)
        return CU_get_error();

    ADD_TEST(test_decode_store_from_reg_no_offset)
    ADD_TEST(test_decode_load_preindex_reg_offset_lsr)
    ADD_TEST(test_decode_load_preindex_reg_offset_lsr_with_writeback)
    ADD_TEST(test_decode_load_negative_preindex_reg_offset_lsr)
    ADD_TEST(test_decode_load_postindex_imm_offset)
    ADD_TEST(test_decode_block_store_all_registers)
    ADD_TEST(test_decode_block_store_all_registers_with_writeback_and_psr)
    ADD_TEST(test_block_transfer_without_stack_smash)
    ADD_TEST(test_decode_load_halfword_down_reg_offset)
    ADD_TEST(test_decode_load_halfword_down_immediate_offset)

    return CUE_SUCCESS;
}
