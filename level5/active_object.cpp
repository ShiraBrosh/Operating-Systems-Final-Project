#include "active_object.hpp"
#include <iostream>

// Since most implementation is in the header file as it's a template class,
// we'll implement some utility functions and error handling here

class ActiveObjectException : public std::runtime_error {
public:
    explicit ActiveObjectException(const std::string& message)
        : std::runtime_error(message) {}
};

void handleActiveObjectError(const std::string& error_message) {
    throw ActiveObjectException(error_message);
}

// Helper function to check if a task is valid
bool isTaskValid(const std::function<void()>& task) {
    return task != nullptr;
}

// Helper function to log active object events (if logging is enabled)
void logActiveObjectEvent(const std::string& event) {
    std::cout << "ActiveObject: " << event << std::endl;
}

// Error checking wrapper for task execution
template<typename T>
T executeTaskSafely(const std::function<T()>& task, const std::string& taskName) {
    try {
        if (!isTaskValid(task)) {
            handleActiveObjectError("Invalid task: " + taskName);
        }
        return task();
    } catch (const std::exception& e) {
        handleActiveObjectError("Task execution failed: " + std::string(e.what()));
    }
    // This return is just to satisfy the compiler, 
    // the function will throw before reaching here
    return T();
}

// Additional utility functions that can be used with ActiveObject
namespace active_object_utils {
    
    // Function to create a delayed task
    template<typename T>
    std::function<T()> createDelayedTask(
        std::function<T()> task, 
        std::chrono::milliseconds delay
    ) {
        return [task, delay]() {
            std::this_thread::sleep_for(delay);
            return task();
        };
    }

    // Function to create a task with timeout
    template<typename T>
    std::function<T()> createTaskWithTimeout(
        std::function<T()> task, 
        std::chrono::milliseconds timeout
    ) {
        return [task, timeout]() {
            auto future = std::async(std::launch::async, task);
            auto status = future.wait_for(timeout);
            
            if (status != std::future_status::ready) {
                handleActiveObjectError("Task timed out");
            }
            
            return future.get();
        };
    }

    // Function to create a retrying task
    template<typename T>
    std::function<T()> createRetryingTask(
        std::function<T()> task, 
        int maxRetries
    ) {
        return [task, maxRetries]() {
            int retries = 0;
            while (retries < maxRetries) {
                try {
                    return task();
                } catch (const std::exception& e) {
                    retries++;
                    if (retries >= maxRetries) {
                        throw;
                    }
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(100 * retries)
                    );
                }
            }
            throw ActiveObjectException("Max retries exceeded");
        };
    }
}