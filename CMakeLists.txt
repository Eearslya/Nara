cmake_minimum_required(VERSION 3.21)
project(Nara LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin")

add_subdirectory(Engine)
add_subdirectory(Sandbox)
