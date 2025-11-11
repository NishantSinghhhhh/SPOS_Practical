Perfect 👍 — your folder clearly contains a full **Two-Pass Assembler project (Assignment 1)** with separate source files for each assembler phase, table handling, and display logic.
Here’s a complete and polished **`README.md`** you can drop directly inside your `assn1` directory.

---

# ⚙️ Assignment 1 — Two-Pass Assembler (C++17)

This project implements a **Two-Pass Assembler** for a simple assembly language as part of the **SPOS (System Programming and Operating Systems)** lab.

It simulates how an assembler converts assembly source code into machine code, using **Pass 1** to build symbol/literal tables and **Pass 2** to generate the final machine code.

---

## 🧠 Overview

| Pass       | Description                                                                                                          |
| ---------- | -------------------------------------------------------------------------------------------------------------------- |
| **Pass 1** | Reads the source code, assigns addresses, and builds **Symbol Table**, **Literal Table**, and **Intermediate Code**. |
| **Pass 2** | Uses the intermediate file and tables to generate the final **Machine Code (Output File)**.                          |

---

## 📁 Folder Structure

```
assn1/
├── assembler.hpp           # Header file for declarations and data structures
├── display.cpp             # Code to print or format tables
├── input.txt               # Input assembly source program
├── intermediate.txt        # Generated intermediate code (Pass 1 output)
├── literal_table.txt       # Generated Literal Table
├── main.cpp                # Main driver program (calls Pass 1 & Pass 2)
├── output.txt              # Final machine code (Pass 2 output)
├── pass1.cpp               # Pass 1 implementation
├── pass2.cpp               # Pass 2 implementation
├── symbol_table.txt        # Generated Symbol Table
├── tables.cpp              # Table-handling logic (SYMTAB, LITTAB, etc.)
└── README.md               # Documentation (this file)
```

---

## ⚙️ Compilation

Make sure you’re inside the `assn1` directory.

Compile all `.cpp` files together:

```bash
g++ -std=c++17 -O2 main.cpp pass1.cpp pass2.cpp tables.cpp display.cpp -o assembler
```

✅ This will produce an executable named:

```
assembler
```

---

## ▶️ Running the Program

You can run it using:

```bash
./assembler
```

or, if your code expects arguments (depends on implementation):

```bash
./assembler input.txt intermediate.txt symbol_table.txt literal_table.txt output.txt
```

---

## 🧩 Example Input (input.txt)

```asm
START 100
MOVER AREG,='5'
ADD   AREG,='1'
LABEL1 DS 1
MOVEM AREG,LABEL1
END
```

---

## 📊 Output Files (Generated)

### 1️⃣ `intermediate.txt`

Intermediate code with addresses and instruction references:

```
(AD,01) (C,100)
(IS,04) (1) (L,1)
(IS,01) (1) (L,2)
(DL,01) (C,1)
(IS,05) (1) (S,1)
(AD,02)
```

---

### 2️⃣ `symbol_table.txt`

```
Index   Symbol   Address
1       LABEL1   103
```

---

### 3️⃣ `literal_table.txt`

```
Index   Literal   Address
1       ='5'      100
2       ='1'      101
```

---

### 4️⃣ `output.txt` (Final Machine Code)

```
100) 04 01 105
101) 01 01 106
102) 05 01 103
```

---

## 🧱 Tables Used

| Table                      | Description                                                   |
| -------------------------- | ------------------------------------------------------------- |
| **SYMTAB (Symbol Table)**  | Stores user-defined labels with assigned addresses.           |
| **LITTAB (Literal Table)** | Tracks literals and their corresponding addresses.            |
| **POOLTAB** *(optional)*   | Tracks literal pools if multiple `LTORG` directives are used. |
| **OPTAB**                  | Contains opcode mnemonics and their machine codes.            |

---

## 🧰 Example Workflow

```bash
# Step 1: Compile
g++ -std=c++17 -O2 main.cpp pass1.cpp pass2.cpp tables.cpp display.cpp -o assembler

# Step 2: Run
./assembler

# Step 3: Check outputs
cat intermediate.txt
cat symbol_table.txt
cat literal_table.txt
cat output.txt
```

---

## 🧠 Concept Summary

| Component                  | Description                                                               |
| -------------------------- | ------------------------------------------------------------------------- |
| **Pass 1**                 | Scans the source program and generates intermediate code + tables.        |
| **Pass 2**                 | Converts intermediate code into machine code using symbol/literal tables. |
| **IC (Intermediate Code)** | Bridge format between source and final machine code.                      |
| **Tables**                 | Provide address and symbol resolution between passes.                     |

---

## 🧑‍💻 Author

**Nishant Singh**

> SPOS Lab — Assignment 1
> Implementation of a Two-Pass Assembler in C++17.

---

## 💡 Notes

* All output files are automatically overwritten each run.
* Use `dos2unix input.txt` if the file has Windows-style newlines.
* Make sure the `input.txt` source ends with the **`END`** directive.
* `display.cpp` can be used to pretty-print all generated tables.

---

Would you like me to include a **Makefile** for this as well (so you can compile and run with just `make` and `make run`)?
