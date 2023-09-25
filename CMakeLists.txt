cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(voyage 
    VERSION 0.0.1
    DESCRIPTION "a small programming language."
    LANGUAGES C CXX 
)

set(VOYAGE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(VOYAGE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/source")
message(STATUS "source dir ${VOYAGE_SOURCE_DIR}")
message(STATUS "include dir ${VOYAGE_INCLUDE_DIR}")

set(CXX_OPTIONS -Wall -Wextra -Wconversion -Wdeprecated -Wpedantic -Werror)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_subdirectory(source)