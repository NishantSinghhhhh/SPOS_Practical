#include "pass1.hpp"
#include <fstream>
#include <iomanip>
#include <map>

using namespace std;

int run_pass1(const string& sourcePath){
    ifstream in(sourcePath);
    if (!in){ cerr<<"Cannot open "<<sourcePath<<"\n"; return 1; }

    map<string, Sym> ST;
    vector<Lit> LT;
    vector<int> PT;        // pool starts (indices into LT)
    vector<ICLine> IC;
    int LC = 0;

    auto start_new_pool = [&](){ PT.push_back((int)LT.size()); };

    auto assign_pool_literals = [&](){
        if (PT.empty()) return;
        int start = PT.back();
        for (int i=start; i<(int)LT.size(); ++i){
            if (LT[i].address == -1){
                LT[i].address = LC;
                LC += 1;    // assume 1 word per literal
            }
        }
    };

    string raw;
    while (getline(in, raw)){
        string line = trim(raw);
        if (line.empty() || line.rfind("#",0)==0 || line.rfind("//",0)==0) continue;

        vector<string> tok = split_ws_commas(line);
        string label, mnem; size_t idx = 0;

        // label?
        if (idx<tok.size() && MOT().count(tok[idx])==0 && tok[idx]!="LTORG" && tok[idx]!="END"){
            label = tok[idx++];
        }
        if (idx<tok.size()) mnem = tok[idx++];

        if (!label.empty()){
            auto &sym = ST[label];
            sym.name = label;
            if (sym.address==-1 || sym.address==0) sym.address = LC;
        }
        if (mnem.empty()) continue;

        if (MOT().count(mnem)==0){
            cerr<<"Unknown mnemonic: "<<mnem<<"\n";
            continue;
        }
        Instr I = MOT().at(mnem);

        vector<string> ops;
        while (idx<tok.size()) ops.push_back(tok[idx++]);

        if (I.type==IType::AD){
            if (mnem=="START"){
                int start = (ops.size()? stoi(ops[0]) : 0);
                LC = start;
                IC.push_back({-1,"(AD,01)","(C,"+to_string(start)+")",""});
                start_new_pool();
            } else if (mnem=="END"){
                assign_pool_literals();
                IC.push_back({-1,"(AD,02)","",""});
                break;
            } else if (mnem=="LTORG"){
                assign_pool_literals();
                IC.push_back({-1,"(AD,05)","",""});
                start_new_pool();
            } else if (mnem=="ORIGIN"){
                string expr = (ops.empty()? "0" : ops[0]);
                int val = eval_expr(expr, ST);
                LC = val;
                IC.push_back({-1,"(AD,03)","(C,"+to_string(val)+")",""});
            } else if (mnem=="EQU"){
                if (label.empty()){
                    cerr<<"EQU without label\n";
                } else {
                    int val = eval_expr(ops[0], ST);
                    ST[label].address = val;
                    IC.push_back({-1,"(AD,04)","(S,"+label+")","(C,"+to_string(val)+")"});
                }
            }
        }
        else if (I.type==IType::DL){
            if (mnem=="DS"){
                int size = stoi(ops[0]);
                if (!label.empty()){ ST[label].address = LC; ST[label].length = size; }
                IC.push_back({LC,"(DL,01)","(C,"+to_string(size)+")",""});
                LC += size;
            } else if (mnem=="DC"){
                int val = stoi(ops[0]);
                if (!label.empty()) ST[label].address = LC;
                IC.push_back({LC,"(DL,02)","(C,"+to_string(val)+")",""});
                LC += 1;
            }
        }
        else { // IType::IS
            ICLine ic; ic.lc = LC;
            ic.a = "(IS," + (I.opcode<10? string("0")+to_string(I.opcode): to_string(I.opcode)) + ")";

            // operand 1 (reg/cc/sym/const)
            if (!ops.empty()){
                if (REG().count(ops[0])) ic.b = "(R,"+to_string(REG().at(ops[0]))+")";
                else if (CC().count(ops[0])) ic.b = "(CC,"+to_string(CC().at(ops[0]))+")";
                else if (!is_literal(ops[0]) && !is_number(ops[0])) {
                    ST[ops[0]].name = ops[0];
                    ic.b = "(S,"+ops[0]+")";
                } else if (is_number(ops[0])) {
                    ic.b = "(C,"+ops[0]+")";
                }
            }
            // operand 2 (sym/lit/const)
            if (ops.size()>=2){
                string op2 = ops[1];
                if (is_literal(op2)){
                    int val = literal_value_of(op2);
                    int idxLit = -1;
                    for (int i=0;i<(int)LT.size();++i) if (LT[i].literal==op2){ idxLit=i; break; }
                    if (idxLit==-1){
                        LT.push_back({op2,val,-1});
                        if (PT.empty()) start_new_pool();
                    }
                    ic.c = "(L," + to_string(idxLit==-1? (int)LT.size()-1 : idxLit) + ")";
                } else if (is_number(op2)) {
                    ic.c = "(C,"+op2+")";
                } else {
                    ST[op2].name = op2;
                    ic.c = "(S,"+op2+")";
                }
            }
            IC.push_back(ic);
            LC += I.length;
        }
    }
    in.close();

    // --------- Outputs ----------
    {
        ofstream f("intermediate.txt");
        for (auto &x: IC){
            if (x.lc>=0) f << setw(4) << setfill('0') << x.lc << "  ";
            else f << "     ";
            f << left << setw(10) << x.a;
            if (!x.b.empty()) f << " " << left << setw(10) << x.b;
            if (!x.c.empty()) f << " " << left << setw(10) << x.c;
            f << "\n";
        }
    }
    {
        ofstream f("symbol_table.txt");
        f << left << setw(16) << "SYMBOL" << setw(8) << "ADDR" << setw(8) << "LEN" << "\n";
        for (auto &kv: ST){
            f << left << setw(16) << kv.second.name
              << setw(8) << kv.second.address
              << setw(8) << kv.second.length << "\n";
        }
    }
    {
        ofstream f("literal_table.txt");
        f << left << setw(16) << "LITERAL" << setw(8) << "VALUE" << setw(8) << "ADDR" << "\n";
        for (size_t i=0;i<LT.size();++i){
            f << left << setw(16) << LT[i].literal
              << setw(8) << LT[i].value
              << setw(8) << LT[i].address << "\n";
        }
    }
    {
        ofstream f("pool_table.txt");
        f << "POOL-START-INDICES (0-based into Literal Table)\n";
        for (size_t i=0;i<PT.size(); ++i)
            f << i << ": " << PT[i] << "\n";
    }

    cout << "PASS-1 COMPLETED\n"
         << "Generated: intermediate.txt, symbol_table.txt, literal_table.txt, pool_table.txt\n";
    return 0;
}
