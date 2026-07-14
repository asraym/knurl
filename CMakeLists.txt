cmake_minimum_required(VERSION 3.16)
project(knurl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(knurl
    app/main.cpp
    scanning/FileWalker.cpp
    scanning/PyImportExtractor.cpp
)

target_include_directories(knurl PRIVATE ${CMAKE_SOURCE_DIR})
