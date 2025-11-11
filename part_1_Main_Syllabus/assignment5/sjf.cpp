#include "scheduler.hpp"
#include <climits>   // for INT_MAX (used to find smallest remaining time)
using namespace std;

/*
 * --------------------------------------------------------------------
 * Function: run_sjf
 * Purpose : Simulates Shortest Job First (Preemptive) Scheduling,
 *           also known as Shortest Remaining Time First (SRTF).
 *
 * Characteristics:
 * - Preemptive scheduling algorithm.
 * - At every time unit, the process with the smallest remaining time
 *   among all *arrived* processes is selected to run.
 * - If a new process arrives with a smaller burst time, the CPU is
 *   preempted and given to that process.
 * - Tends to minimize average waiting time.
 * - Can cause starvation for long processes if short ones keep arriving.
 *
 * Outputs:
 * - Gantt chart showing process execution order and time segments.
 * - Completion Time (CT), Turnaround Time (TAT), Waiting Time (WT),
 *   and Response Time (RT) for each process.
 * --------------------------------------------------------------------
 */
void run_sjf(vector<Proc> ps) {
    int n = ps.size();  // total number of processes

    // Initialize remaining time (rt) for each process = its burst time
    for (auto &p : ps)
        p.rt = p.bt;

    // Stores Gantt chart (start, end, process ID)
    vector<Seg> gantt;

    int t = 0;               // current simulation time
    int completed = 0;       // number of completed processes

    /*
     * The simulation continues until all processes are completed.
     * At each time unit, we check which processes have arrived,
     * then choose the one with the smallest remaining time (SRTF).
     */
    while (completed < n) {
        int idx = -1;         // index of currently selected process
        int best = INT_MAX;   // smallest remaining time found so far

        /*
         * Loop through all processes and find the one that:
         * - Has already arrived (arrival time <= current time)
         * - Is not yet finished
         * - Has the smallest remaining time (rt)
         */
        for (int i = 0; i < n; ++i) {
            if (!ps[i].finished && ps[i].at <= t && ps[i].rt > 0) {
                // Choose process with smaller remaining time
                if (ps[i].rt < best ||
                   // tie-breaker: earlier arrival time
                   (ps[i].rt == best && ps[i].at < ps[idx].at)) {
                    best = ps[i].rt;
                    idx = i;
                }
            }
        }

        // If no process has arrived yet, increment time (CPU idle)
        if (idx == -1) { 
            t++;
            continue;
        }

        /*
         * If process is starting for the first time, record its response time:
         * Response Time = (first time it starts) - (arrival time)
         */
        if (ps[idx].start_time == -1) {
            ps[idx].start_time = t;
            ps[idx].resp = t - ps[idx].at;
        }

        // Start executing process for 1 time unit
        int start = t;
        t++;               // CPU time advances by 1
        ps[idx].rt--;      // one unit of burst time consumed

        /*
         * Update Gantt chart:
         * If same process continues from previous unit, extend its segment;
         * otherwise, add a new segment.
         */
        if (!gantt.empty() && gantt.back().pid == ps[idx].pid && gantt.back().end == start)
            gantt.back().end = t;  // extend existing segment
        else
            gantt.push_back({ps[idx].pid, start, t});  // add new one

        /*
         * If the process has finished execution (remaining time = 0):
         * - Mark as finished
         * - Record completion, turnaround, and waiting times
         */
        if (ps[idx].rt == 0) {
            ps[idx].finished = true;
            ps[idx].ct = t;                 // Completion Time
            ps[idx].tat = ps[idx].ct - ps[idx].at;  // Turnaround Time
            ps[idx].wt  = ps[idx].tat - ps[idx].bt; // Waiting Time
            completed++;                    // increase completed count
        }
    }

    // Finally, print all results: Gantt chart + per-process table
    print_results(ps, gantt);
}

// 🧪 Test Case for run_sjf()
// 🔹 Input (input.txt)
// 4
// 1 0 8 0
// 2 1 4 0
// 3 2 9 0
// 4 3 5 0


// Priority field (last column) is unused in SJF — just for compatibility.
// Arrival and burst times are the main inputs.

// ⚙️ Run Command
// ./out sjf < input.txt

// 🧮 Step-by-Step Execution
// Time	Arrived Processes	Running	Remaining Times	Comments
// 0	P1	P1	P1=8	Only P1 available, start P1
// 1	P1, P2	P2	P1=7, P2=4	P2 arrives, has smaller burst (4 < 7), preempts P1
// 2	P1, P2, P3	P2	P1=7, P2=3, P3=9	Continue P2
// 3	P1, P2, P3, P4	P2	P1=7, P2=2, P4=5	P2 still shortest (2)
// 4	all	P2	P2=1	Continue P2
// 5	all	P2 finishes	—	P2 done at t=5
// 5	all	P4	P1=7, P3=9, P4=5	P4 next shortest (5 < 7 < 9)
// 6–10	all	P4	P4 finishes	P4 done at t=10
// 10	all	P1	P1=7, P3=9	P1 next shortest
// 10–17	—	P1	P1 finishes	P1 done at t=17
// 17	P3 left	P3	—	P3 runs till end
// 17–26	—	P3	P3 finishes	P3 done at t=26
// 🕓 Gantt Chart
// [0-1]:P1  [1-5]:P2  [5-10]:P4  [10-17]:P1  [17-26]:P3

// 🧾 Process Table
// PID	AT	BT	CT	TAT = CT - AT	WT = TAT - BT	RT = Start - AT
// 1	0	8	17	17	9	0
// 2	1	4	5	4	0	0
// 3	2	9	26	24	15	8
// 4	3	5	10	7	2	2
// 📊 Average Metrics

// Average Turnaround Time (ATAT) = (17 + 4 + 24 + 7) / 4 = 13.0

// Average Waiting Time (AWT) = (9 + 0 + 15 + 2) / 4 = 6.5

// Average Response Time (ART) = (0 + 0 + 8 + 2) / 4 = 2.5

// 🖥️ Expected Output
// Gantt:
// [0-1]:P1  [1-5]:P2  [5-10]:P4  [10-17]:P1  [17-26]:P3  

// PID   AT      BT      PR        CT      TAT     WT      RT      
// 1     0       8       0         17      17      9       0
// 2     1       4       0         5       4       0       0
// 3     2       9       0         26      24      15      8
// 4     3       5       0         10      7       2       2

// Averages ->  ATAT: 13.00  AWT: 6.50  ART: 2.50

// ✅ Summary
// Feature	Description
// Type	Preemptive (Shortest Remaining Time First)
// Goal	Minimize waiting and turnaround time
// Preemption	Occurs whenever a shorter job arrives
// Pros	Optimal for minimizing average waiting time
// Cons	Can cause starvation for long jobs
// Use Case	Systems needing short response times

// Would you like me to do the same for all four algorithms (FCFS, SJF, Priority, RR) in one single summarized README explanation section (good for submission/report)?