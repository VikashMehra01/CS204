.data
n: .byte 3

.text
label:
addi x3, x3, -1
lb x5, 0(x2)
lb x6, 1(x2) #swapping of help and to
jal x1, func
lb x3, 3(x2)
lb x5, 1(x2)
lb x4, 2(x2)
add x8, x5, x0
addi x31, x0, 4
sll x4, x4, x31
add x8, x8, x4
addi x31, x0, 8
sll x3, x3, x31
add x8, x8, x3
sw x8, 0(x20)
addi x20, x20, 4
lb x3, 3(x2)
addi x3, x3, -1
lb x4, 0(x2) #from
lb x5, 1(x2) #to
lb x6, 2(x2) #help #swapping of from and help
jal x1, func
lw x1, 4(x2)
addi x2, x2, 8
jalr x0, 0(x1)
func:
exit: