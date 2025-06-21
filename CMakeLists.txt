cmake_minimum_required(VERSION 3.10)

SET(PROJECT_NAME "LabHelper")

project(${PROJECT_NAME})




# Add header files
file(GLOB_RECURSE HEADER_FILES src/*.h)

# Add source files
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(
    ${PROJECT_NAME}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

include_directories(
    src
)




#include_directories(
#    libraries/include
#)
#
#add_library(
#    glad
#    STATIC
#    libraries/lib/glad.c
#)
#
#SET(
#    GLFW_LIBS
#    "${CMAKE_SOURCE_DIR}/Client/libraries/lib/glfw3.lib"
#)
#
#SET(
#    FREETYPE_LIBS
#    "${CMAKE_SOURCE_DIR}/Client/libraries/lib/freetype.lib"
#)
#
#target_link_libraries(
#    ${CLIENT_ExECUTABLE_NAME}
#    ${FREETYPE_LIBS}
#    ${GLFW_LIBS}
#    glad
#)