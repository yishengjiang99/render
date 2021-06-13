cmake_minimum_required(VERSION 3.14)
project(my_project)
# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

set(DCMAKE_TOOLCHAIN_FILE /usr/local/lib/vcpkg/scripts/buildsystems/vcpkg.cmake)

add_executable(
  sf2test
  tests/sf2.test.cc
)
add_executable(
  rendertest
  tests/render.test.cc
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