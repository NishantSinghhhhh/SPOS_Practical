#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "semaphore.cpp"

// ===================== 1) Producer–Consumer =====================
namespace PC {
constexpr int BUF_SIZE = 8;
constexpr int PRODUCERS = 3;
constexpr int CONSUMERS = 3;
constexpr int ITEMS_PER_PROD = 6;

static int buf[BUF_SIZE];
static int inIdx = 0, outIdx = 0;
static std::mutex buf_mtx;
static Semaphore empty_slots(BUF_SIZE);
static Semaphore full_slots(0);

static void producer(int id) {
    for (int k = 0; k < ITEMS_PER_PROD; ++k) {
        int item = id * 100 + k;
        empty_slots.acquire();
        {
            std::lock_guard<std::mutex> lk(buf_mtx);
            buf[inIdx] = item;
            inIdx = (inIdx + 1) % BUF_SIZE;
            std::cout << "[PROD " << id << "] -> " << item << "\n";
        }
        full_slots.release();
        msleep(rnd(40, 120));
    }
}

static void consumer(int id) {
    int total = PRODUCERS * ITEMS_PER_PROD / CONSUMERS;
    for (int k = 0; k < total; ++k) {
        full_slots.acquire();
        int item;
        {
            std::lock_guard<std::mutex> lk(buf_mtx);
            item = buf[outIdx];
            outIdx = (outIdx + 1) % BUF_SIZE;
            std::cout << "          [CONS " << id << "] <- " << item << "\n";
        }
        empty_slots.release();
        msleep(rnd(60, 160));
    }
}

void run() {
    std::vector<std::thread> tp, tc;
    for (int i = 0; i < PRODUCERS; ++i) tp.emplace_back(producer, i);
    for (int i = 0; i < CONSUMERS; ++i) tc.emplace_back(consumer, i);
    for (auto& t : tp) t.join();
    for (auto& t : tc) t.join();
}
} // namespace PC

// ===================== 2) Readers–Writers (readers pref) =====================
namespace RW {
constexpr int READERS = 5;
constexpr int WRITERS = 3;
constexpr int ROUNDS  = 4;

static int shared_data = 0;
static int read_count = 0;
static std::mutex rc_mtx;     // protects read_count
static Semaphore rw_mutex(1); // writers lock / shared readers

static void reader(int id) {
    for (int i = 0; i < ROUNDS; ++i) {
        {
            std::lock_guard<std::mutex> lk(rc_mtx);
            ++read_count;
            if (read_count == 1) rw_mutex.acquire(); // first reader blocks writers
        }

        std::cout << "[R" << id << "] reading value = " << shared_data << "\n";
        msleep(rnd(40, 100));

        {
            std::lock_guard<std::mutex> lk(rc_mtx);
            --read_count;
            if (read_count == 0) rw_mutex.release(); // last reader unblocks writers
        }
        msleep(rnd(40, 120));
    }
}

static void writer(int id) {
    for (int i = 0; i < ROUNDS; ++i) {
        rw_mutex.acquire();
        int newv = shared_data + 1;
        std::cout << "    [W" << id << "] writing value = " << newv << "\n";
        shared_data = newv;
        msleep(rnd(60, 140));
        rw_mutex.release();
        msleep(rnd(60, 140));
    }
}

void run() {
    std::vector<std::thread> r, w;
    for (int i = 0; i < READERS; ++i) r.emplace_back(reader, i);
    for (int i = 0; i < WRITERS; ++i) w.emplace_back(writer, i);
    for (auto& t : r) t.join();
    for (auto& t : w) t.join();
    std::cout << "Final shared_data = " << shared_data << "\n";
}
} // namespace RW

// ===================== 3) Dining Philosophers =====================
namespace DP {
constexpr int N = 5;
constexpr int MEALS = 4;

static std::mutex forks[N];
static Semaphore table_limit(N - 1); // allow at most N-1 philosophers

static void philosopher(int id) {
    int left = id;
    int right = (id + 1) % N;

    for (int m = 0; m < MEALS; ++m) {
        std::cout << "[P" << id << "] thinking\n";
        msleep(rnd(60, 140));

        table_limit.acquire();

        std::unique_lock<std::mutex> lkL(forks[left]);
        std::unique_lock<std::mutex> lkR(forks[right]);

        std::cout << "    [P" << id << "] eating (" << (m + 1) << "/" << MEALS << ")\n";
        msleep(rnd(60, 140));

        lkR.unlock();
        lkL.unlock();

        table_limit.release();
    }
}

void run() {
    std::vector<std::thread> t;
    for (int i = 0; i < N; ++i) t.emplace_back(philosopher, i);
    for (auto& th : t) th.join();
}
} // namespace DP

// expose simple dispatcher for main.cpp
// expose simple dispatcher for main.cpp
namespace Problems {
    void run_prodcon() { PC::run(); }
    void run_rw()      { RW::run(); }
    void run_phil()    { DP::run(); }
}
