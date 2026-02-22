cmake_minimum_required(VERSION 3.10)

link_libraries(${LIBRARY_NAME})

if (WIN32)
	add_executable(debug-windows-example debug_windows_example.cpp)
endif()