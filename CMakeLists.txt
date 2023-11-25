# https://cliutils.gitlab.io/modern-cmake/
# https://github.com/raysan5/raylib/blob/master/projects/CMake/
# https://github.com/krzosa/raylib_hot_reload_template/

cmake_minimum_required(VERSION 3.7...3.19)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif ()

project(tarch VERSION 0.1 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)
include_directories(deps)
include_directories(deps/flecs/include)
include_directories(deps/sx/include)

option(ENABLE_HOTRELOAD "Enable hot reloading" ON)
if (ENABLE_HOTRELOAD)
    add_compile_definitions(ENABLE_HOTRELOAD=1)

    # Only required when using hot-reloading, to link the same static libraries into both
    # the reloadable module and runner application.
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif ()

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
add_subdirectory(deps/raylib)

option(SX_BUILD_TESTS "" OFF)
add_subdirectory(deps/sx)

add_library(stb STATIC deps/stb.c)

add_library(tarch STATIC deps/tarch.c)

option(FLECS_SHARED_LIBS "" OFF)
add_subdirectory(deps/flecs)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(DEFAULT_BUILD_TYPE "Release")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined")

if (MSVC)
  add_compile_options(/W4 /WX)
else ()
  add_compile_options(-Wall -Wextra -Werror -Wno-gnu -Wno-c++98-compat)
endif ()

set(DEPS sx stb tarch flecs_static raylib)
if (APPLE)
    # raylib dependencies
    list(APPEND DEPS "-framework IOKit")
    list(APPEND DEPS "-framework Cocoa")
    list(APPEND DEPS "-framework OpenGL")
endif()

if (ENABLE_HOTRELOAD)
  file(GLOB MODULE_SOURCES src/*.c)
  list(FILTER MODULE_SOURCES EXCLUDE REGEX ".*\\/main\\.c")

  file(GLOB RUNNER_SOURCES src/runner/*.c)

  add_library(game_tmp MODULE ${MODULE_SOURCES})
  target_link_libraries(game_tmp PRIVATE ${DEPS})

  # rename at end of build to prevent hot reload from loading partially built library
  add_custom_command(
      TARGET game_tmp POST_BUILD COMMAND ${CMAKE_COMMAND} -E rename
      $<TARGET_FILE:game_tmp>
      $<TARGET_FILE_PREFIX:game_tmp>game$<TARGET_FILE_SUFFIX:game_tmp>
  )

  if (CMAKE_SYSTEM_NAME STREQUAL Windows)
      add_executable(runner WIN32 src/main.c)
  else ()
      add_executable(runner src/main.c)
  endif ()
  target_sources(runner PRIVATE ${RUNNER_SOURCES})
else ()
  file(GLOB_RECURSE C_SOURCES src/*.c)
  if (CMAKE_SYSTEM_NAME STREQUAL Windows)
      add_executable(runner WIN32 ${C_SOURCES})
  else ()
      add_executable(runner ${C_SOURCES})
  endif ()
endif ()
target_link_libraries(runner PRIVATE ${DEPS})
