#include <stdint.h>
#include <stdio.h>
#include "utest/utest.h"
#include "cpu.h"
#include "cpu_internal.h"
#include "memory.h"

struct RzCpuTestFixture {
    rz_cpu_p cpu;
};

UTEST_F_SETUP(RzCpuTestFixture) {
    utest_fixture->cpu = rz_create_cpu();
    ASSERT_NE((void *)utest_fixture->cpu, NULL);
    utest_fixture->cpu->r_pc = TEXT_OFFSET;
}

UTEST_F_TEARDOWN(RzCpuTestFixture) {
    rz_free_cpu(utest_fixture->cpu);
}

UTEST_F(RzCpuTestFixture, EcallPrint) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = ECALL_CODE;
    utest_fixture->cpu->r_x[10] = PRINT_INT;
    utest_fixture->cpu->r_x[11] = 42;

    FILE *original_stdout = stdout;
    FILE *temp_file = tmpfile();
    stdout = temp_file;
    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    fflush(temp_file);
    stdout = original_stdout;

    rewind(temp_file);
    char output[16] = {0};
    fgets(output, sizeof(output), temp_file), NULL;
    fclose(temp_file);

    ASSERT_STREQ("42\n", output);
}

UTEST_F(RzCpuTestFixture, EcallRead) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = ECALL_CODE;
    utest_fixture->cpu->r_x[10] = READ_INT;

    FILE *original_stdin = stdin;
    FILE *temp_input = tmpfile();
    fprintf(temp_input, "123\n");
    rewind(temp_input);
    stdin = temp_input;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    stdin = original_stdin;
    fclose(temp_input);

    ASSERT_EQ(123, utest_fixture->cpu->r_x[11]);
}

UTEST_F(RzCpuTestFixture, EcallUnsupported) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = ECALL_CODE;
    utest_fixture->cpu->r_x[10] = -1;

    ASSERT_FALSE(rz_cycle(utest_fixture->cpu));
}

UTEST_F(RzCpuTestFixture, BranchEqualPos) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BEQ << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 5;
    utest_fixture->cpu->r_x[2] = 5;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 8 * 2, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchEqualNeg) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BEQ << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 5;
    utest_fixture->cpu->r_x[2] = 4;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 4, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchNotEqualPos) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BNE << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 5;
    utest_fixture->cpu->r_x[2] = 4;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 8 * 2, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchNotEqualNeg) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BNE << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 5;
    utest_fixture->cpu->r_x[2] = 5;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 4, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchLessThanPos) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BLT << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 4;
    utest_fixture->cpu->r_x[2] = 5;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 8 * 2, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchLessThanNeg) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BLT << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 5;
    utest_fixture->cpu->r_x[2] = 4;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 4, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchGreaterEqualPos) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BGE << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 5;
    utest_fixture->cpu->r_x[2] = 4;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 8 * 2, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchGreaterEqualNeg) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BGE << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 4;
    utest_fixture->cpu->r_x[2] = 5;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 4, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchLessThanUnsignedPos) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BLTU << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 3;
    utest_fixture->cpu->r_x[2] = 4;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 8 * 2, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchLessThanUnsignedNeg) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BLTU << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 5;
    utest_fixture->cpu->r_x[2] = 4;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 4, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchGreaterEqualUnsignedPos) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BGEU << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 5;
    utest_fixture->cpu->r_x[2] = 4;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 8 * 2, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, BranchGreaterEqualUnsignedNeg) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = B_FORMAT | (BGEU << FUNC3_OFFS);
    instr->b.rs1 = 1;
    instr->b.rs2 = 2;
    instr->b.imm1_4 = 8;

    utest_fixture->cpu->r_x[1] = 4;
    utest_fixture->cpu->r_x[2] = 5;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 4, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, JumpAndLink) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);

    instr->whole = 0b00000100000000000001 << 12;
    instr->j.op = J_FORMAT;
    instr->j.rd = 1;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));

    ASSERT_EQ(TEXT_OFFSET + 4, utest_fixture->cpu->r_x[1]);
    ASSERT_EQ(TEXT_OFFSET + 0b00000001000001000000, utest_fixture->cpu->r_pc);
}

UTEST_F(RzCpuTestFixture, JumpAndLinkRegister) {
    rz_instruction_t *instr = (rz_instruction_t *)mem_access(utest_fixture->cpu->r_pc);
    instr->whole = JALR_FORMAT;
    instr->i.rd = 1;
    instr->i.rs1 = 2;
    instr->i.imm0_11 = 4;

    utest_fixture->cpu->r_x[2] = 100;

    ASSERT_TRUE(rz_cycle(utest_fixture->cpu));
    ASSERT_EQ(TEXT_OFFSET + 4, utest_fixture->cpu->r_x[1]);
    ASSERT_EQ(104, utest_fixture->cpu->r_pc);
}

UTEST_MAIN()
