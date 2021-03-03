# https://cliutils.gitlab.io/modern-cmake/
# https://github.com/floooh/pacman.c/blob/main/CMakeLists.txt
# https://github.com/septag/rizz/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 3.7...3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(tarch VERSION 0.1 LANGUAGES C)

include_directories(include)

option(TARCH_ENABLE_HOTRELOAD "Enable hot reloading" ON)
if(TARCH_ENABLE_HOTRELOAD)
    add_compile_definitions(TARCH_ENABLE_HOTRELOAD=1)

    # Only required when using hot-reloading, to link the same static libraries into both
    # the reloadable module and runner application.
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

option(SX_BUILD_TESTS "" OFF)
add_subdirectory(src/sx)

add_library(sokol STATIC src/sokol/sokol.c)
# Linux -pthread shenanigans
if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    # compile sokol.c as Objective-C
    target_compile_options(sokol PRIVATE -x objective-c)
    target_link_libraries(sokol
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework MetalKit"
        "-framework Metal"
        "-framework AudioToolbox")
else()
    if (CMAKE_SYSTEM_NAME STREQUAL Linux)
        target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL asound dl m)
        target_link_libraries(sokol PUBLIC Threads::Threads)
    endif()
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(DEFAULT_BUILD_TYPE "Release")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined")

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-gnu -Wno-c++98-compat)
endif()

file(GLOB C_SOURCES src/*.c)
set(DEPS sx sokol)
if(TARCH_ENABLE_HOTRELOAD)
  list(REMOVE_ITEM C_SOURCES src/main.c src/hot_reload.c)
  add_library(game_tmp MODULE ${C_SOURCES})
  target_link_libraries(game_tmp PRIVATE ${DEPS})

  # rename at end of build to prevent hot reload from loading partially built library
  add_custom_command(
      TARGET game_tmp POST_BUILD COMMAND ${CMAKE_COMMAND} -E rename
      $<TARGET_FILE:game_tmp>
      $<TARGET_FILE_PREFIX:game_tmp>game$<TARGET_FILE_SUFFIX:game_tmp>
  )

  if(CMAKE_SYSTEM_NAME STREQUAL Windows)
      add_executable(runner WIN32 src/main.c)
  else()
      add_executable(runner src/main.c)
  endif()
  target_sources(runner PRIVATE ${C_SOURCES})
else()
  if(CMAKE_SYSTEM_NAME STREQUAL Windows)
      add_executable(runner WIN32 ${C_SOURCES})
  else()
      add_executable(runner ${C_SOURCES})
  endif()
endif()
target_link_libraries(runner PRIVATE ${DEPS})
