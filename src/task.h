#pragma once

#include <atomic>
#include <functional>
#include <cstdint>


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
    std::mutex deps_mutex

    public:
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        Task(uint64_t id, std::function<void()> work):
            id_(id),
            state_(TaskState::PENDING),
            work_(std::move(work))
        { }

        void execute() {
            state_.store(TaskState::RUNNING, std::memory_order_release);
            work_();
            state_.store(TaskState::COMPLETED, std::memory_order_release);
            onComplete()
        }

        std::uint64_t getId() const{
            return id_;
        }

        TaskState getState() const{
            return state_.load(std::memory_order_acquire);
        }

        void addDependency(Task* dependency){
            std::lock_guard<std::mutex> lock(deps_mutex_);
            dependency->dependents_.emplace_back(this);
            pending_deps_ ++;
        }

        void onComplete() {
            for (auto& dependent : dependents_) {
                dependent->pending_deps_ --;
            }
        }

        bool isReady() const {
            if (pending_deps_ == 0) {
                return true;
            } else {
                return false;
            }
        }

};