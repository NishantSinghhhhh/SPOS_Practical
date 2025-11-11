#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

using namespace std;

// -------------------- Data Types --------------------
struct SymbolTableEntry {
    string symbol;
    int address;
    int length;

    SymbolTableEntry() : symbol(""), address(0), length(0) {}
    SymbolTableEntry(const string& s, int a, int l) : symbol(s), address(a), length(l) {}
};

struct LiteralTableEntry {
    string literal; // e.g., =’5’ or =5 depending on your format
    int value;
    int address; // -1 if not assigned

    LiteralTableEntry() : literal(""), value(0), address(-1) {}
    LiteralTableEntry(const string& lit, int val, int addr) : literal(lit), value(val), address(addr) {}
};

struct IntermediateCodeLine {
    int locationCounter = 0;

    string type;            // AD / IS / DL
    string opcode;          // numeric string

    string operand1Type;    // R / CC / (maybe empty)
    string operand1Value;   // register no / condition code / numeric string

    string operand2Type;    // S / L / (maybe empty)
    string operand2Value;   // symbol name OR literal index as string
};

struct AssemblerData {
    map<string, SymbolTableEntry> symbolTable;
    vector<LiteralTableEntry>     literalTable;
    vector<IntermediateCodeLine>  intermediateCode;
};

// -------------------- Loaders --------------------
void loadSymbolTable(const string& filename, AssemblerData& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        return;
    }

    string symbol;
    int address, length;
    while (file >> symbol >> address >> length) {
        data.symbolTable[symbol] = SymbolTableEntry(symbol, address, length);
    }
    file.close();
}

void loadLiteralTable(const string& filename, AssemblerData& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        return;
    }

    int index, value, address;
    string literal;
    while (file >> index >> literal >> value >> address) {
        // We ignore the incoming index and store in order.
        data.literalTable.push_back(LiteralTableEntry(literal, value, address));
    }
    file.close();
}

void loadIntermediateCode(const string& filename, AssemblerData& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        IntermediateCodeLine ic;
        istringstream iss(line);

        iss >> ic.locationCounter;

        string token;

        // First token like (IS,04) / (AD,01) / (DL,02)
        if (!(iss >> token)) { continue; }
        {
            size_t comma = token.find(',');
            ic.type   = token.substr(1, comma - 1);                             // between '(' and ','
            ic.opcode = token.substr(comma + 1, token.length() - comma - 2);    // between ',' and ')'
        }

        // Operand1 like (R,1) or (CC,1)
        if (iss >> token) {
            size_t comma = token.find(',');
            ic.operand1Type  = token.substr(1, comma - 1);
            ic.operand1Value = token.substr(comma + 1, token.length() - comma - 2);
        }

        // Operand2 like (S,LOOP) or (L,0)
        if (iss >> token) {
            size_t comma = token.find(',');
            ic.operand2Type  = token.substr(1, comma - 1);
            ic.operand2Value = token.substr(comma + 1, token.length() - comma - 2);
        }

        data.intermediateCode.push_back(ic);
    }
    file.close();
}

// -------------------- PASS 2 --------------------
void pass2(const string& intermediateFile, const string& symbolFile,
           const string& literalFile, const string& outputFile, AssemblerData& data) {

    loadSymbolTable(symbolFile, data);
    loadLiteralTable(literalFile, data);
    loadIntermediateCode(intermediateFile, data);

    ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        cerr << "Error: Cannot create " << outputFile << endl;
        return;
    }

    outFile << "ADDRESS  MACHINE CODE" << endl;
    outFile << "==============================" << endl;

    for (const auto& ic : data.intermediateCode) {
        // Skip assembler directives that don't generate code
        if (ic.type == "AD") {
            // Common AD opcodes like START(01), END(02), ORIGIN(03), EQU(04), LTORG(05)
            if (ic.opcode == "1" || ic.opcode == "2" || ic.opcode == "3" ||
                ic.opcode == "4" || ic.opcode == "5") {
                continue;
            }
        }

        outFile << setw(4) << setfill('0') << ic.locationCounter << "     ";

        if (ic.type == "IS") {
            outFile << "+";

            outFile << setw(2) << setfill('0') << ic.opcode;

            // Operand 1 (reg/cc)
            if (!ic.operand1Type.empty()) {
                if (ic.operand1Type == "R" || ic.operand1Type == "CC") {
                    outFile << " " << ic.operand1Value;
                } else {
                    outFile << " 0";
                }
            } else {
                outFile << " 0";
            }

            // Operand 2 (symbol/literal/address)
            if (!ic.operand2Type.empty()) {
                if (ic.operand2Type == "S") {
                    auto it = data.symbolTable.find(ic.operand2Value);
                    if (it != data.symbolTable.end()) {
                        outFile << " " << setw(4) << setfill('0') << it->second.address;
                    } else {
                        outFile << " 0000";
                    }
                } else if (ic.operand2Type == "L") {
                    int litIndex = 0;
                    try { litIndex = stoi(ic.operand2Value); } catch (...) { litIndex = -1; }
                    if (litIndex >= 0 && litIndex < (int)data.literalTable.size()) {
                        outFile << " " << setw(4) << setfill('0')
                                << data.literalTable[litIndex].address;
                    } else {
                        outFile << " 0000";
                    }
                } else {
                    outFile << " 0000";
                }
            } else {
                outFile << " 0000";
            }

        } else if (ic.type == "DL") {
            // DL 01 = DS (declare storage), DL 02 = DC (declare constant)
            if (ic.opcode == "1") { // DS
                int size = 0;
                try { size = stoi(ic.operand1Value); } catch (...) { size = 0; }
                for (int i = 0; i < size; i++) {
                    if (i > 0) {
                        outFile << "\n" << setw(4) << setfill('0') << (ic.locationCounter + i) << "     ";
                    }
                    outFile << "+00 0 0000";
                }
            } else if (ic.opcode == "2") { // DC
                int val = 0;
                try { val = stoi(ic.operand1Value); } catch (...) { val = 0; }
                outFile << "+00 0 " << setw(4) << setfill('0') << val;
            } else {
                outFile << "+00 0 0000";
            }
        } else {
            // Unknown type, still print a placeholder
            outFile << "+00 0 0000";
        }

        outFile << endl;
    }

    // Emit literals that have assigned addresses
    for (const auto& lit : data.literalTable) {
        if (lit.address != -1) {
            outFile << setw(4) << setfill('0') << lit.address << "     ";
            outFile << "+00 0 " << setw(4) << setfill('0') << lit.value << endl;
        }
    }

    outFile.close();

    cout << "\nPASS 2 COMPLETED" << endl;
    cout << "Machine code written to: " << outputFile << endl;
}

// -------------------- Example main (optional) --------------------
int main(int argc, char* argv[]) {
    // Usage: ./pass2 <intermediate.txt> <symtab.txt> <littab.txt> <output.txt>
    if (argc < 5) {
        cerr << "Usage: " << argv[0]
             << " <intermediate.txt> <symtab.txt> <littab.txt> <output.txt>\n";
        return 1;
    }
    AssemblerData data;
    pass2(argv[1], argv[2], argv[3], argv[4], data);
    return 0;
}
