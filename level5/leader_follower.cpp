#include "leader_follower.hpp"
#include <iostream>
#include <stdexcept>

LeaderFollowerPool::LeaderFollowerPool(size_t num_threads)
    : running(true)
    , leader_count(0)
    , thread_count(num_threads) {
    
    if (num_threads == 0) {
        throw std::invalid_argument("Thread count must be greater than 0");
    }

    std::cout << "[DEBUG] Creating thread pool with " << num_threads << " threads.\n";

    // Create worker threads
    for (size_t i = 0; i < num_threads; ++i) {
        auto worker = std::make_unique<Worker>();
        worker->thread = std::thread([this, w = worker.get()]() {
            worker_loop(w);
        });
        workers.push_back(std::move(worker));
    }

    std::cout << "[DEBUG] Promoting initial leader...\n";
    promote_new_leader();
}

LeaderFollowerPool::~LeaderFollowerPool() {
    shutdown();
}

void LeaderFollowerPool::shutdown() {
    {
        std::lock_guard<std::mutex> lock(pool_mutex);
        running = false;
    }
    
    pool_cv.notify_all();
    for (auto& worker : workers) {
        worker->cv.notify_one();
    }

    for (auto& worker : workers) {
        if (worker->thread.joinable()) {
            worker->thread.join();
        }
    }
}

void LeaderFollowerPool::submit(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(pool_mutex);
        tasks.push(std::move(task));
        std::cout << "[DEBUG] Task added to queue. Current size: " << tasks.size() << "\n";
    }
    pool_cv.notify_one();
}

void LeaderFollowerPool::promote_new_leader() {
    std::lock_guard<std::mutex> lock(pool_mutex);
    for (auto& worker : workers) {
        if (!worker->is_leader && !worker->processing) {
            worker->is_leader = true;
            leader_count++;
            worker->cv.notify_one();
            std::cout << "[DEBUG] New leader promoted.\n";
            return;
        }
    }
    std::cout << "[WARNING] No available worker to promote as leader!\n";
}

void LeaderFollowerPool::worker_loop(Worker* worker) {
    while (running) {
        std::unique_lock<std::mutex> lock(pool_mutex);

        std::cout << "[DEBUG] Worker waiting to become leader...\n";
        worker->cv.wait(lock, [this, worker]() { return worker->is_leader || !running; });

        if (!running) {
            std::cout << "[DEBUG] Worker shutting down.\n";
            return;
        }

        // Check if there are tasks available
        if (tasks.empty()) {
            std::cout << "[DEBUG] No tasks available, promoting new leader.\n";
            worker->is_leader = false;
            promote_new_leader();
            continue;
        }

        std::cout << "[DEBUG] Worker processing task.\n";
        auto task = std::move(tasks.front());
        tasks.pop();
        worker->is_leader = false;
        leader_count--;
        promote_new_leader();

        lock.unlock();

        try {
            task();
            std::cout << "[DEBUG] Task executed successfully.\n";
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Task execution failed: " << e.what() << "\n";
        }

        lock.lock();
        worker->processing = false;
    }
}

size_t LeaderFollowerPool::get_pending_tasks() const {
    std::lock_guard<std::mutex> lock(pool_mutex);
    return tasks.size();
}

bool LeaderFollowerPool::has_active_tasks() const {
    std::lock_guard<std::mutex> lock(pool_mutex);
    for (const auto& worker : workers) {
        if (worker->processing) {
            return true;
        }
    }
    return !tasks.empty();
}
