#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "test_macros.h"
#include "thumb_decoder.h"
#include "disasm.h"

void test_thumb_mov_offset_r0() {
    uint16_t instruction = 0x2006;
    CU_ASSERT_TRUE(IS_IMMEDIATE_OP(instruction))

    char actual[64];
    memset(actual, 0, 64);

    decode_thumb_instruction(instruction, actual);
    printf("\nBuffer: %s", actual);
    CU_ASSERT_STRING_EQUAL(actual, "MOV R0, #0x6")
}

void test_thumb_load_address() {
    uint16_t instruction = 0x0600;
    CU_ASSERT_TRUE(IS_MOVE_SHIFTED_REGISTER(instruction))

    char actual[64];
    memset(actual, 0, 64);
    decode_thumb_instruction(instruction, actual);
    printf("\nBuffer: %s", actual);
    CU_ASSERT_STRING_EQUAL(actual, "LSL R0, R0, #0x18")
}
void test_thumb_push_registers() {
    uint16_t instruction = 0xb530;
    CU_ASSERT_TRUE(IS_PUSH_POP_REGISTERS(instruction))

    char actual[64];
    memset(actual, 0, 64);
    decode_thumb_instruction(instruction, actual);
    printf("\nBuffer: %s", actual);
    CU_ASSERT_STRING_EQUAL(actual, "POP {R2, LR}")
}
void test_thumb_push_empty_registers() {
    uint16_t instruction = 0xbc00;
    CU_ASSERT_TRUE(IS_PUSH_POP_REGISTERS(instruction))

    char actual[64];
    memset(actual, 0, 64);
    decode_thumb_instruction(instruction, actual);
    printf("\nBuffer: %s", actual);
    CU_ASSERT_STRING_EQUAL(actual, "PUSH {}")
}
void test_thumb_store_byte() {
    uint16_t instruction = 0x7800;
    CU_ASSERT_TRUE(IS_TRANSFER_IMMEDIATE_OFFSET(instruction))

    char actual[64];
    memset(actual, 0, 64);
    decode_thumb_instruction(instruction, actual);
    printf("\nBuffer: %s", actual);
    CU_ASSERT_STRING_EQUAL(actual, "LDRB R0, [R0, #0x0]")
}
void test_thumb_undefined() {
    uint16_t instruction = 0xe977;
    CU_ASSERT_FALSE(IS_UNCONDITIONAL_BRANCH(instruction))

    char actual[64];
    memset(actual, 0, 64);
    decode_thumb_instruction(instruction, actual);
    printf("\nBuffer: %s", actual);
    CU_ASSERT_STRING_EQUAL(actual, "UNDEFINED")
}

int add_thumb_instruction_tests() {
    CU_pSuite suite = CU_add_suite("Thumb Decoder Tests", 0, 0);

    if (suite == NULL)
        return CU_get_error();

    ADD_TEST(test_thumb_mov_offset_r0)
    ADD_TEST(test_thumb_load_address)
    ADD_TEST(test_thumb_push_registers)
    ADD_TEST(test_thumb_push_empty_registers)
    ADD_TEST(test_thumb_store_byte)
    ADD_TEST(test_thumb_undefined)

    return CUE_SUCCESS;
}
