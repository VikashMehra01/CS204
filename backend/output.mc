0x10000000 0xa 0x0 0x0 0x0 
0x10000004 # Data segment start
0x0 # Text segment start
0x0 0x10000537  lui x10, 0x10000 # 0110111-XX-01010-XX-00010000000000000000
0x4 0x00052503  lw x10, x10 0 # 0000011-010-000000000000-01010-01010
0x8 0x00152593  slti x11, x10, 1 # 0010011-010-000000000001-01011-01010
0xc 0x04059A63  bne x11, x0, end # 1100011-001-XX-01011-00000-XX-0000001010100
0x10 0x00000293  addi x5, x0, 0 # 0010011-000-000000000000-00101-00000
0x14 0x00100313  addi x6, x0, 1 # 0010011-000-000000000001-00110-00000
0x18 0x100043B7  lui x7, 0x10004 # 0110111-XX-00111-XX-00010000000000000100
0x1c 0x0053A023  sw x5, 0(x7) # 0100011-010-000000000000-00101-00111
0x20 0x00438393  addi x7, x7, 4 # 0010011-000-000000000100-00111-00111
0x24 0xFFF50513  addi x10, x10, -1 # 0010011-000-111111111111-01010-01010
0x28 0x00152593  slti x11, x10, 1 # 0010011-010-000000000001-01011-01010
0x2c 0x02059A63  bne x11, x0, end # 1100011-001-XX-01011-00000-XX-0000000110100
0x30 0x0063A023  sw x6, 0(x7) # 0100011-010-000000000000-00110-00111
0x34 0x00438393  addi x7, x7, 4 # 0010011-000-000000000100-00111-00111
0x38 0xFFF50513  addi x10, x10, -1 # 0010011-000-111111111111-01010-01010
0x3c 0x00152593  slti x11, x10, 1 # 0010011-010-000000000001-01011-01010
0x40 0x02059063  bne x11, x0, end # 1100011-001-XX-01011-00000-XX-0000000100000
0x44 0x00628433  add x8, x5, x6 # 0110011-000-0000000-01000-00101-00110
0x48 0x0083A023  sw x8, 0(x7) # 0100011-010-000000000000-01000-00111
0x4c 0x00030293  addi x5, x6, 0 # 0010011-000-000000000000-00101-00110
0x50 0x00040313  addi x6, x8, 0 # 0010011-000-000000000000-00110-01000
0x54 0x00438393  addi x7, x7, 4 # 0010011-000-000000000100-00111-00111
0x58 0xFFF50513  addi x10, x10, -1 # 0010011-000-111111111111-01010-01010
0x5c 0xFE1FF06F  jal x0, loop # 1101111-XX-00000-XX-111111111111111100000
0x60 0x05D00893  addi x17, x0, 93 # 0010011-000-000001011101-10001-00000
0x64 0x00000513  addi x10, x0, 0 # 0010011-000-000000000000-01010-00000
Vikash 0x00000013  # End of program
