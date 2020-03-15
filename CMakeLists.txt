cmake_minimum_required(VERSION 3.10)
project(cabin-seater)

add_compile_options(--std=c++11)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(cabin-seater ${SOURCES})
