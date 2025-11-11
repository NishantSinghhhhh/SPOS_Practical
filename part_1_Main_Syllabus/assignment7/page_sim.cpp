#include <bits/stdc++.h>
using namespace std;

/* 
 * Each step in the page replacement process is represented by this struct.
 * - frames: the current state of the frame buffer (pages loaded in memory)
 * - hit: true if the current reference caused a page hit
 * - ref: the page number being referenced in this step
 */
struct Step {
    vector<int> frames; 
    bool hit;
    int ref;
};

/*
 * Utility function to print the simulation result.
 * Displays:
 *  - Algorithm name
 *  - Number of frames
 *  - Number of references
 *  - Detailed step-by-step state of frames
 *  - Total hits, total faults, hit ratio, fault ratio
 */
static void print_run(const string& name, int framesCount, const vector<int>& refs, const vector<Step>& steps) {
    cout << "Algorithm: " << name << "\n";
    cout << "Frames: " << framesCount << " | References: " << refs.size() << "\n\n";

    // Print table header
    cout << left << setw(8) << "Ref";
    for (int f=0; f<framesCount; ++f) cout << "F" << f << setw(5) << "";
    cout << "Result\n";

    int hits=0, faults=0;

    // Print each step in the simulation
    for (const auto& st : steps) {
        cout << left << setw(8) << st.ref;
        for (int x : st.frames) {
            if (x == -1) cout << setw(6) << "-"; // empty frame
            else         cout << setw(6) << x;
        }
        if (st.hit) { cout << "HIT\n"; hits++; }
        else        { cout << "FAULT\n"; faults++; }
    }

    // Print final summary statistics
    cout << "\nTotal Hits: " << hits
         << " | Total Faults: " << faults
         << " | Hit Ratio: "  << fixed << setprecision(2) << (hits ? (double)hits/steps.size() : 0.0)
         << " | Fault Ratio: " << fixed << setprecision(2) << (faults ? (double)faults/steps.size() : 0.0)
         << "\n\n";
}

/* ---------------- FIFO (First-In-First-Out) ----------------
 * The oldest loaded page is replaced first.
 * Uses a queue to track the order in which pages were inserted.
 */
static vector<Step> simulate_fifo(int framesCount, const vector<int>& refs) {
    vector<int> frames(framesCount, -1); // initialize all frames as empty
    queue<int> q; // holds indices of frames in the order they were filled
    vector<Step> steps;

    for (int r : refs) {
        bool hit=false;

        // Check for page hit
        for (int x : frames) if (x == r) { hit=true; break; }

        if (!hit) {
            // If an empty frame is available, use it
            int empty = -1;
            for (int i=0;i<framesCount;i++) if (frames[i]==-1) { empty=i; break; }

            if (empty != -1) {
                frames[empty] = r;
                q.push(empty);
            } 
            // Otherwise, replace the oldest frame (front of queue)
            else {
                int victim = q.front(); q.pop();
                frames[victim] = r;
                q.push(victim);
            }
        }

        // Store current state
        steps.push_back({frames, hit, r});
    }
    return steps;
}

/* ---------------- LRU (Least Recently Used) ----------------
 * The page that was least recently accessed is replaced.
 * Uses a 'lastUsed' vector to track when each frame was last used.
 */
static vector<Step> simulate_lru(int framesCount, const vector<int>& refs) {
    vector<int> frames(framesCount, -1);
    vector<int> lastUsed(framesCount, -1); // tracks time since last use
    vector<Step> steps;
    int time=0;

    for (int r : refs) {
        bool hit=false;
        int pos=-1;

        // Check if page is already in a frame
        for (int i=0;i<framesCount;i++) if (frames[i]==r) { hit=true; pos=i; break; }

        if (hit) {
            // Update last used time
            lastUsed[pos]=time;
        } else {
            // Find an empty frame or the least recently used frame
            int empty=-1;
            for (int i=0;i<framesCount;i++) if (frames[i]==-1) { empty=i; break; }

            if (empty!=-1) {
                frames[empty]=r;
                lastUsed[empty]=time;
            } else {
                // Choose frame with minimum lastUsed value (oldest use)
                int victim=0;
                for (int i=1;i<framesCount;i++) if (lastUsed[i]<lastUsed[victim]) victim=i;
                frames[victim]=r;
                lastUsed[victim]=time;
            }
        }
        // Record current step
        steps.push_back({frames, hit, r});
        time++;
    }
    return steps;
}

/* ---------------- OPT (Optimal Page Replacement) ----------------
 * Replaces the page that will not be used for the longest period of time in the future.
 * This algorithm gives the minimal number of page faults but is theoretical (requires future knowledge).
 */
static vector<Step> simulate_opt(int framesCount, const vector<int>& refs) {
    vector<int> frames(framesCount, -1);
    vector<Step> steps;

    for (int t=0;t<(int)refs.size();++t) {
        int r = refs[t];
        bool hit=false; 
        int pos=-1;

        // Check for page hit
        for (int i=0;i<framesCount;i++) if (frames[i]==r) { hit=true; pos=i; break; }

        if (!hit) {
            // Find empty frame
            int empty=-1;
            for (int i=0;i<framesCount;i++) if (frames[i]==-1) { empty=i; break; }

            if (empty!=-1) {
                frames[empty]=r;
            } else {
                // Choose the frame whose page will be used farthest in the future (or never again)
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

/* ---------------- CLOCK Algorithm ----------------
 * A practical approximation of LRU.
 * Uses a circular buffer (hand) and a reference bit for each frame.
 * When a page fault occurs, the algorithm checks the reference bits:
 *   - If refbit == 1, set it to 0 and move the hand forward.
 *   - If refbit == 0, replace that page.
 */
static vector<Step> simulate_clock(int framesCount, const vector<int>& refs) {
    vector<int> frames(framesCount, -1); // store pages
    vector<int> refbit(framesCount, 0);  // reference bits (0 or 1)
    int hand=0;                          // points to the current frame
    vector<Step> steps;

    for (int r : refs) {
        bool hit=false;

        // Check for page hit and set reference bit if found
        for (int i=0;i<framesCount;i++) 
            if (frames[i]==r) { hit=true; refbit[i]=1; break; }

        if (!hit) {
            // Keep rotating the hand until a victim is found
            while (true) {
                // Empty frame found
                if (frames[hand]==-1) {
                    frames[hand]=r; 
                    refbit[hand]=1; 
                    hand=(hand+1)%framesCount; 
                    break;
                }
                // Victim found (refbit == 0)
                if (refbit[hand]==0) {
                    frames[hand]=r; 
                    refbit[hand]=1; 
                    hand=(hand+1)%framesCount; 
                    break;
                } 
                // Reset refbit and move forward
                else {
                    refbit[hand]=0;
                    hand=(hand+1)%framesCount;
                }
            }
        }

        // Record this simulation step
        steps.push_back({frames, hit, r});
    }
    return steps;
}

/* ---------------- MAIN FUNCTION ----------------
 * Entry point of the program.
 * Takes one command-line argument: algorithm name ("fifo", "lru", "opt", "clock")
 * Reads:
 *   - F (number of frames)
 *   - N (number of page references)
 *   - N integers (page reference sequence)
 * Then simulates the requested algorithm and prints the results.
 */
int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Check proper usage
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <fifo|lru|opt|clock>\n";
        return 1;
    }
    string mode = argv[1];

    int F,N;
    if (!(cin >> F)) { cerr << "Failed to read frames\n"; return 1; }
    if (!(cin >> N)) { cerr << "Failed to read length\n"; return 1; }

    // Read page references
    vector<int> refs(N);
    for (int i=0;i<N;i++) cin >> refs[i];

    // Run appropriate algorithm
    vector<Step> steps;
    if (mode=="fifo")      steps = simulate_fifo(F, refs);
    else if (mode=="lru")  steps = simulate_lru(F, refs);
    else if (mode=="opt")  steps = simulate_opt(F, refs);
    else if (mode=="clock")steps = simulate_clock(F, refs);
    else { cerr << "Unknown mode\n"; return 1; }

    // Print the simulation output
    print_run(mode, F, refs, steps);
    return 0;
}
