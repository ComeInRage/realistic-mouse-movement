cmake_minimum_required(VERSION 3.5)

project(realistic-mouse-movement CXX)

add_library(realistic-mouse-movement STATIC RealisticMouse.h
                                            RealisticMouse.cpp
)

target_compile_features(realistic-mouse-movement PRIVATE cxx_std_20)