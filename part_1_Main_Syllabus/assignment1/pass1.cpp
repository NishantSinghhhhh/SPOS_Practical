#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

using namespace std;

// Complete struct definitions
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

// Utility functions
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

void addLiteral(const string& literal, AssemblerData& data) {
    for (const auto& lit : data.literalTable) {
        if (lit.literal == literal) return;
    }
    int value = getLiteralValue(literal);
    data.literalTable.push_back(LiteralTableEntry(literal, value));
}

void processLTORG(AssemblerData& data) {
    for (auto& lit : data.literalTable) {
        if (lit.address == -1) {
            lit.address = data.locationCounter;
            data.locationCounter++;
        }
    }
}

void addSymbol(const string& symbol, int address, AssemblerData& data) {
    auto it = data.symbolTable.find(symbol);
    if (it != data.symbolTable.end()) {
        if (it->second.address != 0) {
            data.errors.push_back("Error: Symbol '" + symbol + "' already defined");
        } else {
            it->second.address = address;
        }
    } else {
        data.symbolTable[symbol] = SymbolTableEntry(symbol, address, 1);
    }
}

int getSymbolAddress(const string& symbol, AssemblerData& data) {
    auto it = data.symbolTable.find(symbol);
    if (it != data.symbolTable.end()) {
        return it->second.address;
    }
    data.symbolTable[symbol] = SymbolTableEntry(symbol, 0, 1);
    return 0;
}

int evaluateExpression(const string& expr, AssemblerData& data) {
    size_t plusPos = expr.find('+');
    size_t minusPos = expr.find('-');
    
    if (plusPos != string::npos) {
        string base = trim(expr.substr(0, plusPos));
        string offset = trim(expr.substr(plusPos + 1));
        return getSymbolAddress(base, data) + stoi(offset);
    } else if (minusPos != string::npos) {
        string base = trim(expr.substr(0, minusPos));
        string offset = trim(expr.substr(minusPos + 1));
        return getSymbolAddress(base, data) - stoi(offset);
    }
    return getSymbolAddress(expr, data);
}

void processLine(const string& line, int lineNum, AssemblerData& data) {
    string cleanLine = line;
    size_t commentPos = cleanLine.find(';');
    if (commentPos != string::npos) {
        cleanLine = cleanLine.substr(0, commentPos);
    }
    
    cleanLine = trim(cleanLine);
    if (cleanLine.empty()) return;
    
    istringstream iss(cleanLine);
    vector<string> tokens;
    string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    if (tokens.empty()) return;
    
    string label, mnemonic, operand1, operand2;
    size_t idx = 0;
    
    string upperFirst = tokens[0];
    transform(upperFirst.begin(), upperFirst.end(), upperFirst.begin(), ::toupper);
    
    if (data.MOT.find(upperFirst) == data.MOT.end() && tokens.size() > 1) {
        label = tokens[idx++];
    }
    
    if (idx < tokens.size()) {
        mnemonic = tokens[idx++];
        transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(), ::toupper);
    }
    
    if (idx < tokens.size()) {
        string remaining;
        for (size_t i = idx; i < tokens.size(); i++) {
            remaining += tokens[i];
            if (i < tokens.size() - 1) remaining += " ";
        }
        auto operands = split(remaining, ',');
        if (operands.size() > 0) operand1 = operands[0];
        if (operands.size() > 1) operand2 = operands[1];
    }
    
    IntermediateCodeLine ic;
    ic.lineNumber = lineNum;
    ic.locationCounter = data.locationCounter;
    
    if (mnemonic == "START") {
        if (!operand1.empty()) {
            data.startingAddress = stoi(operand1);
            data.locationCounter = data.startingAddress;
            ic.locationCounter = data.locationCounter;
        }
        ic.type = "AD";
        ic.opcode = "1";
        ic.operand1Type = "C";
        ic.operand1Value = to_string(data.startingAddress);
        data.intermediateCode.push_back(ic);
        return;
    }
    
    if (mnemonic == "END") {
        processLTORG(data);
        ic.type = "AD";
        ic.opcode = "2";
        data.intermediateCode.push_back(ic);
        return;
    }
    
    if (mnemonic == "ORIGIN") {
        if (!operand1.empty()) {
            data.locationCounter = evaluateExpression(operand1, data);
            ic.locationCounter = data.locationCounter;
        }
        ic.type = "AD";
        ic.opcode = "3";
        ic.operand1Type = "";
        ic.operand1Value = operand1;
        data.intermediateCode.push_back(ic);
        return;
    }
    
    if (mnemonic == "EQU") {
        if (!label.empty() && !operand1.empty()) {
            int addr = evaluateExpression(operand1, data);
            addSymbol(label, addr, data);
        }
        ic.type = "AD";
        ic.opcode = "4";
        ic.operand1Type = "";
        ic.operand1Value = operand1;
        data.intermediateCode.push_back(ic);
        return;
    }
    
    if (mnemonic == "LTORG") {
        processLTORG(data);
        ic.type = "AD";
        ic.opcode = "5";
        data.intermediateCode.push_back(ic);
        return;
    }
    
    if (!label.empty()) {
        addSymbol(label, data.locationCounter, data);
    }
    
    if (data.MOT.find(mnemonic) == data.MOT.end()) {
        data.errors.push_back("Line " + to_string(lineNum) + ": Unknown instruction '" + mnemonic + "'");
        return;
    }
    
    Instruction instruction = data.MOT[mnemonic];
    
    if (instruction.type == InstructionType::IMPERATIVE) {
        ic.type = "IS";
        ic.opcode = to_string(instruction.opcode);
        
        if (!operand1.empty()) {
            string op1 = operand1;
            transform(op1.begin(), op1.end(), op1.begin(), ::toupper);
            
            if (data.REGISTERS.find(op1) != data.REGISTERS.end()) {
                ic.operand1Type = "R";
                ic.operand1Value = to_string(data.REGISTERS[op1]);
            } else if (data.CONDITION_CODES.find(op1) != data.CONDITION_CODES.end()) {
                ic.operand1Type = "CC";
                ic.operand1Value = to_string(data.CONDITION_CODES[op1]);
            }
        }
        
        if (!operand2.empty()) {
            if (isLiteral(operand2)) {
                addLiteral(operand2, data);
                ic.operand2Type = "L";
                for (size_t i = 0; i < data.literalTable.size(); i++) {
                    if (data.literalTable[i].literal == operand2) {
                        ic.operand2Value = to_string(i);
                        break;
                    }
                }
            } else {
                getSymbolAddress(operand2, data);
                ic.operand2Type = "S";
                ic.operand2Value = operand2;
            }
        }
        
        data.intermediateCode.push_back(ic);
        data.locationCounter += instruction.length;
        
    } else if (instruction.type == InstructionType::DECLARATIVE) {
        ic.type = "DL";
        ic.opcode = to_string(instruction.opcode);
        
        if (mnemonic == "DS") {
            int size = !operand1.empty() ? stoi(operand1) : 1;
            ic.operand1Type = "C";
            ic.operand1Value = to_string(size);
            data.intermediateCode.push_back(ic);
            data.locationCounter += size;
        } else if (mnemonic == "DC") {
            string value = operand1;
            if (!value.empty() && value[0] == '\'' && value[value.length()-1] == '\'') {
                value = value.substr(1, value.length() - 2);
            }
            ic.operand1Type = "C";
            ic.operand1Value = value;
            data.intermediateCode.push_back(ic);
            data.locationCounter += 1;
        }
    }
}

void pass1(const string& inputFile, const string& intermediateFile, 
           const string& symbolFile, const string& literalFile, AssemblerData& data) {
    
    ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        cerr << "Error: Cannot open " << inputFile << endl;
        return;
    }
    
    string line;
    int lineNum = 1;
    while (getline(inFile, line)) {
        processLine(line, lineNum++, data);
    }
    inFile.close();
    
    ofstream icFile(intermediateFile);
    for (const auto& ic : data.intermediateCode) {
        icFile << ic.locationCounter << " "
               << "(" << ic.type << "," << ic.opcode << ")";
        if (!ic.operand1Type.empty()) {
            icFile << " (" << ic.operand1Type << "," << ic.operand1Value << ")";
        }
        if (!ic.operand2Type.empty()) {
            icFile << " (" << ic.operand2Type << "," << ic.operand2Value << ")";
        }
        icFile << endl;
    }
    icFile.close();
    
    ofstream symFile(symbolFile);
    for (const auto& pair : data.symbolTable) {
        symFile << pair.second.symbol << " " 
                << pair.second.address << " " 
                << pair.second.length << endl;
    }
    symFile.close();
    
    ofstream litFile(literalFile);
    for (size_t i = 0; i < data.literalTable.size(); i++) {
        litFile << i << " " 
                << data.literalTable[i].literal << " "
                << data.literalTable[i].value << " "
                << data.literalTable[i].address << endl;
    }
    litFile.close();
    
    cout << "PASS 1 COMPLETED" << endl;
    cout << "Intermediate code written to: " << intermediateFile << endl;
    cout << "Symbol table written to: " << symbolFile << endl;
    cout << "Literal table written to: " << literalFile << endl;
}