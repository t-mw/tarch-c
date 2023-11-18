# https://cliutils.gitlab.io/modern-cmake/
# https://github.com/floooh/pacman.c/blob/main/CMakeLists.txt
# https://github.com/septag/rizz/blob/master/CMakeLists.txt
# https://github.com/floooh/sokol-samples/blob/master/CMakeLists.txt
# https://github.com/floooh/sokol-samples/blob/master/sapp/CMakeLists.txt

cmake_minimum_required(VERSION 3.7...3.19)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif ()

project(tarch VERSION 0.1 LANGUAGES C)

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

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)

option(SX_BUILD_TESTS "" OFF)
add_subdirectory(deps/sx)

if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(sokol_backend SOKOL_GLES3)
    set(sokol_slang "glsl300es:glsl100")
else ()
    set(sokol_backend SOKOL_GLCORE33)
    set(sokol_slang "glsl330")
endif ()
add_definitions(-D${sokol_backend})

find_program(SOKOL_SHDC sokol-shdc HINTS ${SOKOL_SHDC_DIR})
if (NOT SOKOL_SHDC)
    message(FATAL_ERROR "Could not find sokol-shdc binary")
endif ()

file(GLOB shader_files "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.glsl")

foreach (file ${shader_files})
    message(STATUS "Compiling shader ${file}")
    execute_process(COMMAND ${SOKOL_SHDC} --input ${file} --output ${file}.h --slang ${sokol_slang})
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${file})
endforeach ()

add_library(sokol STATIC deps/sokol.c)
# Linux -pthread shenanigans
if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL Darwin)
    # compile sokol.c as Objective-C
    target_compile_options(sokol PRIVATE -x objective-c)
    target_link_libraries(sokol
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework MetalKit"
        "-framework Metal"
        "-framework AudioToolbox")
else ()
    if (CMAKE_SYSTEM_NAME STREQUAL Linux)
        target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL asound dl m)
        target_link_libraries(sokol PUBLIC Threads::Threads)
    endif ()
endif ()

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

file(GLOB_RECURSE C_SOURCES src/*.c)

set(DEPS sx sokol stb tarch flecs_static ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
if (ENABLE_HOTRELOAD)
  list(FILTER C_SOURCES EXCLUDE REGEX ".*\\/main\\.c")

  add_library(game_tmp MODULE ${C_SOURCES})
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
  target_sources(runner PRIVATE ${C_SOURCES})
else ()
  if (CMAKE_SYSTEM_NAME STREQUAL Windows)
      add_executable(runner WIN32 ${C_SOURCES})
  else ()
      add_executable(runner ${C_SOURCES})
  endif ()
endif ()
target_link_libraries(runner PRIVATE ${DEPS})
