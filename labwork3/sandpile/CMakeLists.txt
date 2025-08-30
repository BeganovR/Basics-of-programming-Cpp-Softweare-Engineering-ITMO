add_library(sandpile STATIC sandpile.cpp)

target_include_directories(sandpile PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
