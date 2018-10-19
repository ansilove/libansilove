cmake_minimum_required (VERSION 2.6)

set(LIB_VERSION_MAJOR 0)
set(LIB_VERSION_MINOR 0)
set(LIB_VERSION_PATCH 0)
set(LIB_VERSION_STRING ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})

project (ansilove C)

include(CheckFunctionExists)
include(GNUInstallDirs)

# Check if system has strtonum
check_function_exists(strtonum HAVE_STRTONUM)

# Additional include directories for compat functions
include_directories("compat")

# GD
find_path(GD_INCLUDE_DIRS gd.h)
find_library(GD_LIBRARIES NAMES gd REQUIRED)
include_directories(${GD_INCLUDE_DIRS})

set(SRC src/drawchar.c src/fonts.c src/error.c src/loadfile.c src/init.c src/output.c src/savefile.c)
set(LOADERS src/loaders/ansi.c src/loaders/artworx.c src/loaders/binary.c src/loaders/icedraw.c src/loaders/pcboard.c src/loaders/tundra.c src/loaders/xbin.c)

if(NOT HAVE_STRTONUM)
  set (SRC ${SRC} compat/strtonum.c)
endif()

add_definitions(-Wall -Wextra -std=c99 -pedantic)
add_library(ansilove SHARED ${SRC} ${LOADERS})

target_link_libraries(ansilove ${GD_LIBRARIES} m)
set_target_properties(ansilove PROPERTIES VERSION ${LIB_VERSION_STRING}
                               SOVERSION ${LIB_VERSION_MAJOR})

install(TARGETS ansilove DESTINATION lib)
install(FILES include/ansilove.h DESTINATION include)
