cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig)

project("yal" C CXX)
set(TARGET_NAME yal)

option(ENABLE_TESTS "Set to ON to build tests" OFF)
option(YAL_ARDUINO_SUPPORT "Set to ON to enable arduino support" OFF)
# todo this is not portable
if (NOT ENABLE_TESTS)
    set(CMAKE_CXX_FLAGS "-Os")
endif()
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

if (YAL_ARDUINO_SUPPORT)
    add_compile_definitions(HAVE_ARDUINO true)
endif()

add_library(
        ${TARGET_NAME}
        STATIC
        ${CMAKE_CURRENT_LIST_DIR}/src/yal.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/abstractions.cpp)

target_include_directories(
        ${TARGET_NAME}
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include)
# todo this is not portable
target_compile_options(${TARGET_NAME} PRIVATE -Wall)

if (ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
