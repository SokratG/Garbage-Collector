cmake_minimum_required(VERSION 3.6)

set(memory_leak_detection "GCMLD")
project(${memory_leak_detection} LANGUAGES C)

set(memory_leak_detection_VERSION_MAJOR 1)
set(memory_leak_detection_VERSION_MINOR 2)
set(memory_leak_detection_VERSION "${memory_leak_detection_VERSION_MAJOR}.${memory_leak_detection_VERSION_MINOR}")


if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMALE_C_FLAGS} -std=c11 -pedantic -Wall -O2")
elseif (CMAKE_C_COMPILE_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMALE_CXX_FLAGS} /std:c++latest /Wall /O2")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    message(FATAL_ERROR "Error compiler")
endif()

file(GLOB SRCMLD "src/*.c")

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}")


add_executable(${PROJECT_NAME} ${SRCMLD})

#set_property(TARGET ${memory_leak_detection} PROPERTY C_STANDARD 11)
