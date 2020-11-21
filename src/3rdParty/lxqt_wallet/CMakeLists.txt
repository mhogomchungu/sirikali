cmake_minimum_required(VERSION 3.1.0)

set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON)

project(lxqt_wallet)

set(LIBRARY_VERSION "3.2.0")

add_definitions(-Wextra -Wall -pedantic)

find_package(PkgConfig)

include(GNUInstallDirs)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

add_subdirectory(backend)
add_subdirectory(frontend)
