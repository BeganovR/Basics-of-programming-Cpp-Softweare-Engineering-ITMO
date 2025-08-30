# Библиотека pars_args
add_library(pars-args pars-args.cpp)

# Устанавливаем include директорию
target_include_directories(pars-args PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
