cmake_minimum_required(VERSION 3.20)
project(ares_drone_telemetry LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_library(ares_drone_telemetry
  collector/telemetry_collector.cpp
  validation/validator.cpp
)

target_include_directories(ares_drone_telemetry PUBLIC .)

add_executable(telemetry_tests tests/telemetry_tests.cpp)
target_link_libraries(telemetry_tests PRIVATE ares_drone_telemetry)

add_test(NAME telemetry_tests COMMAND telemetry_tests)
