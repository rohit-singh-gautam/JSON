cmake_minimum_required(VERSION 3.28)

project(JSON)

find_package(GTest CONFIG REQUIRED)

enable_testing()

add_compile_options(-Wall -Werror -Wpedantic -Wextra -Weffc++ -Wsuggest-override -fanalyzer)

add_executable(JSONTest src/jsontest.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

set_property(TARGET JSONTest PROPERTY CXX_STANDARD 23)

target_link_libraries(JSONTest PRIVATE GTest::gtest_main)

add_test(JSONTest JSONTest)

file(COPY resources DESTINATION .)

add_subdirectory(examples)
