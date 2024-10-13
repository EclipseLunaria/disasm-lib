#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../internal/arm_decoder.h"
#include "test_macros.h"

void test_mrs_decoding() {
    uint32_t instruction = 0x010F4000;
    char actual[BUFFER_SIZE];
    memset(actual, 0, BUFFER_SIZE);

    decode_psr_operation(instruction, actual);
    printf("\nactual: %s\n", actual);
    CU_ASSERT_STRING_EQUAL(actual, "MRSEQ R4, CPSR")
}

void test_register_msr_register_decoding() {
    uint32_t instruction = 0xE169F002;
    char actual[BUFFER_SIZE];
    memset(actual, 0, BUFFER_SIZE);

    decode_psr_operation(instruction, actual);
    printf("\nactual: %s\n", actual);
    CU_ASSERT_STRING_EQUAL(actual, "MSR SPSR, R2")
}

void test_flag_msr_decoding() {
    uint32_t instruction = 0xE368F002;
    char actual[BUFFER_SIZE];
    memset(actual, 0, BUFFER_SIZE);

    decode_psr_operation(instruction, actual);
    printf("\nactual: %s\n", actual);
    CU_ASSERT_STRING_EQUAL(actual, "MSR SPSR_flg, #0x2")
}

int add_psr_tests() {
    CU_pSuite suite = CU_add_suite("PSR Tests", 0, 0);

    if (suite == NULL)
        return CU_get_error();

    ADD_TEST(test_register_msr_register_decoding);
    ADD_TEST(test_mrs_decoding);
    ADD_TEST(test_flag_msr_decoding);
    return CUE_SUCCESS;
}
