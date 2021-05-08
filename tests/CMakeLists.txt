#Inspired by https://stackoverflow.com/questions/57919183/modern-cmake-project-for-header-only-library-with-unit-test

cmake_minimum_required(VERSION 3.15)

include(CTest)

function(add_exec name)
  add_executable(${name})

  target_sources(${name}
      PRIVATE
      ${name}.cpp)

  target_include_directories(${name}
      PRIVATE 
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

  target_compile_features(${name}
    PRIVATE 
    cxx_std_17)
endfunction()

add_exec(simple)
add_exec(stl_algos)

add_test(NAME Simple COMMAND simple)
add_test(NAME STL_Algorithms COMMAND stl_algos)
