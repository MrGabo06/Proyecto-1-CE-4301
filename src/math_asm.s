.section .text
.globl block
.globl quarter_round


quarter_round:
    #Se cargan las variables
    mv t0, a0
    mv t1, a1
    mv t2, a2
    mv t3, a3


    #Logica de los rounds
    add t0, t0, t1
    xor t3, t3, t0

    slli t4, t3, 16
    srli t5, t3, 16
    or   t3, t4, t5

    add t2, t2, t3
    xor t1, t1, t2

    slli t4, t1, 12
    srli t5, t1, 20
    or   t1, t4, t5

    add t0, t0, t1
    xor t3, t3, t0

    slli t4, t3, 8
    srli t5, t3, 24
    or   t3, t4, t5

    add t2, t2, t3
    xor t1, t1, t2

    slli t4, t1, 7
    srli t5, t1, 25
    or   t1, t4, t5

    #Se carga el resultado
    mv a0, t0
    mv a1, t1
    mv a2, t2
    mv a3, t3
    ret


block:
    # memoria reservada
    addi sp, sp, -144
    sw ra, 140(sp)
    sw s0, 136(sp)
    sw s1, 132(sp)
    sw s2, 128(sp)
    sw s3, 124(sp)
    sw s4, 120(sp)
    sw s5, 116(sp)
    sw s6, 112(sp)
    sw s7, 108(sp)
    sw s8, 104(sp)
    sw s9, 100(sp)
    sw s10, 96(sp)
    sw s11, 92(sp)

    # puntero de salida
    sw a3, 88(sp)

    # constantes
    li a4, 0x61707865
    li a5, 0x3320646e
    li a6, 0x79622d32
    li a7, 0x6b206574
    li t6, 10

    # key
    lw s0, 0(a0)
    lw s1, 4(a0)
    lw s2, 8(a0)
    lw s3, 12(a0)
    lw s4, 16(a0)
    lw s5, 20(a0)
    lw s6, 24(a0)
    lw s7, 28(a0)

    # counter
    mv s8, a2

    # nonce 
    lw s9, 0(a1)
    lw s10, 4(a1)
    lw s11, 8(a1)

    # estado original
    sw a4, 24(sp)
    sw a5, 28(sp)
    sw a6, 32(sp)
    sw a7, 36(sp)
    sw s0, 40(sp)
    sw s1, 44(sp)
    sw s2, 48(sp)
    sw s3, 52(sp)
    sw s4, 56(sp)
    sw s5, 60(sp)
    sw s6, 64(sp)
    sw s7, 68(sp)
    sw s8, 72(sp)
    sw s9, 76(sp)
    sw s10, 80(sp)
    sw s11, 84(sp)

loop_rounds:

    # Iteracion de rounds
    mv a0, a4
    mv a1, s0
    mv a2, s4
    mv a3, s8
    jal ra, quarter_round
    mv a4, a0
    mv s0, a1
    mv s4, a2
    mv s8, a3

   
    mv a0, a5
    mv a1, s1
    mv a2, s5
    mv a3, s9
    jal ra, quarter_round
    mv a5, a0
    mv s1, a1
    mv s5, a2
    mv s9, a3

    
    mv a0, a6
    mv a1, s2
    mv a2, s6
    mv a3, s10
    jal ra, quarter_round
    mv a6, a0
    mv s2, a1
    mv s6, a2
    mv s10, a3

   
    mv a0, a7
    mv a1, s3
    mv a2, s7
    mv a3, s11
    jal ra, quarter_round
    mv a7, a0
    mv s3, a1
    mv s7, a2
    mv s11, a3

  
    mv a0, a4
    mv a1, s1
    mv a2, s6
    mv a3, s11
    jal ra, quarter_round
    mv a4, a0
    mv s1, a1
    mv s6, a2
    mv s11, a3

   
    mv a0, a5
    mv a1, s2
    mv a2, s7
    mv a3, s8
    jal ra, quarter_round
    mv a5, a0
    mv s2, a1
    mv s7, a2
    mv s8, a3

   
    mv a0, a6
    mv a1, s3
    mv a2, s4
    mv a3, s9
    jal ra, quarter_round
    mv a6, a0
    mv s3, a1
    mv s4, a2
    mv s9, a3

    
    mv a0, a7
    mv a1, s0
    mv a2, s5
    mv a3, s10
    jal ra, quarter_round
    mv a7, a0
    mv s0, a1
    mv s5, a2
    mv s10, a3

    addi t6, t6, -1
    bnez t6, loop_rounds

    # suma final 
    lw t0, 24(sp)
    add a4, a4, t0
    lw t0, 28(sp)
    add a5, a5, t0
    lw t0, 32(sp)
    add a6, a6, t0
    lw t0, 36(sp)
    add a7, a7, t0

    lw t0, 40(sp)
    add s0, s0, t0
    lw t0, 44(sp)
    add s1, s1, t0
    lw t0, 48(sp)
    add s2, s2, t0
    lw t0, 52(sp)
    add s3, s3, t0

    lw t0, 56(sp)
    add s4, s4, t0
    lw t0, 60(sp)
    add s5, s5, t0
    lw t0, 64(sp)
    add s6, s6, t0
    lw t0, 68(sp)
    add s7, s7, t0

    lw t0, 72(sp)
    add s8, s8, t0
    lw t0, 76(sp)
    add s9, s9, t0
    lw t0, 80(sp)
    add s10, s10, t0
    lw t0, 84(sp)
    add s11, s11, t0

    # escribir keystream completo 
    lw t0, 88(sp)

    sw a4,   0(t0)
    sw a5,   4(t0)
    sw a6,   8(t0)
    sw a7,  12(t0)
    sw s0,  16(t0)
    sw s1,  20(t0)
    sw s2,  24(t0)
    sw s3,  28(t0)
    sw s4,  32(t0)
    sw s5,  36(t0)
    sw s6,  40(t0)
    sw s7,  44(t0)
    sw s8,  48(t0)
    sw s9,  52(t0)
    sw s10, 56(t0)
    sw s11, 60(t0)


    lw a0, 88(sp)

    #restaurar registros
    lw s11, 92(sp)
    lw s10, 96(sp)
    lw s9, 100(sp)
    lw s8, 104(sp)
    lw s7, 108(sp)
    lw s6, 112(sp)
    lw s5, 116(sp)
    lw s4, 120(sp)
    lw s3, 124(sp)
    lw s2, 128(sp)
    lw s1, 132(sp)
    lw s0, 136(sp)
    lw ra, 140(sp)
    addi sp, sp, 144
    ret






