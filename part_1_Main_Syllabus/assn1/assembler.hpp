#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

enum class InstructionType { IMPERATIVE, DECLARATIVE, ASSEMBLER };

struct Instruction {
    std::string mnemonic;
    int opcode;
    int length;
    InstructionType type;
    Instruction() : opcode(-1), length(0), type(InstructionType::IMPERATIVE) {}
    Instruction(const std::string& mn, int op, int len, InstructionType t)
        : mnemonic(mn), opcode(op), length(len), type(t) {}
};

struct SymbolTableEntry {
    std::string symbol;
    int address;
    int length;
    SymbolTableEntry() : address(0), length(1) {}
    SymbolTableEntry(const std::string& sym, int addr, int len = 1)
        : symbol(sym), address(addr), length(len) {}
};

struct LiteralTableEntry {
    std::string literal;
    int value;
    int address; // -1 if not assigned
    LiteralTableEntry() : value(0), address(-1) {}
    LiteralTableEntry(const std::string& lit, int val, int addr = -1)
        : literal(lit), value(val), address(addr) {}
};

struct IntermediateCodeLine {
    int lineNumber{0};
    int locationCounter{0};
    std::string type;           // AD / IS / DL
    std::string opcode;         // numeric string
    std::string operand1Type;   // R / CC / (maybe empty)
    std::string operand1Value;  // reg/cc number or const
    std::string operand2Type;   // S / L / (maybe empty)
    std::string operand2Value;  // symbol name OR literal index
};

struct AssemblerData {
    std::unordered_map<std::string, Instruction> MOT;
    std::unordered_map<std::string, int> REGISTERS;
    std::unordered_map<std::string, int> CONDITION_CODES;

    std::map<std::string, SymbolTableEntry> symbolTable;
    std::vector<LiteralTableEntry> literalTable;
    std::vector<int> poolTable;
    std::vector<IntermediateCodeLine> intermediateCode;
    std::vector<std::string> errors;

    int locationCounter{0};
    int startingAddress{0};
};

// ----- declarations -----
void initializeTables(AssemblerData& data);

// pass 1 & pass 2
void pass1(const std::string& inputFile,
           const std::string& intermediateFile,
           const std::string& symbolFile,
           const std::string& literalFile,
           AssemblerData& data);

void pass2(const std::string& intermediateFile,
           const std::string& symbolFile,
           const std::string& literalFile,
           const std::string& outputFile,
           AssemblerData& data);

// display helpers
void displaySymbolTable(const AssemblerData& data);
void displayLiteralTable(const AssemblerData& data);
void displayIntermediateCode(const AssemblerData& data);
void displaySourceCode(const std::string& filename);
void displayMachineCode(const std::string& filename);
void displayErrors(const AssemblerData& data);
