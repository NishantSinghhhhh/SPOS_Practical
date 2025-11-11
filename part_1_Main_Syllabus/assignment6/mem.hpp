// mem.hpp
#pragma once
#include <vector>
#include <string>

// Allocation result:
// - allocation[i] = block index chosen for process i (or -1 if not allocated)
// - internalFrag[i] = leftover space inside the chosen block for process i
// - remaining[b] = remaining capacity of block b after all allocations
struct Result {
    std::vector<int> allocation;
    std::vector<int> internalFrag;
    std::vector<int> remaining;
};

// Algorithms
Result first_fit(const std::vector<int>& blocks, const std::vector<int>& procs);
Result next_fit (const std::vector<int>& blocks, const std::vector<int>& procs);
Result best_fit (const std::vector<int>& blocks, const std::vector<int>& procs);
Result worst_fit(const std::vector<int>& blocks, const std::vector<int>& procs);

// Pretty printer (shared)
void print_result(const std::string& name,
                  const std::vector<int>& blocks,
                  const std::vector<int>& procs,
                  const Result& res);
