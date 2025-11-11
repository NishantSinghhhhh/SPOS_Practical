Absolutely ✅ — here’s a **ready-to-use `README.md`** for your **Assignment 4 – Classical Synchronization Problems** project (Producer–Consumer, Readers–Writers, and Dining Philosophers), written in **C++17** using **threads**, **mutexes**, and **custom semaphores**.

---

# 🧵 Assignment 4 — Classical Synchronization Problems (C++17)

This project demonstrates three of the most important **Operating System synchronization problems** using **threads**, **mutexes**, and a **custom semaphore** in modern **C++17**.

These implementations simulate real-world concurrent resource sharing and show how synchronization primitives prevent race conditions and deadlocks.

---

## 📘 Problems Implemented

| Problem                     | Description                                                                                                                         | Synchronization Used |
| --------------------------- | ----------------------------------------------------------------------------------------------------------------------------------- | -------------------- |
| 🏭 **Producer–Consumer**    | Multiple producers and consumers share a bounded buffer. Ensures producers don’t overfill and consumers don’t underflow the buffer. | Mutex + Semaphore    |
| 📚 **Readers–Writers**      | Multiple readers and writers share a common resource. Readers have priority; no writer starvation allowed.                          | Mutex + Semaphore    |
| 🍽️ **Dining Philosophers** | N philosophers alternate between thinking and eating using shared forks. Avoids deadlock using N−1 semaphore limit.                 | Mutex + Semaphore    |

---

## 🧩 File Structure

```
assignment4/
│
├── main.cpp          # Entry point (chooses which problem to run)
├── problems.cpp      # Implementations of all 3 problems
├── semaphore.cpp     # Custom semaphore class + utility helpers
└── README.md         # Documentation (this file)
```

---

## ⚙️ Compilation

Make sure you’re inside the `assignment4` folder, then compile using:

```bash
g++ -std=c++17 -O2 -pthread main.cpp problems.cpp semaphore.cpp -o sync_classics
```

> 💡 The `-pthread` flag is required for multi-threading.

This will create an executable named:

```
sync_classics
```

---

## 🚀 Running the Program

Each mode corresponds to one synchronization problem.
Run the desired mode by providing it as a command-line argument.

### 🏭 **1) Producer–Consumer**

```bash
./sync_classics prodcon
```

### 📚 **2) Readers–Writers**

```bash
./sync_classics rw
```

### 🍽️ **3) Dining Philosophers**

```bash
./sync_classics phil
```

---

## 📊 Sample Outputs

### 🏭 Producer–Consumer

```
[PROD 0] -> 0
[PROD 1] -> 100
          [CONS 0] <- 0
          [CONS 1] <- 100
[PROD 2] -> 200
          [CONS 2] <- 200
...
```

---

### 📚 Readers–Writers

```
[R0] reading value = 0
[R1] reading value = 0
    [W0] writing value = 1
[R2] reading value = 1
[R4] reading value = 1
Final shared_data = 2
```

---

### 🍽️ Dining Philosophers

```
[P0] thinking
[P1] thinking
    [P0] eating (1/4)
[P3] thinking
    [P4] eating (1/4)
[P2] thinking
...
```

---

## 🧠 Concept Recap

| Concept                 | Description                                                                       |
| ----------------------- | --------------------------------------------------------------------------------- |
| **Semaphore**           | Controls access to shared resources through counting signals.                     |
| **Mutex**               | Provides mutual exclusion to ensure only one thread accesses a section at a time. |
| **Producer–Consumer**   | Coordinates buffer access between producer and consumer threads.                  |
| **Readers–Writers**     | Allows concurrent reading but exclusive writing.                                  |
| **Dining Philosophers** | Avoids deadlock by limiting simultaneous fork usage.                              |

---

## 🧩 Example Workflow

```bash
# Step 1: Compile
g++ -std=c++17 -O2 -pthread main.cpp problems.cpp semaphore.cpp -o sync_classics

# Step 2: Run a problem
./sync_classics prodcon
./sync_classics rw
./sync_classics phil
```

---

## 🧑‍💻 Author

**Nishant Singh**

> Operating Systems Lab — Assignment 4
> Implementation of Classical Synchronization Problems using C++17 Threads and Semaphores.

---

## 💡 Notes

* Uses **custom semaphore** implemented via `std::mutex` + `std::condition_variable`.
* Introduces random sleep intervals to simulate realistic concurrency.
* Thread-safe output ensures proper synchronization between producer/consumer or philosophers.

---

Would you like me to include a **Makefile** (so you can just run `make rw` or `make all` instead of typing the full compile command each time)?
