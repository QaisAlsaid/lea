cmake_minimum_required(VERSION 3.10)
project(lea LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_compile_options(-g -O0)


set(SRC
  src/lexer.cpp
  src/main.cpp
  src/repl.cpp
  src/token.cpp
  src/parser.cpp
  src/evaluator.cpp
  src/runner.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
