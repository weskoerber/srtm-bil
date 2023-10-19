cmake_minimum_required(VERSION 3.27)

set(CMAKE_C_STANDARD 23)

project(srtm)

add_executable(srtm)

target_include_directories(
    srtm
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_sources(
    srtm
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src/header.c
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/slice.c
)
