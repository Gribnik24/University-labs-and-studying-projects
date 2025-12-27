cmake_minimum_required(VERSION 3.10)
project(SLAE_Solver CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})

set(SOURCE_FILES
    src/main.cpp
    src/LU_Solver.cpp
    src/QR_Solver.cpp
    src/MatrixOperations.cpp
)

add_executable(SLAE_solver ${SOURCE_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native)
endif()

if(WIN32)
    target_compile_options(SLAE_solver PRIVATE /bigobj)
endif()