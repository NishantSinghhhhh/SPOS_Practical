#include <iostream>
#include <iomanip>
#include <string>
#include "assembler.hpp"

int main() {
    std::string inputFile        = "input.txt";
    std::string intermediateFile = "intermediate.txt";
    std::string symbolFile       = "symbol_table.txt";
    std::string literalFile      = "literal_table.txt";
    std::string outputFile       = "output.txt";

    std::cout << std::string(70,'=') << "\n"
              << "     TWO-PASS ASSEMBLER FOR PSEUDO MACHINE\n"
              << std::string(70,'=') << "\n";

    displaySourceCode(inputFile);

    std::cout << "\n" << std::string(70,'=') << "\nEXECUTING PASS 1\n" << std::string(70,'=') << "\n";
    AssemblerData pass1Data; initializeTables(pass1Data);
    pass1(inputFile, intermediateFile, symbolFile, literalFile, pass1Data);
    displaySymbolTable(pass1Data);
    displayLiteralTable(pass1Data);
    displayIntermediateCode(pass1Data);
    displayErrors(pass1Data);

    if (!pass1Data.errors.empty()) {
        std::cout << "\nPass 1 completed with errors. Cannot proceed to Pass 2.\n";
        return 1;
    }

    std::cout << "\n" << std::string(70,'=') << "\nEXECUTING PASS 2\n" << std::string(70,'=') << "\n";
    AssemblerData pass2Data; initializeTables(pass2Data);
    pass2(intermediateFile, symbolFile, literalFile, outputFile, pass2Data);

    displayMachineCode(outputFile);

    std::cout << "\n" << std::string(70,'=') << "\nASSEMBLY COMPLETED SUCCESSFULLY\n" << std::string(70,'=') << "\n"
              << "\nFiles Generated:\n"
              << "  - " << intermediateFile << " (Intermediate Code)\n"
              << "  - " << symbolFile       << " (Symbol Table)\n"
              << "  - " << literalFile      << " (Literal Table)\n"
              << "  - " << outputFile       << " (Machine Code)\n";
    return 0;
}
