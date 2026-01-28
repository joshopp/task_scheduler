// /src/task.h

#pragma once

#include <atomic>
#include <functional>
#include <cstdint>
#include <vector>
#include <mutex>

enum class TaskState {
    PENDING,
    RUNNING,
    COMPLETED
};

class Task {
private:
    uint64_t id_;
    std::atomic<TaskState> state_;
    std::function<void()> work_;

    std::atomic<int> pending_deps_{0};
    std::vector<Task*> dependents_;
    std::mutex deps_mutex_;

    std::function<void(Task*)> on_complete_callback_;

public:
    // disable copying
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    // Constructor
    Task(uint64_t id, std::function<void()> work):
        id_(id),
        state_(TaskState::PENDING),
        work_(std::move(work))
    {}

    // Getters
    uint64_t getId() const{
        return id_;
    }
    TaskState getState() const{
        return state_.load(std::memory_order_acquire);
    }

    // Setter (callback)
    void setOnCompleteCallback(std::function<void(Task*)> callback) {
    on_complete_callback_ = callback;
}

    // executes the task
    void execute() {
        state_.store(TaskState::RUNNING, std::memory_order_release);
        work_();
        state_.store(TaskState::COMPLETED, std::memory_order_release);
        onComplete();
    }

    // adds a task that depends on this task
    void addDependency(Task* dependency){
        pending_deps_.fetch_add(1, std::memory_order_relaxed);
        
        // Lock mutex of dependency and add this task to its dependents list
        std::lock_guard<std::mutex> lock(dependency->deps_mutex_);
        dependency->dependents_.emplace_back(this);  
    }

    // called on completion of this task
    void onComplete() {
        std::lock_guard<std::mutex> lock(deps_mutex_);

        for (Task* dependent : dependents_) {
            dependent->pending_deps_.fetch_sub(1, std::memory_order_relaxed);
        }

        if (on_complete_callback_) {
            on_complete_callback_(this);
        }
    }

    bool isReady() const {
        return pending_deps_.load(std::memory_order_acquire) == 0;
    }

};