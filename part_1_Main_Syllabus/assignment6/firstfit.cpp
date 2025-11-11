#include "mem.hpp"
#include <vector>
using namespace std;

// Scan blocks from the start for each process, place in first block that fits.
Result first_fit(const vector<int>& blocks, const vector<int>& procs) {
    vector<int> left = blocks;                  // current remaining size per block
    vector<int> alloc(procs.size(), -1);        // block index per process
    vector<int> ifrag(procs.size(), 0);         // internal frag per process

    for (size_t i = 0; i < procs.size(); ++i) {
        for (size_t b = 0; b < left.size(); ++b) {
            if (left[b] >= procs[i]) {
                alloc[i] = (int)b;
                ifrag[i] = left[b] - procs[i];
                left[b] -= procs[i];
                break;
            }
        }
    }
    return {alloc, ifrag, left};
}
