#include "scheduler.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/*
Input:
N
pid arrival burst priority   (N lines)
- priority is used by Priority scheduling (lower = higher).
*/

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <fcfs|sjf|priority|rr> [quantum]\n";
        return 1;
    }
    string mode = argv[1];
    int q = (argc >= 3 ? stoi(argv[2]) : 0);

    int N;
    if (!(cin >> N)) { cerr << "Failed to read N\n"; return 1; }
    vector<Proc> ps(N);
    for (int i = 0; i < N; ++i) {
        cin >> ps[i].pid >> ps[i].at >> ps[i].bt >> ps[i].pr;
        ps[i].rt = ps[i].bt;
    }

    if (mode == "fcfs") run_fcfs(ps);
    else if (mode == "sjf") run_sjf(ps);
    else if (mode == "priority") run_priority(ps);
    else if (mode == "rr") {
        if (q <= 0) { cerr << "Round Robin requires positive quantum.\n"; return 1; }
        run_rr(ps, q);
    } else {
        cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }
    return 0;
}
