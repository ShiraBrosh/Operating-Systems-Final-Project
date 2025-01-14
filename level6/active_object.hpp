// active_object.hpp
#ifndef ACTIVE_OBJECT_HPP
#define ACTIVE_OBJECT_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <memory>
#include <future>

// Template class for processing tasks asynchronously
template<typename T>
class ActiveObject {
private:
    bool running;
    std::queue<std::function<void()>> message_queue;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::thread worker_thread;

    // Worker thread function
    void run() {
        while (running) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                condition.wait(lock, [this] { 
                    return !message_queue.empty() || !running; 
                });
                
                if (!running && message_queue.empty()) {
                    return;
                }
                
                task = std::move(message_queue.front());
                message_queue.pop();
            }
            // Execute the task
            task();
        }
    }

public:
    // Constructor - starts the worker thread
    ActiveObject() : running(true) {
        worker_thread = std::thread(&ActiveObject::run, this);
    }

    // Destructor - ensures clean shutdown
    ~ActiveObject() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            running = false;
        }
        condition.notify_one();
        if (worker_thread.joinable()) {
            worker_thread.join();
        }
    }

    // Enqueues a task and returns a future
    template<typename F>
    std::future<T> enqueue(F&& f) {
        auto task = std::make_shared<std::packaged_task<T()>>(std::forward<F>(f));
        auto future = task->get_future();
        
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            message_queue.emplace([task]() { 
                (*task)(); 
            });
        }
        condition.notify_one();
        return future;
    }

    // Delete copy constructor and assignment operator
    ActiveObject(const ActiveObject&) = delete;
    ActiveObject& operator=(const ActiveObject&) = delete;
};

#endif // ACTIVE_OBJECT_HPP