add_library(parser-tsv read-tsv.cpp)
target_link_libraries(parser-tsv PRIVATE functions)
target_include_directories(parser-tsv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
