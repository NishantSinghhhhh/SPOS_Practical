Perfect 👍 — let’s go step by step so you can **compile and run** the Page Replacement simulator successfully on your Linux system (like Ubuntu).

---

## 🧩 STEP 1 — Create a new folder

```bash

mkdir ~/Desktop/page_replacement
cd ~/Desktop/page_replacement

```

---

## 🧩 STEP 2 — Create the source file

Create a file called **`page_sim.cpp`**:

```bash
nano page_sim.cpp
```

Now **paste** the full code I gave you earlier into it.
Then press:

```
Ctrl + O   (save)
Enter
Ctrl + X   (exit)
```

---

## 🧩 STEP 3 — Create the input file

Make a small file for testing:

```bash
nano input.txt
```

Paste this inside:

```
3
20
7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1
```

Then save and exit (`Ctrl+O`, `Enter`, `Ctrl+X`).

---

## 🧩 STEP 4 — Compile using g++

Run this command:

```bash
g++ -std=c++17 -O2 page_sim.cpp -o pages
```

✅ If it compiles successfully, you’ll now have an executable file named `pages`:

```bash
ls
```

Output should include:

```
page_sim.cpp  pages  input.txt
```

---

## 🧩 STEP 5 — Run different algorithms

Each algorithm is chosen using a **command-line argument** (`fifo`, `lru`, `opt`, `clock`).

### ▶ FIFO

```bash
./pages fifo < input.txt
```

### ▶ LRU

```bash
./pages lru < input.txt
```

### ▶ Optimal

```bash
./pages opt < input.txt
```

### ▶ Clock

```bash
./pages clock < input.txt
```

---

## 🧩 STEP 6 — Example output (for FIFO)

You’ll see something like this:

```
Algorithm: fifo
Frames: 3 | References: 20

Ref     F0    F1    F2    Result
7       7     -     -     FAULT
0       7     0     -     FAULT
1       7     0     1     FAULT
2       2     0     1     FAULT
0       2     0     1     HIT
3       2     3     1     FAULT
0       2     3     0     FAULT
...
Total Hits: 7 | Total Faults: 13 | Hit Ratio: 0.35 | Fault Ratio: 0.65
```

---

## 🧩 STEP 7 — (Optional) Try interactive mode

You can also type references manually instead of using a file:

```bash
./pages fifo
```

Then enter:

```
3
5
1 2 3 4 1
```

---

## ✅ Summary

| Command                                    | Description |
| ------------------------------------------ | ----------- |
| `g++ -std=c++17 -O2 page_sim.cpp -o pages` | Compile     |
| `./pages fifo < input.txt`                 | Run FIFO    |
| `./pages lru < input.txt`                  | Run LRU     |
| `./pages opt < input.txt`                  | Run Optimal |
| `./pages clock < input.txt`                | Run Clock   |

---

Would you like me to split this same program into multiple files (like `fifo.cpp`, `lru.cpp`, `opt.cpp`, `clock.cpp`, and `main.cpp`) for your next assignment?
