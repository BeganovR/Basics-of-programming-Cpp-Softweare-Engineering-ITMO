cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)

project(
    labwork2
    VERSION 1.0
    DESCRIPTION "C++ Year 2024 Labwork2"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

include_directories(lib)

add_subdirectory(lib)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)