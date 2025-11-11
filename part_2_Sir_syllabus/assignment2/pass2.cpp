#include <bits/stdc++.h>
using namespace std;

/*
  Pass–II for the two-pass assembler used in Pass–I you built earlier.

  INPUT FILES (from Pass–I):
    - intermediate.txt
        Lines look like:
          0100  (IS,04) (R,1) (S,VALUE)
              or (AD,01) (C,100)
              or 0105  (DL,02) (C,10)
    - symbol_table.txt
        Header then rows: SYMBOL  ADDR  LEN
    - literal_table.txt
        Header then rows: LITERAL  VALUE  ADDR

  OUTPUT:
    - machine_code.txt
        Lines like:
          0100  +04 1 0108
          0101  +00 0 0000   (for DS words)
          0105  +00 0 0010   (for DC 10)

  NOTES:
    - (IS,xx) => opcode = xx
      operand1 may be (R,k) or (CC,k) → goes to the “reg” field
      operand2 may be (S,name) or (L,i) or (C,val) → address/immediate field
    - (DL,01) DS n → emit n lines of +00 0 0000
    - (DL,02) DC c → emit +00 0 c
    - (AD,**) lines are ignored here (they were already resolved in Pass-I)
*/

struct Sym {
    string name;
    int addr = -1;
    int len  = 1;
};
struct Lit {
    string lit;
    int val = 0;
    int addr = -1;
};

// ---------- helpers ----------
static string trim(const string& s){
    size_t i = s.find_first_not_of(" \t\r\n");
    if (i==string::npos) return "";
    size_t j = s.find_last_not_of(" \t\r\n");
    return s.substr(i, j-i+1);
}
static bool is_digits(const string& s){
    if (s.empty()) return false;
    size_t i=0; if (s[0]=='+'||s[0]=='-') i=1;
    for (; i<s.size(); ++i) if (!isdigit((unsigned char)s[i])) return false;
    return true;
}

// Parse a tuple like "(IS,04)" -> {"IS","04"}
static bool parse_tuple(const string& tok, string& kind, string& val){
    if (tok.size()<5 || tok.front()!='(' || tok.back()!=')') return false;
    auto comma = tok.find(',');
    if (comma==string::npos) return false;
    kind = tok.substr(1, comma-1);
    val  = tok.substr(comma+1, tok.size()-comma-2);
    return true;
}

// Read Symbol Table
static map<string, Sym> load_symbols(const string& file){
    ifstream in(file);
    map<string, Sym> ST;
    if (!in) return ST;
    string line; getline(in,line); // header
    while (getline(in,line)){
        line = trim(line); if (line.empty()) continue;
        istringstream iss(line);
        Sym s;
        if (!(iss >> s.name >> s.addr >> s.len)) continue;
        ST[s.name] = s;
    }
    return ST;
}

// Read Literal Table
static vector<Lit> load_literals(const string& file){
    ifstream in(file);
    vector<Lit> LT;
    if (!in) return LT;
    string line; getline(in,line); // header
    while (getline(in,line)){
        line = trim(line); if (line.empty()) continue;
        istringstream iss(line);
        Lit L;
        if (!(iss >> L.lit >> L.val >> L.addr)) continue;
        LT.push_back(L);
    }
    return LT;
}

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // file names are fixed to match Pass-I outputs
    const string icFile  = "intermediate.txt";
    const string stFile  = "symbol_table.txt";
    const string ltFile  = "literal_table.txt";
    const string outFile = "machine_code.txt";

    // Load tables
    auto ST = load_symbols(stFile);
    auto LT = load_literals(ltFile);

    ifstream ic(icFile);
    if (!ic){ cerr << "Cannot open " << icFile << "\n"; return 1; }
    ofstream out(outFile);
    if (!out){ cerr << "Cannot create " << outFile << "\n"; return 1; }

    string line;
    while (getline(ic, line)){
        string s = trim(line);
        if (s.empty()) continue;

        // Tokenize: first field may be LC or blank; then tuples
        istringstream iss(s);
        string first; iss >> first;

        int lc = -1;
        vector<string> tuples;

        if (!first.empty() && is_digits(first)) {
            lc = stoi(first);
        } else {
            // first wasn't LC; it's a tuple
            tuples.push_back(first);
        }
        // read remaining tokens
        string t;
        while (iss >> t) tuples.push_back(t);

        // Parse first tuple to know what this line is
        if (tuples.empty()) continue;
        string k0, v0;
        if (!parse_tuple(tuples[0], k0, v0)) continue;

        // Handle AD (assembler directives) -> no machine code
        if (k0 == "AD") {
            continue;
        }

        // Handle DL (declaratives)
        if (k0 == "DL") {
            if (v0 == "01") { // DS
                // Expect (C,n) as next tuple
                int n = 0;
                if (tuples.size() >= 2) {
                    string k1, v1; parse_tuple(tuples[1], k1, v1);
                    if (k1=="C" && is_digits(v1)) n = stoi(v1);
                }
                for (int i=0;i<n;i++){
                    out << setw(4) << setfill('0') << (lc+i) << "  "
                        << "+00 0 0000\n";
                }
            } else if (v0 == "02") { // DC
                // Expect (C,val)
                int val = 0;
                if (tuples.size() >= 2) {
                    string k1, v1; parse_tuple(tuples[1], k1, v1);
                    if (k1=="C" && is_digits(v1)) val = stoi(v1);
                }
                out << setw(4) << setfill('0') << lc << "  "
                    << "+00 0 " << setw(4) << setfill('0') << val << "\n";
            }
            continue;
        }

        // Handle IS (imperatives) -> emit one machine instruction
        if (k0 == "IS") {
            int opcode = stoi(v0);
            int reg = 0;       // default register/cc field
            int addr = 0;      // default address/immediate

            // Iterate over remaining tuples to fill reg/addr
            for (size_t i=1; i<tuples.size(); ++i){
                string k,v; if (!parse_tuple(tuples[i], k, v)) continue;
                if (k=="R" || k=="CC") {
                    if (is_digits(v)) reg = stoi(v);
                } else if (k=="C") {
                    if (is_digits(v)) addr = stoi(v);
                } else if (k=="S") {
                    // Here Pass-I put symbol name directly: (S,NAME)
                    // If your Pass-I encodes S as an index, adapt here.
                    string name = v;
                    if (ST.count(name)) addr = ST[name].addr;
                    else addr = 0;
                } else if (k=="L") {
                    // literal index (0-based)
                    int idx = is_digits(v) ? stoi(v) : -1;
                    if (idx>=0 && idx<(int)LT.size()) addr = LT[idx].addr;
                }
            }

            out << setw(4) << setfill('0') << lc << "  "
                << "+" << setw(2) << setfill('0') << opcode
                << " " << reg
                << " " << setw(4) << setfill('0') << addr
                << "\n";
            continue;
        }

        // Unknown tuple class; skip
    }

    cout << "PASS-2 COMPLETED\nMachine code written to: " << outFile << "\n";
    return 0;
}
