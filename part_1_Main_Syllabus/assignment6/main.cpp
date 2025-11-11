#include "mem.hpp"
#include <bits/stdc++.h>
using namespace std;

/*
Input format (from stdin):
B
b0 b1 b2 ... b(B-1)
P
p0 p1 p2 ... p(P-1)

Example:
5
100 500 200 300 600
4
212 417 112 426
*/

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <first|next|best|worst|all>\n";
        return 1;
    }
    string mode = argv[1];

    int B; if (!(cin >> B)) { cerr << "Failed to read B\n"; return 1; }
    vector<int> blocks(B);
    for (int i = 0; i < B; ++i) cin >> blocks[i];

    int P; if (!(cin >> P)) { cerr << "Failed to read P\n"; return 1; }
    vector<int> procs(P);
    for (int i = 0; i < P; ++i) cin >> procs[i];

    auto run_and_print = [&](const string& name, Result (*fn)(const vector<int>&, const vector<int>&)) {
        auto res = fn(blocks, procs);
        print_result(name, blocks, procs, res);
    };

    if (mode == "first")      run_and_print("First Fit",  first_fit);
    else if (mode == "next")  run_and_print("Next Fit",   next_fit);
    else if (mode == "best")  run_and_print("Best Fit",   best_fit);
    else if (mode == "worst") run_and_print("Worst Fit",  worst_fit);
    else if (mode == "all") {
        run_and_print("First Fit",  first_fit);
        run_and_print("Next Fit",   next_fit);
        run_and_print("Best Fit",   best_fit);
        run_and_print("Worst Fit",  worst_fit);
    } else {
        cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }
    return 0;
}
