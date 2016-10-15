cmake_minimum_required(VERSION 3.4)
project(cl_gl_bootstrap)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# for providing custom FindXXX.cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

################################################
############ External dependencies #############
################################################
add_subdirectory(external)
include_directories(${EXTERNAL_INCLUDE_DIRS})

# find all source, header and inline files
file(GLOB_RECURSE SOURCE_FILES src/*cpp)
file(GLOB_RECURSE HEADER_FILES src/*hpp)
set(SOURCE_FILES ${SOURCE_FILES} main.cpp)

include_directories(src)

add_executable(cl_gl_bootstrap ${SOURCE_FILES})

target_link_libraries(cl_gl_bootstrap ${EXTERNAL_LIBRARIES})