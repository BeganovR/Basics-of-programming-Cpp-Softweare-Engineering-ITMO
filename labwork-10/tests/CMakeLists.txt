include(FetchContent)

FetchContent_Declare(
  googletests
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletests)

enable_testing()

add_executable(
  itmoscript_tests
  mytests/lexer_test.cpp
  mytests/string_test.cpp
  mytests/list_test.cpp
  mytests/math_test.cpp
  mytests/func_test.cpp
  mytests/conditionals_test.cpp
  mytests/variables_test.cpp
  mytests/loops_test.cpp
  mytests/basic_ops_test.cpp
  mytests/string_func_test.cpp
  function_test.cpp
  types_test.cpp
  loop_and_branch_test.cpp
  illegal_ops_test.cpp
)

target_link_libraries(
  itmoscript_tests
  itmoscript
  GTest::gtest_main
)

target_include_directories(itmoscript_tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(itmoscript_tests)