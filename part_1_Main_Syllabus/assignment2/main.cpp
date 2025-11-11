#include "macroprocessor.cpp"

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc != 6) {
        cerr << "Usage: " << argv[0]
             << " <source.asm> <mnt.txt> <mdt.txt> <intermediate.txt> <expanded.asm>\n";
        return 1;
    }

    string source = argv[1];
    string mnt = argv[2];
    string mdt = argv[3];
    string intermediate = argv[4];
    string expanded = argv[5];

    pass1_build_tables_and_intermediate(source, mnt, mdt, intermediate);
    pass2_expand(intermediate, mnt, mdt, expanded);

    return 0;
}
