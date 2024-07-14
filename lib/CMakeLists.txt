cmake_minimum_required(VERSION 3.11)

project(hope-profiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O3 -fPIC" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -O3 -fPIC" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

file(GLOB CL_HEADERS
    hope-profiler/*.h
    hope-profiler/third_party/*.h
)

file(GLOB CL_SOURCES
    hope-profiler/*.cpp
    hope-profiler/third_party/*.cpp
)

add_library(hope-profiler STATIC ${CL_HEADERS} ${CL_SOURCES})
target_include_directories(hope-profiler PUBLIC ../lib)

