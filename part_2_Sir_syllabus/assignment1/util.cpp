#include "pass1.hpp"
#include <cctype>
#include<bits/stdc++.h>
using namespace std;

string trim(const string& s){
    size_t i = s.find_first_not_of(" \t\r\n");
    if (i==string::npos) return "";
    size_t j = s.find_last_not_of(" \t\r\n");
    return s.substr(i, j-i+1);
}

vector<string> split_ws_commas(const string& line){
    vector<string> out; string tok;
    for(char c: line){
        if (c==',' || isspace((unsigned char)c)){
            if (!tok.empty()){ out.push_back(tok); tok.clear(); }
        } else tok.push_back(c);
    }
    if (!tok.empty()) out.push_back(tok);
    return out;
}

bool is_number(const string& s){
    if(s.empty()) return false;
    size_t i=0; if(s[0]=='+'||s[0]=='-') i=1;
    for(; i<s.size(); ++i) if(!isdigit((unsigned char)s[i])) return false;
    return true;
}

bool is_literal(const string& s){
    return s.size()>=2 && s[0]=='=';
}

int literal_value_of(const string& lit){
    if (lit.size()<2) return 0;
    string v = lit.substr(1);
    if (v.size()>=2 && ((v.front()=='\'' && v.back()=='\'') || (v.front()=='"' && v.back()=='"')))
        v = v.substr(1, v.size()-2);
    if (is_number(v)) return stoi(v);
    if (v.size()==1) return (int)(unsigned char)v[0];
    return 0;
}

// Evaluate SYMBOL / SYMBOL±K / K (K numeric)
int eval_expr(const string& expr, const map<string, Sym>& ST){
    auto posp = expr.find('+');
    auto posm = expr.find('-');
    if (posp==string::npos && posm==string::npos){
        if (is_number(expr)) return stoi(expr);
        auto it=ST.find(expr);
        if (it!=ST.end() && it->second.address!=-1) return it->second.address;
        return 0;
    }
    auto parseTerm = [&](const string& t)->int{
        if (is_number(t)) return stoi(t);
        auto it=ST.find(t);
        return (it!=ST.end()? it->second.address : 0);
    };
    if (posp!=string::npos) return parseTerm(expr.substr(0,posp)) + parseTerm(expr.substr(posp+1));
    return parseTerm(expr.substr(0,posm)) - parseTerm(expr.substr(posm+1));
}
