#include "scheduler.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;

void print_results(const vector<Proc>& ps_raw, const vector<Seg>& gantt_raw) {
    // merge adjacent identical segments
    vector<Seg> gantt;
    for (auto s : gantt_raw) {
        if (!gantt.empty() && gantt.back().pid == s.pid && gantt.back().end == s.start) {
            gantt.back().end = s.end;
        } else {
            gantt.push_back(s);
        }
    }

    cout << "Gantt:\n";
    for (auto& s : gantt) cout << "[" << s.start << "-" << s.end << "]:P" << s.pid << "  ";
    cout << "\n\n";

    auto ps = ps_raw;
    sort(ps.begin(), ps.end(), [](const Proc& a, const Proc& b){ return a.pid < b.pid; });

    cout << left << setw(6) << "PID" << setw(8) << "AT" << setw(8) << "BT"
         << setw(10) << "PR" << setw(8) << "CT" << setw(8) << "TAT"
         << setw(8) << "WT" << setw(8) << "RT" << "\n";

    double sum_tat=0, sum_wt=0, sum_rt=0;
    for (auto &p : ps) {
        cout << left << setw(6) << p.pid
             << setw(8) << p.at
             << setw(8) << p.bt
             << setw(10) << p.pr
             << setw(8) << p.ct
             << setw(8) << p.tat
             << setw(8) << p.wt
             << setw(8) << p.resp
             << "\n";
        sum_tat += p.tat; sum_wt += p.wt; sum_rt += p.resp;
    }
    int n = (int)ps.size();
    cout << fixed << setprecision(2);
    cout << "\nAverages ->  ATAT: " << (sum_tat/n)
         << "  AWT: " << (sum_wt/n)
         << "  ART: " << (sum_rt/n) << "\n";
}
