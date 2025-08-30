cmake_minimum_required(VERSION 3.10)
project(Labwork3)

add_subdirectory(bmp)
add_subdirectory(functions)
add_subdirectory(matrix)
add_subdirectory(pars-args)
add_subdirectory(parser-tsv)
add_subdirectory(sandpile)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE bmp functions matrix pars-args parser-tsv)
