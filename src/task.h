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
    uint64_t id_;
    std::atomic<TaskState> state_;
    std::function<void()> work_;

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
        }

        std::uint64_t getId() const{
            return id_;
        }

        TaskState getState() const{
            return state_.load(std::memory_order_acquire);
        }

};