#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "../internal/arm_decoder.h"
#include "test_macros.h"

// ALU OPERATIONS
void test_mov_instruction() {
    // MOV R0, #1
    uint32_t instruction = 0xE3A00001;

    CU_ASSERT_TRUE(IS_ALU_OPERTAION(instruction))
}
void test_add_instruction() {
    // ADD R1, R0, R2
    uint32_t instruction = 0xE0801002;
    CU_ASSERT_TRUE(IS_ALU_OPERTAION(instruction))
}
void test_sub_instruction() {
    // SUB R3, R1, #5
    uint32_t instruction = 0xE2413005;
    CU_ASSERT_TRUE(IS_ALU_OPERTAION(instruction))
}
void test_and_instruction() {
    // AND R4, R1, R2
    uint32_t instruction = 0xE0014002;
    CU_ASSERT_TRUE(IS_ALU_OPERTAION(instruction))
}
void test_orr_instruction() {
    // ORR R5, R3, #0xFF
    uint32_t instruction = 0xE38350FF;
    CU_ASSERT_TRUE(IS_ALU_OPERTAION(instruction))
}

// Load Store Instructions
void test_load_word_instruction() {
    // LDR R0, [R1]
    uint32_t instruction = 0xE5900000;
    CU_ASSERT_TRUE(IS_LOAD_STORE_REGISTER_UBYTE(instruction))
}
void test_store_word_instruction() {
    // STR R2, [R3, #4]
    uint32_t instruction = 0xE5832004;
    CU_ASSERT_TRUE(IS_LOAD_STORE_REGISTER_UBYTE(instruction))
}
void test_load_word_update_instruction() {
    // LDR R4, [R5, #8]
    uint32_t instruction = 0xE5954008;
    CU_ASSERT_TRUE(IS_LOAD_STORE_REGISTER_UBYTE(instruction))
}

// test branch instructions

void test_branch_instruction() {
    // B 0x00000020
    uint32_t instruction = 0xEA000005;
    CU_ASSERT_TRUE(IS_BRANCH(instruction))
}
void test_branch_link_instruction() {
    // BL 0x00000040
    uint32_t instruction = 0xEB000010;
    CU_ASSERT_TRUE(IS_BRANCH(instruction))
}

/*
#define IS_MRS_OPERATION(x) (x & 0x0FBF0FFF) == 0x010F0000;
#define IS_REGISTER_MSR(x) (x & 0x0FBFFFF0) == 0x0129F000;
#define IS_FLAG_BIT_MSR(x) (x & 0x0DBFF000) == 0x0128F000;
*/

void test_mrs_macro() {
    uint32_t valid_mrs_instruction = 0x010F4000;
    CU_ASSERT_TRUE(IS_MRS_OPERATION(valid_mrs_instruction))

    uint32_t invalid_mrs_instruction = 0xE064F002;
    CU_ASSERT_FALSE(IS_MRS_OPERATION(invalid_mrs_instruction))
}

void test_register_msr_macro() {
    uint32_t valid_msr_instruction = 0xE169F002;
    CU_ASSERT_TRUE(IS_REGISTER_MSR(valid_msr_instruction))

    uint32_t invalid_msr_instruction = 0xE064F002;
    CU_ASSERT_FALSE(IS_REGISTER_MSR(invalid_msr_instruction))
}

void test_flag_msr_macro() {
    uint32_t valid_flag_msr_instruction = 0xE368F002;
    CU_ASSERT_TRUE(IS_FLAG_BIT_MSR(valid_flag_msr_instruction))

    uint32_t invalid_flag_msr_instruction = 0xE064F002;
    CU_ASSERT_FALSE(IS_FLAG_BIT_MSR(invalid_flag_msr_instruction))
}
int add_macro_tests() {
    CU_pSuite suite = CU_add_suite("Macro Tests", 0, 0);

    if (suite == NULL)
        return CU_get_error();

    // test alu op macros
    ADD_TEST(test_mov_instruction)
    ADD_TEST(test_add_instruction)
    ADD_TEST(test_sub_instruction)
    ADD_TEST(test_and_instruction)
    ADD_TEST(test_orr_instruction)

    // test load/store macros
    ADD_TEST(test_load_word_instruction)
    ADD_TEST(test_load_word_update_instruction)
    ADD_TEST(test_store_word_instruction)

    // test branch op macros
    ADD_TEST(test_branch_link_instruction)
    ADD_TEST(test_branch_instruction)

    // test psr macros
    ADD_TEST(test_register_msr_macro)
    ADD_TEST(test_mrs_macro);
    ADD_TEST(test_flag_msr_macro)

    return CUE_SUCCESS;
}
