#include <iostream>
#include <string>
#include "semaphore.cpp"

namespace Problems { void run_prodcon(); void run_rw(); void run_phil(); }

int main(int argc, char** argv){
    if(argc!=2){
        std::cerr<<"Usage: "<<argv[0]<<" <mode>\n"
                 <<"  modes:\n"
                 <<"    prodcon  - Producer-Consumer\n"
                 <<"    rw       - Readers-Writers\n"
                 <<"    phil     - Dining Philosophers\n";
        return 1;
    }
    std::string mode=argv[1];
    if(mode=="prodcon") Problems::run_prodcon();
    else if(mode=="rw") Problems::run_rw();
    else if(mode=="phil") Problems::run_phil();
    else { std::cerr<<"Unknown mode: "<<mode<<"\n"; return 1; }
    return 0;
}
