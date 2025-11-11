#include "mem.hpp"
#include <vector>
#include <limits>
using namespace std;

// Choose the smallest block that still fits the process.
Result best_fit(const vector<int>& blocks, const vector<int>& procs) {
    vector<int> left = blocks;
    vector<int> alloc(procs.size(), -1);
    vector<int> ifrag(procs.size(), 0);

    for (size_t i = 0; i < procs.size(); ++i) {
        int bestIdx = -1;
        int bestLeft = numeric_limits<int>::max();
        for (size_t b = 0; b < left.size(); ++b) {
            if (left[b] >= procs[i]) {
                int remain = left[b] - procs[i];
                if (remain < bestLeft) {
                    bestLeft = remain;
                    bestIdx = (int)b;
                }
            }
        }
        if (bestIdx != -1) {
            alloc[i] = bestIdx;
            ifrag[i] = left[bestIdx] - procs[i];
            left[bestIdx] -= procs[i];
        }
    }
    return {alloc, ifrag, left};
}
