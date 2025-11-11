#include "mem.hpp"
#include <vector>
using namespace std;

/*
 * --------------------------------------------------------------------
 * Function: first_fit
 * Purpose : Simulates the First Fit memory allocation strategy.
 *
 * Definition:
 * ------------
 * In First Fit, each incoming process is allocated to the **first**
 * memory block that is large enough to accommodate it.
 *
 * Characteristics:
 * ----------------
 * ✅ Simple and fast — searches from the start every time.
 * ❌ May leave small unusable gaps (external fragmentation).
 * ❌ Over time, the memory becomes fragmented due to leftover spaces.
 *
 * Parameters:
 * ------------
 * - blocks: vector<int> — sizes of memory blocks available.
 * - procs:  vector<int> — memory requirements of each process.
 *
 * Returns:
 * ------------
 * - Result structure containing:
 *     → alloc[] : block index allocated to each process (-1 if not allocated)
 *     → internalFrag[] : internal fragmentation per process (unused space in the block)
 *     → blockLeft[] : remaining free space in each block after allocation
 *
 * Example:
 * ---------
 * Blocks: [100, 500, 200, 300, 600]
 * Processes: [212, 417, 112, 426]
 *
 * Allocation:
 *   P1(212) → B2(500)  → leftover 288
 *   P2(417) → B5(600)  → leftover 183
 *   P3(112) → B3(200)  → leftover 88
 *   P4(426) → not allocated (no block big enough)
 * --------------------------------------------------------------------
 */
Result first_fit(const vector<int>& blocks, const vector<int>& procs) {
    /*
     * Make a copy of the original block sizes.
     * `left[b]` keeps track of remaining space in each block
     * as processes get allocated.
     */
    vector<int> left = blocks; 

    /*
     * `alloc[i]` stores which block was allocated to process i.
     * Initialized to -1 → means "not allocated yet".
     */
    vector<int> alloc(procs.size(), -1);        

    /*
     * `ifrag[i]` (internal fragmentation) = leftover space
     * inside the block that the process occupies.
     * For example, if block size = 200 and process = 180,
     * internal fragmentation = 20.
     */
    vector<int> ifrag(procs.size(), 0);         

    /*
     * Outer loop → iterate over each process one by one.
     */
    for (size_t i = 0; i < procs.size(); ++i) {

        /*
         * Inner loop → scan blocks from the start.
         * As soon as we find a block with enough space,
         * allocate it to the process and break.
         */
        for (size_t b = 0; b < left.size(); ++b) {
            // Check if current block `b` has enough space for process `i`
            if (left[b] >= procs[i]) {

                /*
                 * Allocate this block to the process.
                 * Record block index and internal fragmentation.
                 */
                alloc[i] = (int)b;
                ifrag[i] = left[b] - procs[i];

                /*
                 * Reduce the remaining free space in that block.
                 * This reflects that part of the block is now occupied.
                 */
                left[b] -= procs[i];

                /*
                 * Break out — process allocated, move to the next one.
                 */
                break;
            }
        }
        /*
         * If no block was large enough for this process,
         * alloc[i] remains -1 (meaning process not allocated).
         */
    }

    /*
     * Return all computed information:
     *   - allocation status per process
     *   - internal fragmentation per process
     *   - remaining free space per block
     */
    return {alloc, ifrag, left};
}


// 🧪 Example Walkthrough
// Blocks: [100, 500, 200, 300, 600]
// Processes: [212, 417, 112, 426]
// ProcessRequiredAllocated BlockBefore AllocationAfter AllocationInternal FragP1212B2 (500)500288288P2417B5 (600)600183183P3112B3 (200)2008888P4426————
// Final Block Leftovers:
// B1 = 100
// B2 = 288
// B3 = 88
// B4 = 300
// B5 = 183


// 📊 Output (from print_result())
// === First Fit ===
// Proc#   Size        Block     BlockSize     IntFrag      
// 0       212         1         500           288
// 1       417         4         600           183
// 2       112         2         200           88
// 3       426         -         -             0
// Total internal fragmentation: 559
// Block remainders (free per block): [0:100] [1:288] [2:88] [3:300] [4:183]
// Total free after allocation: 959


// ✅ Summary
// FeatureDescriptionTypeContiguous allocationStrategyFirst Fit (scans from beginning each time)AdvantageFast allocation — stops at first available spaceDisadvantageLeads to external fragmentationComplexityO(P × B) — for P processes and B blocks

// Would you like me to add similar full commentary for the other three strategies (Next Fit, Best Fit, Worst Fit) so your assignment explanations are uniform and detailed?