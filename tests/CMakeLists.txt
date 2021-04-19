#Inspired by https://stackoverflow.com/questions/57919183/modern-cmake-project-for-header-only-library-with-unit-test

cmake_minimum_required(VERSION 3.15)

include(CTest)

add_executable(simple_test)

target_sources(simple_test
    PRIVATE
    simple.cpp)

target_include_directories(simple_test
    PRIVATE 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_compile_features(simple_test
  PRIVATE 
  cxx_std_17)

add_test(NAME Simple COMMAND simple_test)
