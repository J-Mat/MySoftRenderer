cmake_minimum_required(VERSION 3.5)

project(ZeroRealTimeSoftRender)


file(GLOB HEADERS 
    "src/platform/*.h"
    "src/core/*.h"
)


file(GLOB SOURCES 
    "src/platform/*.cpp"
    "src/core/*.cpp"
    "src/main.cpp"
)

set(INCLUDE
    src/platform
    src/core 
)

include_directories(${INCLUDE})

add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${HEADERS} 
    src/main.cpp
)

IF(WIN32)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 
ENDIF(WIN32)

add_executable("Test"
    WIN32
    src/test/test.cpp
)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ZeroRealTimeSoftRender)


source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

