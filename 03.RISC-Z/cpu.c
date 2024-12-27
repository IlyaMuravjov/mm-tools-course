#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "misc.h"
#include "cpu.h"
#include "memory.h"
#include "cpu_internal.h"

const char *rz_cpu_info(const rz_cpu_p pcpu)
{
    return pcpu->info;
}

rz_cpu_p rz_create_cpu(void){
    rz_cpu_p pcpu = malloc(sizeof(rz_cpu_t));
    pcpu->info = "RISC-Z.32.2023";

    pcpu->r_pc = TEXT_OFFSET;

    memset(pcpu->r_x, 0, sizeof(pcpu->r_x));
    pcpu->r_x[2] = STACK_OFFSET + STACK_SIZE - (unsigned) sizeof(rz_register_t); // sp
    pcpu->r_x[3] = DATA_OFFSET; // gp

    return pcpu;
}

void rz_free_cpu(rz_cpu_p pcpu){
    free(pcpu);
}

bool rz_r_cycle(rz_cpu_p pcpu, rz_instruction_t instr) {
    switch(instr.whole & (OPCODE_MASK | FUNC3_MASK | FUNC7_MASK)) {
        case ADD_CODE:
            pcpu->r_x[instr.r.rd] = pcpu->r_x[instr.r.rs1] + pcpu->r_x[instr.r.rs2];
        break;
        case SUB_CODE:
            pcpu->r_x[instr.r.rd] = pcpu->r_x[instr.r.rs1] - pcpu->r_x[instr.r.rs2];
        break;
        case XOR_CODE:
            pcpu->r_x[instr.r.rd] = pcpu->r_x[instr.r.rs1] ^ pcpu->r_x[instr.r.rs2];
        break;
        default:
            return false;
    }
    return true;
}

static inline rz_register_t sign_extend(unsigned some_bits, int how_many_bits) {
    rz_register_t result = some_bits;
    if(1 << (how_many_bits - 1) & result) // then negative
        result |= -1 << how_many_bits; // -1 is full of 1s
    return result;
}

bool rz_i_cycle(rz_cpu_p pcpu, rz_instruction_t instr) {
        switch(instr.whole & (OPCODE_MASK | FUNC3_MASK)) {
        case ADDI_CODE:
            pcpu->r_x[instr.i.rd] = pcpu->r_x[instr.i.rs1] + sign_extend(instr.i.imm0_11, 12);
        break;
        case SLLI_CODE:
            pcpu->r_x[instr.i.rd] = pcpu->r_x[instr.i.rs1] << instr.r.rs2;
        break;
        break;
        default:
            return false;
    }
    return true;
}

static bool rz_ecall(rz_cpu_p pcpu) {
    rz_register_t ecall_id = pcpu->r_x[10];

    switch (ecall_id) {
        case PRINT_INT:
            printf("%u\n", pcpu->r_x[11]);
            return true;
        case READ_INT: {
            int input;
            printf("Enter integer:\n");
            if (scanf("%d", &input) != 1) {
                fprintf(stderr, "Error reading integer\n");
                return false;
            }
            pcpu->r_x[11] = input;
            return true;
        }
        default:
            fprintf(stderr, "Unsupported ecall ID %u\n", ecall_id);
            return false;
    }
}

bool rz_sys_cycle(rz_cpu_p pcpu, rz_instruction_t instr) {
    switch(instr.whole & (1u << 20)){
        case 0:
            return rz_ecall(pcpu);
        case 1:
        default:
            return false;
    }
}

static bool rz_b_cycle(rz_cpu_p pcpu, rz_instruction_t instr) {
    int32_t imm = ( (instr.b.imm12 << 12)
                   | (instr.b.imm11 << 11)
                   | (instr.b.imm5_10 << 5)
                   | (instr.b.imm1_4 << 1));
    imm = sign_extend(imm, 13);

    rz_register_t rs1_val = pcpu->r_x[instr.b.rs1];
    rz_register_t rs2_val = pcpu->r_x[instr.b.rs2];

    bool take = false;
    switch (instr.b.f3) {
        case BEQ:
            take = (rs1_val == rs2_val);
        break;
        case BNE:
            take = (rs1_val != rs2_val);
        break;
        case BLT:
            take = ((int32_t)rs1_val < (int32_t)rs2_val);
        break;
        case BGE:
            take = ((int32_t)rs1_val >= (int32_t)rs2_val);
        break;
        case BLTU:
            take = (rs1_val < rs2_val);
        break;
        case BGEU:
            take = (rs1_val >= rs2_val);
        break;
        default:
            return false;
    }

    if (take) {
        pcpu->r_pc += (imm - 4);
    }

    return true;
}

static bool rz_j_cycle(rz_cpu_p pcpu, rz_instruction_t instr) {
    int32_t imm = ((instr.j.imm20 << 20)
                   | (instr.j.imm12_19 << 12)
                   | (instr.j.imm11 << 11)
                   | (instr.j.imm1_10 << 1));
    imm = sign_extend(imm, 21);

    pcpu->r_x[instr.j.rd] = pcpu->r_pc + 4;
    pcpu->r_pc += (imm - 4);
    return true;
}

static bool rz_jalr_cycle(rz_cpu_p pcpu, rz_instruction_t instr) {
    int32_t imm = sign_extend(instr.i.imm0_11, 12);
    rz_register_t base = pcpu->r_x[instr.i.rs1];
    rz_register_t target = (base + imm) & ~1U;

    pcpu->r_x[instr.i.rd] = pcpu->r_pc + 4;
    pcpu->r_pc = target - 4;
    return true;
}

bool rz_cycle(rz_cpu_p pcpu) {
    pcpu->r_x[0] = 0u;
    rz_instruction_t instr = *((rz_instruction_t *)mem_access(pcpu->r_pc));
    bool goon = true;

    switch(instr.whole & OPCODE_MASK) {
        case R_FORMAT:
            goon = rz_r_cycle(pcpu, instr);
        break;
        case I_FORMAT:
            goon = rz_i_cycle(pcpu, instr);
        break;
        case L_FORMAT:
        break;
        case S_FORMAT:
        break;
        case LUI_FORMAT:
            pcpu->r_x[instr.u.rd] = instr.u.imm12_31 << 12;
        break;
        case AUIPC_FORMAT:
            pcpu->r_x[instr.u.rd] = (instr.u.imm12_31 << 12) + pcpu->r_pc;
        break;
        case J_FORMAT:
            goon = rz_j_cycle(pcpu, instr);
        break;
        case JALR_FORMAT:
            goon = rz_jalr_cycle(pcpu, instr);
        break;
        case MEM_FORMAT:
        break;
        case SYS_FORMAT:
            goon = rz_sys_cycle(pcpu, instr);
        break;
        case B_FORMAT:
            goon = rz_b_cycle(pcpu, instr);
        break;
        default:
            fprintf(stderr, "Invalid instruction %08X format, opcode %08X\n", instr.whole, instr.whole & OPCODE_MASK);
            goon = false;
    }
    pcpu->r_pc += sizeof(rz_instruction_t);
    return goon;
}
