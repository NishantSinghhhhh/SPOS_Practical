// pass2.cpp — generates machine code from the outputs of Pass 1
// Inputs  : intermediate.txt, symbol_table.txt, literal_table.txt
// Output  : output.txt (addressed machine code)
// Depends : assembler.hpp (shared data structures + declarations)

#include "assembler.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

using std::string;

// -----------------------------
// Helpers to load Pass 1 outputs
// -----------------------------

// Load symbol table produced by pass1: lines like
//   <symbol> <address> <length>
static void loadSymbolTable(const string& filename, AssemblerData& data) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Error: Cannot open " << filename << "\n";
        return;
    }
    string symbol; int address, length;
    // Fill/overwrite entries in data.symbolTable
    while (f >> symbol >> address >> length) {
        data.symbolTable[symbol] = SymbolTableEntry(symbol, address, length);
    }
}

// Load literal table produced by pass1: lines like
//   <index> <literal> <value> <address>
// Note: we ignore the incoming index and push in file order.
static void loadLiteralTable(const string& filename, AssemblerData& data) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Error: Cannot open " << filename << "\n";
        return;
    }
    int index, value, address; string literal;
    while (f >> index >> literal >> value >> address) {
        data.literalTable.push_back(LiteralTableEntry(literal, value, address));
    }
}

// Load intermediate code lines produced by pass1: lines like
//   <LC> (IS,04) (R,1) (S,LOOP)
//   <LC> (DL,02) (C,10)
//   <LC> (AD,01) (C,100)
// The format is stable: LC, then up to three parenthesized tokens.
static void loadIntermediateCode(const string& filename, AssemblerData& data) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Error: Cannot open " << filename << "\n";
        return;
    }
    string line;
    while (std::getline(f, line)) {
        if (line.empty()) continue;

        IntermediateCodeLine ic;
        std::istringstream iss(line);

        // First field is always LC (location counter)
        iss >> ic.locationCounter;

        // Next token must be like "(IS,04)" or "(AD,03)" or "(DL,02)"
        string token;
        if (!(iss >> token)) continue;

        // Parse "(TYPE,OP)" -> TYPE=IS/AD/DL, OP is numeric code as string
        {
            size_t c = token.find(',');
            ic.type   = token.substr(1, c - 1);                   // skip '('
            ic.opcode = token.substr(c + 1, token.size() - c - 2); // drop trailing ')'
        }

        // Optional operand1 like "(R,1)" or "(CC,3)" or "(C,10)"
        if (iss >> token) {
            size_t c = token.find(',');
            ic.operand1Type  = token.substr(1, c - 1);
            ic.operand1Value = token.substr(c + 1, token.size() - c - 2);
        }

        // Optional operand2 like "(S,LOOP)" or "(L,0)"
        if (iss >> token) {
            size_t c = token.find(',');
            ic.operand2Type  = token.substr(1, c - 1);
            ic.operand2Value = token.substr(c + 1, token.size() - c - 2);
        }

        data.intermediateCode.push_back(ic);
    }
}

// -----------------------------------
// PASS 2 — generate addressed machine code
// -----------------------------------
//
// Rules implemented (typical 2-pass assembler model):
// - AD (assembler directives) generally do not emit machine code, so we skip them.
// - IS (imperative statements) emit opcode, reg/cc (or 0), and an address
//   resolved via symbol table (S) or literal table (L).
// - DL (declaratives):
//     * DS: reserves storage — we emit placeholders for each reserved cell.
//     * DC: defines constant — we emit a data word with that value.
// - Finally, we also output literal values at their assigned addresses
//   (useful when pass1 allocated literals via LTORG/END).
//
void pass2(const std::string& intermediateFile, const std::string& symbolFile,
           const std::string& literalFile, const std::string& outputFile, AssemblerData& data) {

    // Bring in all the artifacts from Pass 1
    loadSymbolTable(symbolFile, data);
    loadLiteralTable(literalFile, data);
    loadIntermediateCode(intermediateFile, data);

    // Prepare the output listing file
    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Error: Cannot create " << outputFile << "\n";
        return;
    }

    // Simple header for human-readable output
    out << "ADDRESS  MACHINE CODE\n";
    out << "==============================\n";

    // Convert each intermediate instruction to final code
    for (const auto& ic : data.intermediateCode) {
        // AD = assembler directives START/END/ORIGIN/EQU/LTORG — no code emitted
        if (ic.type == "AD") {
            if (ic.opcode=="1"||ic.opcode=="2"||ic.opcode=="3"||ic.opcode=="4"||ic.opcode=="5")
                continue;
        }

        // Print the address first (4 digits, zero-padded)
        out << std::setw(4) << std::setfill('0') << ic.locationCounter << "     ";

        if (ic.type == "IS") {
            // Imperative statement: +<opcode> <r/cc> <address>
            out << "+";
            out << std::setw(2) << std::setfill('0') << ic.opcode;

            // Operand 1: register or condition code; if absent -> 0
            if (!ic.operand1Type.empty() && (ic.operand1Type=="R" || ic.operand1Type=="CC")) {
                out << " " << ic.operand1Value;
            } else {
                out << " 0";
            }

            // Operand 2: address field resolved from symbol (S) or literal (L)
            if (!ic.operand2Type.empty()) {
                if (ic.operand2Type == "S") {
                    // Resolve symbol -> absolute address
                    auto it = data.symbolTable.find(ic.operand2Value);
                    if (it != data.symbolTable.end())
                        out << " " << std::setw(4) << std::setfill('0') << it->second.address;
                    else
                        out << " 0000"; // unknown symbol fallback
                } else if (ic.operand2Type == "L") {
                    // Resolve literal index -> literal address
                    int idx = -1;
                    try { idx = std::stoi(ic.operand2Value); } catch (...) {}
                    if (idx >= 0 && idx < (int)data.literalTable.size())
                        out << " " << std::setw(4) << std::setfill('0') << data.literalTable[idx].address;
                    else
                        out << " 0000"; // bad index fallback
                } else {
                    out << " 0000";     // unsupported operand type
                }
            } else {
                out << " 0000";         // no second operand
            }
        }
        else if (ic.type == "DL") {
            // Declaratives
            if (ic.opcode == "1") { // DS — reserve N locations; emit placeholders at each LC+i
                int size = 0; try { size = std::stoi(ic.operand1Value); } catch (...) {}
                for (int i=0;i<size;i++) {
                    if (i>0) out << "\n" << std::setw(4) << std::setfill('0') << (ic.locationCounter + i) << "     ";
                    out << "+00 0 0000"; // placeholder word
                }
            } else if (ic.opcode == "2") { // DC — define constant; emit value in address field
                int val = 0; try { val = std::stoi(ic.operand1Value); } catch (...) {}
                out << "+00 0 " << std::setw(4) << std::setfill('0') << val;
            } else {
                // Unknown DL variant — emit a safe placeholder
                out << "+00 0 0000";
            }
        } else {
            // Unknown type — keep output shape stable
            out << "+00 0 0000";
        }

        out << "\n";
    }

    // Emit literal pool values (if pass1 assigned addresses via LTORG/END)
    // Each literal becomes a data word: "+00 0 <value>"
    for (const auto& lit : data.literalTable) {
        if (lit.address != -1) {
            out << std::setw(4) << std::setfill('0') << lit.address << "     "
                << "+00 0 " << std::setw(4) << std::setfill('0') << lit.value << "\n";
        }
    }

    out.close();
    std::cout << "PASS 2 COMPLETED\nMachine code: " << outputFile << "\n";
}
