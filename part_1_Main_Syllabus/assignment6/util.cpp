#include "mem.hpp"
#include <iostream>
#include <iomanip>
#include <numeric>
using namespace std;

void print_result(const string& title,
                  const vector<int>& blocks,
                  const vector<int>& procs,
                  const Result& res)
{
    cout << "\n=== " << title << " ===\n";
    cout << left
         << setw(8)  << "Proc#"
         << setw(12) << "Size"
         << setw(10) << "Block"
         << setw(14) << "BlockSize"
         << setw(14) << "IntFrag" << "\n";

    long long totalIF = 0;
    for (size_t i = 0; i < procs.size(); ++i) {
        int b = (i < res.allocation.size() ? res.allocation[i] : -1);
        int bsize = (b >= 0 && b < (int)blocks.size()) ? blocks[b] : -1;

        cout << left
             << setw(8)  << static_cast<int>(i)                  // process index (0-based)
             << setw(12) << procs[i]                             // process size
             << setw(10) << (b >= 0 ? to_string(b) : string("-"))// block index
             << setw(14) << (b >= 0 ? to_string(bsize) : string("-"))
             << setw(14) << (i < res.internalFrag.size() ? res.internalFrag[i] : 0)
             << "\n";

        if (i < res.internalFrag.size()) totalIF += res.internalFrag[i];
    }
    cout << "Total internal fragmentation: " << totalIF << "\n";

    long long totalFree = 0;
    for (int x : res.remaining) totalFree += x;

    cout << "Block remainders (free per block): ";
    for (size_t i = 0; i < res.remaining.size(); ++i) {
        cout << "[" << i << ":" << res.remaining[i] << "] ";
    }
    cout << "\nTotal free after allocation: " << totalFree << "\n\n";
}
