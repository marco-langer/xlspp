cmake_minimum_required(VERSION 3.18)

project(xlspp LANGUAGES CXX)

include(CTest)
include(cmake/format.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(examples)
add_subdirectory(src)

if (${BUILD_TESTING})
    add_subdirectory(tests)
endif()
