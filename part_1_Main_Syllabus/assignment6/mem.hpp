#ifndef MEM_HPP
#define MEM_HPP

#include <vector>
#include <string>

struct Result {
    // For each process i -> index of allocated block (or -1).
    std::vector<int> alloc;          
    // Internal fragmentation per process (0 if not allocated)
    std::vector<int> internalFrag;   
    // Block remainders after allocation
    std::vector<int> blockLeft;      
};

Result first_fit(const std::vector<int>& blocks, const std::vector<int>& procs);
Result next_fit (const std::vector<int>& blocks, const std::vector<int>& procs);
Result best_fit (const std::vector<int>& blocks, const std::vector<int>& procs);
Result worst_fit(const std::vector<int>& blocks, const std::vector<int>& procs);

void print_result(const std::string& title,
                  const std::vector<int>& blocks,
                  const std::vector<int>& procs,
                  const Result& res);

#endif
