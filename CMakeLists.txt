cmake_minimum_required(VERSION 3.5)

project(realistic-mouse-movement C CXX)

add_library(realistic-mouse-movement STATIC RealisticMouse.h
                                            RealisticMouse.cpp
)

target_include_directories  (realistic-mouse-movement PUBLIC src)
target_compile_features     (realistic-mouse-movement PRIVATE cxx_std_20)