cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
find_package(PkgConfig)

project("yal" C CXX)
set(TARGET_NAME yal)

option(ENABLE_TESTS "Set to ON to build tests" OFF)
# todo this is not portable
if (NOT ENABLE_TESTS)
    set(CMAKE_CXX_FLAGS "-Os")
endif()
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

include(CheckIncludeFile)
CHECK_INCLUDE_FILE("Arduino.h" HAVE_ARDUINO)
if (NOT HAVE_UNITTESTXX)
    unset(HAVE_UNITTESTXX CACHE)
    message(STATUS "Arduino.h not found, mocking implementation")
endif ()

add_library(
        ${TARGET_NAME}
        STATIC
        ${CMAKE_CURRENT_LIST_DIR}/src/yal.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/abstractions.cpp)

target_include_directories(
        ${TARGET_NAME}
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include)

if (ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
