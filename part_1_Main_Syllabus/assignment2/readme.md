Here’s a clean, drop-in **README.md** for your **Assignment 2 – Macro Processor** folder. It matches the files you showed and the code you pasted (Pass-I + Pass-II with MNT/MDT, intermediate and expanded output).

---

# 🧩 Assignment 2 — Two-Pass Macro Processor (C++17)

This project implements a simple **two-pass macro processor** for an assembly-like language.

* **Pass-I** builds the **Macro Name Table (MNT)** and **Macro Definition Table (MDT)** and writes a plain **intermediate** file with macro definitions removed.
* **Pass-II** reads **MNT/MDT** and **intermediate** to produce the final **expanded** source (macros expanded inline).

---

## 📁 Folder layout

```
assignment2/
├── main.cpp                 # CLI driver: calls Pass-I and Pass-II
├── pass1.cpp                # Pass-I: builds MNT/MDT + intermediate.txt
├── pass2.cpp                # Pass-II: expands macros using MNT/MDT
├── macroprocessor.hpp       # Decls for structs + functions
├── source.asm               # Input assembly with MACRO/MEND
├── mnt.txt                  # (output) Macro Name Table
├── mdt.txt                  # (output) Macro Definition Table
├── intermediate.txt         # (output) Source without macro defs
├── expanded.asm             # (output) Fully expanded source
```

> If you accidentally have `#include "macroprocessor.cpp"` anywhere, replace it with `#include "macroprocessor.hpp"` and **compile .cpp files separately** (don’t include .cpp files).

---

## ⚙️ Build

From inside `assignment2/`:

```bash
g++ -std=c++17 -O2 main.cpp pass1.cpp pass2.cpp -o macroprocessor
```

> If your headers are placed differently, add `-I` include paths as needed.

---

## ▶️ Run

The program expects **five arguments**:

```
./macroprocessor <source.asm> <mnt.txt> <mdt.txt> <intermediate.txt> <expanded.asm>
```

Example (using your filenames):

```bash
./macroprocessor source.asm mnt.txt mdt.txt intermediate.txt expanded.asm
```

After a successful run, you’ll have:

* `mnt.txt` and `mdt.txt` filled by **Pass-I**
* `intermediate.txt` (original source with macro bodies removed)
* `expanded.asm` (final, with macros expanded inline)

---

## 📝 Example input (source.asm)

```asm
START 100
MACRO
INCR &A,&B
MOVER AREG,&A
ADD   AREG,&B
MOVEM AREG,&A
MEND
READ  X
INCR  X,5
PRINT X
END
```

> ⚠️ Make sure the last line is **`END`** (no stray characters like `ENDv`).
> Also ensure there isn’t any accidental text appended after the source (your paste showed some trailing junk).

---

## ✅ Expected outputs (conceptually)

### `mnt.txt` (example)

```
INCR 0 2
```

* `name mdtIndex paramCount`

### `mdt.txt` (example)

```
MOVER AREG,#1
ADD   AREG,#2
MOVEM AREG,#1
MEND
```

### `intermediate.txt` (example)

```
START 100
READ  X
INCR  X,5
PRINT X
END
```

### `expanded.asm` (example)

```
START 100
READ  X
MOVER AREG,X
ADD   AREG,5
MOVEM AREG,X
PRINT X
END
```

---

## 🐞 Common pitfalls

* **“file not found”** → check paths and run from the folder that actually contains the files.
* **Wrong include** → do **not** `#include "macroprocessor.cpp"`. Use `macroprocessor.hpp` and compile `pass1.cpp` / `pass2.cpp` separately.
* **Garbage in files** → ensure there’s no stray text at the end of `source.asm` (your paste had `MENDINCR 0 2START 100 ...` appended).
* **Windows newlines** → if files came from Windows, convert with `dos2unix source.asm`.

---

## 🧰 Optional: Makefile

Create a `Makefile` in `assignment2/` so you can do `make` / `make run`:

```makefile
CXX := g++
CXXFLAGS := -std=c++17 -O2

SRC := main.cpp pass1.cpp pass2.cpp
BIN := macroprocessor

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

run: $(BIN)
	./$(BIN) source.asm mnt.txt mdt.txt intermediate.txt expanded.asm

clean:
	rm -f $(BIN) mnt.txt mdt.txt intermediate.txt expanded.asm
```

---

## 🧠 How placeholders work

* In **Pass-I**, formal parameters like `&A`, `&B` are replaced in MDT with placeholders `#1`, `#2`, … in the same order as the macro header.
* In **Pass-II**, actual arguments replace `#1`, `#2`, … to generate the expanded body.

---

If you hit any build error, paste the **exact compiler output** and the **first 20 lines of each file** (`main.cpp`, `pass1.cpp`, `pass2.cpp`, `macroprocessor.hpp`) and I’ll align them for you.
