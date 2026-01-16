// tests/test_threadpool.cpp

#include "thread_pool.h"
#include "task.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Test 1: 100 incrementations of a counter with 4 threads." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    ThreadPool threads1(4);
    std::atomic<int> counter{0};
    std::vector<std::unique_ptr<Task>> tasks;

    for (int i = 0; i < 100; ++i) {
        tasks.push_back(std::make_unique<Task>(i, [&counter]() {
            counter.fetch_add(1, std::memory_order_relaxed);
        }));
    }
    for (auto& task : tasks) {
        threads1.submit(task.get());
    }

    // wait for completion
    for (auto& task : tasks) {
        while (task->getState() != TaskState::COMPLETED) {
            std::this_thread::yield();
        }
    }
    
    assert(counter == 100);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "✅ Test 1 passed" << std::endl;
    std::cout << "100 tasks took " << duration.count() << "ms" << std::endl;


    std::cout << "Test 2: 1000 incrementation of a counter with 8 threads." << std::endl;
    
    auto start2 = std::chrono::high_resolution_clock::now();
    ThreadPool threads2(8);
    std::atomic<int> counter2{0};
    std::vector<std::unique_ptr<Task>> tasks2;

    for (int i = 0; i < 1000; ++i) {
        tasks2.push_back(std::make_unique<Task>(i, [&counter2]() {
            counter2.fetch_add(1, std::memory_order_relaxed);
        }));
    }
    for (auto& task : tasks2) {
        threads2.submit(task.get());
    }

    // wait for completion
    for (auto& task : tasks2) {
        while (task->getState() != TaskState::COMPLETED) {
            std::this_thread::yield();
        }
    }
    
    assert(counter2 == 1000);
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);
    std::cout << "✅ Test 2 passed" << std::endl;
    std::cout << "1000 tasks took " << duration2.count() << "ms" << std::endl;
}