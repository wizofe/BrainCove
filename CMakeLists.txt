cmake_minimum_required (VERSION 2.6) 
project (CorrelationMatrix) 

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake-Modules")

IF (MSVC)
    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4290")
ENDIF (MSVC)

SET(USE_NQVTK OFF CACHE BOOL "Sets whether to use nqvtk for rendering")

FIND_PATH(DATA_DIR NAMES *.vti PATHS ${CMAKE_CURRENT_SOURCE_DIR})
ADD_DEFINITIONS(-DDATA_DIR=\"${DATA_DIR}\")

add_subdirectory(MatrixIO)
add_subdirectory(Rendering)
add_subdirectory(UI)
add_subdirectory(MatrixIOTests)
