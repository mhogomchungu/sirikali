cmake_minimum_required(VERSION 3.0.0)

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

if( WIN32 )
    add_library( lxqtwallet-backend STATIC backend/fake_wallet_backend.c )

    set_target_properties( lxqtwallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -pedantic" )
else()
    add_subdirectory(backend)
endif()

add_subdirectory(frontend)
