cmake_minimum_required(VERSION 3.10)

set(LIB_VERSION_MAJOR 1)
set(LIB_VERSION_MINOR 4)
set(LIB_VERSION_PATCH 2)
set(LIB_VERSION_STRING ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(ansilove C)

include(CheckFunctionExists)
include(GNUInstallDirs)

option(ANSILOVE_BUILD_WASM "Build the WebAssembly wrapper" OFF)

set(IS_EMSCRIPTEN OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  set(IS_EMSCRIPTEN ON)
endif()

# Check if system has strtonum
check_function_exists(strtonum HAVE_STRTONUM)
check_function_exists(reallocarray HAVE_REALLOCARRAY)

# Additional include directories for compat functions and headers
include_directories("compat" "include" "src")

if(NOT IS_EMSCRIPTEN)
  # GD
  find_path(GD_INCLUDE_DIRS gd.h)
  find_library(GD_LIBRARIES NAMES gd REQUIRED)
  include_directories(${GD_INCLUDE_DIRS})
endif()

set(SRC src/clean.c src/drawchar.c src/fonts.c src/error.c src/loadfile.c src/init.c src/output.c src/savefile.c)
set(LOADERS src/loaders/ansi.c src/loaders/artworx.c src/loaders/binary.c src/loaders/icedraw.c src/loaders/pcboard.c src/loaders/tundra.c src/loaders/xbin.c)

if(NOT HAVE_STRTONUM)
  set(SRC ${SRC} compat/strtonum.c)
endif()

if(NOT HAVE_REALLOCARRAY)
  set(SRC ${SRC} compat/reallocarray.c)
endif()

add_definitions(-D_GNU_SOURCE -Wall -Wextra -pedantic)

if(NOT IS_EMSCRIPTEN)
  add_library(objlib OBJECT ${SRC} ${LOADERS})
  set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE ON)

  add_library(ansilove SHARED $<TARGET_OBJECTS:objlib>)
  add_library(ansilove-static STATIC $<TARGET_OBJECTS:objlib>)

  target_link_libraries(ansilove ${GD_LIBRARIES} m)

  set_target_properties(ansilove PROPERTIES
    VERSION ${LIB_VERSION_STRING} SOVERSION ${LIB_VERSION_MAJOR}
    C_VISIBILITY_PRESET hidden)

  install(TARGETS ansilove DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(TARGETS ansilove-static DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(FILES include/ansilove.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  install(FILES man/libansilove.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_clean.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_error.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_init.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_loadfile.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_savefile.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_ansi.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_artworx.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_binary.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_icedraw.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_pcboard.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_tundra.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
  install(FILES man/ansilove_xbin.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3/)
endif()

if(ANSILOVE_BUILD_WASM)
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    message(FATAL_ERROR "ANSILOVE_BUILD_WASM requires the Emscripten toolchain (use emcmake).")
  endif()

  add_library(objlib_wasm OBJECT ${SRC} ${LOADERS})
  target_include_directories(objlib_wasm PRIVATE include src compat wasm)
  target_compile_definitions(objlib_wasm PRIVATE ANSILOVE_WASM)

  add_executable(ansilove_wasm
    wasm/ansilove_wasm.c
    wasm/minigd.c
    $<TARGET_OBJECTS:objlib_wasm>)
  target_include_directories(ansilove_wasm PRIVATE include src compat wasm)
  target_compile_definitions(ansilove_wasm PRIVATE ANSILOVE_WASM)
  set_target_properties(ansilove_wasm PROPERTIES
    OUTPUT_NAME "libansilove"
    SUFFIX ".js"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/wasm")
  target_link_options(ansilove_wasm PRIVATE
    "-sMODULARIZE=1"
    "-sEXPORT_NAME=Libansilove"
    "-sEXPORTED_FUNCTIONS=['_ansilove_wasm_version','_ansilove_wasm_render_ansi','_ansilove_wasm_get_png_ptr','_ansilove_wasm_get_png_length','_ansilove_wasm_free_png','_malloc','_free']"
    "-sEXPORTED_RUNTIME_METHODS=['cwrap','ccall','UTF8ToString']"
    "-sENVIRONMENT=web,worker,node"
    "-sALLOW_MEMORY_GROWTH=1")
endif()
