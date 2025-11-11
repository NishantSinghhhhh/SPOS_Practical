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
        int b = res.alloc[i];
        int bsize = (b >= 0 ? blocks[b] : -1);
        cout << left
             << setw(8)  << (int)i
             << setw(12) << procs[i]
             << setw(10) << (b >= 0 ? to_string(b) : string("-"))
             << setw(14) << (b >= 0 ? to_string(bsize) : string("-"))
             << setw(14) << res.internalFrag[i]
             << "\n";
        totalIF += res.internalFrag[i];
    }
    cout << "Total internal fragmentation: " << totalIF << "\n";

    long long totalFree = 0;
    for (int x : res.blockLeft) totalFree += x;
    cout << "Block remainders (free per block): ";
    for (size_t i = 0; i < res.blockLeft.size(); ++i) {
        cout << "[" << i << ":" << res.blockLeft[i] << "] ";
    }
    cout << "\nTotal free after allocation: " << totalFree << "\n\n";
}
