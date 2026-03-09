# Assembly function to calculate sum from 1 to n
# Function signature: int sum_to_n(int n)
# a0 = input parameter n
# a0 = return value

.section .text
.globl sum_to_n
.globl subtract_two_numbers
.globl quarter_round

.text
.globl quarter_round

quarter_round:
    mv t0, a0
    mv t1, a1
    mv t2, a2
    mv t3, a3

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

    mv a0, t0
    mv a1, t1
    mv a2, t2
    mv a3, t3
    ret








