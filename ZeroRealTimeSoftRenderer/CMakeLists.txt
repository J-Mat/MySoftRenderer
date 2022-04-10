cmake_minimum_required(VERSION 3.5)

project(ZeroRealTimeSoftRender)


file(GLOB HEADERS 
    "src/platform/*.h"
    "src/core/*.h"
    "src/core/*.hpp"
    "src/core/glm/*.hpp"
    "src/shader/*.h"
)


file(GLOB SOURCES 
    "src/platform/*.cpp"
    "src/core/*.cpp"
    "src/main.cpp"
    "src/shader/*.cpp"
)

set(INCLUDE
    src
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
    src/attachment_prj/test.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_property(TARGET "Test" PROPERTY WIN32_EXECUTABLE true)
endif()

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ZeroRealTimeSoftRender)


source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

file(GLOB IRADIANCEMAP_INCLUDE
    "src/core/core.h"
    "src/core/debug.h"
    "src/core/math_ext.h"
    "src/core/math_ext.cpp"
    "src/core/tgaimage.h"
    "src/core/tgaimage.cpp"
    "src/core/utils.h"
    "src/core/utils.cpp"
    "src/core/mesh.h"
    "src/core/mesh.cpp"
)


add_executable(
    "IrradianceMap"
    ${IRADIANCEMAP_INCLUDE}
    src/attachment_prj/iradiance_map.cpp
)
# 这个预计算项目是个控制台项目，跟上面的不一样
set_target_properties("IrradianceMap" PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)

