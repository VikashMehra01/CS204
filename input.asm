# R-type
add x1, x2, x3
sub x4, x5, x6
sll x7, x8, x9
slt x10, x11, x12
sltu x13, x14, x15
xor x16, x17, x18
srl x19, x20, x21
sra x22, x23, x24
or x25, x26, x27
and x28, x29, x30


# I-type
addi x1, x2, 5
slti x3, x4, 10
sltiu x5, x6, 15
xori x7, x8, 20
ori x9, x10, 25
andi x11, x12, 30
slli x13, x14, 2
srli x15, x16, 3
srai x17, x18, 4
jalr x19, x20, 0

# S-type
sw x1, 0(x2)
sd x1, 200(x2)
sh x3, 4(x4)
sb x5, 8(x6)

# B-type
label1:
beq x1, x2, label1
bne x3, x4, label1
blt x5, x6, label1
bge x7, x8, label1
bltu x9, x10, label1
bgeu x11, x12, label1

# U-type
lui x1, 0x10000
auipc x2, 0x20000

# J-type
jal x3, label1
