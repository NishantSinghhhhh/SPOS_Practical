#include "assembler.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

using std::string;
using std::vector;

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * Trims whitespace characters from both ends of a string
 * @param s The input string to trim
 * @return A new string with leading and trailing whitespace removed
 */
static string trim(const string& s) {
    // Find the first character that is not whitespace
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return ""; // String is all whitespace
    
    // Find the last character that is not whitespace
    size_t b = s.find_last_not_of(" \t\r\n");
    
    // Extract the substring between first and last non-whitespace characters
    return s.substr(a, b - a + 1);
}

/**
 * Splits a string by a delimiter and returns vector of trimmed tokens
 * @param str The input string to split
 * @param delimiter The character to split on (e.g., comma)
 * @return Vector of trimmed, non-empty tokens
 */
static vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    size_t start = 0, end = 0;
    
    // Find each occurrence of the delimiter
    while ((end = str.find(delimiter, start)) != string::npos) {
        // Extract substring between delimiters
        token = trim(str.substr(start, end - start));
        if (!token.empty()) tokens.push_back(token); // Only add non-empty tokens
        start = end + 1; // Move past the delimiter
    }
    
    // Add the last token after the final delimiter
    token = trim(str.substr(start));
    if (!token.empty()) tokens.push_back(token);
    
    return tokens;
}

// ============================================================================
// LITERAL HANDLING FUNCTIONS
// ============================================================================

/**
 * Checks if an operand is a literal (starts with '=' character)
 * @param operand The operand string to check
 * @return true if the operand is a literal, false otherwise
 */
static bool isLiteral(const string& operand) {
    return !operand.empty() && operand[0] == '=';
}

/**
 * Extracts the numeric value from a literal
 * Handles both plain numbers (=5) and character literals (='C')
 * @param literal The literal string including the '=' prefix
 * @return The integer value of the literal
 */
static int getLiteralValue(const string& literal) {
    // Remove the '=' prefix
    string v = literal.substr(1);
    
    // Handle character literals enclosed in single quotes
    if (!v.empty() && v.front() == '\'' && v.back() == '\'') {
        v = v.substr(1, v.size() - 2); // Remove quotes
    }
    
    // Convert to integer, return 0 if conversion fails
    try {
        return std::stoi(v);
    } catch (...) {
        return 0;
    }
}

/**
 * Adds a literal to the literal table if it doesn't already exist
 * @param lit The literal string to add
 * @param d Reference to the assembler data structure
 */
static void addLiteral(const string& lit, AssemblerData& d) {
    // Check if literal already exists in the table
    for (auto& L : d.literalTable) {
        if (L.literal == lit) return; // Already exists, don't add duplicate
    }
    
    // Add new literal entry (address will be assigned later)
    d.literalTable.push_back(LiteralTableEntry(lit, getLiteralValue(lit)));
}

/**
 * Processes LTORG directive - assigns addresses to all pending literals
 * This is called when LTORG is encountered or at END of program
 * @param d Reference to the assembler data structure
 */
static void processLTORG(AssemblerData& d) {
    // Iterate through all literals in the table
    for (auto& L : d.literalTable) {
        // Only process literals that haven't been assigned an address yet
        if (L.address == -1) {
            // Assign current location counter as the literal's address
            L.address = d.locationCounter;
            // Increment location counter (each literal takes 1 word)
            d.locationCounter++;
        }
    }
}

// ============================================================================
// SYMBOL TABLE FUNCTIONS
// ============================================================================

/**
 * Adds or updates a symbol in the symbol table
 * @param sym The symbol name to add
 * @param addr The address to assign to the symbol
 * @param d Reference to the assembler data structure
 */
static void addSymbol(const string& sym, int addr, AssemblerData& d) {
    // Check if symbol already exists in the table
    auto it = d.symbolTable.find(sym);
    
    if (it != d.symbolTable.end()) {
        // Symbol exists - check if it was forward referenced (address = 0)
        if (it->second.address != 0) {
            // Symbol already defined - this is an error (duplicate label)
            d.errors.push_back("Error: Symbol '" + sym + "' already defined");
        } else {
            // Forward reference - now we can set its actual address
            it->second.address = addr;
        }
    } else {
        // New symbol - add it to the table
        d.symbolTable[sym] = SymbolTableEntry(sym, addr, 1);
    }
}

/**
 * Retrieves the address of a symbol from the symbol table
 * If symbol doesn't exist, creates a forward reference entry with address 0
 * @param sym The symbol name to look up
 * @param d Reference to the assembler data structure
 * @return The address of the symbol (0 if forward reference)
 */
static int getSymbolAddress(const string& sym, AssemblerData& d) {
    auto it = d.symbolTable.find(sym);
    
    if (it != d.symbolTable.end()) {
        // Symbol found - return its address
        return it->second.address;
    }
    
    // Symbol not found - create forward reference with address 0
    d.symbolTable[sym] = SymbolTableEntry(sym, 0, 1);
    return 0;
}

/**
 * Evaluates an expression (symbol, symbol+offset, or symbol-offset)
 * Used for ORIGIN and EQU directives
 * @param expr The expression string to evaluate
 * @param d Reference to the assembler data structure
 * @return The calculated address value
 */
static int evaluateExpression(const string& expr, AssemblerData& d) {
    // Check for addition operation (e.g., "LOOP+5")
    size_t p = expr.find('+');
    if (p != string::npos) {
        // Split into symbol and offset, then add them
        return getSymbolAddress(trim(expr.substr(0, p)), d) + 
               std::stoi(trim(expr.substr(p + 1)));
    }
    
    // Check for subtraction operation (e.g., "LOOP-3")
    size_t m = expr.find('-');
    if (m != string::npos) {
        // Split into symbol and offset, then subtract
        return getSymbolAddress(trim(expr.substr(0, m)), d) - 
               std::stoi(trim(expr.substr(m + 1)));
    }
    
    // No operation - just a simple symbol
    return getSymbolAddress(expr, d);
}

// ============================================================================
// LINE PROCESSING FUNCTION
// ============================================================================

/**
 * Processes a single line of assembly source code
 * Handles labels, mnemonics, operands, and generates intermediate code
 * @param line The source code line to process
 * @param lineNum The line number (for error reporting)
 * @param data Reference to the assembler data structure
 */
static void processLine(const string& line, int lineNum, AssemblerData& data) {
    // ========== STEP 1: Clean the line ==========
    string clean = line;
    
    // Remove comments (anything after semicolon)
    size_t cpos = clean.find(';');
    if (cpos != string::npos) {
        clean = clean.substr(0, cpos);
    }
    
    // Trim whitespace and skip empty lines
    clean = trim(clean);
    if (clean.empty()) return;

    // ========== STEP 2: Tokenize the line ==========
    std::istringstream iss(clean);
    vector<string> toks;
    string tok;
    
    // Split line into whitespace-separated tokens
    while (iss >> tok) {
        toks.push_back(tok);
    }
    
    if (toks.empty()) return;

    // ========== STEP 3: Parse label, mnemonic, and operands ==========
    string label, mnemonic, operand1, operand2;
    size_t idx = 0;

    // Check if first token is a label (not a mnemonic)
    string first = toks[0];
    std::transform(first.begin(), first.end(), first.begin(), ::toupper);
    
    // If first token is not in MOT and there are more tokens, it's a label
    if (data.MOT.find(first) == data.MOT.end() && toks.size() > 1) {
        label = toks[idx++];
    }

    // Get the mnemonic (instruction or directive)
    if (idx < toks.size()) {
        mnemonic = toks[idx++];
        std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(), ::toupper);
    }

    // Parse operands (comma-separated)
    if (idx < toks.size()) {
        // Combine remaining tokens back into a single string
        string rest;
        for (size_t i = idx; i < toks.size(); ++i) {
            rest += toks[i];
            if (i + 1 < toks.size()) rest += " ";
        }
        
        // Split operands by comma
        auto ops = split(rest, ',');
        if (!ops.empty()) operand1 = ops[0];
        if (ops.size() > 1) operand2 = ops[1];
    }

    // ========== STEP 4: Create intermediate code entry ==========
    IntermediateCodeLine ic;
    ic.lineNumber = lineNum;
    ic.locationCounter = data.locationCounter;

    // ========== STEP 5: Process assembler directives ==========
    
    // START directive - sets the starting address of the program
    if (mnemonic == "START") {
        if (!operand1.empty()) {
            data.startingAddress = std::stoi(operand1);
            data.locationCounter = data.startingAddress;
            ic.locationCounter = data.locationCounter;
        }
        ic.type = "AD"; // Assembler Directive
        ic.opcode = "1"; // START opcode
        ic.operand1Type = "C"; // Constant
        ic.operand1Value = std::to_string(data.startingAddress);
        data.intermediateCode.push_back(ic);
        return;
    }
    
    // END directive - marks end of program and processes pending literals
    if (mnemonic == "END") {
        processLTORG(data); // Assign addresses to all pending literals
        ic.type = "AD";
        ic.opcode = "2"; // END opcode
        data.intermediateCode.push_back(ic);
        return;
    }
    
    // ORIGIN directive - changes the location counter
    if (mnemonic == "ORIGIN") {
        if (!operand1.empty()) {
            data.locationCounter = evaluateExpression(operand1, data);
            ic.locationCounter = data.locationCounter;
        }
        ic.type = "AD";
        ic.opcode = "3"; // ORIGIN opcode
        ic.operand1Value = operand1;
        data.intermediateCode.push_back(ic);
        return;
    }
    
    // EQU directive - assigns a value to a symbol without allocating memory
    if (mnemonic == "EQU") {
        if (!label.empty() && !operand1.empty()) {
            addSymbol(label, evaluateExpression(operand1, data), data);
        }
        ic.type = "AD";
        ic.opcode = "4"; // EQU opcode
        ic.operand1Value = operand1;
        data.intermediateCode.push_back(ic);
        return;
    }
    
    // LTORG directive - forces literal pool generation
    if (mnemonic == "LTORG") {
        processLTORG(data);
        ic.type = "AD";
        ic.opcode = "5"; // LTORG opcode
        data.intermediateCode.push_back(ic);
        return;
    }

    // ========== STEP 6: Process label if present ==========
    // If this line has a label, add it to symbol table with current LC
    if (!label.empty()) {
        addSymbol(label, data.locationCounter, data);
    }

    // ========== STEP 7: Look up instruction in MOT ==========
    auto it = data.MOT.find(mnemonic);
    if (it == data.MOT.end()) {
        // Unknown instruction - add error and skip
        data.errors.push_back("Line " + std::to_string(lineNum) + 
                             ": Unknown instruction '" + mnemonic + "'");
        return;
    }

    Instruction ins = it->second;

    // ========== STEP 8: Process imperative statements (machine instructions) ==========
    if (ins.type == InstructionType::IMPERATIVE) {
        ic.type = "IS"; // Imperative Statement
        ic.opcode = std::to_string(ins.opcode);

        // Process first operand (usually a register or condition code)
        if (!operand1.empty()) {
            string op1 = operand1;
            std::transform(op1.begin(), op1.end(), op1.begin(), ::toupper);
            
            // Check if operand is a register (e.g., AREG, BREG)
            if (data.REGISTERS.count(op1)) {
                ic.operand1Type = "R"; // Register
                ic.operand1Value = std::to_string(data.REGISTERS[op1]);
            }
            // Check if operand is a condition code (e.g., LT, EQ, GT)
            else if (data.CONDITION_CODES.count(op1)) {
                ic.operand1Type = "CC"; // Condition Code
                ic.operand1Value = std::to_string(data.CONDITION_CODES[op1]);
            }
        }

        // Process second operand (memory address, literal, or register)
        if (!operand2.empty()) {
            // Check if operand is a literal (starts with '=')
            if (isLiteral(operand2)) {
                addLiteral(operand2, data); // Add to literal table
                ic.operand2Type = "L"; // Literal
                
                // Find the literal's index in the table
                for (size_t i = 0; i < data.literalTable.size(); ++i) {
                    if (data.literalTable[i].literal == operand2) {
                        ic.operand2Value = std::to_string((int)i);
                        break;
                    }
                }
            } else {
                // Operand is a symbol (memory address)
                getSymbolAddress(operand2, data); // Add to symbol table if needed
                ic.operand2Type = "S"; // Symbol
                ic.operand2Value = operand2;
            }
        }
        
        // Add intermediate code line
        data.intermediateCode.push_back(ic);
        
        // Increment location counter by instruction length
        data.locationCounter += ins.length;
    }
    // ========== STEP 9: Process declarative statements (data declarations) ==========
    else if (ins.type == InstructionType::DECLARATIVE) {
        ic.type = "DL"; // Declarative Statement
        ic.opcode = std::to_string(ins.opcode);
        
        // DS (Define Storage) - reserves memory space
        if (mnemonic == "DS") {
            int size = operand1.empty() ? 1 : std::stoi(operand1);
            ic.operand1Type = "C"; // Constant
            ic.operand1Value = std::to_string(size);
            data.intermediateCode.push_back(ic);
            data.locationCounter += size; // Reserve 'size' words
        }
        // DC (Define Constant) - defines an initialized data value
        else if (mnemonic == "DC") {
            string v = operand1;
            
            // Remove quotes from string literals
            if (!v.empty() && v.front() == '\'' && v.back() == '\'') {
                v = v.substr(1, v.size() - 2);
            }
            
            ic.operand1Type = "C"; // Constant
            ic.operand1Value = v;
            data.intermediateCode.push_back(ic);
            data.locationCounter += 1; // Takes 1 word of memory
        }
    }
}

// ============================================================================
// PASS 1 MAIN FUNCTION
// ============================================================================

/**
 * Performs Pass 1 of the two-pass assembler
 * - Reads source code and processes each line
 * - Builds symbol table, literal table, and intermediate code
 * - Writes output files for Pass 2
 * 
 * @param inputFile Path to the source assembly file
 * @param intermediateFile Path to write intermediate code
 * @param symbolFile Path to write symbol table
 * @param literalFile Path to write literal table
 * @param data Reference to assembler data structure (modified)
 */
void pass1(const std::string& inputFile, const std::string& intermediateFile,
           const std::string& symbolFile, const std::string& literalFile, 
           AssemblerData& data) {
    
    // ========== STEP 1: Open and read input file ==========
    std::ifstream in(inputFile);
    if (!in.is_open()) {
        std::cerr << "Error: Cannot open " << inputFile << "\n";
        return;
    }
    
    // Process each line of the source file
    string line;
    int ln = 1; // Line number counter
    while (std::getline(in, line)) {
        processLine(line, ln++, data);
    }
    in.close();

    // ========== STEP 2: Write intermediate code file ==========
    std::ofstream ic(intermediateFile);
    for (const auto& x : data.intermediateCode) {
        // Format: LC (TYPE,OPCODE) (OP1TYPE,OP1VAL) (OP2TYPE,OP2VAL)
        ic << x.locationCounter << " (" << x.type << "," << x.opcode << ")";
        
        // Add first operand if present
        if (!x.operand1Type.empty()) {
            ic << " (" << x.operand1Type << "," << x.operand1Value << ")";
        }
        
        // Add second operand if present
        if (!x.operand2Type.empty()) {
            ic << " (" << x.operand2Type << "," << x.operand2Value << ")";
        }
        
        ic << "\n";
    }
    ic.close();

    // ========== STEP 3: Write symbol table file ==========
    std::ofstream st(symbolFile);
    for (const auto& p : data.symbolTable) {
        // Format: SYMBOL ADDRESS LENGTH
        st << p.second.symbol << " " 
           << p.second.address << " " 
           << p.second.length << "\n";
    }
    st.close();

    // ========== STEP 4: Write literal table file ==========
    std::ofstream lt(literalFile);
    for (size_t i = 0; i < data.literalTable.size(); ++i) {
        // Format: INDEX LITERAL VALUE ADDRESS
        lt << i << " " 
           << data.literalTable[i].literal << " " 
           << data.literalTable[i].value << " " 
           << data.literalTable[i].address << "\n";
    }
    lt.close();

    // ========== STEP 5: Print completion message ==========
    std::cout << "PASS 1 COMPLETED\n";
    std::cout << "Intermediate: " << intermediateFile << "\n"
              << "Symbols: " << symbolFile << "\n"
              << "Literals: " << literalFile << "\n";
}