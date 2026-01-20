// tests/test_scheduler.cpp

#include "task_scheduler.h"
#include <iostream>
#include <cassert>


int main() {
    std::cout << "Test: Smart Scheduler with Dependencies" << std::endl;
    
    TaskScheduler scheduler(4);
    int data = 0;
    
    // Erstelle Tasks
    Task* taskA = new Task(1, [&data]() {
        std::cout << "  TaskA: Loading..." << std::endl;
        data = 10;
    });
    
    Task* taskB = new Task(2, [&data]() {
        std::cout << "  TaskB: Processing..." << std::endl;
        data *= 2;
    });
    
    Task* taskC = new Task(3, [&data]() {
        std::cout << "  TaskC: Saving..." << std::endl;
        data += 5;
    });
    
    // setup Dependencies
    taskB->addDependency(taskA);
    taskC->addDependency(taskB);
    
    // submit all and wait
    scheduler.submit(taskA);
    scheduler.submit(taskB);
    scheduler.submit(taskC);
    scheduler.waitAll();
    
    assert(data == 25);
    
    std::cout << "✅ Scheduler test passed! data = " << data << std::endl;
    return 0;
}