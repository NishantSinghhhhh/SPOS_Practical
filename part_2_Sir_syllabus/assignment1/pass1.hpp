#ifndef PASS1_HPP
#define PASS1_HPP

#include <cctype>
#include<bits/stdc++.h>
#include <string>
#include <unordered_map>
#include <vector>

// ------------------ Core types ------------------
enum class IType { IS, DL, AD };

struct Instr {
    IType type;
    int opcode;
    int length;   // words occupied (used for LC advance)
};

struct Sym {
    std::string name;
    int address = -1;
    int length = 1;
};

struct Lit {
    std::string literal; // e.g., ="5"
    int value = 0;
    int address = -1;
};

struct ICLine {
    int lc;            // location counter at this line (-1 for pseudo)
    std::string a, b, c; // e.g. (IS,04) (R,1) (S,VALUE)
};

// ------------------ Tables API ------------------
// Must be called once before run_pass1
void init_tables();

// Read-only accessors for MOT/REG/CC
const std::unordered_map<std::string, Instr>& MOT();
const std::unordered_map<std::string, int>&   REG();
const std::unordered_map<std::string, int>&   CC();

// ------------------ Utilities (used by pass1.cpp) ------------------
std::string trim(const std::string& s);
std::vector<std::string> split_ws_commas(const std::string& line);
bool is_number(const std::string& s);
bool is_literal(const std::string& s);
int  literal_value_of(const std::string& lit);
int  eval_expr(const std::string& expr, const std::map<std::string, Sym>& ST);

// ------------------ Pass-I driver ------------------
int run_pass1(const std::string& sourcePath);

#endif // PASS1_HPP
