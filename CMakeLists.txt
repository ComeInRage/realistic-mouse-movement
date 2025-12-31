cmake_minimum_required(VERSION 3.10)

project(realistic-mouse-movement CXX)

add_library(realistic-mouse-movement STATIC RealisticMouse.cpp
                                            DefaultMousePolicies.cpp
)

target_compile_features(realistic-mouse-movement PRIVATE cxx_std_20)