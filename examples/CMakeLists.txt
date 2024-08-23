cmake_minimum_required(VERSION 3.28)

add_executable(beautify beautify.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

set_property(TARGET beautify PROPERTY CXX_STANDARD 23)
