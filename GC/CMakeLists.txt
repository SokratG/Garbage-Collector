
cmake_minimum_required (VERSION 3.6)

project ("GarbageCollector")

set(gc_VERSION_MAJOR 1)
set(gc_VERSION_MINOR 0)
set(gc_VERSION_PATCH 1)
set(gc_VERSION "${gc_VERSION_MAJOR}.${gc_VERSION_MINOR}.${gc_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 17)


include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_BINARY_DIR}")

file(GLOB SRCCPPS "src/*.cpp")

foreach(ARG ${SRCCPPS})
	message(${ARG})
endforeach()


add_executable (${PROJECT_NAME} ${SRCCPPS})

