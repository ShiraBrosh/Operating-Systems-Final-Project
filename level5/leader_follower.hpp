// leader_follower.hpp
#ifndef LEADER_FOLLOWER_HPP
#define LEADER_FOLLOWER_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <atomic>
#include <iostream>

class LeaderFollowerPool {
private:
    struct Worker {
        std::thread thread;
        bool is_leader;
        std::condition_variable cv;
        std::mutex mutex;
        bool processing;
        
        Worker() : is_leader(false), processing(false) {}
    };

    std::vector<std::unique_ptr<Worker>> workers;
    std::queue<std::function<void()>> tasks;
    mutable std::mutex pool_mutex;
    std::condition_variable pool_cv;
    std::atomic<bool> running;
    std::atomic<int> leader_count;
    size_t thread_count;

    void worker_loop(Worker* worker);
    void promote_new_leader();

public:
    explicit LeaderFollowerPool(size_t num_threads);
    ~LeaderFollowerPool();

    // Delete copy constructor and assignment operator
    LeaderFollowerPool(const LeaderFollowerPool&) = delete;
    LeaderFollowerPool& operator=(const LeaderFollowerPool&) = delete;

    void submit(std::function<void()> task);
    size_t get_thread_count() const { return thread_count; }
    size_t get_pending_tasks() const;
    bool has_active_tasks() const;
    void shutdown();
};

#endif // LEADER_FOLLOWER_HPP
