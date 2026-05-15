cmake_minimum_required(VERSION 3.13)
project(shell-cpp)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
list(FILTER SRC_FILES EXCLUDE REGEX "main\\.cpp|tests/|test_")

add_library(shell_lib STATIC ${SRC_FILES})
target_include_directories(shell_lib PUBLIC "${CMAKE_SOURCE_DIR}/src")

add_executable(shell src/main.cpp)
target_link_libraries(shell PRIVATE shell_lib)

enable_testing()

file(GLOB_RECURSE TEST_FILES "src/tests/*.cpp")

if(TEST_FILES)
    add_executable(shell_tests ${TEST_FILES})
    target_link_libraries(shell_tests PRIVATE
            shell_lib
            gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(shell_tests)
endif()