// tests/benchmark_scaling.cpp

#include "task_scheduler.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <atomic>

// Benchmark: Measure performance scaling with number of threads
void benchmark_scaling() {
    const int NUM_TASKS = 10000;
    
    std::cout << "Benchmark: Thread Scaling (10,000 tasks)\n";
    std::cout << "Threads | Time (ms) | Tasks/sec | Speedup\n";
    std::cout << "--------|-----------|-----------|--------\n";
    
    double baseline_time = 0;
    
    for (size_t num_threads : {1, 2, 4, 8, 16}) {
        TaskScheduler scheduler(num_threads);
        std::atomic<int> counter{0};
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Submit 10 000 independant tasks
        for (int i = 0; i < NUM_TASKS; ++i) {
            auto task = std::make_unique<Task>(i, [&counter]() {
                // simulate work
                counter.fetch_add(1, std::memory_order_relaxed);
                
                // simulate high CPU work -> larger delay than scheduling overhead
                volatile int x = 0;
                for (int j = 0; j < 10000; ++j) {
                    x += j;
                }
            });
            scheduler.submit(std::move(task));
        }
        
        scheduler.waitAll();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        double time_ms = duration.count();
        double tasks_per_sec = (NUM_TASKS / time_ms) * 1000;
        double speedup = (num_threads == 1) ? 1.0 : (baseline_time / time_ms);
        
        if (num_threads == 1) baseline_time = time_ms;
        
        printf("%7zu | %9.1f | %9.0f | %6.2fx\n", 
               num_threads, time_ms, tasks_per_sec, speedup);
    }
    
    std::cout << "\n";
}



// Benchmark: Measure overhead of dispatching a task
void benchmark_overhead() {
    const int NUM_MEASUREMENTS = 1000;
    TaskScheduler scheduler(4);
    
    std::vector<uint64_t> latencies;
    latencies.reserve(NUM_MEASUREMENTS);
    
    for (int i = 0; i < NUM_MEASUREMENTS; ++i) {
        auto submit_time = std::chrono::high_resolution_clock::now();
        
        std::atomic<bool> task_started{false};
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto task = std::make_unique<Task>(i, [&]() {
            start_time = std::chrono::high_resolution_clock::now();
            task_started.store(true, std::memory_order_release);
        });
        
        scheduler.submit(std::move(task));
        
        // wait until task has started
        while (!task_started.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        
        auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(
            start_time - submit_time).count();
        
        latencies.push_back(latency);
    }
    
    // Stat tracking
    std::sort(latencies.begin(), latencies.end());
    
    uint64_t min = latencies.front();
    uint64_t max = latencies.back();
    uint64_t median = latencies[NUM_MEASUREMENTS / 2];
    uint64_t p95 = latencies[(NUM_MEASUREMENTS * 95) / 100];
    uint64_t p99 = latencies[(NUM_MEASUREMENTS * 99) / 100];
    
    double avg = std::accumulate(latencies.begin(), latencies.end(), 0.0) / NUM_MEASUREMENTS;
    
    std::cout << "Benchmark: Dispatch Overhead (1,000 measurements)\n";
    std::cout << "  Min:    " << min << " ns\n";
    std::cout << "  Avg:    " << avg << " ns\n";
    std::cout << "  Median: " << median << " ns\n";
    std::cout << "  P95:    " << p95 << " ns\n";
    std::cout << "  P99:    " << p99 << " ns\n";
    std::cout << "  Max:    " << max << " ns\n\n";
}



// Benchmark: Compare execution time with and without dependencies
void benchmark_dependencies() {
    const int NUM_TASKS = 1000;
    
    std::cout << "Benchmark: Dependency Resolution Overhead\n\n";
    
    // Test 1: Without Dependencies
    {
        TaskScheduler scheduler(8);
        std::atomic<int> counter{0};
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < NUM_TASKS; ++i) {
            auto task = std::make_unique<Task>(i, [&counter]() {
                counter++;
            });
            scheduler.submit(std::move(task));
        }
        
        scheduler.waitAll();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Without Dependencies: " << duration.count() << " μs\n";
    }
    
    // Test 2: With Dependencies (Chain)
    {
        TaskScheduler scheduler(8);
        int data = 0;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<Task*> tasks;
        for (int i = 0; i < NUM_TASKS; ++i) {
            auto task = std::make_unique<Task>(i, [&data]() {
                data++;
            });
            
            Task* raw = task.get();
            if (i > 0) {
                task->addDependency(tasks.back());  // Chain: 1→2→3→...
            }
            
            tasks.push_back(raw);
            scheduler.submit(std::move(task));
        }
        
        scheduler.waitAll();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "With Dependencies (chain): " << duration.count() << " μs\n";
    }
    
    // Test 3: With Dependencies (Fan-out)
    {
        TaskScheduler scheduler(8);
        std::atomic<int> counter{0};
        
        auto start = std::chrono::high_resolution_clock::now();
        
        auto root = std::make_unique<Task>(0, [&counter]() {
            counter++;
        });
        Task* root_raw = root.get();
        scheduler.submit(std::move(root));
        
        // 999 tasks waiting for root
        for (int i = 1; i < NUM_TASKS; ++i) {
            auto task = std::make_unique<Task>(i, [&counter]() {
                counter++;
            });
            task->addDependency(root_raw);
            scheduler.submit(std::move(task));
        }
        
        scheduler.waitAll();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "With Dependencies (fan-out): " << duration.count() << " μs\n\n";
    }
}



// Benchmark: Measure object pooling for Task allocation
void benchmark_allocation() {
    const int NUM_ITERATIONS = 100000;
    
    std::cout << "Benchmark: Task Allocation\n\n";
    
    // Test 1: new/delete
    {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            Task* task = new Task(i, [](){});
            delete task;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "new/delete: " << duration.count() << " μs\n";
        std::cout << "  Per task: " << (duration.count() * 1000.0 / NUM_ITERATIONS) << " ns\n";
    }
    
    // Test 2: make_unique
    {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            auto task = std::make_unique<Task>(i, [](){});
            // Automatic delete
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "make_unique: " << duration.count() << " μs\n";
        std::cout << "  Per task: " << (duration.count() * 1000.0 / NUM_ITERATIONS) << " ns\n\n";
    }
}



// Benchmark: DAG Processing -> Realistic Scenario
void benchmark_dag() {
    std::cout << "Benchmark: DAG Processing (Realistic Workload)\n\n";
    
    TaskScheduler scheduler(8);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // simulate workflow:
    // Layer 1: 10 Data-Load Tasks
    // Layer 2: 50 Processing Tasks (wait for layer 1)
    // Layer 3: 10 Aggregation Tasks (wait for layer 2)
    // Layer 4: 1 Final Task (wait for layer 3)
    
    std::vector<Task*> layer1, layer2, layer3;
    std::atomic<int> result{0};
    
    // Layer 1: Load
    for (int i = 0; i < 10; ++i) {
        auto task = std::make_unique<Task>(i, [&result]() {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            result.fetch_add(1, std::memory_order_relaxed);
        });
        layer1.push_back(task.get());
        scheduler.submit(std::move(task));
    }
    
    // Layer 2: Process
    for (int i = 0; i < 50; ++i) {
        auto task = std::make_unique<Task>(10 + i, [&result]() {
            std::this_thread::sleep_for(std::chrono::microseconds(50));
            result.fetch_add(1, std::memory_order_relaxed);
        });
        
        task->addDependency(layer1[i % 10]);
        task->addDependency(layer1[(i + 1) % 10]);
        
        layer2.push_back(task.get());
        scheduler.submit(std::move(task));
    }
    
    // Layer 3: Aggregate
    for (int i = 0; i < 10; ++i) {
        auto task = std::make_unique<Task>(60 + i, [&result]() {
            std::this_thread::sleep_for(std::chrono::microseconds(20));
            result.fetch_add(1, std::memory_order_relaxed);
        });
        
        for (int j = i * 5; j < (i + 1) * 5; ++j) {
            task->addDependency(layer2[j]);
        }
        
        layer3.push_back(task.get());
        scheduler.submit(std::move(task));
    }
    
    // Layer 4: Final
    auto final_task = std::make_unique<Task>(70, [&result]() {
        result.fetch_add(1, std::memory_order_relaxed);
    });
    
    for (auto* t : layer3) {
        final_task->addDependency(t);
    }
    
    scheduler.submit(std::move(final_task));
    scheduler.waitAll();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Total time: " << duration.count() << " ms\n";
    std::cout << "Tasks executed: " << result.load() << "\n\n";
}



int main() {
    std::cout << "========================================\n";
    std::cout << "  Task Scheduler Performance Benchmarks\n";
    std::cout << "========================================\n\n";
    
    // run benchmarks
    benchmark_scaling();
    // benchmark_overhead();
    // benchmark_dependencies();
    // benchmark_allocation();
    // benchmark_dag();
    
    std::cout << "All benchmarks completed!\n";
    return 0;
}

