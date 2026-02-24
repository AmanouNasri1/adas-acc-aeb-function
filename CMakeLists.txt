cmake_minimum_required(VERSION 3.20)
project(adas_acc_aeb_function VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ACC_ENABLE_WERROR "Treat warnings as errors" ON)
option(ACC_ENABLE_SANITIZERS "Enable ASan/UBSan (Debug only)" ON)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  if(ACC_ENABLE_WERROR)
    add_compile_options(-Werror)
  endif()
endif()

if(ACC_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

# --- Library ---
add_library(acc_core
  src/acc/dummy.cpp
  src/acc/function.cpp
  src/acc/fsm.cpp
  src/acc/plausibility.cpp
  src/sim/scenario.cpp
)
target_include_directories(acc_core PUBLIC include)

# --- App ---
add_executable(sim_runner src/sim/sim_runner.cpp)
target_link_libraries(sim_runner PRIVATE acc_core)
target_include_directories(sim_runner PRIVATE include)

# --- Testing ---
include(CTest)
enable_testing()

include(FetchContent)

# Robust fetch method (avoids URL list parsing issues)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(acc_tests
  tests/test_dummy.cpp
  tests/test_fsm.cpp
  tests/test_scenarios.cpp
)
target_link_libraries(acc_tests PRIVATE acc_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(acc_tests)