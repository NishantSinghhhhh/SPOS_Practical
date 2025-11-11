#include "macroprocessor.cpp"

static void load_mnt_mdt(const string& mntPath, const string& mdtPath,
                         vector<MNTEntry>& MNT, vector<string>& MDT) {
    ifstream fmnt(mntPath);
    ifstream fmdt(mdtPath);
    if (!fmnt || !fmdt) { cerr << "Error: cannot open MNT/MDT files\n"; exit(1); }

    MNT.clear(); MDT.clear();
    string name; int idx, pc;
    while (fmnt >> name >> idx >> pc) MNT.push_back({name, idx, pc});

    string line;
    while (getline(fmdt, line)) MDT.push_back(line);
}

static int find_macro(const vector<MNTEntry>& MNT, const string& name) {
    for (int i = 0; i < (int)MNT.size(); ++i)
        if (MNT[i].name == name) return i;
    return -1;
}

static void expand_macro(const vector<string>& MDT, int startIndex,
                         const vector<string>& actuals, ostream& out) {
    for (int i = startIndex; i < (int)MDT.size(); ++i) {
        string l = MDT[i];
        if (trim(l) == "MEND") break;
        for (int p = 0; p < (int)actuals.size(); ++p)
            replace_all(l, "#" + to_string(p + 1), actuals[p]);
        out << l << "\n";
    }
}

void pass2_expand(const string& intermediatePath,
                  const string& mntPath,
                  const string& mdtPath,
                  const string& expandedPath) {
    vector<MNTEntry> MNT;
    vector<string> MDT;
    load_mnt_mdt(mntPath, mdtPath, MNT, MDT);

    ifstream fin(intermediatePath);
    if (!fin) { cerr << "Error: cannot open " << intermediatePath << "\n"; exit(1); }
    ofstream fout(expandedPath);
    if (!fout) { cerr << "Error: cannot create " << expandedPath << "\n"; exit(1); }

    string line;
    while (getline(fin, line)) {
        string t = trim(line);
        if (is_comment_or_blank(t)) {
            fout << line << "\n";
            continue;
        }
        auto [head, rest] = head_and_rest(t);
        int mi = find_macro(MNT, head);
        if (mi < 0) {
            fout << line << "\n";
            continue;
        }
        vector<string> actuals = split_params(rest);
        expand_macro(MDT, MNT[mi].mdtIndex, actuals, fout);
    }
}
