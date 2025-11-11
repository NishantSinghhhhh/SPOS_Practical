#include "pass1.hpp"
using namespace std;

namespace {
    unordered_map<string, Instr> g_MOT;
    unordered_map<string, int>   g_REG;
    unordered_map<string, int>   g_CC;
}

void init_tables() {
    // Imperative (IS)
    g_MOT["STOP"]  = {IType::IS, 0, 1};
    g_MOT["ADD"]   = {IType::IS, 1, 1};
    g_MOT["SUB"]   = {IType::IS, 2, 1};
    g_MOT["MULT"]  = {IType::IS, 3, 1};
    g_MOT["MOVER"] = {IType::IS, 4, 1};
    g_MOT["MOVEM"] = {IType::IS, 5, 1};
    g_MOT["COMP"]  = {IType::IS, 6, 1};
    g_MOT["BC"]    = {IType::IS, 7, 1};
    g_MOT["DIV"]   = {IType::IS, 8, 1};
    g_MOT["READ"]  = {IType::IS, 9, 1};
    g_MOT["PRINT"] = {IType::IS, 10, 1};

    // Declaratives (DL)
    g_MOT["DS"]    = {IType::DL, 1, 0};
    g_MOT["DC"]    = {IType::DL, 2, 1};

    // Assembler directives (AD)
    g_MOT["START"] = {IType::AD, 1, 0};
    g_MOT["END"]   = {IType::AD, 2, 0};
    g_MOT["ORIGIN"]= {IType::AD, 3, 0};
    g_MOT["EQU"]   = {IType::AD, 4, 0};
    g_MOT["LTORG"] = {IType::AD, 5, 0};

    // Registers
    g_REG["AREG"]=1; g_REG["BREG"]=2; g_REG["CREG"]=3; g_REG["DREG"]=4;

    // Condition Codes
    g_CC["LT"]=1; g_CC["LE"]=2; g_CC["EQ"]=3; g_CC["GT"]=4; g_CC["GE"]=5; g_CC["ANY"]=6;
}

const unordered_map<string, Instr>& MOT(){ return g_MOT; }
const unordered_map<string, int>&   REG(){ return g_REG; }
const unordered_map<string, int>&   CC(){  return g_CC; }
