cmake_minimum_required(VERSION 3.10)

project(realistic-mouse-movement C CXX)

set(LIBRARY_NAME realistic-mouse-movement)

add_compile_definitions(DEBUG_COUNTERS)

add_library(${LIBRARY_NAME} STATIC src/common.cpp
                                   src/ticks_generator.cpp
                                   #src/tasks_queue.cpp
                                   src/trajectories.cpp
)

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_20)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)