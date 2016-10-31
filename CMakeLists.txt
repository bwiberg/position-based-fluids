cmake_minimum_required(VERSION 3.4)
project(position-based-fluids)

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

# setup shaders- and kernels directories
add_definitions(-DOUTPUT_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/output/")
add_definitions(-DSHADERS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")
add_definitions(-DKERNELS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/kernels/")
add_definitions(-DRES_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/res/")

include_directories(src)

add_executable(pbf ${SOURCE_FILES})

target_link_libraries(pbf ${EXTERNAL_LIBRARIES})