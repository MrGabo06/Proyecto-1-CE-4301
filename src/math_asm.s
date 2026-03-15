.section .text
.globl block
.globl quarter_round
.globl chacha20_encrypt


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
    sw s1, 136(sp)

    # Estado original y de trabajo
    li t0, 0x61707865
    sw t0, 0(sp)    # original 0
    sw t0, 64(sp)   # working 0

    li t0, 0x3320646e
    sw t0, 4(sp)    # original 1
    sw t0, 68(sp)   # working 1

    li t0, 0x79622d32
    sw t0, 8(sp)    # original 2
    sw t0, 72(sp)   # working 2

    li t0, 0x6b206574
    sw t0, 12(sp)   # original 3
    sw t0, 76(sp)   # working 3

    # key
    lw t0, 0(a0)
    sw t0, 16(sp)   # original 4
    sw t0, 80(sp)   # working 4

    lw t0, 4(a0)
    sw t0, 20(sp)   # original 5
    sw t0, 84(sp)   # working 5

    lw t0, 8(a0)
    sw t0, 24(sp)   # original 6
    sw t0, 88(sp)   # working 6

    lw t0, 12(a0)
    sw t0, 28(sp)   # original 7
    sw t0, 92(sp)   # working 7

    lw t0, 16(a0)
    sw t0, 32(sp)   # original 8
    sw t0, 96(sp)   # working 8

    lw t0, 20(a0)
    sw t0, 36(sp)   # original 9
    sw t0, 100(sp)  # working 9

    lw t0, 24(a0)
    sw t0, 40(sp)   # original 10
    sw t0, 104(sp)  # working 10

    lw t0, 28(a0)
    sw t0, 44(sp)   # original 11
    sw t0, 108(sp)  # working 11

    # counter
    mv t0, a2
    sw t0, 48(sp)   # original 12
    sw t0, 112(sp)  # working 12

    # nonce
    lw t0, 0(a1)
    sw t0, 52(sp)   # original 13
    sw t0, 116(sp)  # working 13

    lw t0, 4(a1)
    sw t0, 56(sp)   # original 14
    sw t0, 120(sp)  # working 14

    lw t0, 8(a1)
    sw t0, 60(sp)   # original 15
    sw t0, 124(sp)  # working 15

    # contador de loop_rounds
    li s1, 10

    # guardar buffer de salida
    mv t0, a3
    sw t0, 132(sp)

loop_rounds:
    # columnas
    lw a0, 64(sp)
    lw a1, 80(sp)
    lw a2, 96(sp)
    lw a3, 112(sp)
    jal ra, quarter_round
    sw a0, 64(sp)
    sw a1, 80(sp)
    sw a2, 96(sp)
    sw a3, 112(sp)

    lw a0, 68(sp)
    lw a1, 84(sp)
    lw a2, 100(sp)
    lw a3, 116(sp)
    jal ra, quarter_round
    sw a0, 68(sp)
    sw a1, 84(sp)
    sw a2, 100(sp)
    sw a3, 116(sp)

    lw a0, 72(sp)
    lw a1, 88(sp)
    lw a2, 104(sp)
    lw a3, 120(sp)
    jal ra, quarter_round
    sw a0, 72(sp)
    sw a1, 88(sp)
    sw a2, 104(sp)
    sw a3, 120(sp)

    lw a0, 76(sp)
    lw a1, 92(sp)
    lw a2, 108(sp)
    lw a3, 124(sp)
    jal ra, quarter_round
    sw a0, 76(sp)
    sw a1, 92(sp)
    sw a2, 108(sp)
    sw a3, 124(sp)

    # diagonales
    lw a0, 64(sp)
    lw a1, 84(sp)
    lw a2, 104(sp)
    lw a3, 124(sp)
    jal ra, quarter_round
    sw a0, 64(sp)
    sw a1, 84(sp)
    sw a2, 104(sp)
    sw a3, 124(sp)

    lw a0, 68(sp)
    lw a1, 88(sp)
    lw a2, 108(sp)
    lw a3, 112(sp)
    jal ra, quarter_round
    sw a0, 68(sp)
    sw a1, 88(sp)
    sw a2, 108(sp)
    sw a3, 112(sp)

    lw a0, 72(sp)
    lw a1, 92(sp)
    lw a2, 96(sp)
    lw a3, 116(sp)
    jal ra, quarter_round
    sw a0, 72(sp)
    sw a1, 92(sp)
    sw a2, 96(sp)
    sw a3, 116(sp)

    lw a0, 76(sp)
    lw a1, 80(sp)
    lw a2, 100(sp)
    lw a3, 120(sp)
    jal ra, quarter_round
    sw a0, 76(sp)
    sw a1, 80(sp)
    sw a2, 100(sp)
    sw a3, 120(sp)

    addi s1, s1, -1
    bnez s1, loop_rounds

    # suma final
    lw t0, 0(sp)
    lw t1, 64(sp)
    add t1, t1, t0
    sw t1, 64(sp)

    lw t0, 4(sp)
    lw t1, 68(sp)
    add t1, t1, t0
    sw t1, 68(sp)

    lw t0, 8(sp)
    lw t1, 72(sp)
    add t1, t1, t0
    sw t1, 72(sp)

    lw t0, 12(sp)
    lw t1, 76(sp)
    add t1, t1, t0
    sw t1, 76(sp)

    lw t0, 16(sp)
    lw t1, 80(sp)
    add t1, t1, t0
    sw t1, 80(sp)

    lw t0, 20(sp)
    lw t1, 84(sp)
    add t1, t1, t0
    sw t1, 84(sp)

    lw t0, 24(sp)
    lw t1, 88(sp)
    add t1, t1, t0
    sw t1, 88(sp)

    lw t0, 28(sp)
    lw t1, 92(sp)
    add t1, t1, t0
    sw t1, 92(sp)

    lw t0, 32(sp)
    lw t1, 96(sp)
    add t1, t1, t0
    sw t1, 96(sp)

    lw t0, 36(sp)
    lw t1, 100(sp)
    add t1, t1, t0
    sw t1, 100(sp)

    lw t0, 40(sp)
    lw t1, 104(sp)
    add t1, t1, t0
    sw t1, 104(sp)

    lw t0, 44(sp)
    lw t1, 108(sp)
    add t1, t1, t0
    sw t1, 108(sp)

    lw t0, 48(sp)
    lw t1, 112(sp)
    add t1, t1, t0
    sw t1, 112(sp)

    lw t0, 52(sp)
    lw t1, 116(sp)
    add t1, t1, t0
    sw t1, 116(sp)

    lw t0, 56(sp)
    lw t1, 120(sp)
    add t1, t1, t0
    sw t1, 120(sp)

    lw t0, 60(sp)
    lw t1, 124(sp)
    add t1, t1, t0
    sw t1, 124(sp)

    # escribir keystream en el buffer de salida
    lw t2, 132(sp)

    lw t0, 64(sp)
    sw t0, 0(t2)

    lw t0, 68(sp)
    sw t0, 4(t2)

    lw t0, 72(sp)
    sw t0, 8(t2)

    lw t0, 76(sp)
    sw t0, 12(t2)

    lw t0, 80(sp)
    sw t0, 16(t2)

    lw t0, 84(sp)
    sw t0, 20(t2)

    lw t0, 88(sp)
    sw t0, 24(t2)

    lw t0, 92(sp)
    sw t0, 28(t2)

    lw t0, 96(sp)
    sw t0, 32(t2)

    lw t0, 100(sp)
    sw t0, 36(t2)

    lw t0, 104(sp)
    sw t0, 40(t2)

    lw t0, 108(sp)
    sw t0, 44(t2)

    lw t0, 112(sp)
    sw t0, 48(t2)

    lw t0, 116(sp)
    sw t0, 52(t2)

    lw t0, 120(sp)
    sw t0, 56(t2)

    lw t0, 124(sp)
    sw t0, 60(t2)

    # devolver puntero de salida en a0
    lw a0, 132(sp)

    # restaurar registros
    lw s1, 136(sp)
    lw ra, 140(sp)
    addi sp, sp, 144
    ret


chacha20_encrypt:
    # reservar memoria
    addi sp, sp, -144
    sw ra, 140(sp)
    sw s2, 136(sp)
    sw s3, 132(sp)
    sw s4, 128(sp)
    sw s5, 124(sp)
    sw s6, 120(sp)
    sw s7, 116(sp)
    sw s8, 112(sp)

    # guardar key, nonce, counter, plaintext y out
    mv s4, a0
    mv s5, a1
    mv s6, a2
    mv s7, a3
    mv s8, a5

    # calcular cantidad de loops
    mv t0, a4
    li t1, 64
    div s2, t0, t1  # cantidad de loops de bloques completos
    rem s3, t0, t1  # cantidad de loops en el bloque incompleto

    # verifica si el bloque que viene esta completo
    beq s2, zero, check_incomplete
    

complete_block_loop:
    mv a0, s4
    mv a1, s5
    mv a2, s6
    mv a3, s8
    jal ra, block

    mv t0, a0
    lw t1, 0(t0)
    lw t2, 0(s7)
    xor t3, t1, t2
    sw t3, 0(s8)

    mv t0, a0
    lw t1, 4(t0)
    lw t2, 4(s7)
    xor t3, t1, t2
    sw t3, 4(s8)

    mv t0, a0
    lw t1, 8(t0)
    lw t2, 8(s7)
    xor t3, t1, t2
    sw t3, 8(s8)

    mv t0, a0
    lw t1, 12(t0)
    lw t2, 12(s7)
    xor t3, t1, t2
    sw t3, 12(s8)

    mv t0, a0
    lw t1, 16(t0)
    lw t2, 16(s7)
    xor t3, t1, t2
    sw t3, 16(s8)

    mv t0, a0
    lw t1, 20(t0)
    lw t2, 20(s7)
    xor t3, t1, t2
    sw t3, 20(s8)

    mv t0, a0
    lw t1, 24(t0)
    lw t2, 24(s7)
    xor t3, t1, t2
    sw t3, 24(s8)

    mv t0, a0
    lw t1, 28(t0)
    lw t2, 28(s7)
    xor t3, t1, t2
    sw t3, 28(s8)

    mv t0, a0
    lw t1, 32(t0)
    lw t2, 32(s7)
    xor t3, t1, t2
    sw t3, 32(s8)

    mv t0, a0
    lw t1, 36(t0)
    lw t2, 36(s7)
    xor t3, t1, t2
    sw t3, 36(s8)

    mv t0, a0
    lw t1, 40(t0)
    lw t2, 40(s7)
    xor t3, t1, t2
    sw t3, 40(s8)

    mv t0, a0
    lw t1, 44(t0)
    lw t2, 44(s7)
    xor t3, t1, t2
    sw t3, 44(s8)

    mv t0, a0
    lw t1, 48(t0)
    lw t2, 48(s7)
    xor t3, t1, t2
    sw t3, 48(s8)

    mv t0, a0
    lw t1, 52(t0)
    lw t2, 52(s7)
    xor t3, t1, t2
    sw t3, 52(s8)

    mv t0, a0
    lw t1, 56(t0)
    lw t2, 56(s7)
    xor t3, t1, t2
    sw t3, 56(s8)

    mv t0, a0
    lw t1, 60(t0)
    lw t2, 60(s7)
    xor t3, t1, t2
    sw t3, 60(s8)

    mv a0, s4
    mv a1, s5
    addi s6, s6, 1
    mv a2, s6
    addi s8, s8, 64
    addi s7, s7, 64
    addi s2, s2, -1
    bnez s2, complete_block_loop


    # verifica si hay bytes sueltos
check_incomplete:
    beq s3, zero, end_encrypt

last_block:
    mv a0, s4
    mv a1, s5
    mv a2, s6
    addi t0, sp, 0
    mv a3, t0
    jal ra, block
    mv t0, a0        

block_incomplete_loop:
    lbu t1, 0(t0)
    lbu t2, 0(s7)
    xor t3, t1, t2
    sb t3, 0(s8)

    addi t0, t0, 1
    addi s7, s7, 1
    addi s8, s8, 1
    addi s3, s3, -1
    bnez s3, block_incomplete_loop

end_encrypt:
    # devolver puntero al plaintext copiado
    mv a0, s8

    # restaurar registros
    lw s8, 112(sp)
    lw s7, 116(sp)
    lw s6, 120(sp)
    lw s5, 124(sp)
    lw s4, 128(sp)
    lw s3, 132(sp)
    lw s2, 136(sp)
    lw ra, 140(sp)
    addi sp, sp, 144
    ret























