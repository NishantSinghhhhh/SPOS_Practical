#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <random>

class Semaphore {
    std::mutex m;
    std::condition_variable cv;
    ptrdiff_t count;
public:
    explicit Semaphore(ptrdiff_t initial = 0) : count(initial) {}
    void acquire() {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&]{ return count > 0; });
        --count;
    }
    void release(ptrdiff_t n = 1) {
        {
            std::lock_guard<std::mutex> lk(m);
            count += n;
        }
        cv.notify_all();
    }
};

// small helpers shared by problems.cpp
inline void msleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
inline int rnd(int a, int b) {
    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}

#endif // SEMAPHORE_HPP
