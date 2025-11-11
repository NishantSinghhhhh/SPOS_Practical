#include "assembler.hpp"

void initializeTables(AssemblerData& data) {
    // Imperative
    data.MOT["STOP"]  = Instruction("STOP", 0, 1, InstructionType::IMPERATIVE);
    data.MOT["ADD"]   = Instruction("ADD", 1, 1, InstructionType::IMPERATIVE);
    data.MOT["SUB"]   = Instruction("SUB", 2, 1, InstructionType::IMPERATIVE);
    data.MOT["MULT"]  = Instruction("MULT", 3, 1, InstructionType::IMPERATIVE);
    data.MOT["MOVER"] = Instruction("MOVER", 4, 1, InstructionType::IMPERATIVE);
    data.MOT["MOVEM"] = Instruction("MOVEM", 5, 1, InstructionType::IMPERATIVE);
    data.MOT["COMP"]  = Instruction("COMP", 6, 1, InstructionType::IMPERATIVE);
    data.MOT["BC"]    = Instruction("BC", 7, 1, InstructionType::IMPERATIVE);
    data.MOT["DIV"]   = Instruction("DIV", 8, 1, InstructionType::IMPERATIVE);
    data.MOT["READ"]  = Instruction("READ", 9, 1, InstructionType::IMPERATIVE);
    data.MOT["PRINT"] = Instruction("PRINT",10, 1, InstructionType::IMPERATIVE);

    // Declarative
    data.MOT["DS"] = Instruction("DS", 1, 0, InstructionType::DECLARATIVE);
    data.MOT["DC"] = Instruction("DC", 2, 1, InstructionType::DECLARATIVE);

    // Assembler directives
    data.MOT["START"] = Instruction("START", 1, 0, InstructionType::ASSEMBLER);
    data.MOT["END"]   = Instruction("END",   2, 0, InstructionType::ASSEMBLER);
    data.MOT["ORIGIN"]= Instruction("ORIGIN",3, 0, InstructionType::ASSEMBLER);
    data.MOT["EQU"]   = Instruction("EQU",   4, 0, InstructionType::ASSEMBLER);
    data.MOT["LTORG"] = Instruction("LTORG", 5, 0, InstructionType::ASSEMBLER);

    // Registers
    data.REGISTERS["AREG"] = 1;
    data.REGISTERS["BREG"] = 2;
    data.REGISTERS["CREG"] = 3;
    data.REGISTERS["DREG"] = 4;

    // Condition codes
    data.CONDITION_CODES["LT"]  = 1;
    data.CONDITION_CODES["LE"]  = 2;
    data.CONDITION_CODES["EQ"]  = 3;
    data.CONDITION_CODES["GT"]  = 4;
    data.CONDITION_CODES["GE"]  = 5;
    data.CONDITION_CODES["ANY"] = 6;
}
