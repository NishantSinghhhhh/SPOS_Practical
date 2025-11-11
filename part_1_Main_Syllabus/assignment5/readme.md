Here’s a **README.md** you can drop into your assignment folder that uses only `g++` (no Makefile) and runs the binary as `./out`.

---

# CPU Scheduling Algorithms Simulator

Implements:

* **FCFS** (First Come First Serve)
* **SJF (Preemptive / SRTF)**
* **Priority** (Non-Preemptive; smaller number = higher priority)
* **Round Robin** (Preemptive)

Outputs a Gantt chart, per-process metrics (CT, TAT, WT, RT), and averages.

## Files

```
scheduler.hpp
fcfs.cpp
sjf.cpp
priority.cpp
rr.cpp
util.cpp
main.cpp
```

## Compile (no Makefile)

From inside the folder containing the files:

```bash
# Build everything into a single executable named `out`
g++ -std=c++17 -O2 main.cpp util.cpp fcfs.cpp sjf.cpp priority.cpp rr.cpp -o out
```

> If you change only one file, just re-run the same command to rebuild.

## Run

Program reads from **stdin**. General syntax:

```bash
./out <algorithm> [time_quantum] < input.txt
```

Algorithms / modes:

* `fcfs`
* `sjf`
* `priority`
* `rr` (requires a positive `time_quantum`)

### Examples

```bash
# FCFS
./out fcfs < input.txt

# SJF (preemptive / SRTF)
./out sjf < input.txt

# Priority (non-preemptive)
./out priority < input.txt

# Round Robin (quantum = 3)
./out rr 3 < input.txt
```

## Input format

```
N
pid arrival_time burst_time priority
pid arrival_time burst_time priority
...
```

* `N` = number of processes
* `priority` is used by Priority scheduling (smaller = higher). It’s still read for all algorithms.

## Quick test (no file needed)

Use a here-doc to feed input directly:

```bash
./out fcfs << 'EOF'
4
1 0 7 2
2 2 4 1
3 4 1 3
4 5 4 2
EOF
```

Other modes:

```bash
./out sjf << 'EOF'
4
1 0 7 2
2 2 4 1
3 4 1 3
4 5 4 2
EOF

./out priority << 'EOF'
4
1 0 7 2
2 2 4 1
3 4 1 3
4 5 4 2
EOF

./out rr 3 << 'EOF'
4
1 0 7 2
2 2 4 1
3 4 1 3
4 5 4 2
EOF
```

## Output

* **Gantt**: time segments per process, e.g. `[0-2]:P1  [2-5]:P2 ...`
* **Table**: PID, AT, BT, PR, CT, TAT, WT, RT
* **Averages**: ATAT, AWT, ART

## Notes

* Round Robin requires `time_quantum > 0`.
* All times are integers.
* If the CPU is idle before the first arrival, the simulation waits until the next arrival.
