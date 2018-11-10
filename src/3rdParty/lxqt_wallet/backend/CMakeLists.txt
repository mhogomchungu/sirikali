cmake_minimum_required(VERSION 2.6)

INCLUDE(FindPkgConfig)

find_file( GCRYPT_INCLUDE_FILE gcrypt.h )
find_path( GCRYPT_INCLUDE_PATH gcrypt.h )

find_library( GCRYPT_LIBRARY gcrypt )

if( NOT GCRYPT_INCLUDE_FILE )
	MESSAGE( FATAL_ERROR "could not find gcrypt header file" )
else()
	MESSAGE( STATUS "found gcrypt header file: ${GCRYPT_INCLUDE_FILE}" )
endif()

if( NOT GCRYPT_LIBRARY )
	MESSAGE( FATAL_ERROR "could not find gcrypt library(libgcrypt.so)" )
else()
	MESSAGE( STATUS "found gcrypt library: ${GCRYPT_LIBRARY}" )
endif()

get_filename_component( GCRYPT_LIBRARY_PATH ${GCRYPT_LIBRARY} DIRECTORY )

add_library( lxqt-wallet-backend STATIC lxqtwallet.c )

if( CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0 )
	if( WIN32 )
		set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -pedantic -Wformat-truncation=0 -I${GCRYPT_INCLUDE_PATH}" )
	else()
		set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -fPIC -pedantic -Wformat-truncation=0 -I${GCRYPT_INCLUDE_PATH}" )
	endif()
else()
	if( WIN32 )
		set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -pedantic -I${GCRYPT_INCLUDE_PATH}" )
	else()
		set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -fPIC -pedantic -I${GCRYPT_INCLUDE_PATH}" )
	endif()
endif()

if( WIN32 )
    set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -pedantic -I${GCRYPT_INCLUDE_PATH}" )
else()
    set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -fPIC -pedantic -I${GCRYPT_INCLUDE_PATH}" )
endif()

set_target_properties( lxqt-wallet-backend PROPERTIES LINK_FLAGS "-pie" )
TARGET_LINK_LIBRARIES( lxqt-wallet-backend "${GCRYPT_LIBRARY}" )
link_directories( "${GCRYPT_LIBRARY_PATH}" )
