#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <vector>
#include <string>

struct Proc {
    int pid{}, at{}, bt{}, pr{};
    int rt{};               // remaining time
    int start_time{-1};     // first time it runs (for response)
    int ct{0};              // completion time
    int tat{0};             // turnaround time
    int wt{0};              // waiting time
    int resp{-1};           // response time
    bool finished{false};
};

struct Seg { // for Gantt chart
    int pid;
    int start;
    int end; // exclusive
};

// --- Common output helper ---
void print_results(const std::vector<Proc>& ps, const std::vector<Seg>& gantt);

// --- Algorithms ---
void run_fcfs(std::vector<Proc> ps);
void run_sjf(std::vector<Proc> ps);                 // preemptive SRTF
void run_priority(std::vector<Proc> ps);            // non-preemptive (lower pr = higher)
void run_rr(std::vector<Proc> ps, int quantum);     // preemptive

#endif
