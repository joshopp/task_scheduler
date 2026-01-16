// tests/test_basic.cpp

#include "task.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Test 1: Basic incrementation of a counter." << std::endl;
    int counter = 41;
    Task incrementCounter(1, [&counter]() {counter++; });

    assert(incrementCounter.getState() == TaskState::PENDING);

    incrementCounter.execute();

    assert(counter == 42);
    assert(incrementCounter.getState() == TaskState::COMPLETED);
    std::cout << "✅ Test 1 passed" << std::endl;

    
    std::cout << "\nTest 2: Incrementation of multiple counters." << std::endl;
    int counter1 = 41;
    int counter2 = 401;
    int counter3 = 1870;

    Task incC1(2, [&counter1]() {counter1++; });
    Task incC2(3, [&counter2]() {counter2++; });
    Task incC3(4, [&counter3]() {counter3++; });

    assert(incC1.getState() == TaskState::PENDING);
    assert(incC2.getState() == TaskState::PENDING);
    assert(incC3.getState() == TaskState::PENDING);

    incC1.execute();
    incC2.execute();
    incC3.execute();

    assert(counter1 == 42);
    assert(counter2 == 402);
    assert(counter3 == 1871);

    assert(incC1.getState() == TaskState::COMPLETED);
    assert(incC2.getState() == TaskState::COMPLETED);
    assert(incC3.getState() == TaskState::COMPLETED);
    std::cout << "✅ Test 2 passed" << std::endl;
    

    std::cout << "\nTest 3: Complex function and Id test." << std::endl;
    int n = 11;
    Task mathOperation(5, [&n]() {n = n*n +3*n - 49; });

    assert(mathOperation.getState() == TaskState::PENDING);

    mathOperation.execute();

    assert(n == 105);
    assert(mathOperation.getState() == TaskState::COMPLETED);
    assert(mathOperation.getId() == 5);
    std::cout << "✅ Test 3 passed" << std::endl;

    std::cout << "\nAssert copying does not work" << std::endl;
    // Task copy = mathOperation; // triggers a compile error when uncommented


    std::cout << "\n🎉 All tests passed!" << std::endl;
}