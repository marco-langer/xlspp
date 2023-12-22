cmake_minimum_required(VERSION 3.18.4)

project(xlspp VERSION 0.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(PreventInSourceBuilds)
include(Format)
include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(examples)
add_subdirectory(src bin)

if (${BUILD_TESTING})
    add_subdirectory(tests)
endif()
