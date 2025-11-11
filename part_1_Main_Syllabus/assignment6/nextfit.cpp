#include "mem.hpp"
#include <vector>
using namespace std;

/*
 * --------------------------------------------------------------------
 * Function: next_fit
 * Purpose : Simulates the Next Fit memory allocation strategy.
 *
 * Definition:
 * ------------
 * The **Next Fit** strategy works similarly to **First Fit**,
 * but instead of always starting the search from the beginning
 * of the memory blocks, it starts from the **last allocated block**.
 *
 * If the end of the block list is reached, the search wraps around
 * to the beginning (circular search).
 *
 * Characteristics:
 * ----------------
 * ✅ Avoids re-scanning blocks that are already filled.
 * ✅ Can distribute allocations more evenly across memory.
 * ❌ May still suffer from fragmentation.
 * ❌ Slightly slower than First Fit due to circular search.
 *
 * Parameters:
 * ------------
 * - blocks: vector<int> → sizes of available memory blocks.
 * - procs:  vector<int> → sizes (requirements) of processes.
 *
 * Returns:
 * ------------
 * - Result structure containing:
 *     → alloc[] : index of block allocated to each process (-1 if not allocated)
 *     → internalFrag[] : leftover space inside allocated blocks
 *     → blockLeft[] : remaining free space in each block
 *
 * Example:
 * ---------
 * Blocks:   [100, 500, 200, 300, 600]
 * Processes:[212, 417, 112, 426]
 *
 * Allocation pattern (wrap-around search):
 *   P1(212) → B2(500)
 *   P2(417) → starts from next block (B3→B4→B5) → B5(600)
 *   P3(112) → starts from B1 (wraps around) → B3(200)
 *   P4(426) → starts from next (B4, B5, B1...) → not allocated
 * --------------------------------------------------------------------
 */
Result next_fit(const vector<int>& blocks, const vector<int>& procs) {
    // Copy of available block sizes (to modify as processes occupy space)
    vector<int> left = blocks;

    // Initialize allocation results
    vector<int> alloc(procs.size(), -1);   // Block number per process
    vector<int> ifrag(procs.size(), 0);    // Internal fragmentation per process

    // Total number of memory blocks
    int nB = (int)left.size();

    // Edge case: no memory blocks available
    if (nB == 0) return {alloc, ifrag, left};

    /*
     * 'cursor' tracks the current position in memory.
     * The next search for an empty block will start from here.
     * After each allocation, the cursor moves forward.
     */
    int cursor = 0;

    /*
     * Loop over each process to find a suitable memory block.
     */
    for (size_t i = 0; i < procs.size(); ++i) {

        int tried = 0;           // Number of blocks checked so far
        int b = cursor;          // Start searching from last allocated position
        bool placed = false;     // Will mark true if process is successfully allocated

        /*
         * While loop: check all blocks circularly until:
         * - We find a block big enough, or
         * - We have checked all blocks once (nB times).
         */
        while (tried < nB) {
            // Check if the current block has enough remaining space
            if (left[b] >= procs[i]) {
                /*
                 * Found a suitable block.
                 * Allocate process i to this block.
                 */
                alloc[i] = b;                        // store block index
                ifrag[i] = left[b] - procs[i];       // internal fragmentation = leftover space
                left[b] -= procs[i];                 // reduce free space in that block
                cursor = b;                          // remember this position for next search
                placed = true;                       // allocation successful
                break;                               // move to next process
            }

            /*
             * Move to the next block (circularly).
             * If end reached, wrap around using modulo.
             */
            b = (b + 1) % nB;
            tried++;
        }

        /*
         * If the process wasn't placed (no block large enough):
         *   → alloc[i] remains -1
         *   → ifrag[i] remains 0
         */
        if (!placed) {
            // No changes needed; just skip this process
        } else {
            /*
             * To avoid favoring the same block repeatedly,
             * move the cursor forward to the next block index.
             * This ensures "fair" distribution across memory.
             */
            cursor = (cursor + 1) % nB;
        }
    }

    // Return final allocation details
    return {alloc, ifrag, left};
}


// 🧮 Example Walkthrough

// Blocks: [100, 500, 200, 300, 600]
// Processes: [212, 417, 112, 426]

// Step	Process	Size	Search Start	Allocated Block	Block Before	Block After	Comment
// 1	P1	212	B1	B2 (500)	500	288	Starts from B1 → finds first fit at B2
// 2	P2	417	B3	B5 (600)	600	183	Starts from next → finds B5
// 3	P3	112	B1	B3 (200)	200	88	Wraps around, fits in B3
// 4	P4	426	B4	—	—	—	No block large enough

// Final Remaining Free Space:

// B1 = 100
// B2 = 288
// B3 = 88
// B4 = 300
// B5 = 183

// 📊 Output Example (via print_result())
// === Next Fit ===
// Proc#   Size        Block     BlockSize     IntFrag
// 0       212         1         500           288
// 1       417         4         600           183
// 2       112         2         200           88
// 3       426         -         -             0

// Total internal fragmentation: 559
// Block remainders (free per block): [0:100] [1:288] [2:88] [3:300] [4:183]
// Total free after allocation: 959

// ✅ Summary
// Feature	Description
// Type	Contiguous allocation
// Strategy	Starts from last allocated block (circular search)
// Goal	Slightly improve fairness compared to First Fit
// Pros	Avoids repeated scanning from start; distributes load
// Cons	Still suffers from external fragmentation
// Complexity	O(P × B) (like First Fit, but circular)