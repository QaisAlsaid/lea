cmake_minimum_required(VERSION 3.10)
project(interpreter_tests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Download GTest using FetchContent
message("fetching gtest...")
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1  # Stable release
)

# Prevent GTest from overriding compiler options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Build interpreter library
add_library(interpreter_lib
    ../src/lexer.cpp
    ../src/parser.cpp
    ../src/evaluator.cpp
    ../src/token.cpp
)
target_include_directories(interpreter_lib PUBLIC ../src)

# Test executable
add_executable(run_tests
    test_main.cpp
    test_lexer.cpp
    test_parser.cpp
    test_evaluator.cpp
    test_object.cpp
)
target_include_directories(run_tests PUBLIC ../include .)
target_link_libraries(run_tests PRIVATE gtest gtest_main interpreter_lib)

# Optional: Enable testing and add test
enable_testing()
add_test(NAME LeaTests COMMAND run_tests)
