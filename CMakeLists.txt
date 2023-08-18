cmake_minimum_required (VERSION 3.5)
project (twolame)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_INCLUDE_DIR
	${PROJECT_SOURCE_DIR}/include
)

execute_process(
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMAND "./autogen.sh")

execute_process(
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMAND "./configure")

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

add_subdirectory(libtwolame)
