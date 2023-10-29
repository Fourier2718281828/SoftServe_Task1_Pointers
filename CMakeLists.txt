cmake_minimum_required(VERSION 3.14)
project(Zymovets_Task1)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(
    testing
    test.cpp
    string_list.cpp
)
target_link_libraries(
    testing
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(testing)
