cmake_minimum_required(VERSION 3.29)
project(dynamic-range VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,modernize-*")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(TEST_SOURCES
        Limiter
        Compressor
        Expander
        NoiseGate
)

# Define a function to reduce redundancy
function(add_filter_test test_name)
    set(test_exe ${test_name}Test)
    set(test_source test/${test_name}Test.cpp)

    add_executable(${test_exe} ${test_source})

    target_include_directories(${test_exe} PRIVATE
            ${GTEST_INCLUDE_DIRS}
            ${CMAKE_CURRENT_SOURCE_DIR}/inc
    )

    target_link_libraries(${test_exe} PRIVATE
            GTest::gtest_main
    )

    target_compile_options(${test_exe} PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
            -Wall
    )

    target_link_options(${test_exe} PRIVATE
            -fsanitize=address
    )

    set_target_properties(${test_exe} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
    )

    add_test(NAME test_${test_name} COMMAND ${test_exe})
endfunction()

# Add all tests using the function
foreach (test ${TEST_SOURCES})
    add_filter_test(${test})
endforeach ()
