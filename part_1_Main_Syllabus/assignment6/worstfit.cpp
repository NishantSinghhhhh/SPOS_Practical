#include "mem.hpp"
#include <vector>
#include <limits>
using namespace std;

// Choose the largest available block (to reduce future external fragmentation).
Result worst_fit(const vector<int>& blocks, const vector<int>& procs) {
    vector<int> left = blocks;
    vector<int> alloc(procs.size(), -1);
    vector<int> ifrag(procs.size(), 0);

    for (size_t i = 0; i < procs.size(); ++i) {
        int worstIdx = -1;
        int worstSize = -1;
        for (size_t b = 0; b < left.size(); ++b) {
            if (left[b] >= procs[i] && left[b] > worstSize) {
                worstSize = left[b];
                worstIdx = (int)b;
            }
        }
        if (worstIdx != -1) {
            alloc[i] = worstIdx;
            ifrag[i] = left[worstIdx] - procs[i];
            left[worstIdx] -= procs[i];
        }
    }
    return {alloc, ifrag, left};
}
