#include "structures.cpp"
#include "pass1.cpp"
#include "pass2.cpp"
#include "display.cpp"

using namespace std;

int main() {
    string inputFile = "input.txt";
    string intermediateFile = "intermediate.txt";
    string symbolFile = "symbol_table.txt";
    string literalFile = "literal_table.txt";
    string outputFile = "output.txt";
    
    cout << string(70, '=') << endl;
    cout << "     TWO-PASS ASSEMBLER FOR PSEUDO MACHINE" << endl;
    cout << string(70, '=') << endl;
    
    displaySourceCode(inputFile);
    
    cout << "\n" << string(70, '=') << endl;
    cout << "EXECUTING PASS 1" << endl;
    cout << string(70, '=') << endl;
    
    AssemblerData pass1Data;
    initializeTables(pass1Data);
    
    pass1(inputFile, intermediateFile, symbolFile, literalFile, pass1Data);
    
    displaySymbolTable(pass1Data);
    displayLiteralTable(pass1Data);
    displayIntermediateCode(pass1Data);
    displayErrors(pass1Data);
    
    if (!pass1Data.errors.empty()) {
        cout << "\nPass 1 completed with errors. Cannot proceed to Pass 2." << endl;
        return 1;
    }
    
    cout << "\n" << string(70, '=') << endl;
    cout << "EXECUTING PASS 2" << endl;
    cout << string(70, '=') << endl;
    
    AssemblerData pass2Data;
    initializeTables(pass2Data);
    
    pass2(intermediateFile, symbolFile, literalFile, outputFile, pass2Data);
    
    displayMachineCode(outputFile);
    
    cout << "\n" << string(70, '=') << endl;
    cout << "ASSEMBLY COMPLETED SUCCESSFULLY" << endl;
    cout << string(70, '=') << endl;
    cout << "\nFiles Generated:" << endl;
    cout << "  - " << intermediateFile << " (Intermediate Code)" << endl;
    cout << "  - " << symbolFile << " (Symbol Table)" << endl;
    cout << "  - " << literalFile << " (Literal Table)" << endl;
    cout << "  - " << outputFile << " (Machine Code)" << endl;
    
    return 0;
}
