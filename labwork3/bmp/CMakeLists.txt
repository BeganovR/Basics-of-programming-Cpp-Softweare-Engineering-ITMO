add_library(bmp bmp_writer.cpp)
target_link_libraries(bmp PRIVATE matrix)
target_include_directories(bmp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
