cmake_minimum_required(VERSION 3.1.0)

project(realistic-mouse-movement CXX)

add_library(realistic-mouse-movement STATIC src/RealisticMouse.cpp
                                            src/RealisticMouse.h
)

target_include_directories  (realistic-mouse-movement PUBLIC src)
target_compile_features     (realistic-mouse-movement PUBLIC cxx_std_20)