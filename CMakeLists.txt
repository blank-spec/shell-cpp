cmake_minimum_required(VERSION 3.13)

project(shell-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/*.hpp
)

list(FILTER SOURCE_FILES EXCLUDE REGEX "build|CMakeFiles|out|debug|release")

set(CMAKE_CXX_STANDARD 23)

add_executable(shell ${SOURCE_FILES})

target_include_directories(shell PRIVATE "${CMAKE_SOURCE_DIR}/src")