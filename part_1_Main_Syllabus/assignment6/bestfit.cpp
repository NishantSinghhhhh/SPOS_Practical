#include "mem.hpp"
#include <vector>
#include <limits>     // for numeric_limits<int>::max()
using namespace std;

/*
 * --------------------------------------------------------------------
 * Function: best_fit
 * Purpose : Simulates the Best Fit memory allocation strategy.
 *
 * Definition:
 * ------------
 * In the **Best Fit** strategy, each process is allocated to the 
 * smallest memory block that is large enough to hold it.
 *
 * Characteristics:
 * ----------------
 * ✅ Minimizes internal fragmentation by filling the "tightest" spot.
 * ❌ Slower than First Fit — must check *all* blocks to find the best match.
 * ❌ Can lead to high external fragmentation because many small gaps remain.
 *
 * Parameters:
 * ------------
 * - blocks: vector<int> → sizes of memory blocks available.
 * - procs:  vector<int> → memory requirements of processes.
 *
 * Returns:
 * ------------
 * - Result structure containing:
 *     → alloc[] : block index allocated to each process (-1 if not allocated)
 *     → internalFrag[] : internal fragmentation per process
 *     → blockLeft[] : remaining free space per block after allocation
 *
 * Example:
 * ---------
 * Blocks: [100, 500, 200, 300, 600]
 * Processes: [212, 417, 112, 426]
 *
 * Allocation sequence:
 *   P1(212) → B3(300)   → leftover 88
 *   P2(417) → B5(600)   → leftover 183
 *   P3(112) → B1(200)   → leftover 88
 *   P4(426) → B2(500)   → leftover 74
 * --------------------------------------------------------------------
 */
Result best_fit(const vector<int>& blocks, const vector<int>& procs) {
    /*
     * 'left' keeps track of the remaining space in each block.
     * Initially, it's a copy of 'blocks' since all are empty.
     */
    vector<int> left = blocks;

    /*
     * 'alloc[i]' → block number allocated to process i (-1 means unallocated).
     * 'ifrag[i]' → internal fragmentation for process i.
     */
    vector<int> alloc(procs.size(), -1);
    vector<int> ifrag(procs.size(), 0);

    /*
     * Outer loop: iterate through all processes one by one.
     */
    for (size_t i = 0; i < procs.size(); ++i) {
        int bestIdx = -1; // index of the best (smallest fitting) block
        int bestLeft = numeric_limits<int>::max(); // track smallest leftover space found

        /*
         * Inner loop: examine every block to find the one that:
         *  - Has enough space (left[b] >= procs[i])
         *  - Leaves the *least* remaining space (minimize internal fragmentation)
         */
        for (size_t b = 0; b < left.size(); ++b) {
            if (left[b] >= procs[i]) {
                int remain = left[b] - procs[i];
                if (remain < bestLeft) {
                    bestLeft = remain; // update best leftover
                    bestIdx = (int)b;  // remember the index of this block
                }
            }
        }

        /*
         * After scanning all blocks:
         * - If 'bestIdx' is still -1, no block was large enough → process not allocated.
         * - Otherwise, allocate process to that block.
         */
        if (bestIdx != -1) {
            alloc[i] = bestIdx;                        // store block index
            ifrag[i] = left[bestIdx] - procs[i];       // internal fragmentation = leftover
            left[bestIdx] -= procs[i];                 // reduce available space in that block
        }
    }

    /*
     * Return allocation results to be printed or analyzed.
     */
    return {alloc, ifrag, left};
}


// 🧮 Example Walkthrough

// Blocks: [100, 500, 200, 300, 600]
// Processes: [212, 417, 112, 426]

// Process	Size	Best Block (Before)	Best Block (After)	Internal Frag	Comment
// P1	212	B3 (300)	B3 = 88	88	B3 leaves the least leftover (300–212)
// P2	417	B5 (600)	B5 = 183	183	600–417 = 183 (best fit)
// P3	112	B1 (100), B3 (88), B4 (300)	B4 = 188	188	Only B4 fits, smallest leftover
// P4	426	B2 (500)	B2 = 74	74	Smallest block that fits (500–426)

// Final Remaining Space (after allocation):
// [100, 74, 88, 188, 183]

// 📊 Output Example (Printed via print_result())
// === Best Fit ===
// Proc#   Size        Block     BlockSize     IntFrag
// 0       212         2         300           88
// 1       417         4         600           183
// 2       112         3         300           188
// 3       426         1         500           74

// Total internal fragmentation: 533
// Block remainders (free per block): [0:100] [1:74] [2:88] [3:188] [4:183]
// Total free after allocation: 633

// ✅ Summary
// Feature	Description
// Type	Contiguous allocation
// Strategy	Chooses smallest available block that fits
// Goal	Minimize internal fragmentation
// Pros	Efficient space usage for small processes
// Cons	Slower search; can cause many small unusable gaps
// Complexity	O(P × B) → must scan all blocks for each process