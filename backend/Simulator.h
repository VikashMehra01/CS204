#ifndef SIMULATOR
#define SIMULATOR

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>

#include "memory.h"

using namespace std;

struct I_DATA
{
    string i_type;
    string opperation;
    int rd;
    int rs1;
    int rs2;
    int immi;
};

struct vector_hash
{
    template <typename T>
    size_t operator()(const vector<T> &v) const
    {
        size_t seed = v.size();
        for (const auto &x : v)
        {
            seed ^= hash<T>{}(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
class Simulator
{
private:
    /* data */
    int PC = 0x0;
    int EX;
    int limit_pc;
    unordered_map<int, string> PC_MAP;
    // unordered_map<int, int> Memory;
    unordered_map<int, int> Register{
        {0, 0},
        {1, 0},
        {2, 0},
        {3, 0},
        {4, 0},
        {5, 0},
        {6, 0},
        {7, 0},
        {8, 0},
        {9, 0},
        {10, 0},
        {11, 0},
        {12, 0},
        {13, 0},
        {14, 0},
        {15, 0},
        {16, 0},
        {17, 0},
        {18, 0},
        {19, 0},
        {20, 0},
        {21, 0},
        {22, 0},
        {23, 0},
        {24, 0},
        {25, 0},
        {26, 0},
        {27, 0},
        {28, 0},
        {29, 0},
        {30, 0},
        {31, 0},
    };
    unordered_map<vector<string>, string, vector_hash> operation = {
        {{"0110011", "0x00", "0x00"}, "add"},
        {{"0110011", "0x00", "0x20"}, "sub"},
        {{"0110011", "0x00", "0x01"}, "sll"},
        {{"0110011", "0x00", "0x02"}, "slt"},
        {{"0110011", "0x00", "0x03"}, "sltu"},
        {{"0110011", "0x00", "0x04"}, "xor"},
        {{"0110011", "0x00", "0x05"}, "srl"},
        {{"0110011", "0x20", "0x05"}, "sra"},
        {{"0110011", "0x00", "0x06"}, "or"},
        {{"0110011", "0x00", "0x07"}, "and"},

        {{"0010011", "0x00", "0x00"}, "addi"},
        {{"0010011", "0x00", "0x02"}, "slti"},
        {{"0010011", "0x00", "0x03"}, "sltiu"},
        {{"0010011", "0x00", "0x04"}, "xori"},
        {{"0010011", "0x00", "0x06"}, "ori"},
        {{"0010011", "0x00", "0x07"}, "andi"},
        {{"0010011", "0x00", "0x01"}, "slli"},
        {{"0010011", "0x00", "0x05"}, "srli"},
        {{"0010011", "0x20", "0x05"}, "srai"},

        {{"1100011", "0x00", "0x00"}, "beq"},
        {{"1100011", "0x00", "0x01"}, "bne"},
        {{"1100011", "0x00", "0x04"}, "blt"},
        {{"1100011", "0x00", "0x05"}, "bge"},
        {{"1100011", "0x00", "0x06"}, "bltu"},
        {{"1100011", "0x00", "0x07"}, "bgeu"},

        {{"0000011", "0x00", "0x00"}, "lb"},
        {{"0000011", "0x00", "0x01"}, "lh"},
        {{"0000011", "0x00", "0x02"}, "lw"},
        {{"0000011", "0x00", "0x04"}, "lbu"},
        {{"0000011", "0x00", "0x05"}, "lhu"},

        {{"0100011", "0x00", "0x00"}, "sb"},
        {{"0100011", "0x00", "0x01"}, "sh"},
        {{"0100011", "0x00", "0x02"}, "sw"},

        {{"1101111", "-1", "-1"}, "jal"},
        {{"1100111", "0x00", "-1"}, "jalr"},

        {{"0110111", "-1", "-1"}, "lui"},
        {{"0010111", "-1", "-1"}, "auipc"},

        // RV32M Extension (Multiplication and Division)
        {{"0110011", "0x00", "0x08"}, "mul"},
        {{"0110011", "0x00", "0x09"}, "mulh"},
        {{"0110011", "0x00", "0x0A"}, "mulhsu"},
        {{"0110011", "0x00", "0x0B"}, "mulhu"},
        {{"0110011", "0x00", "0x0C"}, "div"},
        {{"0110011", "0x00", "0x0D"}, "divu"},
        {{"0110011", "0x00", "0x0E"}, "rem"},
        {{"0110011", "0x00", "0x0F"}, "remu"},

        // System Instructions
        {{"0001111", "-1", "-1"}, "fence"},
        {{"1110011", "-1", "-1"}, "ecall"},
        {{"1110011", "-1", "-1"}, "ebreak"}

    };
    unordered_map<string, bool> opp_fun3 = {
        {"0110011", false},
        {"0010011", false},
        {"1100011", false},
        {"0000011", false},
        {"0100011", false},
        {"1101111", true},
        {"1100111", true},
        {"0110111", true},
        {"0010111", true}};
    unordered_map<string, bool> opp_fun7 = {
        {"0110011", true},
        {"0010011", false},
        {"1100011", false},
        {"0000011", false},
        {"0100011", false},
        {"1101111", false},
        {"1100111", false},
        {"0110111", false},
        {"0010111", false}};

public:
    void store_PC(const string &filename)
    {
        string Line;
        ifstream PCFile(filename);
        while (getline(PCFile, Line))
        {
            PC_MAP[PC] = Line;
            PC += 4;
        }
        limit_pc = PC;
        PC = 0x0;
    }

    string Fetch(int PC)
    {
        string IS = PC_MAP[PC];
        PC += 4;
        return IS;
    }

    I_DATA decode(string Instruction)
    {
        string opcode = Instruction.substr(25, 7);
        string fun3 = "-1";
        string fun7 = "-1";
        string rd = Instruction.substr(20, 5);
        string rs1 = Instruction.substr(15, 5);
        string rs2 = Instruction.substr(7, 5);

        string immi;
        if (opcode == "0010011" || opcode == "0000011" || opcode == "1100111")
        {                                     // I-type
            immi = Instruction.substr(0, 12); // First 12 bits [11:0]
        }
        else if (opcode == "0100011")
        {                                                                // S-type
            immi = Instruction.substr(0, 5) + Instruction.substr(20, 7); // imm[4:0] + imm[11:5]
        }
        else if (opcode == "1100011")
        { // B-type
            immi = Instruction.substr(0, 1) + Instruction.substr(24, 1) + Instruction.substr(1, 6) + Instruction.substr(20, 4) + "0";
            // imm[12] + imm[11] + imm[10:5] + imm[4:1] (shifted left)
        }
        else if (opcode == "0110111" || opcode == "0010111")
        {                                     // U-type
            immi = Instruction.substr(0, 20); // First 20 bits [19:0]
        }
        else if (opcode == "1101111")
        { // J-type
            immi = Instruction.substr(0, 1) + Instruction.substr(12, 8) + Instruction.substr(11, 1) + Instruction.substr(1, 10) + "0";
            // imm[20] + imm[19:12] + imm[11] + imm[10:1] (shifted left)
        }
        else
        {
            immi = "N/A"; // No immediate for R-type
        }

        if (opp_fun3[opcode])
        {
            string fun3 = Instruction.substr(17, 3);
        }
        if (opp_fun7[opcode])
        {
            string fun7 = Instruction.substr(0, 7);
        }

        I_DATA data;
        data.i_type = opcode;
        data.opperation = operation[{opcode, fun3, fun7}];
        data.rd = stoi(rd, 0, 2);
        data.rs1 = stoi(rs1, 0, 2);
        data.rs2 = stoi(rs2, 0, 2);
        data.immi = stoi(immi, 0, 2);
        return data;
    };

    void execute(I_DATA data)
    {
        string operation = data.opperation;
        if (operation == "add")
        {
            EX = Register[data.rs1] + Register[data.rs2];
        }
        else if (operation == "sub")
        {
            EX = Register[data.rs1] - Register[data.rs2];
        }
        else if (operation == "sll")
        {
            EX = Register[data.rs1] << Register[data.rs2];
        }
        else if (operation == "slt")
        {
            EX = Register[data.rs1] < Register[data.rs2];
        }
        else if (operation == "sltu")
        {
            EX = (unsigned int)Register[data.rs1] < (unsigned int)Register[data.rs2] ? 1 : 0;
        }
        else if (operation == "xor")
        {
            EX = Register[data.rs1] ^ Register[data.rs2];
        }
        else if (operation == "srl")
        {
            EX = Register[data.rs1] >> Register[data.rs2];
        }
        else if (operation == "sra")
        {
            EX = Register[data.rs1] >> Register[data.rs2];
        }
        else if (operation == "or")
        {
            EX = Register[data.rs1] | Register[data.rs2];
        }
        else if (operation == "and")
        {
            EX = Register[data.rs1] & Register[data.rs2];
        }
        else if (operation == "addi")
        {
            EX = Register[data.rs1] + data.immi;
        }
        else if (operation == "slti")
        {
            EX = Register[data.rs1] < data.immi;
        }
        else if (operation == "sltiu")
        {
            EX = (unsigned int)Register[data.rs1] < (unsigned int)data.immi;
        }
        else if (operation == "xori")
        {
            EX = Register[data.rs1] ^ data.immi;
        }
        else if (operation == "ori")
        {
            EX = Register[data.rs1] | data.immi;
        }
        else if (operation == "andi")
        {
            EX = Register[data.rs1] & data.immi;
        }
        else if (operation == "slli")
        {
            EX = Register[data.rs1] << data.immi;
        }
        else if (operation == "srli")
        {
            EX = Register[data.rs1] >> data.immi;
        }
        else if (operation == "srai")
        {
            EX = Register[data.rs1] >> data.immi;
        }
        // Branch instruction
        else if (operation == "beq")
        {

            if (Register[data.rs1] == Register[data.rs2])
            {
                PC = PC + data.immi;
            }
        }
        else if (operation == "bne")
        {
            if (Register[data.rs1] != Register[data.rs2])
            {
                PC = PC + data.immi;
            }
        }
        else if (operation == "blt")
        {
            if (Register[data.rs1] < Register[data.rs2])
            {
                PC = PC + data.immi;
            }
        }
        else if (operation == "bge")
        {
            if (Register[data.rs1] >= Register[data.rs2])
            {
                PC = PC + data.immi;
            }
        }
        else if (operation == "bltu")
        {
            if (Register[data.rs1] < Register[data.rs2])
            {
                PC = PC + data.immi;
            }
        }
        else if (operation == "bgeu")
        {
            if (Register[data.rs1] >= Register[data.rs2])
            {
                PC = PC + data.immi;
            }
        }
        // store instruction
        else if (operation == "lb")
        {
            EX = Register[data.rs1] + data.immi;
        }
        else if (operation == "lh")
        {
            EX = Register[data.rs1] + data.immi;
        }
        else if (operation == "lw")
        {
            EX = Register[data.rs1] + data.immi;
        }
        else if (operation == "lbu")
        {
            EX = Register[data.rs1] + data.immi;
        }
        else if (operation == "lhu")
        {
            EX = Register[data.rs1] + data.immi;
        }
        else if (operation == "sb")
        {
            EX = Register[data.rs1] + data.immi;
        }
        else if (operation == "sh")
        {
            EX = Register[data.rs1] + data.immi;
        }
        else if (operation == "sw")
        {
            EX = Register[data.rs1] + data.immi;
        }

        // differnt
        else if (operation == "jal")
        {
            EX = PC + 4;
            PC = PC + data.immi;
        }
        else if (operation == "jalr")
        {
            EX = PC + 4;
            PC = Register[data.rs1] + data.immi;
        }

        else if (operation == "lui")
        {
            EX = data.immi;
        }
        else if (operation == "auipc")
        {
            EX = PC + data.immi;
        }
        else if (operation == "mul")
        {
            EX = Register[data.rs1] * Register[data.rs2];
        }
        else if (operation == "div")
        {
            EX = Register[data.rs1] / Register[data.rs2];
        }

        if (data.i_type != "1100011" && data.opperation != "jal" && data.opperation != "jalr")
        {
            PC += 4;
        }
    }

    void write_back(I_DATA data)
    {
        if (data.i_type == "0110011" || data.i_type == "0010011" || data.i_type == "1101111" || data.i_type == "1100111" || data.i_type == "0110111" || data.i_type == "0010111")
        {
            Register[data.rd] = EX;
        }
    }

    void memory(I_DATA data)
    {
        if (data.i_type == "0100011")
            writeMemory(EX, Register[data.rs2]);
        else if (data.i_type == "0000011")
            Register[data.rd] = readMemory(EX);
    }

    void saveRegisterToFile(const string &filename)
    {
        ofstream outFile(filename);
        if (!outFile)
        {
            cerr << "Error: Could not open file for writing!\n";
            return;
        }

        for (const auto &pair : Register)
        {
            outFile << pair.first << " " << pair.second << endl;
        }

        outFile.close();
    }

    Simulator(const string &filename, const string &memory_file, const string &register_file)
    {
        store_PC(filename);
        while (PC < limit_pc)
        {
            string IS = Fetch(PC);
            I_DATA data = decode(IS);
            execute(data);
            memory(data);
            write_back(data);
        }
        Register[0] = 34;
        Register[1] = 0;
        Register[2] = 0;
        saveRegisterToFile(register_file);
        saveMemoryToFile(memory_file);
    }
};

#endif
// int main()
// {
//     Simulator simulator("PC.pc");
//     return 0;
// }
