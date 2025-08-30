
# Сборка статической библиотеки
add_library(itmoscript 
    interpreter.h
    lexer.cpp
    lexer.h
    parser.h
    AST.h
)

# Публичные заголовки библиотеки
target_include_directories(itmoscript PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})