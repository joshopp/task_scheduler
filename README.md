# Distributed Task Scheduling System

**High-performance C++17 task scheduler with dependency resolution for multi-threaded workloads.**


[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)


A portfolio project demonstrating modern C++ systems programming by building a high-performance, low-latency task scheduling engine. The system will be optimized to distribute 10,000+ tasks across 8-64 cores with minimal overhead and correct dependency resolution.

---

## Overview

Distributes thousands of tasks across CPU cores with minimal overhead, supporting complex dependency graphs (DAGs). Inspired by semiconductor test platforms.

**Key Metrics:**
- **Throughput:** 370k+ tasks/sec (8 threads)
- **Latency:** ~1.5μs dispatch overhead
- **Scalability:** 7.15x speedup (8 threads vs single-threaded)

---

## Key Features

- **DAG-based Scheduling:** Full support for Directed Acyclic Graph (DAG) task structures with automated dependency resolution.
- **Thread Pool:** Centralized Thread Pool with Condition-Variable synchronization
- **Modern C++:** RAII, move semantics, atomics, smart pointers
- **Memory Safe:** `unique_ptr` ownership, zero leaks
- **Graceful Shutdown:** Implements task draining to ensure all submitted work is completed before system exit.

---

## Quick Start
```bash
git clone https://github.com/joshopp/task_scheduler.git
cd task_scheduler && mkdir build && cd build
cmake .. && make -j$(nproc)

# Run scheduler tests
./test_scheduler

# Run benchmarks
./benchmarks
```

---

## Core Components

The system is built on a modular architecture to ensure scalability and maintainability. Current components are:

- **Task** (`task.h`): Wraps execution logic within an atomic state machine that tracks dependencies and notifies successors upon completion.

- **ThreadPool** (`thread_pool.h`): Manages a fixed set of persistent worker threads that process tasks from a synchronized queue using efficient condition-variable signaling.

- **TaskScheduler** (`task_scheduler.h`): Acts as the high-level orchestrator that manages task ownership, automatically resolves dependency chains, and dispatches ready-to-run tasks to the pool.



---

## Architecture

graph TD
    Client[Client/Main] -->|submit| TS[TaskScheduler]
    TS -->|check deps| Dep[Dependency Graph]
    Dep -->|if ready| TP[ThreadPool]
    subgraph Worker Threads
    TP --> W1[Worker 1]
    TP --> W2[Worker 2]
    TP --> WN[Worker N]
    end

---

## Project Status

**Completed:**
- ✅ Task execution engine
- ✅ Dependency resolution
- ✅ Smart scheduler
- ✅ Comprehensive benchmarks

**Future (Optional):**
- Lock-free work-stealing
- Priority scheduling
- CPU affinity/NUMA

---

## LICENSE

This project is licensed under the MIT License - see the [LICENSE](https://github.com/joshopp/task_scheduler/blob/main/LICENSE) file for details.

---

**Built to demonstrate C++ systems programming for R&D engineering roles.**
