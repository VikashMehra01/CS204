#ifndef SIMULATOR
#define SIMULATOR

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <set>

#include "memory.h"

using namespace std;

struct IF_ID
{
    string IR = "";
    int PC = 0;
    bool null = true;
};
struct I_DATA
{
    string i_type;
    string operation;
    int rd;
    int rs1;
    int rs2;
    int rs1_reg;
    int rs2_reg;
    int EX;
    int immi;
    int PC;
    bool null = true;
};
struct EX_DATA
{
    int EX;
    int rd;
    int rs1;
    int rs2;
    int immi;
    int PC;
    string i_type;
    string operation;
    bool null = true;
    bool Hit = false;
};
struct MEMORY_DATA
{
    int EX = 0;
    int rd;
    int rs2;
    int address;
    int data;
    int PC;
    string i_type;
    bool null = true;
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
    bool DataForwarding = false;
    bool flushing = false;
    int stalls = 0;
    bool stall_installed = false;
    int limit_pc;
    bool branch_prediction = true;
    int temp_PC = 0;
    bool Pipeline = true;
    bool print_registers = false;
    bool print_pipeline_registers = false;
    bool print_BPU = false;
    bool specificInstruction = false;
    int specificInstructionAddress = 0;
    bool printCycle = false;

    int dynamic_instruction_count = 0;
    int CPI;
    int No_of_data_transfers = 0;
    int No_of_ALU_operations = 0;
    int No_of_control_operations = 0;
    int No_of_stalls_data_hazards = 0;
    int No_of_stalls_control_hazards = 0;
    int No_of_data_hazards = 0;
    int No_of_control_hazards = 0;
    int No_of_Mispredictions = 0;
    I_DATA Data;
    MEMORY_DATA MEM;
    EX_DATA EX;
    IF_ID IR;
    unordered_map<int, vector<pair<int, string>>> Instruction_map;
    unordered_map<int, int> BPB;
    unordered_map<int, bool> BPT;
    string BPU = "";
    unordered_map<int, string> PC_MAP;
    map<int, long long int> Register{
        {0, 0},
        {1, 0},
        {2, 2147483612},
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
        {{"0110011", "000", "0000000"}, "add"},
        {{"0110011", "000", "0100000"}, "sub"},
        {{"0110011", "001", "0000000"}, "sll"},
        {{"0110011", "010", "0000000"}, "slt"},
        {{"0110011", "011", "0000000"}, "sltu"},
        {{"0110011", "100", "0000000"}, "xor"},
        {{"0110011", "101", "0000000"}, "srl"},
        {{"0110011", "101", "0100000"}, "sra"},
        {{"0110011", "110", "0000000"}, "or"},
        {{"0110011", "111", "0000000"}, "and"},

        {{"0010011", "000", "-1"}, "addi"},
        {{"0010011", "010", "-1"}, "slti"},
        {{"0010011", "011", "-1"}, "sltiu"},
        {{"0010011", "100", "-1"}, "xori"},
        {{"0010011", "110", "-1"}, "ori"},
        {{"0010011", "111", "-1"}, "andi"},
        {{"0010011", "001", "0000000"}, "slli"},
        {{"0010011", "101", "0000000"}, "srli"},
        {{"0010011", "101", "0100000"}, "srai"},

        {{"1100011", "000", "-1"}, "beq"},
        {{"1100011", "001", "-1"}, "bne"},
        {{"1100011", "100", "-1"}, "blt"},
        {{"1100011", "101", "-1"}, "bge"},
        {{"1100011", "110", "-1"}, "bltu"},
        {{"1100011", "111", "-1"}, "bgeu"},

        {{"0000011", "000", "-1"}, "lb"},
        {{"0000011", "001", "-1"}, "lh"},
        {{"0000011", "010", "-1"}, "lw"},
        {{"0000011", "100", "-1"}, "lbu"},
        {{"0000011", "101", "-1"}, "lhu"},

        {{"0100011", "000", "-1"}, "sb"},
        {{"0100011", "001", "-1"}, "sh"},
        {{"0100011", "010", "-1"}, "sw"},

        {{"1101111", "-1", "-1"}, "jal"},
        {{"1100111", "000", "-1"}, "jalr"},

        {{"0110111", "-1", "-1"}, "lui"},
        {{"0010111", "-1", "-1"}, "auipc"},

        // RV32M Extension (Multiplication and Division)
        {{"0110011", "000", "0000001"}, "mul"},
        {{"0110011", "001", "0000001"}, "mulh"},
        {{"0110011", "010", "0000001"}, "mulhsu"},
        {{"0110011", "011", "0000001"}, "mulhu"},
        {{"0110011", "100", "0000001"}, "div"},
        {{"0110011", "101", "0000001"}, "divu"},
        {{"0110011", "110", "0000001"}, "rem"},
        {{"0110011", "111", "0000001"}, "remu"},

        // System Instructions
        {{"0001111", "-1", "-1"}, "fence"},
        {{"1110011", "-1", "-1"}, "ecall"},
        {{"1110011", "-1", "-1"}, "ebreak"},
    };
    unordered_map<string, bool> opp_fun3 = {
        {"0110011", true},
        {"0010011", true},
        {"1100011", true},
        {"0000011", true},
        {"0100011", true},
        {"1101111", false},
        {"1100111", true},
        {"0110111", false},
        {"0010111", false}};
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
    unordered_map<string, bool> is_rs2 = {
        {"0110011", true},  // R-type
        {"0010011", false}, // I-type
        {"1100011", true},  // B-type
        {"0000011", false}, // I-type
        {"0100011", true},  // S-type
        {"1101111", false}, // J-type
        {"1100111", false}, // I-type
        {"0110111", false}, // U-type
        {"0010111", false}  // U-type
    };
    unordered_map<string, bool> is_rs1 = {
        {"0110011", true},
        {"0010011", true},
        {"1100011", true},
        {"0000011", true},
        {"0100011", true},
        {"1101111", false},
        {"1100111", true},
        {"0110111", false},
        {"0010111", false}};

public:
    void dataHazards(I_DATA &current)
    {
        int stallneeded = 0;
        if (!DataForwarding)
        {
            // Original stall logic (no forwarding)
            if (current.rs1_reg != 0)
            {
                if ((!Data.null && current.rs1_reg == Data.rd))
                {
                    stall_installed = true;
                    stallneeded = max(stallneeded, 3);
                }
                else if ((!EX.null && current.rs1_reg == EX.rd))
                {
                    stall_installed = true;
                    stallneeded = max(stallneeded, 2);
                }
                else if ((!MEM.null && current.rs1_reg == MEM.rd))
                {
                    stall_installed = true;
                    stallneeded = max(stallneeded, 1);
                }
            }
            if (current.rs2_reg != 0 && is_rs2[current.i_type])
            {
                if ((!Data.null && current.rs2_reg == Data.rd))
                {
                    stallneeded = std::max(stallneeded, 3);
                    stall_installed = true;
                }
                else if ((!EX.null && current.rs2_reg == EX.rd))
                {
                    stallneeded = std::max(stallneeded, 2);
                    stall_installed = true;
                }
                else if ((!MEM.null && current.rs2_reg == MEM.rd))
                {
                    stallneeded = std::max(stallneeded, 1);
                    stall_installed = true;
                }
            }
        }
        else
        {
            // Forwarding-enabled logic: Stall only for load-use hazards
            if (current.rs1_reg != 0)
            {
                if (!EX.null && current.rs1_reg == EX.rd)
                {
                    if (EX.i_type == "0000011")
                    {
                        stallneeded = max(stallneeded, 1);
                        stall_installed = true;
                    }
                    else if (EX.i_type != "0100011" && EX.i_type != "1100011")
                    {
                        current.rs1 = EX.EX;
                        if (EX.EX == 40)
                            cout << "I caught inside EX-1" << endl;
                    }
                }
                else if (!MEM.null && MEM.rd == current.rs1_reg)
                {
                    // Load-use hazard: EX stage is a load, result not ready yet

                    if (MEM.i_type == "0000011")
                    {
                        current.rs1 = MEM.data;
                        if (MEM.data == 40)
                            cout << "I caught inside MEM_data-1" << endl;
                    }
                    else if (MEM.i_type != "0100011" && MEM.i_type != "1100011")
                    {
                        current.rs1 = MEM.EX;
                        if (MEM.EX == 40)
                            cout << "I caught inside MEM_EX-1" << endl;
                    }
                }
                else
                {
                    stall_installed = false;
                    stallneeded = 0;
                }
            }

            if (current.rs2_reg != 0 && is_rs2[current.i_type])
            {

                if (!EX.null && current.rs2_reg == EX.rd)
                {
                    if (EX.i_type == "0000011")
                    {
                        stallneeded = max(stallneeded, 1);
                        stall_installed = true;
                    }
                    else if (EX.i_type != "0100011" && EX.i_type != "1100011")
                    {
                        current.rs2 = EX.EX;
                    }
                }

                else if (!MEM.null && MEM.rd == current.rs2_reg)
                {
                    if (MEM.i_type == "0000011")
                    {
                        current.rs2 = MEM.data;
                        if (MEM.data == 40)
                            cout << "I caught inside MEM_data-2" << endl;
                    }
                    else if (MEM.i_type != "0100011" && MEM.i_type != "1100011")
                    {
                        current.rs2 = MEM.EX;
                        if (MEM.EX == 40)
                            cout << "I caught inside MEM_EX-2" << endl;
                    }
                }
                else
                {
                    stall_installed = false;
                    stallneeded = 0;
                }
            }
        }
        stalls = stallneeded;
        No_of_stalls_data_hazards += stallneeded;
        if (stallneeded > 0)
        {
            No_of_data_hazards++;
        }
    }
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

    IF_ID Fetch()
    {
        IF_ID IR;
        if (PC >= limit_pc)
        {
            return IR;
        }
        if (PC_MAP.find(PC) == PC_MAP.end() && Pipeline)
        {
            cout << "Error: PC not found in instruction map." << endl;
            return IR;
        }

        IR.IR = PC_MAP[PC];
        IR.PC = PC;
        IR.null = false;

        // check if for Branch Instructions
        if (BPB.find(PC) != BPB.end())
        {
            // Check if the branch prediction table has a prediction for this PC
            if (BPT[PC] == true)
            {
                PC = BPB[PC];
            }
            else
            {
                PC += 4;
            }
        }
        // not branch
        else
        {
            PC += 4;
        }

        return IR;
    }

    I_DATA decode(IF_ID IR)
    {
        string Instruction = IR.IR;
        I_DATA data;
        if (IR.null)
        {
            return data;
        }

        string opcode = Instruction.substr(25, 7);
        string fun3 = "-1";
        string fun7 = "-1";
        string rd = Instruction.substr(20, 5);
        string rs1 = Instruction.substr(12, 5);
        string rs2 = Instruction.substr(7, 5);

        string immi;
        if (opcode == "0010011" || opcode == "0000011" || opcode == "1100111")
        {                                     // I-type
            immi = Instruction.substr(0, 12); // First 12 bits [11:0]
        }
        else if (opcode == "0100011")
        {                                                                // S-type
            immi = Instruction.substr(0, 7) + Instruction.substr(20, 5); // imxm[4:0] + imm[11:5]
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
            immi = "000"; // No immediate for R-type
        }
        if (opp_fun3[opcode])
        {
            fun3 = Instruction.substr(17, 3);
        }
        if (opp_fun7[opcode] || (opcode == "0010011" && (fun3 == "001" || fun3 == "101")))
        {
            fun7 = Instruction.substr(0, 7);
        }

        data.i_type = opcode;
        data.operation = operation[{opcode, fun3, fun7}];

        data.rd = stoi(rd, 0, 2);
        data.rs1_reg = stoi(rs1, 0, 2);
        data.rs2_reg = stoi(rs2, 0, 2);
        data.rs1 = Register[stoi(rs1, 0, 2)];
        data.rs2 = Register[stoi(rs2, 0, 2)];
        int temp_immi = stoi(immi, 0, 2);
        int bits = immi.size();
        if (temp_immi & (1 << (bits - 1)))
        {
            data.immi = temp_immi - (1 << bits);
        }
        else
        {
            data.immi = temp_immi;
        }
        data.null = false;
        data.PC = IR.PC;

        if (Pipeline)
            dataHazards(data);

        return data;
    }

    EX_DATA execute(I_DATA &data)
    {
        EX_DATA EX_data;
        if (data.null)
        {
            return EX_data;
        }

        No_of_ALU_operations++;

        if (data.i_type == "1100011" || data.i_type == "1101111" || data.i_type == "1100111")
        {
            No_of_control_operations++;
        }
        string operation = data.operation;
        if (operation == "add")
        {
            EX_data.EX = data.rs1 + data.rs2;
        }
        else if (operation == "sub")
        {
            EX_data.EX = data.rs1 - data.rs2;
        }
        else if (operation == "sll")
        {
            EX_data.EX = data.rs1 << data.rs2;
        }
        else if (operation == "slt")
        {
            EX_data.EX = data.rs1 < data.rs2;
        }
        else if (operation == "sltu")
        {
            EX_data.EX = (unsigned int)data.rs1 < (unsigned int)data.rs2 ? 1 : 0;
        }
        else if (operation == "xor")
        {
            EX_data.EX = data.rs1 ^ data.rs2;
        }
        else if (operation == "srl")
        {
            EX_data.EX = data.rs1 >> data.rs2;
        }
        else if (operation == "sra")
        {
            EX_data.EX = data.rs1 >> data.rs2;
        }
        else if (operation == "or")
        {
            EX_data.EX = data.rs1 | data.rs2;
        }
        else if (operation == "and")
        {
            EX_data.EX = data.rs1 & data.rs2;
        }
        else if (operation == "addi")
        {
            EX_data.EX = data.rs1 + data.immi;
        }
        else if (operation == "slti")
        {
            EX_data.EX = data.rs1 < data.immi;
        }
        else if (operation == "sltiu")
        {
            EX_data.EX = (unsigned int)data.rs1 < (unsigned int)data.immi;
        }
        else if (operation == "xori")
        {
            EX_data.EX = data.rs1 ^ data.immi;
        }
        else if (operation == "ori")
        {
            EX_data.EX = data.rs1 | data.immi;
        }
        else if (operation == "andi")
        {
            EX_data.EX = data.rs1 & data.immi;
        }
        else if (operation == "slli")
        {
            int shift_amount = data.immi & 0x1F; // Mask lower 5 bits
            EX_data.EX = data.rs1 << shift_amount;
        }
        else if (operation == "srli")
        {
            int shift_amount = data.immi & 0x1F;
            EX_data.EX = (unsigned int)data.rs1 >> shift_amount; // Logical shift
        }
        else if (operation == "srai")
        {
            int shift_amount = data.immi & 0x1F;
            EX_data.EX = data.rs1 >> shift_amount; // Arithmetic shift (C++ behavior for signed int)
        }

        // Branch instruction
        else if (operation == "beq")
        {
            BPU = " PC: " + to_string(data.PC) + " Prediction: " + to_string(BPT[data.PC]);
            if (data.rs1 == data.rs2)
            {
                BPB[data.PC] = data.PC + data.immi;
                EX_data.EX = BPB[data.PC];
                // branch prediction
                if (BPT[data.PC] == true)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else if (BPT[data.PC] == false)
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;
                    BPU += " Incorrect prediction";
                    PC = BPB[data.PC];
                    BPT[data.PC] = true;
                    flushing = true;
                }
            }
            else
            {

                if (BPT[data.PC] == false)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;
                    // Incorrect prediction
                    BPU += " Incorrect prediction";
                    // Incorrect prediction
                    PC = data.PC + 4;
                    BPT[data.PC] = false;
                    flushing = true;
                }
            }
        }
        else if (operation == "bne")
        {
            BPU = "PC: " + to_string(data.PC) + " Prediction: " + to_string(BPT[data.PC]);
            if (data.rs1 != data.rs2)
            {
                BPB[data.PC] = data.PC + data.immi;
                EX_data.EX = BPB[data.PC];
                // branch prediction
                if (BPT[data.PC] == true)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;
                    BPU += " Incorrect prediction";
                    PC = BPB[data.PC];
                    BPT[data.PC] = true;
                    flushing = true;
                }
            }
            else
            {
                if (BPT[data.PC] == false)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;
                    BPU += " Incorrect prediction";
                    // Incorrect prediction
                    PC = data.PC + 4;
                    BPT[data.PC] = false;
                    flushing = true;
                }
            }
        }
        else if (operation == "blt")
        {
            BPU = "PC: " + to_string(data.PC) + " Prediction: " + to_string(BPT[data.PC]);
            if (data.rs1 < data.rs2)
            {

                BPB[data.PC] = data.PC + data.immi;
                EX_data.EX = BPB[data.PC];
                // branch prediction
                if (BPT[data.PC] == true)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;
                    BPU += " Incorrect prediction";
                    PC = BPB[data.PC];
                    BPT[data.PC] = true;
                    flushing = true;
                }
            }
            else
            {
                if (BPT[data.PC] == false)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;
                    BPU += " Incorrect prediction";
                    // Incorrect prediction
                    PC = data.PC + 4;
                    BPT[data.PC] = false;
                    flushing = true;
                }
            }
        }
        else if (operation == "bge")
        {
            BPU = "PC: " + to_string(data.PC) + " Prediction: " + to_string(BPT[data.PC]);
            if (data.rs1 >= data.rs2)
            {
                BPB[data.PC] = data.PC + data.immi;
                EX_data.EX = BPB[data.PC];
                // branch prediction
                if (BPT[data.PC] == true)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;

                    BPU += " Incorrect prediction";
                    PC = BPB[data.PC];
                    BPT[data.PC] = true;
                    flushing = true;
                }
            }
            else
            {
                if (BPT[data.PC] == false)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;
                    BPU += " Incorrect prediction";
                    // Incorrect prediction
                    PC = data.PC + 4;
                    BPT[data.PC] = false;
                    flushing = true;
                }
            }
        }
        else if (operation == "bltu")
        {
            BPU = "PC: " + to_string(data.PC) + " Prediction: " + to_string(BPT[data.PC]);
            if (data.rs1 < data.rs2)
            {

                BPB[data.PC] = data.PC + data.immi;
                EX_data.EX = BPB[data.PC];
                // branch prediction
                if (BPT[data.PC] == true)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;
                    BPU += " Incorrect prediction";
                    PC = BPB[data.PC];
                    BPT[data.PC] = true;
                    flushing = true;
                }
            }
            else
            {
                if (BPT[data.PC] == false)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;

                    BPU += " Incorrect prediction";
                    // Incorrect prediction
                    PC = data.PC + 4;
                    BPT[data.PC] = false;
                    flushing = true;
                }
            }
        }
        else if (operation == "bgeu")
        {
            BPU = "PC: " + to_string(data.PC) + " Prediction: " + to_string(BPT[data.PC]);
            if (data.rs1 >= data.rs2)
            {

                BPB[data.PC] = data.PC + data.immi;
                EX_data.EX = BPB[data.PC];
                // branch prediction
                if (BPT[data.PC] == true)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;

                    BPU += " Incorrect prediction";
                    PC = BPB[data.PC];
                    BPT[data.PC] = true;
                    flushing = true;
                }
            }
            else
            {
                if (BPT[data.PC] == false)
                {
                    BPU += " Correct prediction";
                    // Correct prediction
                }
                else
                {
                    No_of_Mispredictions++;
                    No_of_control_hazards++;

                    BPU += " Incorrect prediction";
                    // Incorrect prediction
                    PC = data.PC + 4;
                    BPT[data.PC] = false;
                    flushing = true;
                }
            }
        }
        // store instruction
        else if (operation == "lb")
        {
            EX_data.EX = data.rs1 + data.immi;
        }
        else if (operation == "lh")
        {
            EX_data.EX = data.rs1 + data.immi;
        }
        else if (operation == "lw")
        {
            EX_data.EX = data.rs1 + data.immi;
        }
        else if (operation == "lbu")
        {
            EX_data.EX = data.rs1 + data.immi;
        }
        else if (operation == "lhu")
        {
            EX_data.EX = data.rs1 + data.immi;
        }
        else if (operation == "sb")
        {
            EX_data.EX = data.rs1 + data.immi;
        }
        else if (operation == "sh")
        {
            EX_data.EX = data.rs1 + data.immi;
        }
        else if (operation == "sw")
        {
            EX_data.EX = data.rs1 + data.immi;
        }

        // differnt
        else if (operation == "jal")
        {
            BPU = "PC: " + to_string(data.PC) + " Predicted-Target: " + to_string(BPB[data.PC]);
            EX_data.EX = data.PC + 4;
            int target = data.PC + data.immi;
            if (BPB[data.PC] == target)
            {
                BPU += " Correct prediction";
                // Correct prediction
            }
            else
            {
                No_of_control_hazards++;
                BPU += " Incorrect prediction ";
                BPU += " Actual-Target: " + to_string(target);
                PC = target;
                BPB[data.PC] = target;
                BPT[data.PC] = true;
                flushing = true;
            }
        }
        else if (operation == "jalr")
        {
            BPU = "PC: " + to_string(data.PC) + " Predicted-Target: " + to_string(BPB[data.PC]);
            EX_data.EX = data.PC + 4;
            int target = data.rs1 + data.immi;
            if (BPB[data.PC] == target)
            {
                BPU += " Correct prediction";
                // Correct prediction
            }
            else
            {
                No_of_control_hazards++;
                BPU += " Incorrect prediction ";
                BPU += " Actual-Target: " + to_string(target);
                // Incorrect prediction
                PC = target;
                BPB[data.PC] = target;
                BPT[data.PC] = true;
                flushing = true;
            }
        }

        else if (operation == "lui")
        {
            EX_data.EX = data.immi << 12;
        }
        else if (operation == "auipc")
        {
            EX_data.EX = data.PC + (data.immi << 12);
        }
        else if (operation == "mul")
        {
            EX_data.EX = data.rs1 * data.rs2;
        }
        else if (operation == "div")
        {
            EX_data.EX = data.rs1 / data.rs2;
        }

        EX_data.rd = data.rd;
        EX_data.operation = operation;
        EX_data.rs1 = data.rs1;
        EX_data.rs2 = data.rs2;
        EX_data.immi = data.immi;
        EX_data.i_type = data.i_type;
        EX_data.null = false;
        EX_data.PC = data.PC;
        Data.EX = EX_data.EX;
        EX_data.Hit = !flushing;

        return EX_data;
    }

    MEMORY_DATA memory(EX_DATA EX)
    {
        MEMORY_DATA mem;
        if (EX.null)
        {
            return mem;
        }
        mem.EX = EX.EX;
        mem.rd = EX.rd;
        mem.rs2 = EX.rs2;
        mem.PC = EX.PC;
        mem.i_type = EX.i_type;
        mem.null = false;

        if (EX.i_type == "0100011")
        {
            No_of_data_transfers++;
            int val = EX.rs2;
            string curr = EX.operation;

            if (curr == "sb")
            {
                writeMemory(EX.EX, val % (1 << 8));
            }
            else if (curr == "sh")
            {
                writeMemory(EX.EX, val % 256);
                val >>= 8;
                writeMemory(EX.EX + 1, val % 256);
            }
            else
            {
                writeMemory(EX.EX, val % 256);
                val >>= 8;
                writeMemory(EX.EX + 1, val % 256);
                val >>= 8;
                writeMemory(EX.EX + 2, val % 256);
                val >>= 8;
                writeMemory(EX.EX + 3, val % 256);
            }
        }

        else if (EX.i_type == "0000011")
        {

            No_of_data_transfers++;
            string curr = EX.operation;
            if (curr == "lb")
            {
                int d = readMemory(EX.EX);
                if (d & (1 << 7))
                {
                    mem.data = d - (1 << 8);
                }
                else
                {
                    mem.data = d;
                }
            }

            else if (curr == "lw")
            {
                int d = (readMemory(EX.EX + 3) << 24) + (readMemory(EX.EX + 2) << 16) + (readMemory(EX.EX + 1) << 8) + readMemory(EX.EX);
                if (d & (1 << 31))
                {
                    mem.data = d - (1LL << 32);
                }
                else
                {
                    mem.data = d;
                }
            }
            else if (curr == "lbu")
            {
                mem.data = readMemory(EX.EX);
            }
            else if (curr == "lhu")
            {
                mem.data = readMemory(EX.EX) + (readMemory(EX.EX + 1) << 8);
            }
        }
        return mem;
    }

    void write_back(MEMORY_DATA MEM)
    {

        if (MEM.null)
        {
            return;
        }
        dynamic_instruction_count++;
        if (MEM.rd == 0)
        {
            return;
        }
        if (MEM.i_type == "0110011" || MEM.i_type == "0010011" || MEM.i_type == "1101111" || MEM.i_type == "1100111" || MEM.i_type == "0110111" || MEM.i_type == "0010111")
        {
            Register[MEM.rd] = MEM.EX;
        }
        else if (MEM.i_type == "0000011")
        {
            Register[MEM.rd] = MEM.data;
        }
        else if (MEM.i_type == "0100011")
        {
            // No write-back for store instructions
        }
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
            outFile << pair.first << " ";
            outFile << pair.second << endl;
        }

        outFile.close();
    }

    void NonPiplined(const string &filename, const string &outputFile, const string &memory_file, const string &register_file)
    {
        store_PC(filename);
        ofstream outFile(outputFile);
        int cycles = 0;
        while (PC < limit_pc)
        {
            if (printCycle)
            {
                outFile << "Cycle " << cycles << ": " << endl;
            }
            IF_ID IR = Fetch();
            outFile << "Fetch   :" << IR.IR << " " << PC << endl;
            I_DATA data = decode(IR);
            outFile << "Decode  :" << data.operation << " ";
            if (!(data.i_type == "0100011" || data.i_type == "1100011"))
            {
                outFile << "rd:" << data.rd;
            }
            if (!(data.i_type == "0110111" || data.i_type == "0010111" || data.i_type == "1101111"))
            {
                outFile << " rs1:" << data.rs1;
            }
            if (!(data.i_type == "0010011" || data.i_type == "0000011" || data.i_type == "1101111" || data.i_type == "0110111" || data.i_type == "0010111"))
            {
                outFile << " rs2:" << data.rs2;
            }
            if (!(data.i_type == "0110011"))
            {
                outFile << " imm:" << data.immi;
            }
            outFile << endl;
            EX_DATA EX = execute(data);
            outFile << "Execute : Ex:" << EX.EX << " PC" << MEM.PC << endl;
            MEMORY_DATA MEM = memory(EX);
            if (data.i_type == "0000011")
            {
                outFile << "Memory  :" << "Load " << "Data: " << MEM.data << " Address:" << MEM.EX << endl;
                cycles += 1;
            }
            else if (data.i_type == "0100011")
            {
                outFile << "Memory  :" << "Store " << "Data: " << MEM.rs2 << " Address:" << MEM.EX << endl;
                cycles += 1;
            }
            else
            {
                outFile << "Memory  :" << "Not used" << endl;
            }
            write_back(MEM);
            if (data.i_type == "0100011" || data.i_type == "1100011")
            {
                outFile << "WriteBack   :" << "Not used" << endl;
            }
            else
            {
                outFile << "WriteBack   :" << "Rd:" << data.rd << " Data:" << Register[data.rd] << endl;
                cycles += 1;
            }
            if (print_registers)
            {
                outFile << "Registers:" << endl;
                for (const auto &pair : Register)
                {
                    cout << pair.first << ": " << pair.second << endl;
                }
            }
            outFile << "------------------------------------------------------------------------------------" << endl;
            cycles += 3;
        }
        outFile << "Total Cycles: " << cycles << endl;
        outFile << "Total Instructions: " << dynamic_instruction_count << endl;
        outFile << "CPI" << ": " << (float)cycles / dynamic_instruction_count << endl;
        outFile << "Total ALU Operations: " << No_of_ALU_operations << endl;
        outFile << "Total Data Transfers: " << No_of_data_transfers << endl;
        outFile << "Total Control Operations: " << No_of_control_operations << endl;
        // outFile << "Total Data Hazards: " << No_of_data_hazards << endl;
        // outFile << "Total Control Hazards: " << No_of_control_hazards << endl;
        // outFile << "Total Mispredictions: " << No_of_Mispredictions << endl;
        // outFile << "Total Stalls: " << No_of_stalls_data_hazards + No_of_stalls_control_hazards << endl;
        // outFile << " Stalls due to Data Hazards: " << No_of_stalls_data_hazards << endl;
        // outFile << " Stalls due to Control Hazards: " << No_of_stalls_control_hazards << endl;
        outFile.close();
        saveRegisterToFile(register_file);
        saveMemoryToFile(memory_file);
        cout << "Simulation Completed" << endl;
    }
    void Pipelined(const string &filename, const string &outputFile, const string &memory_file, const string &register_file)
    {
        store_PC(filename);
        ofstream outFile(outputFile);
        int cycles = 0;
        IF_ID IR_Buffer;
        I_DATA Data_buffer;
        EX_DATA EX_buffer;
        MEMORY_DATA memory_buffer;

        while (true)
        {
            bool active_stages = false;
            string F, D, E, M, W;

            if (stalls > 0)
            {
                stalls -= 1;
            }

            // WriteBack;
            write_back(MEM);
            if (!MEM.null && (MEM.i_type == "0110011" || MEM.i_type == "0010011" || MEM.i_type == "0000011" || MEM.i_type == "1101111" || MEM.i_type == "1100111" || MEM.i_type == "0110111" || MEM.i_type == "0010111"))
            {
                active_stages = true;
                W = "WriteBack: Rd:" + to_string(MEM.rd) + " Data" + to_string(Register[MEM.rd]) + " PC:" + to_string(MEM.PC);
                Instruction_map[MEM.PC / 4].push_back({cycles, W});
            }
            else
            {
                W = "WriteBack: Not used";
            }

            memory_buffer = memory(EX);

            MEM = memory_buffer;
            // Memory
            if (!EX.null && (EX.i_type == "0000011" || EX.i_type == "0100011"))
            {
                active_stages = true;
                if (memory_buffer.i_type == "0000011")
                {
                    M = "Memory:     Load  Data:" + to_string(memory_buffer.data) + " Address:" + to_string(memory_buffer.EX) + " PC:" + to_string(memory_buffer.PC);
                }
                else
                {
                    M = "Memory:     Store Data:" + to_string(memory_buffer.rs2) + " Address:" + to_string(memory_buffer.EX) + " PC:" + to_string(memory_buffer.PC);
                }
                Instruction_map[memory_buffer.PC / 4].push_back({cycles, M});
            }
            else
            {
                M = "Memory: Not used";
            }

            EX_buffer = execute(Data);
            // Execute
            EX = EX_buffer;
            if (!Data.null)
            {
                active_stages = true;
                E = "Execute: Ex:" + to_string(EX_buffer.EX) + " PC:" + to_string(EX_buffer.PC);
                if (EX_buffer.i_type == "1100011")
                {
                    E += " HIT: " + to_string(EX_buffer.Hit);
                }
                Instruction_map[EX_buffer.PC / 4].push_back({cycles, E});
            }
            else
            {
                E = "Execute: Not used";
            }

            if (stalls > 0)
            {
                D = "Decode: Stalls";
                active_stages = true;
            }
            else
            {
                Data_buffer = decode(IR);
                if (!IR.null)
                {
                    active_stages = true;
                    D = "Decode :    operation :" + Data_buffer.operation + " Rd:" + to_string(Data_buffer.rd) + " Rs1:" + to_string(Data_buffer.rs1) + " Rs2:" + to_string(Data_buffer.rs2) + " imm:" + to_string(Data_buffer.immi) + " PC:" + to_string(Data_buffer.PC);
                    Instruction_map[Data_buffer.PC / 4].push_back({cycles, D});
                }
                else
                {
                    D = " Decode : Not Used";
                }
            }

            if (stalls > 0)
            {
                F = "Fetch : Stalls";
                active_stages = true;
            }
            else
            {
                IR_Buffer = Fetch();
                if (!IR_Buffer.null)
                {

                    active_stages = true;
                    F = "Fetch: " + IR_Buffer.IR + " PC:" + to_string(IR_Buffer.PC);
                    Instruction_map[IR_Buffer.PC / 4].push_back({cycles, F});
                }
                else
                {
                    F = "Fetch: Not Used";
                }
            }

            if (!active_stages)
            {
                break;
            }

            memory_buffer.null = true;

            if (printCycle)
            {
                outFile << "Cycle: " << cycles << endl;
            }

            if (!IR_Buffer.null)
            {
                if (print_BPU)
                {
                    outFile << BPU << endl;
                    // outFile << "------------------------------------------------------------------------------------" << endl;
                }
            }

            EX_buffer.null = true;

            // data transfer
            if (stalls == 0)
            {
                Data = Data_buffer;
                Data_buffer.null = true;
                IR = IR_Buffer;
                IR_Buffer.null = true;
            }
            else
            {
                Data.null = true;
                IR_Buffer.null = true;
            }

            if (flushing && stalls == 0)
            {
                flushing = false;
                IR_Buffer.null = true;
                Data.null = true;
                No_of_stalls_control_hazards += 2;
            }

            if (print_pipeline_registers)
            {
                outFile << F << endl;
                outFile << D << endl;
                outFile << E << endl;
                outFile << M << endl;
                outFile << W << endl;
                // outFile << "------------------------------------------------------------------------------------" << endl;
            }

            if (print_registers)
            {
                outFile << "Registers:" << endl;
                for (const auto &pair : Register)
                {
                    outFile << "x" << pair.first << ": " << pair.second << endl;
                }
                // outFile << "------------------------------------------------------------------------------------" << endl;
            }
            cycles++;
            if (print_registers || print_pipeline_registers || print_BPU || printCycle)
            {
                outFile << "------------------------------------------------------------------------------------" << endl;
            }
        }
        if (specificInstruction)
        {
            for (auto &temp : Instruction_map[specificInstructionAddress])
            {
                outFile << "Cycle : " << temp.first << " Instruction_Status " << temp.second << endl;
                // cout << temp << endl;
            }
        }
        outFile << "Total Cycles: " << cycles << endl;
        outFile << "Total Instructions: " << dynamic_instruction_count << endl;
        outFile << "CPI" << ": " << (float)cycles / dynamic_instruction_count << endl;
        outFile << "Total ALU Operations: " << No_of_ALU_operations << endl;
        outFile << "Total Data Transfers: " << No_of_data_transfers << endl;
        outFile << "Total Control Operations: " << No_of_control_operations << endl;
        outFile << "Total Data Hazards: " << No_of_data_hazards << endl;
        outFile << "Total Control Hazards: " << No_of_control_hazards << endl;
        outFile << "Total Mispredictions: " << No_of_Mispredictions << endl;
        outFile << "Total Stalls: " << No_of_stalls_data_hazards + No_of_stalls_control_hazards << endl;
        outFile << "Stalls due to Data Hazards: " << No_of_stalls_data_hazards << endl;
        outFile << "Stalls due to Control Hazards: " << No_of_stalls_control_hazards << endl;

        saveRegisterToFile(register_file);
        saveMemoryToFile(memory_file);
        cout << "Simulation Completed" << endl;
    }

    void changeVariables()
    {
        ifstream inFile("variables.txt");
        if (!inFile)
        {
            cerr << "Error: Could not open file for reading!\n";
            return;
        }
        string line;
        int count = 0;
        while (getline(inFile, line))
        {
            if (count == 0)
            {
                if (line == "1")
                {
                    Pipeline = true;
                }
                else
                {
                    Pipeline = false;
                }
            }

            if (count == 1)
            {
                if (line == "1")
                {
                    DataForwarding = true;
                }
                else
                {
                    DataForwarding = false;
                }
            }
            if (count == 2)
            {
                if (line == "1")
                {
                    print_registers = true;
                }
                else
                {
                    print_registers = false;
                }
            }
            if (count == 3)
            {
                if (line == "1")
                {
                    print_pipeline_registers = true;
                }
                else
                {
                    print_pipeline_registers = false;
                }
            }
            if (count == 4)
            {
                if (line == "1")
                {
                    print_BPU = true;
                }
                else
                {
                    print_BPU = false;
                }
            }
            if (count == 5)
            {
                if (line == "1")
                {
                    specificInstruction = true;
                }
                else
                {
                    specificInstruction = false;
                }
            }
            if (count == 6)
            {
                if (line == "1")
                {
                    printCycle = true;
                }
                else
                {
                    printCycle = false;
                }
            }
            if (count == 7 && specificInstruction)
            {
                specificInstructionAddress = stoi(line);
                specificInstructionAddress--;
            }
            count++;
        }
        inFile.close();
    }
    Simulator(const string &filename, const string &outputFile, const string &memory_file, const string &register_file)
    {
        changeVariables();

        if (Pipeline)
        {
            Pipelined(filename, outputFile, memory_file, register_file);
        }
        else
        {
            NonPiplined(filename, outputFile, memory_file, register_file);
        }
    }
};

#endif