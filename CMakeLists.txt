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

<<<<<<< HEAD
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

target_link_libraries(
  html
  sf2
)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

enable_testing()

=======
set(DCMAKE_TOOLCHAIN_FILE /usr/local/lib/vcpkg/scripts/buildsystems/vcpkg.cmake)
add_library(
  fft
  libs/fft.c
)
add_library(
  runtime
  runtime.c
)
add_library(
  biquad
  libs/biquad.c
)
>>>>>>> ccb63045f94e41b01371b10a1e1f4968357b9326
add_executable(
  sf2test
  tests/sf2.test.cc
)
add_executable(
  rendertest
  tests/render.test.cc
)
add_executable(
  midi
  midi.c
)
target_link_libraries(
  runtime
  biquad
)
target_link_libraries(
  midi
  fft
)
target_link_libraries(
    sf2test
    gtest_main
)
target_link_libraries(
    rendertest
    gtest_main
)
include(GoogleTest)
gtest_discover_tests(rendertest)
gtest_discover_tests(sf2test)