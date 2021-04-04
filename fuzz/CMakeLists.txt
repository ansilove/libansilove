cmake_minimum_required (VERSION 2.8.12)

set(CMAKE_C_COMPILER clang)

project (fuzz C)

# Ansilove library
find_path(ANSILOVE_INCLUDE_DIRS ansilove.h)
find_library(ANSILOVE_LIBRARIES NAMES ansilove REQUIRED)
include_directories(${ANSILOVE_INCLUDE_DIRS})

set(SRC_ANSI ansi.c)
set(SRC_ARTWORX artworx.c)
set(SRC_BINARY binary.c)
set(SRC_ICEDRAW icedraw.c)
set(SRC_PCBOARD pcboard.c)
set(SRC_TUNDRA tundra.c)
set(SRC_XBIN xbin.c)

add_definitions(-Wall -Wextra -std=c99 -pedantic -fsanitize=fuzzer)
add_executable(ansi ${SRC_ANSI})
add_executable(artworx ${SRC_ANSI})
add_executable(binary ${SRC_BINARY})
add_executable(icedraw ${SRC_BINARY})
add_executable(pcboard ${SRC_BINARY})
add_executable(tundra ${SRC_BINARY})
add_executable(xbin ${SRC_BINARY})

target_link_libraries(ansi ${ANSILOVE_LIBRARIES} -fsanitize=fuzzer)
target_link_libraries(artworx ${ANSILOVE_LIBRARIES} -fsanitize=fuzzer)
target_link_libraries(binary ${ANSILOVE_LIBRARIES} -fsanitize=fuzzer)
target_link_libraries(icedraw ${ANSILOVE_LIBRARIES} -fsanitize=fuzzer)
target_link_libraries(pcboard ${ANSILOVE_LIBRARIES} -fsanitize=fuzzer)
target_link_libraries(tundra ${ANSILOVE_LIBRARIES} -fsanitize=fuzzer)
target_link_libraries(xbin ${ANSILOVE_LIBRARIES} -fsanitize=fuzzer)
