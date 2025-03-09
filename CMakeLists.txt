cmake_minimum_required(VERSION 3.29)
project(dynamic-range VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,modernize-*")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Limiter Test
set(LIMITER_TEST_SOURCES
        test/LimiterTest.cpp
)
add_executable(LimiterTest ${LIMITER_TEST_SOURCES})
target_include_directories(LimiterTest PRIVATE
        ${GTEST_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
target_link_libraries(LimiterTest PRIVATE
        GTest::gtest_main
)
target_compile_options(LimiterTest PRIVATE
        -fsanitize=address
        -fno-omit-frame-pointer
        -Wall
)
target_link_options(LimiterTest PRIVATE
        -fsanitize=address
)
set_target_properties(LimiterTest PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
)
add_test(NAME test_LimiterTest COMMAND LimiterTest)