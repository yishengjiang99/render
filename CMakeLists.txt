cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project(my_project)
include_directories(include)

ADD_LIBRARY(
	sf2
  sf2/sf2.c
)

ADD_LIBRARY(
	runtime
  runtime/runtime.c
)

ADD_LIBRARY(
  biquad
  biquad/biquad.c
)

ADD_LIBRARY(
  wavetable_oscillator
  libs/wavetable_oscillator.c
)

add_executable(
  html
  html.c
)

add_executable(
  fftsamples
  fft-samples.c
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

target_link_libraries(
  fftsamples
  runtime
  sf2
  biquads
  wavetable_oscillator
)