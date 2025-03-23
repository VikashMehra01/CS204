.data 
arr: .word 4, 5, 6

.text
addi x3, x0, 1
lui x4, 0x10000
lw x5, 0(x4)
lw x6, 4(x4)
lw x7, 8(x4)
add x3, x3,  x5
add x3, x3, x6
add x3, x3, x7
