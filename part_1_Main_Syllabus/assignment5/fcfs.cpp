#include "scheduler.hpp"
#include <algorithm>

using namespace std;

/*
 * --------------------------------------------------------------------
 * Function: run_fcfs
 * Purpose : Simulates FCFS (First Come First Serve) CPU Scheduling.
 *
 * Characteristics:
 * - Non-preemptive: once a process starts, it runs till completion.
 * - Processes are executed in the order of their arrival times.
 * - If two processes arrive at the same time, the one with the smaller
 *   process ID (PID) is chosen first.
 * - CPU remains idle if no process has arrived yet.
 *
 * Outputs:
 * - Gantt chart showing the order and duration of execution.
 * - Completion Time (CT), Turnaround Time (TAT), Waiting Time (WT),
 *   and Response Time (RT) for each process.
 * --------------------------------------------------------------------
 */
void run_fcfs(vector<Proc> ps) {
    /*
     * Sort processes based on:
     *   1. Arrival Time (ascending)
     *   2. PID (ascending) — for tie-breaking
     */
    sort(ps.begin(), ps.end(), [](const Proc& a, const Proc& b){
        if (a.at != b.at) return a.at < b.at;
        return a.pid < b.pid;
    });

    // Vector to store Gantt chart segments (start-end of each process)
    vector<Seg> gantt;

    // Current simulation time
    int t = 0;

    /*
     * Iterate through each process in sorted order
     * and simulate CPU execution.
     */
    for (auto &p : ps) {
        // If CPU is idle (no process has arrived yet), advance time
        if (t < p.at) t = p.at;

        // If process starts for the first time, record response time
        if (p.start_time == -1) {
            p.start_time = t;
            p.resp = t - p.at;
        }

        // Add to Gantt chart (process runs from 't' to 't + BT')
        gantt.push_back({p.pid, t, t + p.bt});

        // Advance current time by the process's burst duration
        t += p.bt;

        // Compute scheduling metrics
        p.ct  = t;                // Completion Time
        p.tat = p.ct - p.at;      // Turnaround Time
        p.wt  = p.tat - p.bt;     // Waiting Time
        // Response Time (RT) = start_time - arrival_time (already stored)
    }

    // Print the results in table + Gantt chart format
    print_results(ps, gantt);
}

// 

// 🧪 Test Case for run_fcfs()
// 🔹 Input:
// 4
// 1 0 5 2
// 2 2 3 1
// 3 4 1 4
// 4 6 2 3

// PIDArrivalBurstPriority (ignored here)1052223134144623

// 🧮 Step-by-Step Simulation:
// 🔸 Step 1: Sort by Arrival
// Processes already sorted by arrival time → [P1, P2, P3, P4]
// 🔸 Step 2: Start executing
// TimeActionDescription0P1 arrivesStart P1 immediately0–5P1 executesRuns completely since FCFS is non-preemptive5P2 has already arrived (AT=2)Next ready process5–8P2 executes8P3 (AT=4) is waitingNext process8–9P3 executes9P4 (AT=6) readyNext process9–11P4 executes
// ✅ All processes finish by t = 11

// 🕓 Gantt Chart
// [0-5]:P1  [5-8]:P2  [8-9]:P3  [9-11]:P4


// 🧾 Calculations
// PIDATBTCTTAT = CT - ATWT = TAT - BTRT = Start - AT10555002238633341954446211533

// 📊 Average Metrics


// Average Turnaround Time (ATAT) = (5 + 6 + 5 + 5) / 4 = 5.25


// Average Waiting Time (AWT) = (0 + 3 + 4 + 3) / 4 = 2.5


// Average Response Time (ART) = (0 + 3 + 4 + 3) / 4 = 2.5



// 🖥️ Run Command
// ./out fcfs < input.txt

// 📤 Expected Output
// Gantt:
// [0-5]:P1  [5-8]:P2  [8-9]:P3  [9-11]:P4  

// PID   AT      BT      PR        CT      TAT     WT      RT      
// 1     0       5       2         5       5       0       0
// 2     2       3       1         8       6       3       3
// 3     4       1       4         9       5       4       4
// 4     6       2       3         11      5       3       3

// Averages ->  ATAT: 5.25  AWT: 2.50  ART: 2.50


// ✅ Summary:


// FCFS executes processes strictly by arrival time.


// No preemption — once a process starts, it runs to completion.


// CPU idle only when no process has arrived.


// Useful for batch systems, but not optimal for interactive systems due to the Convoy Effect (short jobs waiting behind long ones).



// Would you like me to do the same level of detailed comments and step-by-step test case for SJF (Preemptive) next?
// 