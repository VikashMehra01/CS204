#include "Assembler.h"
#include "Simulator.h"

#include <iostream>

int main()
{
    Assembler assembler("input.asm", "output.mc", "PC.pc");
    Simulator simulator("PC.pc", "memory.mem", "register.reg");
    cout << "Vikash" << endl;
    return 0;
}