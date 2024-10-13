#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../internal/instruction_builder.h"
#include "test_macros.h"

void test_build_immediate_shift_token() {
    // build shift bits
    uint8_t shift_type = 0b01;
    uint16_t imm_value = 4;
    reg_t rm = 5;

    uint16_t shift_bits = imm_value << 7 | shift_type << 5 | rm;
    char actual[16];
    memset(actual, 0, 16);
    build_reg_shift_token(shift_bits, actual);
    printf("shift buffer: %s\n", actual);
    CU_ASSERT_STRING_EQUAL(actual, "R5, LSR #4")
}
void test_build_immediate_shift_token_shift_value_zero() { // build shift bits
    uint8_t shift_type = 0b01;
    uint16_t imm_value = 0;
    reg_t rm = 5;

    uint16_t shift_bits = imm_value << 7 | shift_type << 5 | rm;
    char actual[16];
    memset(actual, 0, 16);
    build_reg_shift_token(shift_bits, actual);
    printf("shift buffer: %s\n", actual);
    CU_ASSERT_STRING_EQUAL(actual, "R5")
}
void test_build_register_shift_token() {
    uint8_t shift_type = 0b11;
    uint16_t rs = 2;
    uint16_t rm = 5;

    uint16_t shift_bits = rs << 8 | shift_type << 5 | 1 << 4 | rm;
    char actual[16];
    memset(actual, 0, 16);
    build_reg_shift_token(shift_bits, actual);
    printf("shift buffer: %s\n", actual);
    CU_ASSERT_STRING_EQUAL(actual, "R5, ROR R2")
}

void test_build_register_list() {
    uint16_t reg_list = 0xFF;
    char actual[32];
    memset(actual, 0, 32);
    build_register_list(reg_list, actual);
    printf("actual buffer: %s\n", actual);

    CU_ASSERT_STRING_EQUAL(actual, "R0-R7")
}
void test_build_register_list_single_and_group() {
    uint16_t reg_list = 0x2F;
    char actual[32];
    memset(actual, 0, 32);
    build_register_list(reg_list, actual);
    printf("actual buffer: %s\n", actual);

    CU_ASSERT_STRING_EQUAL(actual, "R0-R3, R5")
}
void test_build_register_list_single_register() {
    uint16_t reg_list = 0x2;
    char actual[32];
    memset(actual, 0, 32);
    build_register_list(reg_list, actual);
    printf("actual buffer: %s\n", actual);

    CU_ASSERT_STRING_EQUAL(actual, "R1")
}
void test_build_register_list_split_register_groups() {
    uint16_t reg_list = 0xF0F;
    char actual[32];
    memset(actual, 0, 32);
    build_register_list(reg_list, actual);
    printf("actual buffer: %s\n", actual);

    CU_ASSERT_STRING_EQUAL(actual, "R0-R3, R8-R11")
}

int add_instruction_builder_tests() {
    CU_pSuite suite = CU_add_suite("Instruction Builder Tests", 0, 0);

    if (suite == NULL)
        return CU_get_error();

    ADD_TEST(test_build_immediate_shift_token);
    ADD_TEST(test_build_immediate_shift_token_shift_value_zero);
    ADD_TEST(test_build_register_shift_token);

    ADD_TEST(test_build_register_list_single_register);
    ADD_TEST(test_build_register_list);
    ADD_TEST(test_build_register_list_single_and_group);
    ADD_TEST(test_build_register_list_split_register_groups);

    return CUE_SUCCESS;
}
