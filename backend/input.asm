.data
array:  .word 10, 3, 8, 2, 7, 5, 4, 6  # Example array
size:   .word 8                        # Array size

.text

_start:
    lui x5, 0x10000       # Load array address
    lui x6, 0x10000       # Load array size
    addi x6, x6, 32
    addi x6, x6, -1    # Adjust for 0-based index

    addi x2, x2, -16   # Allocate stack
    sw x5, 0(x2)       # Store base address
    sw x6, 4(x2)       # Store high index
    jal x1, quick_sort # Call quick_sort

infinite_loop:
    beq x0, x0, infinite_loop # Keep looping (stop execution)

# quick_sort(base, low, high)
quick_sort:
    bge x6, x7, return_qs  # if low >= high, return

    addi x2, x2, -16       # Push stack
    sw x5, 0(x2)           # Store base address
    sw x6, 4(x2)           # Store low
    sw x7, 8(x2)           # Store high
    sw x1, 12(x2)          # Store return address

    jal x1, partition      # Call partition, returns pivot in x10
    add x8, x10, x0        # Store pivot

    addi x6, x8, -1        # low to pivot-1
    jal x1, quick_sort     # Recursive call

    lw x5, 0(x2)           # Restore base
    lw x7, 8(x2)           # Restore high

    addi x6, x8, 1         # pivot+1 to high
    jal x1, quick_sort     # Recursive call

    lw x1, 12(x2)          # Restore return
    addi x2, x2, 16        # Pop stack
    jalr x0, x1, 0         # Return

return_qs:
    jalr x0, x1, 0         # Return

# partition(base, low, high) -> returns pivot index in x10
partition:
    lw x9, 0(x5)        # Pivot = arr[low]
    add x11, x6, x0     # i = low
    add x12, x7, x0     # j = high

partition_loop:
    lw x13, 0(x5)       # Load arr[i]
    bge x9, x13, found_i
    addi x11, x11, 1
    beq x0, x0, partition_loop

found_i:
    lw x14, 0(x5)       # Load arr[j]
    blt x9, x14, found_j
    addi x12, x12, -1
    beq x0, x0, partition_loop

found_j:
    bge x11, x12, ret_partition
    lw x15, 0(x5)       # Swap arr[i] and arr[j]
    sw x14, 0(x5)
    sw x15, 0(x5)
    addi x11, x11, 1
    addi x12, x12, -1
    beq x0, x0, partition_loop

ret_partition:
    add x10, x12, x0    # Return pivot index
    jalr x0, x1, 0      # Return
