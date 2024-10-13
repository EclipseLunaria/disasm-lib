#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../internal/arm_decoder.h"
#include "test_macros.h"

void test_coprocessor_data_operation() {
    // CDP p10, 2, c0, c1, c2, 0
    uint32_t instruction = 0xEE210102;
    char actual[BUFFER_SIZE];
    memset(actual, 0, BUFFER_SIZE);

    decode_coprocessor_data_operation(instruction, actual);

    printf("\nBuffer: %s\n", actual);
    CU_ASSERT_STRING_EQUAL(actual, "CDP p1, 2, c0, c1, c2, 0")
}
void test_coprocessor_data_transfer_ldc_long() {
    // LDCL p1, c3, [r4], #0x10
    uint32_t instruction = 0xECD43110;
    char actual[BUFFER_SIZE];
    memset(actual, 0, BUFFER_SIZE);

    decode_coprocessor_data_transfer(instruction, actual);

    printf("\nBuffer: %s\n", actual);
    CU_ASSERT_STRING_EQUAL(actual, "LDCL p1, c3, [R4], #0x10")
}
void test_coprocessor_register_transfer() {
    // MRC p2,5,R3,c5,c6
    uint32_t instruction = 0xEEB53216;
    char actual[BUFFER_SIZE];
    memset(actual, 0, BUFFER_SIZE);

    decode_coprocessor_register_transfer(instruction, actual);

    printf("\nBuffer: %s\n", actual);
    CU_ASSERT_STRING_EQUAL(actual, "MRC p2, 5, R3, c5, c6, 0")
}

int add_coprocessor_decoding_tests() {
    CU_pSuite suite = CU_add_suite("Coprocessor Decoding Tests", 0, 0);

    if (suite == NULL)
        return CU_get_error();

    ADD_TEST(test_coprocessor_data_operation)
    ADD_TEST(test_coprocessor_data_transfer_ldc_long)
    ADD_TEST(test_coprocessor_register_transfer)

    return CUE_SUCCESS;
}
