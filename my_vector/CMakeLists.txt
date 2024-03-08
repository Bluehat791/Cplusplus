cmake_minimum_required(VERSION 3.25)
project(test CXX)

add_executable(
    ${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/test.cpp
)

add_library(mylib AV_vector.h)