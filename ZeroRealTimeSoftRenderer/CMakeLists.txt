cmake_minimum_required(VERSION 3.5)

project(ZeroRealTimeSoftRender)


file(GLOB HEADERS 
    "src/platform/*.h"
    "src/core/*.h"
    "src/core/*.hpp"
    "src/core/glm/*.hpp"
)


file(GLOB SOURCES 
    "src/platform/*.cpp"
    "src/core/*.cpp"
    "src/main.cpp"
)

set(INCLUDE
    src/platform
    src/core 
    src/core/glm
)

include_directories(${INCLUDE})

IF(WIN32)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 
ENDIF(WIN32)

add_executable(
    ${PROJECT_NAME} 
    WIN32
    ${SOURCES}
    ${HEADERS} 
    src/main.cpp
)




add_executable(
    "Test"
    WIN32
    src/test/test.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_property(TARGET "Test" PROPERTY WIN32_EXECUTABLE true)
endif()

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ZeroRealTimeSoftRender)


source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

