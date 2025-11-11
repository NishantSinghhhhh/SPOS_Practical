#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>

using namespace std;

enum class InstructionType {
    IMPERATIVE,
    DECLARATIVE,
    ASSEMBLER
};

struct Instruction {
    string mnemonic;
    int opcode;
    int length;
    InstructionType type;
    
    Instruction() : opcode(-1), length(0), type(InstructionType::IMPERATIVE) {}
    Instruction(const string& mn, int op, int len, InstructionType t)
        : mnemonic(mn), opcode(op), length(len), type(t) {}
};

struct SymbolTableEntry {
    string symbol;
    int address;
    int length;
    
    SymbolTableEntry() : address(0), length(1) {}
    SymbolTableEntry(const string& sym, int addr, int len = 1)
        : symbol(sym), address(addr), length(len) {}
};

struct LiteralTableEntry {
    string literal;
    int value;
    int address;
    
    LiteralTableEntry() : value(0), address(-1) {}
    LiteralTableEntry(const string& lit, int val, int addr = -1)
        : literal(lit), value(val), address(addr) {}
};

struct IntermediateCodeLine {
    int lineNumber;
    int locationCounter;
    string type;
    string opcode;
    string operand1Type;
    string operand1Value;
    string operand2Type;
    string operand2Value;
    
    IntermediateCodeLine() : lineNumber(0), locationCounter(0) {}
};

struct AssemblerData {
    unordered_map<string, Instruction> MOT;
    unordered_map<string, int> REGISTERS;
    unordered_map<string, int> CONDITION_CODES;
    
    map<string, SymbolTableEntry> symbolTable;
    vector<LiteralTableEntry> literalTable;
    vector<int> poolTable;
    vector<IntermediateCodeLine> intermediateCode;
    vector<string> errors;
    
    int locationCounter;
    int startingAddress;
    
    AssemblerData() : locationCounter(0), startingAddress(0) {}
    
};

void initializeTables(AssemblerData& data) {
    data.MOT["STOP"] = Instruction("STOP", 0, 1, InstructionType::IMPERATIVE);
    data.MOT["ADD"] = Instruction("ADD", 1, 1, InstructionType::IMPERATIVE);
    data.MOT["SUB"] = Instruction("SUB", 2, 1, InstructionType::IMPERATIVE);
    data.MOT["MULT"] = Instruction("MULT", 3, 1, InstructionType::IMPERATIVE);
    data.MOT["MOVER"] = Instruction("MOVER", 4, 1, InstructionType::IMPERATIVE);
    data.MOT["MOVEM"] = Instruction("MOVEM", 5, 1, InstructionType::IMPERATIVE);
    data.MOT["COMP"] = Instruction("COMP", 6, 1, InstructionType::IMPERATIVE);
    data.MOT["BC"] = Instruction("BC", 7, 1, InstructionType::IMPERATIVE);
    data.MOT["DIV"] = Instruction("DIV", 8, 1, InstructionType::IMPERATIVE);
    data.MOT["READ"] = Instruction("READ", 9, 1, InstructionType::IMPERATIVE);
    data.MOT["PRINT"] = Instruction("PRINT", 10, 1, InstructionType::IMPERATIVE);
    
    data.MOT["DS"] = Instruction("DS", 1, 0, InstructionType::DECLARATIVE);
    data.MOT["DC"] = Instruction("DC", 2, 1, InstructionType::DECLARATIVE);
    
    data.MOT["START"] = Instruction("START", 1, 0, InstructionType::ASSEMBLER);
    data.MOT["END"] = Instruction("END", 2, 0, InstructionType::ASSEMBLER);
    data.MOT["ORIGIN"] = Instruction("ORIGIN", 3, 0, InstructionType::ASSEMBLER);
    data.MOT["EQU"] = Instruction("EQU", 4, 0, InstructionType::ASSEMBLER);
    data.MOT["LTORG"] = Instruction("LTORG", 5, 0, InstructionType::ASSEMBLER);
    
    data.REGISTERS["AREG"] = 1;
    data.REGISTERS["BREG"] = 2;
    data.REGISTERS["CREG"] = 3;
    data.REGISTERS["DREG"] = 4;
    
    data.CONDITION_CODES["LT"] = 1;
    data.CONDITION_CODES["LE"] = 2;
    data.CONDITION_CODES["EQ"] = 3;
    data.CONDITION_CODES["GT"] = 4;
    data.CONDITION_CODES["GE"] = 5;
    data.CONDITION_CODES["ANY"] = 6;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    size_t start = 0, end = 0;
    while ((end = str.find(delimiter, start)) != string::npos) {
        token = trim(str.substr(start, end - start));
        if (!token.empty()) tokens.push_back(token);
        start = end + 1;
    }
    token = trim(str.substr(start));
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

bool isLiteral(const string& operand) {
    return !operand.empty() && operand[0] == '=';
}

int getLiteralValue(const string& literal) {
    string value = literal.substr(1);
    if (!value.empty() && value[0] == '\'' && value[value.length()-1] == '\'') {
        value = value.substr(1, value.length() - 2);
    }
    try {
        return stoi(value);
    } catch (...) {
        return 0;
    }
}
