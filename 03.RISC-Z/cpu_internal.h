#ifndef __CPU_INTERNAL_H__
#define __CPU_INTERNAL_H__

#include "memory.h"

#define FUNC3_OFFS 12
#define FUNC7_OFFS 24
#define OPCODE_MASK 0b1111111u
#define FUNC3_MASK  (0b111u << FUNC3_OFFS)
#define FUNC7_MASK  (0b1111111u << FUNC7_OFFS)

/**
 * @brief Opcodes to determine instruction format
 * 
 */
enum rz_formats { // C23
    LUI_FORMAT    = 0b0110111u,
    AUIPC_FORMAT  = 0b0010111u,
    J_FORMAT  = 0b1101111u,
    JALR_FORMAT = 0b1100111u, // JALR
    R_FORMAT  = 0b0110011u,
    S_FORMAT  = 0b0100011u,
    L_FORMAT =  0b0000011u, // Load, I-format
    I_FORMAT = 0b0010011u, // ADDI..ANDI, SLLI, SRLI, SRAI
    MEM_FORMAT = 0b0001111u, // e.g. FENCE
    SYS_FORMAT = 0b1110011u, // e.g. ECALL, EBREAK
    B_FORMAT  = 0b1100011u,
};

/**
 * @brief R-format CODE | F3 | F7
 * 
 */
enum rz_r_codes {
    ADD_CODE = R_FORMAT | ( 0b000u << FUNC3_OFFS ) | ( 0b0000000u << FUNC7_OFFS ),
    SUB_CODE = R_FORMAT | ( 0b000u << FUNC3_OFFS ) | ( 0b0100000u << FUNC7_OFFS ),
    XOR_CODE = R_FORMAT | ( 0b100u << FUNC3_OFFS ) | ( 0b0000000u << FUNC7_OFFS ),
};

/**
  * @brief I-format code | F3 [ | F7 ]
  * 
 */
enum rz_i_codes {
    ADDI_CODE = I_FORMAT | ( 0b000u << FUNC3_OFFS ),
    SLLI_CODE = I_FORMAT | ( 0b001u << FUNC3_OFFS ),
};

/**
 * @brief U-format code
 * 
 */
enum rz_u_codes {
    LUI_CODE = LUI_FORMAT,
    AUIPC_CODE = AUIPC_FORMAT,
};

enum rz_sys_codes {
    ECALL_CODE = SYS_FORMAT,
    EBREAK_CODE = SYS_FORMAT | ( 1u << 20 ),
};

enum rz_ecall_codes {
    PRINT_INT = 1,
    READ_INT = 2,
};

enum rz_branch_codes {
    BEQ = 0b000,
    BNE = 0b001,
    BLT = 0b100,
    BGE = 0b101,
    BLTU = 0b110,
    BGEU = 0b111,
};

/**
 * @brief Represent RISC-V machine code for LE host-machines
 * 
 */
typedef union {
    rz_register_t whole;
    struct {
        unsigned op: 7;
        unsigned rd: 5;
        unsigned f3: 3;
        unsigned rs1: 5;
        unsigned rs2: 5;
        unsigned f7: 7;
    } r;
    struct {
        unsigned op: 7;
        unsigned rd: 5;
        unsigned f3: 3;
        unsigned rs1: 5;
        unsigned imm0_11: 12;
    } i;
    struct {
        unsigned op: 7;
        unsigned imm0_4: 5;
        unsigned f3: 3;
        unsigned rs1: 5;
        unsigned rs2: 5;
        unsigned imm5_11: 7;
    } s;
    struct {
        unsigned op: 7;
        unsigned imm11: 1;
        unsigned imm1_4: 4;
        unsigned f3: 3;
        unsigned rs1: 5;
        unsigned rs2: 5;
        unsigned imm5_10: 6;
        unsigned imm12: 1;
    } b;
    struct {
        unsigned op: 7;
        unsigned rd: 5;
        unsigned imm12_31: 20;
    } u;
    struct {
        unsigned op: 7;
        unsigned rd: 5;
        unsigned imm12_19: 8;
        unsigned imm11: 1;
        unsigned imm1_10: 10;
        unsigned imm20: 1;
    } j;
} rz_instruction_t;

struct rz_cpu_s {
    const char *info;
    rz_register_t r_pc, r_x[32];
};

#endif // __CPU_INTERNAL_H__
