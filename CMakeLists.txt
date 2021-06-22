cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project(my_project)
add_definitions(CMAKEON, 1)

ADD_LIBRARY(
	sf2
  sf2/sf2.c
)

ADD_LIBRARY(
	runtime
  sf2/sf2.c
)

ADD_LIBRARY(
  biquad
  libs/biquad.c
)

add_executable(
  html
  html.c
)

target_link_libraries(
  html
  sf2
)

target_link_libraries(
  runtime
  sf2
  biquads
)
