cmake_minimum_required(VERSION 3.25)
project(my_vector CXX)


# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()
find_package(GTest REQUIRED)

add_executable(
    ${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/test_vector.cpp
    ${PROJECT_SOURCE_DIR}/AV_vector.h
)


target_link_libraries(${PROJECT_NAME} GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})


#add_library(mylib AV_vector.h)