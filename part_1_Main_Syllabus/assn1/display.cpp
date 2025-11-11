#include "assembler.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

using std::cout; using std::endl; using std::left; using std::setw; using std::string;

void displaySymbolTable(const AssemblerData& data) {
    cout << "\n" << string(60, '=') << endl;
    cout << "SYMBOL TABLE" << endl;
    cout << string(60, '=') << endl;
    cout << left << setw(20) << "Symbol" << setw(15) << "Address" << setw(10) << "Length" << endl;
    cout << string(60, '-') << endl;
    for (const auto& p : data.symbolTable)
        cout << left << setw(20) << p.second.symbol << setw(15) << p.second.address << setw(10) << p.second.length << endl;
}

void displayLiteralTable(const AssemblerData& data) {
    cout << "\n" << string(60, '=') << endl;
    cout << "LITERAL TABLE" << endl;
    cout << string(60, '=') << endl;
    cout << left << setw(10) << "Index" << setw(20) << "Literal" << setw(15) << "Value" << setw(15) << "Address" << endl;
    cout << string(60, '-') << endl;
    for (size_t i=0;i<data.literalTable.size();++i)
        cout << left << setw(10) << i << setw(20) << data.literalTable[i].literal
             << setw(15) << data.literalTable[i].value << setw(15) << data.literalTable[i].address << endl;
}

void displayIntermediateCode(const AssemblerData& data) {
    cout << "\n" << string(70, '=') << endl;
    cout << "INTERMEDIATE CODE" << endl;
    cout << string(70, '=') << endl;
    cout << left << setw(8) << "LC" << setw(15) << "Instruction" << setw(20) << "Operand1" << setw(20) << "Operand2" << endl;
    cout << string(70, '-') << endl;

    for (const auto& ic : data.intermediateCode) {
        cout << left << setw(8) << ic.locationCounter;
        string instr = "(" + ic.type + "," + ic.opcode + ")";
        cout << setw(15) << instr;
        if (!ic.operand1Type.empty()) {
            string op1 = "(" + ic.operand1Type + "," + ic.operand1Value + ")";
            cout << setw(20) << op1;
        } else cout << setw(20) << " ";
        if (!ic.operand2Type.empty()) {
            string op2 = "(" + ic.operand2Type + "," + ic.operand2Value + ")";
            cout << setw(20) << op2;
        }
        cout << endl;
    }
}

void displaySourceCode(const string& filename) {
    cout << "\nSOURCE CODE:\n" << string(60, '=') << endl;
    std::ifstream f(filename);
    if (!f.is_open()) { std::cerr << "Error: Cannot open " << filename << endl; return; }
    string line; int ln=1; while (std::getline(f, line)) cout << std::setw(3) << ln++ << ": " << line << endl;
}

void displayMachineCode(const string& filename) {
    cout << "\n" << string(60, '=') << endl;
    cout << "MACHINE CODE (OUTPUT)" << endl;
    cout << string(60, '=') << endl;
    std::ifstream f(filename);
    if (!f.is_open()) { std::cerr << "Error: Cannot open " << filename << endl; return; }
    string line; while (std::getline(f, line)) cout << line << endl;
}

void displayErrors(const AssemblerData& data) {
    if (!data.errors.empty()) {
        cout << "\n" << string(60, '=') << endl;
        cout << "ERRORS" << endl;
        cout << string(60, '=') << endl;
        for (const auto& e : data.errors) cout << e << endl;
    }
}
