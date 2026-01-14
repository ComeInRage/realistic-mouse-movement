cmake_minimum_required(VERSION 3.10)

project(realistic-mouse-movement C CXX)

set(LIBRARY_NAME realistic-mouse-movement)

add_library(${LIBRARY_NAME} STATIC src/common.cpp
                                   #src/tasks_queue.cpp
)

target_include_directories(${LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(${LIBRARY_NAME} PRIVATE cxx_std_20)