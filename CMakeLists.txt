cmake_minimum_required(VERSION 3.15)

project("Moptional")

#INTERFACE expects no source files 
add_library(${PROJECT_NAME} INTERFACE)

message(STATUS "Building Moptional!")

#Use CTest framework
enable_testing()

add_subdirectory(tests)
