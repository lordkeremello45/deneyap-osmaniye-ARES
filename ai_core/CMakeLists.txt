cmake_minimum_required(VERSION 3.20)
project(ares_ai_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

add_library(ares_ai_engine src/engine.cpp)
target_include_directories(ares_ai_engine PUBLIC include)

add_executable(ares_ai_core src/main.cpp)
target_link_libraries(ares_ai_core PRIVATE ares_ai_engine)

add_executable(ares_ai_core_tests tests/engine_tests.cpp)
target_link_libraries(ares_ai_core_tests PRIVATE ares_ai_engine)
add_test(NAME ai_core_tests COMMAND ares_ai_core_tests)
