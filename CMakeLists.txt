cmake_minimum_required(VERSION 3.10)

project(TinyRenderer)

set(CMAKE_C_STANDARD 11)

#SDL package
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

#fuentes de codigo
file(GLOB SRC_FILES src/*cpp)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(TinyRendererLib ${SRC_FILES})

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} TinyRendererLib ${SDL2_LIBRARIES})

file(GLOB TEST_FILES tests/*cpp)
add_executable(${PROJECT_NAME}_tests ${TEST_FILES})
target_link_libraries(${PROJECT_NAME}_tests TinyRendererLib ${SDL2_LIBRARIES})


