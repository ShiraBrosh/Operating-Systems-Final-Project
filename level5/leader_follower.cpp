// leader_follower.cpp
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

    for (size_t i = 0; i < num_threads; ++i) {
        auto worker = std::make_unique<Worker>();
        worker->thread = std::thread([this, w = worker.get()]() {
            worker_loop(w);
        });
        workers.push_back(std::move(worker));
    }

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

    std::queue<std::function<void()>> empty;
    std::swap(tasks, empty);
}

void LeaderFollowerPool::submit(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(pool_mutex);
        if (!running) {
            throw std::runtime_error("Thread pool is shutting down");
        }
        tasks.push(std::move(task));
    }
    pool_cv.notify_one();
}

void LeaderFollowerPool::promote_new_leader() {
    for (auto& worker : workers) {
        if (!worker->is_leader && !worker->processing) {
            worker->is_leader = true;
            leader_count++;
            worker->cv.notify_one();
            return;
        }
    }
}

void LeaderFollowerPool::worker_loop(Worker* worker) {
    while (running) {
        std::unique_lock<std::mutex> lock(pool_mutex);
        
        while (!worker->is_leader && running) {
            worker->cv.wait(lock);
        }

        if (!running) return;
        
        while (tasks.empty() && running) {
            pool_cv.wait(lock);
        }

        if (!running) return;

        auto task = std::move(tasks.front());
        tasks.pop();

        worker->is_leader = false;
        worker->processing = true;
        leader_count--;
        promote_new_leader();

        lock.unlock();
        try {
            task();
        } catch (const std::exception& e) {
            std::cerr << "Error in task execution: " << e.what() << std::endl;
        }

        lock.lock();
        worker->processing = false;

        if (leader_count == 0) {
            worker->is_leader = true;
            leader_count++;
        }
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