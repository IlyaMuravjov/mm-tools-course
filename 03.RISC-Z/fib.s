        .text
        .globl _start

_start:
        li a0, 2
        ecall
        mv a0, a1
        jal ra, fib
        mv a1, a0
        li a0, 1
        ecall
        ebreak

fib:
        li a1, 0
        li a2, 1

fib_loop:
        beq a0, zero, fib_end
        add a3, a1, a2
        mv a1, a2
        mv a2, a3
        addi a0, a0, -1
        jal zero, fib_loop

fib_end:
        mv a0, a1
        jalr zero, ra
