#include "pass1.hpp"
#include <iostream>
using namespace std;

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc != 2){
        cerr << "Usage: " << argv[0] << " <source.asm>\n";
        return 1;
    }
    init_tables();
    return run_pass1(argv[1]);
}
