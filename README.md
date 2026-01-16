# Distributed Task Scheduling System
A project with the goal to build a high-performance, low-latency task scheduling engine. The system will be optimized to distribute 10,000+ tasks across 8-64 cores with minimal overhead and correct dependency resolution. <br>
**This project is currently in work and not finished at this point!**



## Key Features

* **DAG-based Scheduling:** Full support for Directed Acyclic Graph (DAG) task structures with automated dependency resolution.
* **Plug-and-Play Policies:** Strategy Pattern implementation for different scheduling algorithms:
    * **FIFO:** Standard First-In-First-Out processing.
    * **Priority:** Rank-based task execution for critical test paths.
    * **Work-Stealing:** Efficient load balancing using per-thread queues to minimize contention.
* **High Performance:** Targeting < 100ns dispatch overhead.
    * **Zero-allocation** in the hot path to prevent GC-like pauses.
    * **Thread-safety** using lock-free primitives and atomic state machines where applicable.
* **Graceful Shutdown:** Implements task draining to ensure all submitted work is completed before system exit.



## Core Components

The system is built on a modular architecture to ensure scalability and maintainability. Current components are:

* **ThreadPoolExecutor:** Manages the worker threads and implements the low-level execution logic.
<!--
* **TaskScheduler (Facade):** The central entry point. Manages the lifecycle of the Executor and the chosen SchedulingPolicy.
* **SchedulingPolicy (Strategy):** An abstraction layer allowing for easy swapping of scheduling algorithms (Strategy Pattern).
* **Task (Value Object):** A lightweight wrapper around `std::function` with an
-->


## LICENSE

This project is licensed under the MIT License - see the [LICENSE](https://github.com/joshopp/task_scheduler/blob/main/LICENSE) file for details.
