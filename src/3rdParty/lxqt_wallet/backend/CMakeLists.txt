cmake_minimum_required(VERSION 3.0.0)

add_definitions(-Wno-unused-result)

find_file(GCRYPT_INCLUDE_FILE gcrypt.h)

find_library(GCRYPT_LIBRARY gcrypt)

if(NOT GCRYPT_INCLUDE_FILE)
    MESSAGE(FATAL_ERROR "Could not find gcrypt header file")
else()
    MESSAGE(STATUS "Found gcrypt header file: ${GCRYPT_INCLUDE_FILE}")
endif()

if(NOT GCRYPT_LIBRARY)
    MESSAGE(FATAL_ERROR "Could not find gcrypt library(libgcrypt.so)")
else()
    MESSAGE(STATUS "Found gcrypt library: ${GCRYPT_LIBRARY}")
endif()

add_library(lxqtwallet-backend STATIC lxqtwallet.c)
if( CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0 )
        set_target_properties(lxqtwallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -fPIC -pedantic -Wno-format-truncation")
else()
        set_target_properties(lxqtwallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -fPIC -pedantic")
endif()
set_target_properties(lxqtwallet-backend PROPERTIES LINK_FLAGS "-pie")

target_link_libraries(lxqtwallet-backend "${GCRYPT_LIBRARY}")
