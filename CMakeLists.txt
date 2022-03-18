project(memorypool)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_STANDARD_REQUIRED on)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
# set(CMAKE_CXX_EXTENSIONS off)

add_definitions("-g -Wall ")

if(NOT CMAKE_BUILD_TYPE)
    add_definitions("-g -Wall")
    set(CMAKE_BUILD_TYPE "Debug")
    message("--------------------------------CMAKE_BUILD_TYPE default value is ${CMAKE_BUILD_TYPE}-----------------------")
else() 
    message("--------------------------------CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}------------------------------------------")
endif()


add_subdirectory(./src)

add_subdirectory(./test)