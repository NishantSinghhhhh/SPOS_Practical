#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

// Complete struct definitions needed for display functions
struct SymbolTableEntry {
    string symbol;
    int address;
    int length;
};

struct LiteralTableEntry {
    string literal;
    int value;
    int address;
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
};

struct AssemblerData {
    map<string, SymbolTableEntry> symbolTable;
    vector<LiteralTableEntry> literalTable;
    vector<IntermediateCodeLine> intermediateCode;
    vector<string> errors;
};

void displaySymbolTable(const AssemblerData& data) {
    cout << "\n" << string(60, '=') << endl;
    cout << "SYMBOL TABLE" << endl;
    cout << string(60, '=') << endl;
    cout << left << setw(20) << "Symbol" 
         << setw(15) << "Address" 
         << setw(10) << "Length" << endl;
    cout << string(60, '-') << endl;
    
    for (const auto& pair : data.symbolTable) {
        cout << left << setw(20) << pair.second.symbol
             << setw(15) << pair.second.address
             << setw(10) << pair.second.length << endl;
    }
}

void displayLiteralTable(const AssemblerData& data) {
    cout << "\n" << string(60, '=') << endl;
    cout << "LITERAL TABLE" << endl;
    cout << string(60, '=') << endl;
    cout << left << setw(10) << "Index" 
         << setw(20) << "Literal" 
         << setw(15) << "Value" 
         << setw(15) << "Address" << endl;
    cout << string(60, '-') << endl;
    
    for (size_t i = 0; i < data.literalTable.size(); i++) {
        cout << left << setw(10) << i
             << setw(20) << data.literalTable[i].literal
             << setw(15) << data.literalTable[i].value
             << setw(15) << data.literalTable[i].address << endl;
    }
}

void displayIntermediateCode(const AssemblerData& data) {
    cout << "\n" << string(70, '=') << endl;
    cout << "INTERMEDIATE CODE" << endl;
    cout << string(70, '=') << endl;
    cout << left << setw(8) << "LC" 
         << setw(15) << "Instruction" 
         << setw(20) << "Operand1" 
         << setw(20) << "Operand2" << endl;
    cout << string(70, '-') << endl;
    
    for (const auto& ic : data.intermediateCode) {
        cout << left << setw(8) << ic.locationCounter;
        cout << "(" << ic.type << "," << ic.opcode << ")";
        cout << setw(15 - ic.type.length() - ic.opcode.length() - 3) << " ";
        
        if (!ic.operand1Type.empty()) {
            string op1 = "(" + ic.operand1Type + "," + ic.operand1Value + ")";
            cout << setw(20) << op1;
        } else {
            cout << setw(20) << " ";
        }
        
        if (!ic.operand2Type.empty()) {
            string op2 = "(" + ic.operand2Type + "," + ic.operand2Value + ")";
            cout << setw(20) << op2;
        }
        
        cout << endl;
    }
}

void displaySourceCode(const string& filename) {
    cout << "\nSOURCE CODE:" << endl;
    cout << string(60, '=') << endl;
    
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        return;
    }
    
    string line;
    int lineNum = 1;
    while (getline(file, line)) {
        cout << setw(3) << lineNum++ << ": " << line << endl;
    }
    file.close();
}

void displayMachineCode(const string& filename) {
    cout << "\n" << string(60, '=') << endl;
    cout << "MACHINE CODE (OUTPUT)" << endl;
    cout << string(60, '=') << endl;
    
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        return;
    }
    
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
}

void displayErrors(const AssemblerData& data) {
    if (!data.errors.empty()) {
        cout << "\n" << string(60, '=') << endl;
        cout << "ERRORS" << endl;
        cout << string(60, '=') << endl;
        for (const auto& error : data.errors) {
            cout << error << endl;
        }
    }
}