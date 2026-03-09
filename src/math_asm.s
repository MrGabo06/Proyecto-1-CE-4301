# Assembly function to calculate sum from 1 to n
# Function signature: int sum_to_n(int n)
# a0 = input parameter n
# a0 = return value

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
    # reservar stack
    addi sp, sp, -64
    sw ra, 60(sp)
    sw s0, 56(sp)
    sw s1, 52(sp)
    sw s2, 48(sp)
    sw s3, 44(sp)
    sw s4, 40(sp)
    sw s5, 36(sp)
    sw s6, 32(sp)
    sw s7, 28(sp)
    sw s8, 24(sp)
    sw s9, 20(sp)
    sw s10, 16(sp)
    sw s11, 12(sp)

    # Estado ChaCha:
    # a4   = 0x61707865
    # a5   = 0x3320646e
    # a6   = 0x79622d32
    # a7   = 0x6b206574
    # s0   = key[0]
    # s1   = key[1]
    # s2   = key[2]
    # s3   = key[3]
    # s4   = key[4]
    # s5   = key[5]
    # s6   = key[6]
    # s7   = key[7]
    # s8   = counter
    # s9   = nonce[0]
    # s10  = nonce[1]
    # s11  = nonce[2]

    # constantes
    li a4, 0x61707865
    li a5, 0x3320646e
    li a6, 0x79622d32
    li a7, 0x6b206574

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

    mv a0, a4

    # restaurar registros
    lw s11, 12(sp)
    lw s10, 16(sp)
    lw s9, 20(sp)
    lw s8, 24(sp)
    lw s7, 28(sp)
    lw s6, 32(sp)
    lw s5, 36(sp)
    lw s4, 40(sp)
    lw s3, 44(sp)
    lw s2, 48(sp)
    lw s1, 52(sp)
    lw s0, 56(sp)
    lw ra, 60(sp)
    addi sp, sp, 64
    ret







