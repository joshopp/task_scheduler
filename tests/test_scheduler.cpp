// tests/test_scheduler.cpp

#include "task_scheduler.h"
#include <iostream>
#include <cassert>


int main() {
    std::cout << "Test: Smart Scheduler with Dependencies" << std::endl;
    
    TaskScheduler scheduler(4);
    int data = 0;
    
    // Erstelle Tasks
    auto taskA = std::make_unique<Task>(1, [&data]() {
        std::cout << "  TaskA: Loading..." << std::endl;
        data = 10;
    });

    auto taskB = std::make_unique<Task>(2, [&data]() {
        std::cout << "  TaskB: Processing..." << std::endl;
        data *= 2;
    });
    
    auto taskC = std::make_unique<Task>(3, [&data]() {
        std::cout << "  TaskC: Saving..." << std::endl;
        data += 5;
    });
    
    // setup Dependencies
    taskB->addDependency(taskA.get());
    taskC->addDependency(taskB.get());
    
    // submit all and wait
    scheduler.submit(std::move(taskA));
    scheduler.submit(std::move(taskB));
    scheduler.submit(std::move(taskC));
    scheduler.waitAll();
    
    assert(data == 25);
    
    std::cout << "✅ Scheduler test passed! data = " << data << std::endl;
    return 0;
}