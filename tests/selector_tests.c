#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../internal/arm_decoder.h"
#include "test_macros.h"

void test_select_alu() {
    uint32_t instruction;
    // MOV R0, #1
    instruction = 0xE3A00001;
    CU_ASSERT_EQUAL(select_decoder(instruction), decode_alu_operation)

    // ADD R1, R0, R2
    instruction = 0xE0801002;
    CU_ASSERT_EQUAL(select_decoder(instruction), decode_alu_operation)

    // SUB R3, R1, #5
    instruction = 0xE2413005;
    CU_ASSERT_EQUAL(select_decoder(instruction), decode_alu_operation)

    // AND R4, R1, R2
    instruction = 0xE0014002;
    CU_ASSERT_EQUAL(select_decoder(instruction), decode_alu_operation)

    // ORR R5, R3, #0xFF
    instruction = 0xE38350FF;
    CU_ASSERT_EQUAL(select_decoder(instruction), decode_alu_operation)
}
int add_selector_tests() {
    CU_pSuite suite = CU_add_suite("Decoder Selector Tests", 0, 0);

    if (suite == NULL)
        return CU_get_error();

    ADD_TEST(test_select_alu)

    return CUE_SUCCESS;
}
