// sync_classics.cpp
// C++17 implementation of classical synchronization problems using
// mutexes and (userland) semaphores.
//
// Build:
//   g++ -std=c++17 -O2 -pthread sync_classics.cpp -o sync_classics
//
// Run:
//   ./sync_classics prodcon
//   ./sync_classics rw
//   ./sync_classics phil

#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

// ---------- Simple semaphore (C++17) ----------
class Semaphore {
    mutex m;
    condition_variable cv;
    ptrdiff_t count;
public:
    explicit Semaphore(ptrdiff_t initial = 0) : count(initial) {}
    void acquire() {
        unique_lock<mutex> lk(m);
        cv.wait(lk, [&]{ return count > 0; });
        --count;
    }
    void release(ptrdiff_t n = 1) {
        {
            lock_guard<mutex> lk(m);
            count += n;
        }
        cv.notify_all();
    }
};

// ---------- Utils ----------
static void msleep(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}
static int rnd(int a, int b) {
    static thread_local mt19937 rng{random_device{}()};
    uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}

// ============================================================
// 1) Bounded Producer–Consumer (mutex + semaphores)
// ============================================================
namespace PC {
constexpr int BUF_SIZE = 8;
constexpr int PRODUCERS = 3;
constexpr int CONSUMERS = 3;
constexpr int ITEMS_PER_PROD = 6;

int buf[BUF_SIZE];
int inIdx = 0, outIdx = 0;
mutex buf_mtx;
Semaphore empty_slots(BUF_SIZE);
Semaphore full_slots(0);

void producer(int id) {
    for (int k = 0; k < ITEMS_PER_PROD; ++k) {
        int item = id * 100 + k;
        empty_slots.acquire();
        {
            lock_guard<mutex> lk(buf_mtx);
            buf[inIdx] = item;
            inIdx = (inIdx + 1) % BUF_SIZE;
            cout << "[PROD " << id << "] -> " << item << "\n";
        }
        full_slots.release();
        msleep(rnd(40, 120));
    }
}

void consumer(int id) {
    int total = PRODUCERS * ITEMS_PER_PROD / CONSUMERS;
    for (int k = 0; k < total; ++k) {
        full_slots.acquire();
        int item;
        {
            lock_guard<mutex> lk(buf_mtx);
            item = buf[outIdx];
            outIdx = (outIdx + 1) % BUF_SIZE;
            cout << "          [CONS " << id << "] <- " << item << "\n";
        }
        empty_slots.release();
        msleep(rnd(60, 160));
    }
}

void run() {
    vector<thread> tp, tc;
    for (int i = 0; i < PRODUCERS; ++i) tp.emplace_back(producer, i);
    for (int i = 0; i < CONSUMERS; ++i) tc.emplace_back(consumer, i);
    for (auto& t : tp) t.join();
    for (auto& t : tc) t.join();
}
} // namespace PC

// ============================================================
// 2) Readers–Writers (readers-preference)
//    read_count protected by mutex; rw_mutex as semaphore
// ============================================================
namespace RW {
constexpr int READERS = 5;
constexpr int WRITERS = 3;
constexpr int ROUNDS  = 4;

int shared_data = 0;
int read_count = 0;
mutex rc_mtx;         // protects read_count
Semaphore rw_mutex(1); // writers lock / shared readers

void reader(int id) {
    for (int i = 0; i < ROUNDS; ++i) {
        {
            lock_guard<mutex> lk(rc_mtx);
            ++read_count;
            if (read_count == 1) rw_mutex.acquire(); // first reader blocks writers
        }

        cout << "[R" << id << "] reading value = " << shared_data << "\n";
        msleep(rnd(40, 100));

        {
            lock_guard<mutex> lk(rc_mtx);
            --read_count;
            if (read_count == 0) rw_mutex.release(); // last reader unblocks writers
        }
        msleep(rnd(40, 120));
    }
}

void writer(int id) {
    for (int i = 0; i < ROUNDS; ++i) {
        rw_mutex.acquire();
        int newv = shared_data + 1;
        cout << "    [W" << id << "] writing value = " << newv << "\n";
        shared_data = newv;
        msleep(rnd(60, 140));
        rw_mutex.release();
        msleep(rnd(60, 140));
    }
}

void run() {
    vector<thread> r, w;
    for (int i = 0; i < READERS; ++i) r.emplace_back(reader, i);
    for (int i = 0; i < WRITERS; ++i) w.emplace_back(writer, i);
    for (auto& t : r) t.join();
    for (auto& t : w) t.join();
    cout << "Final shared_data = " << shared_data << "\n";
}
} // namespace RW

// ============================================================
// 3) Dining Philosophers (avoid deadlock with N-1 limiter)
// ============================================================
namespace DP {
constexpr int N = 5;
constexpr int MEALS = 4;

mutex forks[N];
Semaphore table_limit(N - 1); // allow at most N-1 philosophers to pick forks

void philosopher(int id) {
    int left = id;
    int right = (id + 1) % N;

    for (int m = 0; m < MEALS; ++m) {
        cout << "[P" << id << "] thinking\n";
        msleep(rnd(60, 140));

        table_limit.acquire();

        // pick forks
        unique_lock<mutex> lkL(forks[left]);
        unique_lock<mutex> lkR(forks[right]);

        cout << "    [P" << id << "] eating (" << (m + 1) << "/" << MEALS << ")\n";
        msleep(rnd(60, 140));

        // release forks
        lkR.unlock();
        lkL.unlock();

        table_limit.release();
    }
}

void run() {
    vector<thread> t;
    for (int i = 0; i < N; ++i) t.emplace_back(philosopher, i);
    for (auto& th : t) th.join();
}
} // namespace DP

// ============================================================
// Main
// ============================================================
int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc != 2) {
        cerr <<
        "Usage: " << argv[0] << " <mode>\n"
        "  modes:\n"
        "    prodcon  - Producer-Consumer (bounded buffer)\n"
        "    rw       - Readers-Writers (readers preference)\n"
        "    phil     - Dining Philosophers (N-1 limit)\n";
        return 1;
    }

    string mode = argv[1];
    if (mode == "prodcon")      PC::run();
    else if (mode == "rw")      RW::run();
    else if (mode == "phil")    DP::run();
    else {
        cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }
    return 0;
}
