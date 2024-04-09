cmake_minimum_required(VERSION 3.25)
project(my_list CXX)

enable_testing()
find_package(GTest REQUIRED)

add_executable(
    ${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/test.cpp
    ${PROJECT_SOURCE_DIR}/AV_List.h
)


target_link_libraries(${PROJECT_NAME} GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
