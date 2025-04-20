#Vikash Mehra
#2023CSB1172
#Question NO - 1

.data
size: .word 6
array: .word 7, 5, 6, 1, 9, 17

.text
lui x5, 0x10000
lw x10, 0(x5) # size
addi x11, x5, 4 #address of array
lui x16, 0x10001

addi x12, x0, 0 #left
addi x13, x10, -1 #right

jal x1, .mergeSort

beq x0, x0, .exit

.mergeSort:
    bge x12, x13, .mergeSort_return
    
    sub x5, x13, x12
    srli x5, x5, 1
    add x14, x5, x12 #mid
    
    addi x2, x2, -16
    
    #storing in to the stack
    sw x12, 0(x2)
    sw x13, 4(x2)
    sw x14, 8(x2)
    sw x1, 12(x2)
    
    add x13, x14, x0 # right == mid
    jal x1, .mergeSort   #calling merge for left
    
    lw x12, 0(x2)
    lw x13, 4(x2)
    lw x14, 8(x2)
    
    addi x12, x14, 1
    jal x1, .mergeSort
    
    
    #restoring data for merging
    lw x12, 0(x2)
    lw x13, 4(x2)
    lw x14, 8(x2)
    lw x1, 12(x2)
    
    addi x2, x2, 16
    
    jal x0, .merge
    
.mergeSort_return:
    jalr x0, x1, 0 
    
    
    
    
.merge:
    addi x5, x12, 0 #i
    addi x6, x14, 1 #j
    addi x7, x0, 0  #k
    
    .merge_loop:
    blt x14, x5, .merge_copy_j
    blt x13, x6, .merge_copy_i
    
    #loading a[i]
    slli x28, x5, 2
    add x28, x28, x11
    lw x28, 0(x28)
    
    #loading a[j]
    slli x29, x6, 2
    add x29, x29, x11
    lw x29, 0(x29)
    
    blt x28, x29, .merge_take_i
    beq x0, x0, .merge_take_j
       
 .merge_take_i:
    slli x30, x7, 2
    add x30, x30, x16 #address of b[k]
    
    sw x28, 0(x30)
    
    addi x5, x5, 1
    addi x7, x7, 1
    
    beq x0, x0, .merge_loop
    
    
 .merge_take_j:
    slli x30, x7, 2
    add x30, x30, x16
     
    sw x29, 0(x30)
    
    addi x6, x6, 1
    addi x7, x7, 1
    
    beq x0, x0, .merge_loop
    
.merge_copy_i:
    blt x14, x5, .copy_done
    
    slli x28, x5, 2
    add x28, x28, x11
    
    lw x28, 0(x28)
    
    slli x29, x7, 2
    add x29, x29, x16
    
    sw x28, 0(x29)
    
    addi x5, x5, 1
    addi x7, x7, 1
    
    beq x0, x0, .merge_copy_i
    
    
.merge_copy_j:
    blt x13, x6, .copy_done
    
    slli x28, x6, 2
    add x28, x28, x11
    
    lw x28, 0(x28)
    
    slli x29, x7, 2
    add x29, x29, x16
    
    sw x28, 0(x29)
    
    addi x6, x6, 1
    addi x7, x7, 1
    
    beq x0, x0, .merge_copy_j
    
    
    
.copy_done:
    add x5, x12, x0
    add x7, x0, x0
    
    .copy_loop:
    blt x13, x5, .done
    
    slli x28, x5, 2
    add x28, x28, x11
    
    slli x29, x7, 2
    add x29, x29, x16
    
    lw x29, 0(x29)
    sw x29, 0(x28)
    
    addi x5, x5, 1
    addi x7, x7, 1
    
    beq x0, x0, .copy_loop
    
    
.done:
    jalr x0, x1, 0
    
.exit: