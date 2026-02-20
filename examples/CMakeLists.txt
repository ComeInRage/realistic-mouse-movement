cmake_minimum_required(VERSION 3.10)

link_libraries(${LIBRARY_NAME})
add_compile_definitions(DEBUG_COUNTERS)

add_executable(ticks-generator-usage-example ticks_generator_usage_example.cpp)

if (WIN32)
	add_executable(debug-windows-example debug_windows_example.cpp)
endif()