cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project(my_project)
include_directories(include)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)


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
  fft
  libs/fft.c
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
  biquad
)

target_link_libraries(
  fftsamples
  runtime
  sf2
  biquad
  wavetable_oscillator
  fft
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

enable_testing()

add_executable(
  sf2test
  tests/sf2.test.cc
)

target_link_libraries(
    sf2test
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(sf2test)