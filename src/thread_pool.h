//src/thread_pool.h
#pragma once

#include "task.h"
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>


class ThreadPool {
private:
    size_t num_threads_;
    std::atomic<bool> stop_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::queue<Task*> task_queue_;
    std::vector<std::thread> threads_;

    void workerLoop(){
        while (true) {
            Task* task = nullptr; // if no task is available

            { // lock queue (unique) and wait for task
                std::unique_lock<std::mutex> lock(queue_mutex_);
                condition_.wait(lock, [this]{ 
                    return stop_.load(std::memory_order_acquire) || !task_queue_.empty();
                });
                // end if no task left after stop signal
                if (stop_.load(std::memory_order_acquire) && task_queue_.empty()) {
                    return;
                }
                // else get next  task
                task = task_queue_.front();
                task_queue_.pop();
            } // lock release here
            if (task) {
                task->execute();
            }
        }
    }

public:
    // disable copy constructor and assignment
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    //Constructor
    explicit ThreadPool(size_t num_threads): 
        num_threads_(num_threads) ,   
        stop_(false) 
    {
        for (size_t i = 0; i < num_threads_; ++i) {
            threads_.emplace_back([this]() {workerLoop(); });
        }
    }

    // Destructor
    ~ThreadPool() {
        stop_.store(true, std::memory_order_release);
        condition_.notify_all();
        // wait for all threads to finish
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    
    // add a new task to the pool
    void submit(Task* newTask) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            task_queue_.push(newTask);
        } // lock released automatically here
        condition_.notify_one();
    }

};