#include "mem.hpp"
#include <vector>
using namespace std;

// Continue searching from the block after the last placement (wraps around).
Result next_fit(const vector<int>& blocks, const vector<int>& procs) {
    vector<int> left = blocks;
    vector<int> alloc(procs.size(), -1);
    vector<int> ifrag(procs.size(), 0);

    int nB = (int)left.size();
    if (nB == 0) return {alloc, ifrag, left};

    int cursor = 0; // next starting block index for search

    for (size_t i = 0; i < procs.size(); ++i) {
        int tried = 0;
        int b = cursor;
        bool placed = false;
        while (tried < nB) {
            if (left[b] >= procs[i]) {
                alloc[i] = b;
                ifrag[i] = left[b] - procs[i];
                left[b] -= procs[i];
                cursor = b;                 // next search continues from here
                placed = true;
                break;
            }
            b = (b + 1) % nB;
            tried++;
        }
        if (!placed) {
            // leave alloc[i] = -1, ifrag = 0
        } else {
            // advance cursor to next block to be fair
            cursor = (cursor + 1) % nB;
        }
    }
    return {alloc, ifrag, left};
}
