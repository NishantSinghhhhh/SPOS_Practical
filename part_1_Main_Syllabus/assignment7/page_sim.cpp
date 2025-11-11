#include <bits/stdc++.h>
using namespace std;

struct Step {
    vector<int> frames; 
    bool hit;
    int ref;
};

static void print_run(const string& name, int framesCount, const vector<int>& refs, const vector<Step>& steps) {
    cout << "Algorithm: " << name << "\n";
    cout << "Frames: " << framesCount << " | References: " << refs.size() << "\n\n";

    // header
    cout << left << setw(8) << "Ref";
    for (int f=0; f<framesCount; ++f) cout << "F" << f << setw(5) << "";
    cout << "Result\n";

    int hits=0, faults=0;
    for (const auto& st : steps) {
        cout << left << setw(8) << st.ref;
        for (int x : st.frames) {
            if (x == -1) cout << setw(6) << "-";
            else         cout << setw(6) << x;
        }
        if (st.hit) { cout << "HIT\n"; hits++; }
        else        { cout << "FAULT\n"; faults++; }
    }
    cout << "\nTotal Hits: " << hits
         << " | Total Faults: " << faults
         << " | Hit Ratio: "  << fixed << setprecision(2) << (hits ? (double)hits/steps.size() : 0.0)
         << " | Fault Ratio: " << fixed << setprecision(2) << (faults ? (double)faults/steps.size() : 0.0)
         << "\n\n";
}

/* ---------------- FIFO ---------------- */
static vector<Step> simulate_fifo(int framesCount, const vector<int>& refs) {
    vector<int> frames(framesCount, -1);
    queue<int> q; // holds indices of frames in FIFO order
    vector<Step> steps;

    for (int r : refs) {
        bool hit=false;
        for (int x : frames) if (x == r) { hit=true; break; }

        if (!hit) {
            // empty frame?
            int empty = -1;
            for (int i=0;i<framesCount;i++) if (frames[i]==-1) { empty=i; break; }
            if (empty != -1) {
                frames[empty] = r;
                q.push(empty);
            } else {
                int victim = q.front(); q.pop();
                frames[victim] = r;
                q.push(victim);
            }
        }
        steps.push_back({frames, hit, r});
    }
    return steps;
}

/* ---------------- LRU ---------------- */
static vector<Step> simulate_lru(int framesCount, const vector<int>& refs) {
    vector<int> frames(framesCount, -1);
    vector<int> lastUsed(framesCount, -1); // time last used
    vector<Step> steps;
    int time=0;

    for (int r : refs) {
        bool hit=false;
        int pos=-1;
        for (int i=0;i<framesCount;i++) if (frames[i]==r) { hit=true; pos=i; break; }

        if (hit) {
            lastUsed[pos]=time;
        } else {
            // find empty or least recently used
            int empty=-1;
            for (int i=0;i<framesCount;i++) if (frames[i]==-1) { empty=i; break; }
            if (empty!=-1) {
                frames[empty]=r;
                lastUsed[empty]=time;
            } else {
                // choose min lastUsed
                int victim=0;
                for (int i=1;i<framesCount;i++) if (lastUsed[i]<lastUsed[victim]) victim=i;
                frames[victim]=r;
                lastUsed[victim]=time;
            }
        }
        steps.push_back({frames, hit, r});
        time++;
    }
    return steps;
}

/* ---------------- OPT (Optimal) ---------------- */
static vector<Step> simulate_opt(int framesCount, const vector<int>& refs) {
    vector<int> frames(framesCount, -1);
    vector<Step> steps;

    for (int t=0;t<(int)refs.size();++t) {
        int r = refs[t];
        bool hit=false; int pos=-1;
        for (int i=0;i<framesCount;i++) if (frames[i]==r) { hit=true; pos=i; break; }

        if (!hit) {
            int empty=-1;
            for (int i=0;i<framesCount;i++) if (frames[i]==-1) { empty=i; break; }
            if (empty!=-1) {
                frames[empty]=r;
            } else {
                // choose victim farthest in future (or never used again)
                int victim=-1, farthest=-1;
                for (int i=0;i<framesCount;i++) {
                    int page = frames[i];
                    int nextUse = INT_MAX;
                    for (int k=t+1;k<(int)refs.size();++k) if (refs[k]==page) { nextUse=k; break; }
                    if (nextUse == INT_MAX) { victim=i; break; } // never used again
                    if (nextUse > farthest) { farthest=nextUse; victim=i; }
                }
                frames[victim]=r;
            }
        }
        steps.push_back({frames, hit, r});
    }
    return steps;
}

/* ---------------- CLOCK ---------------- */
static vector<Step> simulate_clock(int framesCount, const vector<int>& refs) {
    vector<int> frames(framesCount, -1);
    vector<int> refbit(framesCount, 0);
    int hand=0;
    vector<Step> steps;

    for (int r : refs) {
        bool hit=false;
        for (int i=0;i<framesCount;i++) if (frames[i]==r) { hit=true; refbit[i]=1; break; }

        if (!hit) {
            // find victim with refbit=0, clearing 1's along the way
            while (true) {
                if (frames[hand]==-1) {
                    frames[hand]=r; refbit[hand]=1; hand=(hand+1)%framesCount; break;
                }
                if (refbit[hand]==0) {
                    frames[hand]=r; refbit[hand]=1; hand=(hand+1)%framesCount; break;
                } else {
                    refbit[hand]=0;
                    hand=(hand+1)%framesCount;
                }
            }
        }
        steps.push_back({frames, hit, r});
    }
    return steps;
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <fifo|lru|opt|clock>\n";
        return 1;
    }
    string mode = argv[1];

    int F,N;
    if (!(cin >> F)) { cerr << "Failed to read frames\n"; return 1; }
    if (!(cin >> N)) { cerr << "Failed to read length\n"; return 1; }
    vector<int> refs(N);
    for (int i=0;i<N;i++) cin >> refs[i];

    vector<Step> steps;
    if (mode=="fifo")      steps = simulate_fifo(F, refs);
    else if (mode=="lru")  steps = simulate_lru(F, refs);
    else if (mode=="opt")  steps = simulate_opt(F, refs);
    else if (mode=="clock")steps = simulate_clock(F, refs);
    else { cerr << "Unknown mode\n"; return 1; }

    print_run(mode, F, refs, steps);
    return 0;
}
