.data
data: .word 89

.text
addi x2, x0, 2
addi x3, x0, 3
beq x3, x2, exit
addi x8, x8, 6

exit:
addi x9, x8, 9


