cmake_minimum_required(VERSION 2.8.12)

set(LIB_VERSION_MAJOR 1)
set(LIB_VERSION_MINOR 2)
set(LIB_VERSION_PATCH 8)
set(LIB_VERSION_STRING ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})

project(ansilove C)

include(CheckFunctionExists)
include(GNUInstallDirs)

# Check if system has strtonum
check_function_exists(strtonum HAVE_STRTONUM)
check_function_exists(reallocarray HAVE_REALLOCARRAY)

# Additional include directories for compat functions and headers
include_directories("compat" "include" "src")

# GD
find_path(GD_INCLUDE_DIRS gd.h)
find_library(GD_LIBRARIES NAMES gd REQUIRED)
include_directories(${GD_INCLUDE_DIRS})

set(SRC src/clean.c src/drawchar.c src/fonts.c src/error.c src/loadfile.c src/init.c src/output.c src/savefile.c)
set(LOADERS src/loaders/ansi.c src/loaders/artworx.c src/loaders/binary.c src/loaders/icedraw.c src/loaders/pcboard.c src/loaders/tundra.c src/loaders/xbin.c)

if(NOT HAVE_STRTONUM)
  set(SRC ${SRC} compat/strtonum.c)
endif()

if(NOT HAVE_REALLOCARRAY)
  set(SRC ${SRC} compat/reallocarray.c)
endif()

add_definitions(-D_GNU_SOURCE -Wall -Wextra -std=c99 -pedantic)
add_library(ansilove SHARED ${SRC} ${LOADERS})

target_link_libraries(ansilove ${GD_LIBRARIES} m)

set_target_properties(ansilove PROPERTIES
  VERSION ${LIB_VERSION_STRING} SOVERSION ${LIB_VERSION_MAJOR}
  C_VISIBILITY_PRESET hidden)

install(TARGETS ansilove DESTINATION ${CMAKE_INSTALL_LIBDIR})
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
