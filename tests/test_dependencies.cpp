// tests/test_dependencies.cpp

#include "thread_pool.h"
#include "task.h"

#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <cstdlib>

int main() {
    std::cout << "Test: Task Dependencies (Pipeline A->B->C)" << std::endl;

    ThreadPool pool(4);
    int data;

    // define tasks
    auto load_data = std::make_unique<Task>(1, [&data]() {
        data = 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
    auto multiply_data = std::make_unique<Task>(2, [&data]() {
        data *= 2;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
    auto add_to_data = std::make_unique<Task>(3, [&data]() {
        data += 5;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });

    // set dependencies
    multiply_data->addDependency(load_data.get());
    add_to_data->addDependency(multiply_data.get());

    // submit tasks
    if (load_data->isReady()) {
        pool.submit(load_data.get());
    }
    while (load_data->getState() != TaskState::COMPLETED) {
        std::this_thread::yield();
    }

    if (multiply_data->isReady()) {
        pool.submit(multiply_data.get());
    }
    while (multiply_data->getState() != TaskState::COMPLETED) {
        std::this_thread::yield();
    }
    
    if (add_to_data->isReady()) {
        pool.submit(add_to_data.get());
    }
    while (add_to_data->getState() != TaskState::COMPLETED) {
        std::this_thread::yield();
    }

    // assert final result
    assert(data == 25);
    std::cout << "Done! data = " << data << std::endl;
    std::cout << "✅ Dependency test passed!" << std::endl;
    
    return EXIT_SUCCESS;
}