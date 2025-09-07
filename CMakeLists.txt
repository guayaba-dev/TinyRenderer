cmake_minimum_required(VERSION 3.10)
project(TinyRenderer) 
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(SANITAZER_FLAGS
     -fno-omit-frame-pointer
     -fsanitize=address,undefined,leak
     -fsanitize-address-use-after-scope
     -fsanitize=float-divide-by-zero
     -fsanitize=null,alignment   
  )


  add_compile_options("${SANITAZER_FLAGS}")
  add_link_options("${SANITAZER_FLAGS}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(RELEASE_FLAGS
     -O3
     -DNDEBUG
     -flto
  )

  add_compile_options("${RELEASE_FLAGS}")
  add_link_options("${RELEASE_FLAGS}")
endif()



#SDL package
find_package(SDL2 REQUIRED)

#fuentes de codigo
file(GLOB SRC_FILES src/*cpp)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(TinyRendererLib ${SRC_FILES})

target_include_directories(
  TinyRendererLib PRIVATE 
  ${SDL2_INCLUDE_DIRS} 
  ${CMAKE_CURRENT_SOURCE_DIR}.dependencies/
)

target_link_libraries(TinyRendererLib PRIVATE ${SDL2_LIBRARIES})

add_executable(${PROJECT_NAME} src/main.cpp)

# Linkear el ejecutable con TinyRendererLib y EnTT.
# EnTT es header-only, pero usamos su objetivo importado para manejar dependencias.
target_link_libraries(
  ${PROJECT_NAME} PRIVATE 
  TinyRendererLib 
  )


file(GLOB TEST_FILES tests/*cpp)
add_executable(${PROJECT_NAME}_tests ${TEST_FILES})
target_link_libraries(${PROJECT_NAME}_tests TinyRendererLib ${SDL2_LIBRARIES})


