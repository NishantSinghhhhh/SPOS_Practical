#include <iostream>
#include <string>
#include "semaphore.cpp"   // for helpers
// problems.cpp provides these via the namespace below
namespace Problems {
    void run_prodcon();
    void run_rw();
    void run_phil();
}

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    if (argc != 2) {
        std::cerr <<
        "Usage: " << argv[0] << " <mode>\n"
        "  modes:\n"
        "    prodcon  - Producer-Consumer (bounded buffer)\n"
        "    rw       - Readers-Writers (readers preference)\n"
        "    phil     - Dining Philosophers (N-1 limit)\n";
        return 1;
    }

    std::string mode = argv[1];
    if (mode == "prodcon")      Problems::run_prodcon();
    else if (mode == "rw")      Problems::run_rw();
    else if (mode == "phil")    Problems::run_phil();
    else {
        std::cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }
    return 0;
}
