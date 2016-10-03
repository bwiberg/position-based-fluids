#### ################ ####
#### #### OpenCL #### ####
#### ################ ####
find_package(OPENCL)
if (${OPENCL_FOUND})
    message(STATUS "OpenCL found.")
else (${OPENCL_FOUND})
    message(FATAL_ERROR "OpenCL not found.")
endif (${OPENCL_FOUND})

#### ################ ####
#### #### OpenGL #### ####
#### ################ ####
find_package(opengl REQUIRED)

if (${OPENGL_FOUND})
    message(STATUS "OpenGL found.")
else (${OPENGL_FOUND})
    message(FATAL_ERROR "OpenGL not found.")
endif (${OPENGL_FOUND})

#### Khronos C++ OpenCL Wrapper API ####


#### Export libraries and header search paths to CMake parent scope ####
set(EXTERNAL_LIBRARIES
        ${OpenCL_LIBRARIES}
        ${OPENGL_LIBRARIES}
        PARENT_SCOPE)

set(EXTERNAL_INCLUDE_DIRS
        ${OpenCL_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        PARENT_SCOPE)

### Print libraries to CMake output ###
message(STATUS "external libraries:")
foreach (EXTERNAL_LIBRARY ${EXTERNAL_LIBRARIES})
    message(STATUS "	${EXTERNAL_LIBRARY}")
endforeach (EXTERNAL_LIBRARY)
message(STATUS "external include directories:")
foreach (EXTERNAL_INCLUDE_DIR ${EXTERNAL_INCLUDE_DIRS})
    message(STATUS "	${EXTERNAL_INCLUDE_DIR}")
endforeach (EXTERNAL_INCLUDE_DIR)