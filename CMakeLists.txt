cmake_minimum_required(VERSION 3.16)
project(TaskScheduler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

# Main library
add_library(task_scheduler
    src/task.cpp
)

target_include_directories(task_scheduler PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Test executable (später)
add_executable(test_basic tests/test_basic.cpp)
target_link_libraries(test_basic task_scheduler)