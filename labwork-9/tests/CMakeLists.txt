include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1 
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
enable_testing()

add_executable(
    scheduler_tests
    simple_ut.cpp
    ../lib/scheduler.h
)

target_link_libraries(
    scheduler_tests
    scheduler
    GTest::gtest_main
)

target_include_directories(scheduler_tests PUBLIC ${PROJECT_SOURCE_DIR})

gtest_discover_tests(scheduler_tests)
                        