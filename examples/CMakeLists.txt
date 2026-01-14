cmake_minimum_required(VERSION 3.10)

add_executable(ticks-generator-usage-example ticks_generator_usage_example.cpp)

target_link_libraries(ticks-generator-usage-example PRIVATE ${LIBRARY_NAME})
