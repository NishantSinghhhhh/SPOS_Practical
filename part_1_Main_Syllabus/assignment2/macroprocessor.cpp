#ifndef MACROPROCESSOR_HPP
#define MACROPROCESSOR_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct MNTEntry {
    string name;
    int mdtIndex;
    int paramCount;
};

struct Pass1Output {
    vector<MNTEntry> MNT;
    vector<string> MDT;
};

// Helpers (implemented in pass1.cpp)
string trim(const string& s);
bool is_comment_or_blank(const string& line);
pair<string, string> head_and_rest(const string& line);
vector<string> split_params(const string& s);
void replace_all(string& text, const string& from, const string& to);

// Pass-I
Pass1Output pass1_build_tables_and_intermediate(
    const string& sourcePath,
    const string& mntPath,
    const string& mdtPath,
    const string& intermediatePath
);

// Pass-II
void pass2_expand(
    const string& intermediatePath,
    const string& mntPath,
    const string& mdtPath,
    const string& expandedPath
);

#endif
