#include "macroprocessor.cpp"

// ===== Helpers =====
string trim(const string& s) {
    size_t i = s.find_first_not_of(" \t\r\n");
    if (i == string::npos) return "";
    size_t j = s.find_last_not_of(" \t\r\n");
    return s.substr(i, j - i + 1);
}

bool is_comment_or_blank(const string& line) {
    string t = trim(line);
    return t.empty() || t[0] == ';' || t[0] == '*';
}

pair<string, string> head_and_rest(const string& line) {
    string t = trim(line);
    if (t.empty()) return {"", ""};
    size_t sp = t.find_first_of(" \t");
    if (sp == string::npos) return {t, ""};
    return {t.substr(0, sp), trim(t.substr(sp + 1))};
}

vector<string> split_params(const string& s) {
    vector<string> out;
    string token;
    string cur;
    for (char c : s) {
        if (c == ',') {
            token = trim(cur);
            if (!token.empty()) out.push_back(token);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    token = trim(cur);
    if (!token.empty()) out.push_back(token);
    return out;
}

void replace_all(string& text, const string& from, const string& to) {
    if (from.empty()) return;
    size_t pos = 0;
    while ((pos = text.find(from, pos)) != string::npos) {
        text.replace(pos, from.size(), to);
        pos += to.size();
    }
}

static void parse_macro_header(const string& headerLine, string& macroName, vector<string>& params) {
    auto [head, rest] = head_and_rest(headerLine);
    macroName = head;
    params = split_params(rest);
    for (string& p : params) {
        size_t eq = p.find('=');
        if (eq != string::npos) p = trim(p.substr(0, eq));
    }
}

// ===== Pass-I =====
Pass1Output pass1_build_tables_and_intermediate(
    const string& sourcePath,
    const string& mntPath,
    const string& mdtPath,
    const string& intermediatePath
) {
    ifstream fin(sourcePath);
    if (!fin) { cerr << "Error: cannot open " << sourcePath << "\n"; exit(1); }

    ofstream fout(intermediatePath);
    if (!fout) { cerr << "Error: cannot create " << intermediatePath << "\n"; exit(1); }

    Pass1Output out;
    bool inMacro = false;
    string line, macroName;
    vector<string> params;

    while (getline(fin, line)) {
        string t = trim(line);

        if (!inMacro && t == "MACRO") {
            inMacro = true;
            if (!getline(fin, line)) { cerr << "Error: EOF after MACRO\n"; break; }
            parse_macro_header(line, macroName, params);
            out.MNT.push_back(MNTEntry{macroName, (int)out.MDT.size(), (int)params.size()});
            continue;
        }

        if (inMacro) {
            if (t == "MEND") {
                out.MDT.push_back("MEND");
                inMacro = false;
                continue;
            }
            string body = line;
            for (size_t i = 0; i < params.size(); ++i) {
                string key = params[i];
                if (!key.empty() && key[0] != '&') key = "&" + key;
                replace_all(body, key, "#" + to_string(i + 1));
            }
            out.MDT.push_back(body);
            continue;
        }

        // Outside macro: write as-is to intermediate
        fout << line << "\n";
    }

    // Write MNT
    {
        ofstream fmnt(mntPath);
        if (!fmnt) { cerr << "Error: cannot create " << mntPath << "\n"; exit(1); }
        for (const auto& e : out.MNT)
            fmnt << e.name << " " << e.mdtIndex << " " << e.paramCount << "\n";
    }

    // Write MDT
    {
        ofstream fmdt(mdtPath);
        if (!fmdt) { cerr << "Error: cannot create " << mdtPath << "\n"; exit(1); }
        for (const auto& l : out.MDT) fmdt << l << "\n";
    }

    return out;
}
