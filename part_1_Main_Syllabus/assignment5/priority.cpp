#include "scheduler.hpp"
#include <vector>
using namespace std;

/*
 * --------------------------------------------------------------------
 * Function: run_priority
 * Purpose : Simulates Priority Scheduling (Non-Preemptive)
 * 
 * Characteristics:
 * - Each process is selected based on its priority value.
 * - Lower priority number → higher actual priority.
 * - Once a process starts execution, it runs till completion (non-preemptive).
 * - If no process has arrived yet, the CPU stays idle until the next one arrives.
 * 
 * Output:
 * - A Gantt chart (execution timeline)
 * - Completion time, Turnaround time, Waiting time, and Response time
 *   for each process.
 * --------------------------------------------------------------------
 */
void run_priority(vector<Proc> ps) {
    // Total number of processes
    int n = ps.size();

    // Gantt chart will record process execution intervals (start-end)
    vector<Seg> gantt;

    // Boolean flags to keep track of which processes have completed
    vector<char> done(n, false);

    // 't' represents current time in the simulation
    // 'finished' counts how many processes have completed execution
    int t = 0, finished = 0;

    /*
     * Lambda function: available()
     * -----------------------------
     * Returns a list (vector) of process indices that have already "arrived"
     * (i.e., arrival time <= current time) but have not yet completed execution.
     * This helps identify which processes are ready to be scheduled at time 't'.
     */
    auto available = [&](int now) {
        vector<int> idxs;  // store indices of available processes
        for (int i = 0; i < n; ++i)
            if (!done[i] && ps[i].at <= now)  // process not finished & has arrived
                idxs.push_back(i);
        return idxs;  // return the list of available processes
    };

    /*
     * Main scheduling loop:
     * ----------------------
     * Continue looping until all processes are finished.
     */
    while (finished < n) {
        // Get all processes that have arrived by current time 't'
        auto avail = available(t);

        // If no process is available (CPU idle), just increment time by 1
        // This simulates the CPU waiting for the next process to arrive.
        if (avail.empty()) { 
            t++; 
            continue; 
        }

        /*
         * Select the process with the highest priority (smallest priority number)
         * If multiple processes have the same priority, apply tie-breaking rules:
         *   1. Earlier arrival time
         *   2. If arrival times are same, smaller PID
         */
        int pick = avail[0];  // start with the first available process
        for (int i : avail) {
            if (
                ps[i].pr < ps[pick].pr || // higher priority (smaller value)
                (ps[i].pr == ps[pick].pr && 
                    (ps[i].at < ps[pick].at || 
                     (ps[i].at == ps[pick].at && ps[i].pid < ps[pick].pid)))
            ) {
                pick = i;  // select this process as current best
            }
        }

        /*
         * Mark the start time and response time of this process.
         * Response Time = (time when process first gets CPU) - (arrival time)
         */
        if (ps[pick].start_time == -1) { // first time it runs
            ps[pick].start_time = t;
            ps[pick].resp = t - ps[pick].at;
        }

        /*
         * Record this process's execution in the Gantt chart:
         * - It runs from current time 't' to (t + burst time)
         * - Since this is non-preemptive, it will execute fully.
         */
        gantt.push_back({ps[pick].pid, t, t + ps[pick].bt});

        // Advance the simulation time by the burst time of this process
        t += ps[pick].bt;

        /*
         * Calculate key performance metrics:
         * - Completion Time (CT) = time when process finishes
         * - Turnaround Time (TAT) = CT - AT
         * - Waiting Time (WT) = TAT - BT
         */
        ps[pick].ct  = t;
        ps[pick].tat = ps[pick].ct - ps[pick].at;
        ps[pick].wt  = ps[pick].tat - ps[pick].bt;

        // Mark this process as finished
        done[pick] = true;
        finished++;
    }

    /*
     * After all processes are executed, call the helper function
     * to print:
     *   - Gantt Chart
     *   - Detailed process table
     *   - Average TAT, WT, RT
     */
    print_results(ps, gantt);
}

// Perfect 👍 Let’s add a **test case** to explain and demonstrate how this `run_priority()` function works step-by-step.
// This will help you understand how the algorithm chooses processes and computes CT, TAT, WT, and RT.

// ---

// ## 🧪 Test Case Example for `run_priority()`

// ### 🔹 Input

// ```
// 4
// 1 0 5 2
// 2 1 3 1
// 3 2 8 4
// 4 3 6 3
// ```

// ### 🧾 Explanation of Input

// | PID | Arrival Time (AT) | Burst Time (BT) | Priority (PR) |
// | --- | ----------------- | --------------- | ------------- |
// | 1   | 0                 | 5               | 2             |
// | 2   | 1                 | 3               | 1             |
// | 3   | 2                 | 8               | 4             |
// | 4   | 3                 | 6               | 3             |

// > Remember: **Lower priority number = Higher priority.**
// > This means process 2 (priority 1) has the **highest priority**.

// ---

// ### 🧮 Step-by-Step Execution

// #### **t = 0**

// * Processes that have arrived: **P1**
// * Only `P1` is available → CPU executes **P1**
// * P1 runs **non-preemptively** for its entire burst (5 units)

// 🕓 Gantt Chart so far:

// ```
// [0-5]:P1
// ```

// At `t = 5`, **P1 completes**.

// ---

// #### **t = 5**

// * Processes that have arrived: P2 (AT=1), P3 (AT=2), P4 (AT=3)
// * All are waiting.
// * Among these:

//   * P2 has the **highest priority** (1)
//   * P4 has 3, P3 has 4 → lower priority

// ✅ Pick **P2** (PR=1)

// * Runs for its entire burst of 3 units.

// 🕓 Gantt Chart:

// ```
// [0-5]:P1  [5-8]:P2
// ```

// At `t = 8`, **P2 completes**.

// ---

// #### **t = 8**

// Remaining: P3 (PR=4), P4 (PR=3)
// ✅ Pick **P4** (priority 3 < 4)

// * Runs for 6 units.

// 🕓 Gantt Chart:

// ```
// [0-5]:P1  [5-8]:P2  [8-14]:P4
// ```

// At `t = 14`, **P4 completes**.

// ---

// #### **t = 14**

// Remaining: **P3 (PR=4)** only.

// ✅ Pick **P3**

// * Runs for 8 units.

// 🕓 Final Gantt Chart:

// ```
// [0-5]:P1  [5-8]:P2  [8-14]:P4  [14-22]:P3
// ```

// At `t = 22`, **P3 completes**.

// ---

// ### 🧾 Compute Metrics

// | PID | AT | BT | PR | CT | TAT = CT - AT | WT = TAT - BT | RT = Start - AT |
// | --- | -- | -- | -- | -- | ------------- | ------------- | --------------- |
// | 1   | 0  | 5  | 2  | 5  | 5             | 0             | 0               |
// | 2   | 1  | 3  | 1  | 8  | 7             | 4             | 4               |
// | 3   | 2  | 8  | 4  | 22 | 20            | 12            | 12              |
// | 4   | 3  | 6  | 3  | 14 | 11            | 5             | 5               |

// ---

// ### 🧮 Average Values

// * **Average Turnaround Time (ATAT)** = (5 + 7 + 20 + 11) / 4 = **10.75**
// * **Average Waiting Time (AWT)** = (0 + 4 + 12 + 5) / 4 = **5.25**
// * **Average Response Time (ART)** = (0 + 4 + 12 + 5) / 4 = **5.25**

// ---

// ### 🖥️ Output (when you run the program)

// ```bash
// ./out priority < input.txt
// ```

// ### 📤 Expected Output

// ```
// Gantt:
// [0-5]:P1  [5-8]:P2  [8-14]:P4  [14-22]:P3  

// PID   AT      BT      PR        CT      TAT     WT      RT      
// 1     0       5       2         5       5       0       0
// 2     1       3       1         8       7       4       4
// 3     2       8       4         22      20      12      12
// 4     3       6       3         14      11      5       5

// Averages ->  ATAT: 10.75  AWT: 5.25  ART: 5.25
// ```

// ---

// ### ✅ Summary

// * **Process 2** had highest priority, so it ran as soon as CPU was free.
// * CPU stays idle **only if no process has arrived**.
// * Non-preemptive: once a process starts, it runs till the end.
// * Metrics (CT, TAT, WT, RT) are derived directly from the simulated timeline.

// ---

// Would you like me to add **a visual Gantt chart diagram** (ASCII timeline style) in your README to help visualize all four scheduling algorithms?
