#include "Assembler.h"
#include "Simulator.h"

#include <iostream>

int main()
{
    Assembler assembler;
    assembler.assembleFile("input.asm", "output.txt", "PC.pc");
    Simulator simulator("PC.pc", "memory.bin", "register.reg");
    cout << "Vikash" << endl;
    return 0;
}