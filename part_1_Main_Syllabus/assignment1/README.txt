TWO-PASS ASSEMBLER - HOW TO RUN
================================

METHOD 1: Using g++ directly (WITHOUT Makefile)
------------------------------------------------

Step 1: Open terminal/command prompt

Step 2: Navigate to the folder containing the files
   cd path/to/assembler/folder

Step 3: Compile the program
   g++ -o assembler main.cpp

Step 4: Run the program
   Linux/Mac:    ./assembler
   Windows:      assembler.exe

Step 5: Check the output files generated:
   - intermediate.txt (Intermediate Code from Pass 1)
   - symbol_table.txt (Symbol Table from Pass 1)
   - literal_table.txt (Literal Table from Pass 1)
   - output.txt (Final Machine Code from Pass 2)


METHOD 2: Using Makefile (if you have make installed)
------------------------------------------------------
   make
   ./assembler


MODIFYING INPUT
---------------
Edit input.txt to change the assembly source code


TROUBLESHOOTING
---------------
If you get compilation errors:
- Make sure you have g++ installed
- Use: g++ --version to check
- Try: g++ -std=c++17 -o assembler main.cpp


SINGLE COMMAND TO COMPILE AND RUN
----------------------------------
   g++ -o assembler main.cpp && ./assembler

   (On Windows: g++ -o assembler main.cpp && assembler.exe)
