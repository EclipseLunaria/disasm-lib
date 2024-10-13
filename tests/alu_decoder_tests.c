#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../internal/arm_decoder.h"
#include "test_macros.h"

void test_decode_alu_mnemonic() {
    uint32_t instruction;
    // MOV R0, #1
    instruction = 0xE3A00001;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_alu_operation(instruction, buffer1);
    char* expected = "MOV";
    CU_ASSERT_EQUAL(strncmp(buffer1, expected, 3), 0)
}

void test_decode_mov_with_cond() {
    uint32_t instruction;
    // MOV R0, #1
    instruction = 0x03A00001;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_alu_operation(instruction, buffer1);
    char* expected = "MOVEQ";
    CU_ASSERT_EQUAL(strncmp(buffer1, expected, 5), 0)
}

void test_decode_mov_with_s_bit() {
    uint32_t instruction;
    // MOV R0, #1
    instruction = 0xE3B00001;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_alu_operation(instruction, buffer1);
    char* expected = "MOVS";
    printf("\nbuf: \n%s\n%s\n", buffer1, expected);
    CU_ASSERT_EQUAL(strncmp(buffer1, expected, 4), 0)
}
void test_decode_mov_with_s_bit_and_cond() {
    uint32_t instruction;
    // MOV R0, #1
    instruction = 0x23B00001;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_alu_operation(instruction, buffer1);
    printf("buf: %s\n", buffer1);
    char* expected = "MOVCSS";
    CU_ASSERT_EQUAL(strncmp(buffer1, expected, 5), 0)
}

void test_decode_add_op_on_reg_no_shift() {
    uint32_t instruction;
    // ADD r0, r1, r2
    instruction = 0xE0810002;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_alu_operation(instruction, buffer1);
    printf("buf: %s\n", buffer1);
    char* expected = "ADD R0, R1, R2";
    CU_ASSERT_EQUAL(strcmp(buffer1, expected), 0)
}
void test_decode_sub_op_on_imm_no_shift() {
    uint32_t instruction;
    // SUB r3, r4, #5
    instruction = 0xE2443005;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_alu_operation(instruction, buffer1);
    printf("buf: %s\n", buffer1);
    char* expected = "SUB R3, R4, #0x5";
    CU_ASSERT_EQUAL(strcmp(buffer1, expected), 0)
}
void test_decode_mov_imm_value() {
    uint32_t instruction;
    // MOV R6, #100
    instruction = 0xE3A06064;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_alu_operation(instruction, buffer1);
    printf("buf: %s\n", buffer1);
    char* expected = "MOV R6, #0x64";
    CU_ASSERT_EQUAL(strcmp(buffer1, expected), 0)
}

int add_alu_decoder_tests() {
    CU_pSuite suite = CU_add_suite("ALU Decoder Tests", 0, 0);

    if (suite == NULL)
        return CU_get_error();

    ADD_TEST(test_decode_alu_mnemonic)
    ADD_TEST(test_decode_mov_with_cond)
    ADD_TEST(test_decode_mov_with_s_bit)
    ADD_TEST(test_decode_mov_with_s_bit_and_cond)
    ADD_TEST(test_decode_add_op_on_reg_no_shift)
    ADD_TEST(test_decode_sub_op_on_imm_no_shift)
    ADD_TEST(test_decode_mov_imm_value)

    return CUE_SUCCESS;
}
