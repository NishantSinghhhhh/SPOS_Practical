#include "scheduler.hpp"
#include <algorithm>
#include <queue>
using namespace std;

/*
 * --------------------------------------------------------------------
 * Function: run_rr
 * Purpose : Simulates Round Robin (Preemptive) CPU Scheduling
 *
 * Characteristics:
 * - Time-sharing (preemptive) scheduling algorithm.
 * - Each process gets a fixed CPU time slice (quantum).
 * - If the process does not finish in that time, it is preempted and
 *   moved to the end of the ready queue.
 * - Fair scheduling: all processes get equal CPU share in turns.
 *
 * Inputs:
 * - ps : list of processes
 * - q  : time quantum (integer, must be > 0)
 *
 * Outputs:
 * - Gantt chart showing process execution order and time intervals.
 * - CT, TAT, WT, RT for each process.
 * --------------------------------------------------------------------
 */
void run_rr(vector<Proc> ps, int q) {
    // Initialize each process's remaining time = burst time
    for (auto &p : ps)
        p.rt = p.bt;

    /*
     * Sort processes by arrival time (and by PID for tie-breaking)
     * so we can easily check which processes arrive next.
     */
    sort(ps.begin(), ps.end(), [](const Proc& a, const Proc& b){
        if (a.at != b.at) return a.at < b.at;
        return a.pid < b.pid;
    });

    // Stores process execution intervals (Gantt chart)
    vector<Seg> gantt;

    // Ready queue stores indices of processes waiting for CPU
    queue<int> rq;

    int n = ps.size();  // total number of processes
    int t = 0;          // current simulation time
    int i = 0;          // index for the next process that may arrive

    /*
     * Lambda function to enqueue processes that have arrived up to time 'time'.
     * These are processes whose arrival time <= current time.
     */
    auto enqueue_arrivals = [&](int time){
        while (i < n && ps[i].at <= time) {
            rq.push(i); // add process index to ready queue
            i++;
        }
    };

    // Start the simulation at time 0
    if (n > 0) t = 0;

    // Enqueue all processes that have arrived by t=0
    enqueue_arrivals(t);

    /*
     * Main simulation loop:
     * Keep running until both:
     *  - The ready queue is empty, AND
     *  - All processes have arrived and finished.
     */
    while (!rq.empty() || i < n) {

        // If queue is empty (CPU idle), jump to the time of the next arrival
        if (rq.empty()) {
            t = max(t, ps[i].at);  // move to next process arrival time
            enqueue_arrivals(t);   // add that process to the queue
            continue;
        }

        // Fetch the index of the process at the front of the ready queue
        int idx = rq.front();
        rq.pop();

        /*
         * Record response time for the first time a process gets CPU.
         * Response Time = (first CPU allocation time) - (arrival time)
         */
        if (ps[idx].start_time == -1) {
            ps[idx].start_time = t;
            ps[idx].resp = t - ps[idx].at;
        }

        // Determine how long this process will run in this turn:
        // Either the full quantum, or the remaining time if smaller.
        int run = min(q, ps[idx].rt);
        int start = t;
        t += run;            // advance current time by the execution duration
        ps[idx].rt -= run;   // decrease remaining time

        /*
         * Update Gantt chart:
         * If the last segment in Gantt is the same process continuing
         * immediately, extend it. Otherwise, add a new segment.
         */
        if (!gantt.empty() && gantt.back().pid == ps[idx].pid && gantt.back().end == start)
            gantt.back().end = t;
        else
            gantt.push_back({ps[idx].pid, start, t});

        // Enqueue any new arrivals that happened while this process was running
        enqueue_arrivals(t);

        /*
         * If the process still has remaining time, requeue it
         * (it will get another chance in the next round).
         * Otherwise, compute its final metrics.
         */
        if (ps[idx].rt > 0) {
            rq.push(idx);
        } else {
            ps[idx].ct = t;                  // Completion Time
            ps[idx].tat = ps[idx].ct - ps[idx].at;  // Turnaround Time
            ps[idx].wt  = ps[idx].tat - ps[idx].bt; // Waiting Time
        }
    }

    // After simulation, print Gantt chart and process statistics
    print_results(ps, gantt);
}

// 🧪 Test Case for run_rr()
// 🔹 Input (input.txt)
// 4
// 1 0 5 0
// 2 1 4 0
// 3 2 2 0
// 4 4 1 0


// Priority field is ignored in RR (only for compatibility with other algorithms).
// Use a time quantum = 2.


// ⚙️ Run Command
// ./out rr 2 < input.txt


// 🧮 Step-by-Step Execution
// TimeReady Queue (Before Execution)RunningComments0[P1]P1P1 starts (RT=5 → 3 left)2[P2]P1 done 2 unitsP1 not finished, requeued2[P2, P1]P2P2 starts (RT=4 → 2 left)4[P1, P3]P3 arrives at t=2, P2 paused after 2 units4[P1, P3, P2]P1P1 resumes (RT=3 → 1 left)6[P3, P2, P4]P4 arrives, P1 requeued again6[P3, P2, P4, P1]P3P3 starts (RT=2 → 0 left) → finishes at t=88[P2, P4, P1]P2P2 runs (RT=2 → 0 left) → finishes at t=1010[P4, P1]P4P4 runs (RT=1 → 0 left) → finishes at t=1111[P1]P1P1 final run (RT=1 → 0 left) → finishes at t=12
// ✅ All processes complete at t = 12

// 🕓 Gantt Chart
// [0-2]:P1  [2-4]:P2  [4-6]:P1  [6-8]:P3  [8-10]:P2  [10-11]:P4  [11-12]:P1


// 🧾 Process Table
// PIDATBTCTTAT = CT - ATWT = TAT - BTRT = Start - AT10512127021410951322864444111766

// 📊 Average Metrics


// Average Turnaround Time (ATAT) = (12 + 9 + 6 + 7) / 4 = 8.5


// Average Waiting Time (AWT) = (7 + 5 + 4 + 6) / 4 = 5.5


// Average Response Time (ART) = (0 + 1 + 4 + 6) / 4 = 2.75



// 🖥️ Expected Output
// Gantt:
// [0-2]:P1  [2-4]:P2  [4-6]:P1  [6-8]:P3  [8-10]:P2  [10-11]:P4  [11-12]:P1  

// PID   AT      BT      PR        CT      TAT     WT      RT      
// 1     0       5       0         12      12      7       0
// 2     1       4       0         10      9       5       1
// 3     2       2       0         8       6       4       4
// 4     4       1       0         11      7       6       6

// Averages ->  ATAT: 8.50  AWT: 5.50  ART: 2.75


// ✅ Summary


// Each process gets equal 2-unit time slices in round-robin order.


// The ready queue rotates dynamically as processes arrive and are preempted.


// Fair but context-switch heavy scheduling — good for time-sharing systems.


// CPU never stays idle as long as a process is in the ready queue.



// Would you like me to similarly annotate SJF (Preemptive) next with an illustrated example and Gantt chart?