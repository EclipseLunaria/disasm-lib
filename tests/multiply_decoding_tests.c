#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../internal/arm_decoder.h"
#include "test_macros.h"

void test_decode_multiply() {
    uint32_t instruction;
    // MUL R1, R2, R3
    instruction = 0xE0010392;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_multiply(instruction, buffer1);
    printf("buf: %s\n", buffer1);
    char* expected = "MUL R1, R2, R3";
    CU_ASSERT_EQUAL(strcmp(buffer1, expected), 0)
}

void test_decode_multiply_accumulate_with_cond() {
    uint32_t instruction;
    // MLAEQS R1,R2,R3,R4
    instruction = 0x00314392;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_multiply(instruction, buffer1);
    printf("buf: %s\n", buffer1);
    char* expected = "MLAEQS R1, R2, R3, R4";
    CU_ASSERT_EQUAL(strcmp(buffer1, expected), 0)
}
void test_decode_long_multiply() {
    uint32_t instruction;
    // UMULL R1, R4, R2, R3
    instruction = 0xE0841392;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_long_multiply(instruction, buffer1);
    printf("buf: %s\n", buffer1);
    char* expected = "UMULL R1, R4, R2, R3";
    CU_ASSERT_EQUAL(strcmp(buffer1, expected), 0)
}
void test_decode_long_multiply_with_cond_acc_and_signed() {
    uint32_t instruction;
    // UMULL R1, R4, R2, R3
    instruction = 0xE0F51392;
    char buffer1[32];
    memset(buffer1, 0, 32);
    decode_long_multiply(instruction, buffer1);
    printf("buf: %s\n", buffer1);
    char* expected = "SMLALS R1, R5, R2, R3";
    CU_ASSERT_EQUAL(strcmp(buffer1, expected), 0)
}

int add_multiply_decoding_tests() {
    CU_pSuite suite = CU_add_suite("Multiply Decoding Tests", 0, 0);

    if (suite == NULL)
        return CU_get_error();

    ADD_TEST(test_decode_multiply)
    ADD_TEST(test_decode_multiply_accumulate_with_cond)
    ADD_TEST(test_decode_long_multiply)
    ADD_TEST(test_decode_long_multiply_with_cond_acc_and_signed)

    return CUE_SUCCESS;
}
