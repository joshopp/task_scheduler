// src/task_scheduler.h
#pragma once

#include "task.h"
#include "thread_pool.h"
#include <vector>
#include <mutex>
#include <memory>
#include <thread>
#include <algorithm>

class TaskScheduler {
private:
    ThreadPool pool_;
    std::vector<Task*> all_tasks_;
    std::vector<Task*> pending_tasks_;
    std::mutex pending_mutex_;

    // callback to wake up tasks when dependencies are completed
    void onTaskCompleted(Task* completed_task) {
        std::vector<Task*> rdy_tasks;

        {
            std::lock_guard<std::mutex> lock(pending_mutex_);

            for (Task* task : pending_tasks_) {
                if (task->isReady()) {
                    rdy_tasks.push_back(task);
                }
            }

            for (Task* task : rdy_tasks) {
                pending_tasks_.erase(
                    std::remove(pending_tasks_.begin(),pending_tasks_.end(), task), 
                    pending_tasks_.end()
                );
            }
        }

        for (Task* task : rdy_tasks) {
            pool_.submit(task);
        }
    }

public:
    // Non-copyable
    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;
    
    // Constructor
    explicit TaskScheduler(size_t num_threads) 
        : pool_(num_threads)
    {
        // TODO: Body falls nötig
    }
    
    // Destructor
    ~TaskScheduler() {
        waitAll();
    }
    
    // Submit Task (erkennt Dependencies automatisch)
    void submit(Task* task) {
        all_tasks_.push_back(task);
        task->setOnCompleteCallback([this](Task* t){
            this->onTaskCompleted(t);
        });

        if (task->isReady()) {
            pool_.submit(task);
        } else {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            pending_tasks_.push_back(task);
        }
    }
    
    // Warte bis alle Tasks fertig sind
    void waitAll() {
        while (true) {
            bool all_completed = true;

            for (Task* task : all_tasks_) {
                if (task->getState() != TaskState::COMPLETED) {
                    all_completed = false;
                    break;
                }
            }

            if (all_completed) {return;}

            std::this_thread::yield(); 
        }
    }


};
