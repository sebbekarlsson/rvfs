cmake_minimum_required(VERSION 3.20)


project(rvfs)


file(GLOB PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB rvfs_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_compile_options(-g -Wall)

add_executable(rvfs_e ${rvfs_sources})



list(FILTER rvfs_ources EXCLUDE REGEX "main.c")

add_library(rvfs SHARED ${rvfs_sources})
add_library(rvfs_static STATIC ${rvfs_sources})

target_compile_options(rvfs PUBLIC -fPIC)
target_compile_options(rvfs_static PUBLIC -fPIC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
  rvfs
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  rvfs_static
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  rvfs_e
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_compile_options(-g -Wall)
