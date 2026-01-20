cmake_minimum_required(VERSION 3.16)
project(TaskScheduler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

# Main library
add_library(task_scheduler INTERFACE)
target_include_directories(task_scheduler INTERFACE ${CMAKE_SOURCE_DIR}/src)

# Test executable
add_executable(test_basic tests/test_basic.cpp)
target_link_libraries(test_basic task_scheduler)

add_executable(test_threadpool tests/test_threadpool.cpp)
target_link_libraries(test_threadpool task_scheduler pthread)

add_executable(test_dependencies tests/test_dependencies.cpp)
target_link_libraries(test_dependencies task_scheduler pthread)

add_executable(test_scheduler tests/test_scheduler.cpp)
target_link_libraries(test_scheduler task_scheduler pthread)